/**
  ******************************************************************************
  * @file    GPIO_Toggle\main.c
  * @author  MCD Application Team
  * @version V2.0.4
  * @date    26-April-2018
  * @brief   This file contains the main function for GPIO Toggle example.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"
#include "defines.h"

/**
  * @addtogroup GPIO_Toggle
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint16_t Conversion_Value = 0;
uint32_t system_tick_cnt = 0;
uint32_t sys_clock_freq = 0;

FlagStatus task_1_enabled = RESET;

FlagStatus pwm_enabled = RESET; 

/* Private function prototypes -----------------------------------------------*/
static void clock_setup(void);
static void TIM1_Config(void);
static void TIM2_Config(void);
static void ADC_Config(void);
static void pwm_controller(void);

/* Private functions ---------------------------------------------------------*/
/* Public functions ----------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
void main(void)
{
  clock_setup();
  sys_clock_freq = CLK_GetClockFreq();
  /* Initialize I/Os in Output Mode */
  GPIO_Init(LED_GPIO_PORT, (GPIO_Pin_TypeDef)LED_GPIO_PINS, GPIO_MODE_OUT_PP_LOW_SLOW);
  GPIO_Init(PWM_PORT, (GPIO_Pin_TypeDef)PWM_PINS, GPIO_MODE_OUT_PP_HIGH_FAST); 

  
  TIM1_Config();
  // TIM2_Config();
  // ADC_Config();



  while (1)
  {
    /* Toggles LEDs */
    if (SET == task_1_enabled)
    {
      // // pwm_controller();
      // GPIO_WriteReverse(LED_GPIO_PORT, (GPIO_Pin_TypeDef)LED_GPIO_PINS);

      // /* Get converted value */
      // Conversion_Value = ADC1_GetConversionValue();

      task_1_enabled = RESET;
    }
  }

}

static void pwm_controller(void)
{
  static uint8_t cnt = 0;

  switch(cnt)
  {
    case 1:
      GPIO_WriteHigh(PWM_PORT, (GPIO_Pin_TypeDef)(PWM_CH1_PIN | PWM_CH2_PIN));
      GPIO_WriteLow(PWM_PORT,  (GPIO_Pin_TypeDef)(PWM_CH1N_PIN | PWM_CH2N_PIN));
      pwm_enabled = SET;
      break;

    case 2:
      pwm_enabled = RESET;
      break;

    case 5:
      cnt = 0;
      break;

    default:
    break;
  }

  cnt++;
  return;
}

static void clock_setup(void)
{
  CLK_DeInit();
  CLK_HSECmd(DISABLE);
  CLK_LSICmd(DISABLE);

  CLK_HSICmd(ENABLE);
  
  while(CLK_GetFlagStatus(CLK_FLAG_HSIRDY) == RESET);

  CLK_ClockSwitchCmd(ENABLE);

  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);

  CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);

  CLK_ClockSwitchConfig(CLK_SWITCHMODE_MANUAL, CLK_SOURCE_HSI,
  DISABLE, CLK_CURRENTCLOCKSTATE_ENABLE);

  CLK_PeripheralClockConfig(CLK_PERIPHERAL_SPI, DISABLE);

  CLK_PeripheralClockConfig(CLK_PERIPHERAL_I2C, DISABLE);

  CLK_PeripheralClockConfig(CLK_PERIPHERAL_ADC, ENABLE);

  CLK_PeripheralClockConfig(CLK_PERIPHERAL_AWU, DISABLE);

  CLK_PeripheralClockConfig(CLK_PERIPHERAL_UART1, DISABLE);

  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER1, ENABLE);

  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER2, ENABLE);

  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER4, DISABLE);
}

/**
  * @brief  Configure TIM1 to generate PWM signals
  * @param  None
  * @retval None
  */
static void TIM1_Config(void)
{

  TIM1_DeInit();

  /* Time Base configuration */
  TIM1_TimeBaseInit(0, TIM1_COUNTERMODE_UP, PWM_CYCLE - 1, 0);

  // /* Channel 1, 2 Configuration in PWM mode */
  TIM1_OC1Init(TIM1_OCMODE_PWM1, TIM1_OUTPUTSTATE_ENABLE, TIM1_OUTPUTNSTATE_ENABLE,
               PWM_CYCLE / 2, TIM1_OCPOLARITY_HIGH, TIM1_OCNPOLARITY_LOW, TIM1_OCIDLESTATE_SET,
               TIM1_OCNIDLESTATE_RESET);  
  
  TIM1_SetCompare3(PWM_CYCLE * 1 / 4);
  TIM1_SetCompare4(PWM_CYCLE * 3 / 4);

  TIM1_ITConfig(TIM1_IT_CC3, ENABLE);
  TIM1_ITConfig(TIM1_IT_CC4, ENABLE);


  enableInterrupts();

  /* TIM1 counter enable */
  TIM1_Cmd(ENABLE);

  TIM1_CtrlPWMOutputs(ENABLE);
}

/**
  * @brief  Configure TIM2 to act as system tick 
  * @param  None
  * @retval None
  */
static void TIM2_Config(void)
{
  TIM2_DeInit();

  /* Time Base configuration */
  /*
  TIM1_Period = 4095
  TIM1_Prescaler = 0
  TIM1_CounterMode = TIM1_COUNTERMODE_UP
  TIM1_RepetitionCounter = 0
  */

  TIM2_TimeBaseInit(TIM2_PRESCALER_16, 999);
  TIM2_SetAutoreload(999);
  TIM2_ARRPreloadConfig(ENABLE);
  
  TIM2_ITConfig(TIM2_IT_UPDATE, ENABLE);

  /* Enable general interrupts */  
  enableInterrupts();

  /* TIM2 counter enable */
  TIM2_Cmd(ENABLE);
}

/**
  * @brief  Configure ADC2 Continuous Conversion with End Of Conversion interrupt 
  *         enabled .
  * @param  None
  * @retval None
  */
static void ADC_Config()
{
  /*  Init GPIO for ADC1 */
  GPIO_Init(GPIOD, GPIO_PIN_5, GPIO_MODE_IN_FL_NO_IT);
  
  /* De-Init ADC peripheral*/
  ADC1_DeInit();

  /* Init ADC1 peripheral */
  ADC1_Init(ADC1_CONVERSIONMODE_CONTINUOUS, 
            ADC1_CHANNEL_5, 
            ADC1_PRESSEL_FCPU_D18,
            ADC1_EXTTRIG_TIM,
            DISABLE,
            ADC1_ALIGN_RIGHT,
            ADC1_SCHMITTTRIG_CHANNEL5,
            DISABLE);

  /*Start Conversion */
  ADC1_StartConversion();
}

#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
