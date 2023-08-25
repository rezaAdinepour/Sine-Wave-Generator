/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "math.h"
#include "LCD/lcd.h"
#include <stdio.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define PI 3.1415926
#define k1 HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_6)
#define k2 HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_7)
#define k3 HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_8)
#define k4 HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_9)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
DAC_HandleTypeDef hdac;
DMA_HandleTypeDef hdma_dac1;

TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_DAC_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//userinterface variable
enum eType {menu, freq, ampl, set, setFreq, setAmpl} state;
char str[17];
//uint32_t f = 10;
float f[] = 
{
  11, 15, 18, 22, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 36, 37, 39, 41, 43, 45, 47, 50, 53, 56, 60, 64, 69, 75, 81, 90, 100, 112, 128, 150, 150, 180, 225, 
  240, 250, 260, 270, 280, 290, 300, 310, 320, 330, 345, 360, 375, 390, 410, 430, 450, 470, 500, 530, 560, 600, 640, 690, 750, 810, 900, 1000,
  1120, 1280, 1500, 1800, 2250, 3000, 4500, 9000, 90000
}; //length = 73 -- indez of 63 to end KHz


float Ampl_range[] =
{
  0.15151515151515152, 0.18181818181818182, 0.21212121212121213, 0.24242424242424243, 0.2727272727272727, 0.30303030303030304, 
  0.3333333333333333, 0.3636363636363636, 0.3939393939393939, 0.42424242424242425, 0.45454545454545453, 0.48484848484848475,
  0.5151515151515151, 0.5454545454545454, 0.5757575757575757,  0.606060606060606, 0.6363636363636362, 0.6666666666666666,
  0.6969696969696969, 0.7272727272727272, 0.7575757575757575, 0.7878787878787878, 0.818181818181818, 0.8484848484848484,
  0.8787878787878787, 0.9090909090909091, 0.9393939393939393, 0.9696969696969695, 0.9999999999999999
}; // 0.5V to 3.3V

uint32_t Q_range[] = 
{
  80, 60, 50, 40, 360, 350, 340, 330, 320, 310, 300, 290, 280, 270, 260, 250, 240, 230, 220, 210, 200, 190, 180, 170, 160, 150, 140, 130, 120, 110, 100, 90, 
  80, 70, 60, 50, 40, 360, 350, 340, 330, 320, 310, 300, 290, 280, 270, 260, 250, 240, 230, 220, 210, 200, 190, 180, 170, 160, 150, 140, 130, 120, 110, 100, 90,
  80, 70, 60, 50, 40, 30, 20, 1, 1
};

uint32_t Z_range[] = 
{
  1000, 1000, 1000, 1000, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 
  100, 100, 100, 100, 100, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 
  10, 10, 10, 10, 10, 10, 10, 100, 10
};

unsigned char n_f = 0; //index of array frequency
unsigned char n_A= 0; //index of array ampl
uint32_t A = 5;
uint32_t tSys;
uint32_t i = 0;
unsigned char flag = 0;


//generate sine variable
float val = 0.2;
uint32_t var;
uint32_t sinVal[100];
unsigned int N = 9;     //for change frequency of sine wave
float Ampl = 0.1515;  //amplitude of sine wave in volt
float Adb = 20; //amplitude of sine wave in db

uint32_t Q = 1, W = 1; // W only 1 --  Q variable beetwen  1 to 360
uint32_t Z = 1000, X = 1;


float volt2db(float v)
{
    return ((int)(20 * log10(v)));
}

float db2volt(float d)
{
    return ((int)(pow(10, (d/20))));
}


void getSinVal(void)
{
  for(int i = 0; i < 100; i++)
  {
    sinVal[i] = Ampl_range[n_A] * (sin(i*2*PI/100) + 1) * (4096/2) ;
  }
}


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_DAC_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  
  lcd_init();
  lcd_clear();
  lcd_gotoxy(0, 0);
  lcd_puts("******SUT*******");
  lcd_gotoxy(3, 1);
  lcd_puts("OAE  Group");
  HAL_Delay(6000);
  lcd_clear();
  
  //HAL_DAC_Start(&hdac, DAC_CHANNEL_1);
  HAL_TIM_Base_Start(&htim2);
  HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_1, sinVal, 100, DAC_ALIGN_12B_R);
  
  

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  
  
  while (1)
  {
    switch (state)
    {
      case menu:
        lcd_gotoxy(0, 0);
        lcd_puts("1.Freq");
        lcd_gotoxy(10, 0);
        lcd_puts("2.Ampl");
        lcd_gotoxy(0, 1);
        lcd_puts("3.Set");
        lcd_gotoxy(10, 1);
        lcd_puts("4.Back");
        
        if(flag == 0)
        {
          if(k1 == GPIO_PIN_RESET)
          {
            //while(k1 == GPIO_PIN_RESET);   
            tSys = 0;
            state = freq;
            lcd_clear();
            flag = 1;
          }
        }
        else
        {
          if(tSys > 199)
            flag = 0;
        }
        
        
        if(flag == 0)
        {
          if(k2 == GPIO_PIN_RESET)
          {
            //while(k2 == GPIO_PIN_RESET);
            tSys = 0;
            state = ampl;
            lcd_clear();
            flag = 1;
          }
        }
        else
        {
          if(tSys > 199)
            flag = 0;
        }
        
        
        if(flag == 0)
        {
          if(k3 == GPIO_PIN_RESET)
          {
            //while(k3 == GPIO_PIN_RESET);
            tSys = 0;
            state = set;
            lcd_clear();   
            flag = 1;
          }
        }
        else
        {
          if(tSys > 199)
            flag = 0;
        }
        break;
    
      
      case freq:
        lcd_gotoxy(0, 0);
        if(Q == W == 1)
        {
          switch (Z)
          {
          case 10:
            lcd_puts("Freq: 90 KHz");
            break;
          case 100:
            lcd_puts("Freq: 9 KHz");
            break;
          case 1000:
            lcd_puts("Freq: 900 Hz");
            break;
          case 10000:
            lcd_puts("Freq: 90 Hz");
            break;
          }
        }
        
        if(Q == 10 && W == 1)
        {
          switch (Z)
          {
            case 10:
              lcd_puts("Freq: 9 KHz");
              break;
            case 100:
              lcd_puts("Freq: 900 Hz");
              break;
            case 1000:
              lcd_puts("Freq: 90 Hz");
              break;
          }
        }
        
        if(Q == 20 && W == 1)
        {
          switch (Z)
          {
            case 10:
              lcd_puts("Freq: 4.5 KHz");
              break;
            case 100:
              lcd_puts("Freq: 450 Hz");
              break;
            case 1000:
              lcd_puts("Freq: 45 Hz");
              break;
          }
        }
        
        if(Q == 30 && W == 1)
        {
          switch (Z)
          {
            case 10:
              lcd_puts("Freq: 3 KHz");
              break;
            case 100:
              lcd_puts("Freq: 300 Hz");
              break;
            case 1000:
              lcd_puts("Freq: 30 Hz");
              break;
          }
        }
        
        if(Q == 40 && W == 1)
        {
          switch (Z)
          {
            case 10:
              lcd_puts("Freq: 2.25 KHz");
              break;
            case 100:
              lcd_puts("Freq: 225 Hz");
              break;
            case 1000:
              lcd_puts("Freq: 22 Hz");
              break;
          }
        }
        
        if(Q == 50 && W == 1)
        {
          switch (Z)
          {
            case 10:
              lcd_puts("Freq: 1.8 KHz");
              break;
            case 100:
              lcd_puts("Freq: 180 Hz");
              break;
            case 1000:
              lcd_puts("Freq: 18 Hz");
              break;
          }
        }
        
        if(Q == 60 && W == 1)
        {
          switch (Z)
          {
            case 10:
              lcd_puts("Freq: 1.5 KHz");
              break;
            case 100:
              lcd_puts("Freq: 150 Hz");
              break;
            case 1000:
              lcd_puts("Freq: 15 Hz");
              break;
          }
        }
        
        if(Q == 70 && W == 1)
        {
          switch (Z)
          {
            case 10:
              lcd_puts("Freq: 1.28 KHz");
              break;
            case 100:
              lcd_puts("Freq: 128 Hz");
              break;
            case 1000:
              lcd_puts("Freq: 13 Hz");
              break;
          }
        }
        
        if(Q == 80 && W == 1)
        {
          switch (Z)
          {
            case 10:
              lcd_puts("Freq: 1.12 KHz");
              break;
            case 100:
              lcd_puts("Freq: 112 Hz");
              break;
            case 1000:
              lcd_puts("Freq: 11 Hz");
              break;
          }
        }
        
        if(Q == 90 && W == 1)
        {
          switch (Z)
          {
            case 10:
              lcd_puts("Freq: 1 KHz");
              break;
            case 100:
              lcd_puts("Freq: 100 Hz");
              break;
            case 1000:
              lcd_puts("Freq: 10 Hz");
              break;
          }
        }
        
        if(Q == 100 && W == 1)
        {
          switch (Z)
          {
            case 10:
              lcd_puts("Freq: 900 Hz");
              break;
            case 100:
              lcd_puts("Freq: 90 Hz");
              break;
            case 1000:
              lcd_puts("Freq: 9 Hz");
              break;
          }
        }
        
        if(Q == 110 && W == 1)
        {
          switch (Z)
          {
            case 10:
              lcd_puts("Freq: 810 Hz");
              break;
            case 100:
              lcd_puts("Freq: 81 Hz");
              break;
            case 1000:
              lcd_puts("Freq: 8 Hz");
              break;
          }
        }
        
        if(Q == 120 && W == 1)
        {
          switch (Z)
          {
            case 10:
              lcd_puts("Freq: 750 Hz");
              break;
            case 100:
              lcd_puts("Freq: 75 Hz");
              break;
            case 1000:
              lcd_puts("Freq: 7 Hz");
              break;
          }
        }
        
        if(Q == 130 && W == 1)
        {
          switch (Z)
          {
            case 10:
              lcd_puts("Freq: 690 Hz");
              break;
            case 100:
              lcd_puts("Freq: 69 Hz");
              break;
            case 1000:
              lcd_puts("Freq: 6 Hz");
              break;
          }
        }
        
        if(Q == 140 && W == 1)
        {
          switch (Z)
          {
            case 10:
              lcd_puts("Freq: 640 Hz");
              break;
            case 100:
              lcd_puts("Freq: 64 Hz");
              break;
            case 1000:
              lcd_puts("Freq: 6 Hz");
              break;
          }
        }
        
        if(Q == 150 && W == 1)
        {
          switch (Z)
          {
            case 10:
              lcd_puts("Freq: 600 Hz");
              break;
            case 100:
              lcd_puts("Freq: 60 Hz");
              break;
          }
        }
        
        if(Q == 160 && W == 1)
        {
          switch (Z)
          {
            case 10:
              lcd_puts("Freq: 560 Hz");
              break;
            case 100:
              lcd_puts("Freq: 56 Hz");
              break;
          }
        }
        
        if(Q == 170 && W == 1)
        {
          switch (Z)
          {
            case 10:
              lcd_puts("Freq: 530 Hz");
              break;
            case 100:
              lcd_puts("Freq: 53 Hz");
              break;
          }
        }
        
        if(Q == 180 && W == 1)
        {
          switch (Z)
          {
            case 10:
              lcd_puts("Freq: 500 Hz");
              break;
            case 100:
              lcd_puts("Freq: 50 Hz");
              break;
          }
        }
        
        if(Q == 190 && W == 1)
        {
          switch (Z)
          {
            case 10:
              lcd_puts("Freq: 470 Hz");
              break;
            case 100:
              lcd_puts("Freq: 47 Hz");
              break;
          }
        }
        
        if(Q == 200 && W == 1)
        {
          switch (Z)
          {
            case 10:
              lcd_puts("Freq: 450 Hz");
              break;
            case 100:
              lcd_puts("Freq: 45 Hz");
              break;
          }
        }
        
        if(Q == 210 && W == 1)
        {
          switch (Z)
          {
            case 10:
              lcd_puts("Freq: 430 Hz");
              break;
            case 100:
              lcd_puts("Freq: 43 Hz");
              break;
          }
        }
        
        if(Q == 220 && W == 1)
        {
          switch (Z)
          {
            case 10:
              lcd_puts("Freq: 410 Hz");
              break;
            case 100:
              lcd_puts("Freq: 41 Hz");
              break;
          }
        }
        
        if(Q == 230 && W == 1)
        {
          switch (Z)
          {
            case 10:
              lcd_puts("Freq: 390 Hz");
              break;
            case 100:
              lcd_puts("Freq: 39 Hz");
              break;
          }
        }
        
        if(Q == 240 && W == 1)
        {
          switch (Z)
          {
            case 10:
              lcd_puts("Freq: 375 Hz");
              break;
            case 100:
              lcd_puts("Freq: 37.5 Hz");
              break;
          }
        }
        
        if(Q == 250 && W == 1)
        {
          switch (Z)
          {
            case 10:
              lcd_puts("Freq: 360 Hz");
              break;
            case 100:
              lcd_puts("Freq: 36 Hz");
              break;
          }
        }
        
        if(Q == 260 && W == 1)
        {
          switch (Z)
          {
            case 10:
              lcd_puts("Freq: 345 Hz");
              break;
            case 100:
              lcd_puts("Freq: 34.5 Hz");
              break;
          }
        }
        
        if(Q == 270 && W == 1)
        {
          switch (Z)
          {
            case 10:
              lcd_puts("Freq: 330 Hz");
              break;
            case 100:
              lcd_puts("Freq: 33 Hz");
              break;
          }
        }
        
        if(Q == 280 && W == 1)
        {
          switch (Z)
          {
            case 10:
              lcd_puts("Freq: 320 Hz");
              break;
            case 100:
              lcd_puts("Freq: 32 Hz");
              break;
          }
        }
        
        if(Q == 290 && W == 1)
        {
          switch (Z)
          {
            case 10:
              lcd_puts("Freq: 310 Hz");
              break;
              case 100:
              lcd_puts("Freq: 31 Hz");
              break;
          }
        }
        
        if(Q == 300 && W == 1)
        {
          switch (Z)
          {
            case 10:
              lcd_puts("Freq: 300 Hz");
              break;
            case 100:
              lcd_puts("Freq: 30 Hz");
              break;
          }
        }
        
        if(Q == 310 && W == 1)
        {
          switch (Z)
          {
            case 10:
              lcd_puts("Freq: 290 Hz");
              break;
            case 100:
              lcd_puts("Freq: 29 Hz");
              break;
          }
        }
        
        if(Q == 320 && W == 1)
        {
          switch (Z)
          {
            case 10:
              lcd_puts("Freq: 280 Hz");
              break;
            case 100:
              lcd_puts("Freq: 28 Hz");
              break;
          }
        }
        
        if(Q == 330 && W == 1)
        {
          switch (Z)
          {
            case 10:
              lcd_puts("Freq: 270 Hz");
              break;
            case 100:
              lcd_puts("Freq: 27 Hz");
              break;
          }
        }
        
        if(Q == 340 && W == 1)
        {
          switch (Z)
          {
            case 10:
              lcd_puts("Freq: 260 Hz");
              break;
            case 100:
              lcd_puts("Freq: 26 Hz");
              break;
          }
        }
        
        if(Q == 350 && W == 1)
        {
          switch (Z)
          {
            case 10:
              lcd_puts("Freq: 250 Hz");
              break;
            case 100:
              lcd_puts("Freq: 25 Hz");
              break;
          }
        }
        
        if(Q == 360 && W == 1)
        {
          switch (Z)
          {
            case 10:
              lcd_puts("Freq: 240 Hz");
              break;
            case 100:
              lcd_puts("Freq: 25 Hz");
              break;
          }
        }
        
        if(flag == 0)
        {
          if(k4 == GPIO_PIN_RESET)
          {
            //while(k4 == GPIO_PIN_RESET);
            tSys = 0;
            state = menu;
            lcd_clear();
            flag = 1;
          }
        }
        else
        {
          if(tSys > 199)
            flag = 0;
        }
        break;
        
        
      case ampl:
        lcd_gotoxy(0, 0);
        sprintf(str, "Ampl: %.2f V", Ampl_range[n_A] * 3.3);
        lcd_puts(str);
        
        if(flag == 0)
        {
          if(k4 == GPIO_PIN_RESET)
          {
            //while(k4 == GPIO_PIN_RESET);
            tSys = 0;
            state = menu;
            lcd_clear();
            flag = 1;
          }
        }
        else
        {
          if(tSys > 199)
            flag = 0;
        }
        break;
        
        
      case set:
        lcd_gotoxy(0, 0);
        lcd_puts("1.Set Freq");
        lcd_gotoxy(0, 1);
        lcd_puts("2.Set Ampl");
        lcd_gotoxy(11, 1);
        lcd_puts("3.Bck");
        
        if(flag == 0)
        {
          if(k1 == GPIO_PIN_RESET)
          {
            //while(k1 == GPIO_PIN_RESET);
            tSys = 0;
            state = setFreq;
            lcd_clear();
            flag = 1;
          }
        }
        else
        {
          if(tSys > 199)
            flag = 0;
        }
        
        
        if(flag == 0)
        {
          if(k2 == GPIO_PIN_RESET)
          {
            //while(k2 == GPIO_PIN_RESET);
            tSys = 0;
            state = setAmpl;
            lcd_clear();
            flag = 1;
          }
        }
        else
        {
          if(tSys > 199)
            flag = 0;
        }
        
        
        if(flag == 0)
        {
          if(k4 == GPIO_PIN_RESET)
          {
            //while(k3 == GPIO_PIN_RESET);
            tSys = 0;
            state = menu;
            lcd_clear();
            flag = 1;
          }
        }
        else
        {
          if(tSys > 199)
            flag = 0;
        }
        break;
    
        
      case setFreq:
        MX_TIM2_Init();
        getSinVal();
        lcd_gotoxy(0, 0);
        if(n_f > 63)
        {
          sprintf(str, "Freq: %.2f KHz", f[n_f]/1000);
        }
        else
          sprintf(str, "Freq: %.2f Hz", f[n_f]);
        lcd_puts(str);
        
        if(flag == 0)
        {
          if(k1 == GPIO_PIN_RESET)
          {
            //while(k1 == GPIO_PIN_RESET);
            tSys = 0;
            n_f += 1;
            flag = 1;
            if(n_f > 73)
            {
              n_f = 0;
              lcd_clear();
              lcd_gotoxy(0, 0);
              sprintf(str, "Freq: %.2f Hz", f[n_f]);
              lcd_puts(str);
            }
          }
        }
        else
        {
          if(tSys > 199)
            flag = 0;
        }
        
        
        if(flag == 0)
        {
          if(k2 == GPIO_PIN_RESET)
          {
            //while(k2 == GPIO_PIN_RESET);
            tSys = 0;
            n_f -= 1;
            flag = 1;
            if(n_f < 0)
            {
              n_f = 0;
              lcd_clear();
              lcd_gotoxy(0, 0);
              sprintf(str, "Freq: %.2f Hz", f[n_f]);
              lcd_puts(str);
            }
          }
        }
        else
        {
          if(tSys > 199)
            flag = 0;
        }

        if(flag == 0)
        {
          if(k4 == GPIO_PIN_RESET)
          {
            //while(k4 == GPIO_PIN_RESET);
            tSys = 0;
            state = set;
            lcd_clear();
            flag = 1;
          }
        }
        else
        {
          if(tSys > 199)
            flag = 0;
        }
        break;
      
        
      case setAmpl:
        getSinVal();
        lcd_gotoxy(0, 0);
        sprintf(str, "Ampl: %.2f V", Ampl_range[n_A] * 3.3);
        lcd_puts(str);
        
        if(flag == 0)
        {
          if(k1 == GPIO_PIN_RESET)
          {
            //while(k1 == GPIO_PIN_RESET);
            tSys = 0;
            n_A += 1;
            flag = 1;
            if(n_A > 28)
              n_A = 0;
          }
        }
        else
        {
          if(tSys > 199)
            flag = 0;
        }
        
        
        if(flag == 0)
        {
          if(k2 == GPIO_PIN_RESET)
          {
            //while(k2 == GPIO_PIN_RESET);
            tSys = 0;
            n_A -= 0.1;
            flag = 1;
            if(n_A < 0)
              n_A = 0;
          }
        }
        else
        {
          if(tSys > 199)
            flag = 0;
        }
        
        
        if(flag == 0)
        {
          if(k4 == GPIO_PIN_RESET)
          {
            //while(k4 == GPIO_PIN_RESET);
            tSys = 0;
            state = set;
            lcd_clear();
            flag = 1;
          }
        }
        break;
        
        
    }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief DAC Initialization Function
  * @param None
  * @retval None
  */
static void MX_DAC_Init(void)
{

  /* USER CODE BEGIN DAC_Init 0 */

  /* USER CODE END DAC_Init 0 */

  DAC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN DAC_Init 1 */

  /* USER CODE END DAC_Init 1 */
  /** DAC Initialization
  */
  hdac.Instance = DAC;
  if (HAL_DAC_Init(&hdac) != HAL_OK)
  {
    Error_Handler();
  }
  /** DAC channel OUT1 config
  */
  sConfig.DAC_Trigger = DAC_TRIGGER_T2_TRGO;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
  if (HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DAC_Init 2 */

  /* USER CODE END DAC_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */
  

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */
  Q = Q_range[n_f];
  Z = Z_range[n_f];
  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = Q-W;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = Z-X;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA5 PA6 PA7 */
  GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 PB2 PB3
                           PB4 PB5 PB6 PB7 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PC6 PC7 PC8 PC9 */
  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

