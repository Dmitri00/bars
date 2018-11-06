/**
  ******************************************************************************
  * @file    delay.c
  * @author  Microcontroller Division
  * @version V1.1.0
  * @date    08/2010
  * @brief   delay functions
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */

/* Includes ------------------------------------------------------------------*/

#include "stm8l15x_clk.h"
#include "board.h"
/**
  * @brief  delay for some time in ms unit
  * @caller auto_test
  * @param  n_ms is how many ms of time to delay
  * @retval None
  */
void delay_ms(u16 n_ms)
{
/* Init TIMER 2 */
  /* Timer measures time in intervals of 8 us (because value of 
  autoreload-register N =1ms/8us = 1000/8 = 125 is integer and).
  Timer input frequency is 2^(4-clk_presc)*10^6 MHz for HSI 
  and 2^(3 - clk_presc)*10^6 MHz for HSE.
  t(1 tim count) = 2^(p_clk+ p_tim)/f_clk = 8us
  solving for (using HSI):
  2^p_tim = 8us*f_clk/2^p_clk = (8*10-6)*(2^4 *10^6)/2^p_clk = 128/2^p_clk = 
  = 2^(7-p_clk)
  this leads to p_tim = 7 - p_clk.
  With HSE p_tim = 6 - p_clk
  Then Autoreload-register = 125*/

  
  CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, ENABLE);
  uint8_t clk_presc = CLK->CKDIVR;
  uint8_t tim_presc = CLK_MAX_DIV - CLK->CKDIVR;
  if (CLK_GetSYSCLKSource() ==CLK_SYSCLKSource_HSE) 
    tim_presc--;
    
  TIM2->PSCR = tim_presc;


  TIM2->ARRH = 0;
  TIM2->ARRL = 125;
  
  /* No need to add additional counts to starting counter value to 
  compensate tim2 initializing routine:
  all commands till `while(n_ms--)` take ~ 10 mcu cycles,
  while t(mcu cycle) =  1/16 us at f_clk=16 MHz and 8us only at 
  f_clk = 125 KHz. f_clk >= 1Mhz most of the time => error is to small */
    
  TIM2->CNTRH = 0;
  TIM2->CNTRL = 1;

/* clear update flag */
  TIM2->SR1 &= ~TIM_SR1_UIF;

/* Enable Counter */
  TIM2->CR1 |= TIM_CR1_CEN;

  while(n_ms--)
  {
    while((TIM2->SR1 & TIM_SR1_UIF) == 0) ;
    TIM2->SR1 &= ~TIM_SR1_UIF;
  }

/* Disable Counter */
  TIM2->CR1 &= ~TIM_CR1_CEN;
}

/**
  * @brief  delay for some time in 10us unit(partial accurate)
  * @caller auto_test
  * @param n_10us is how many 10us of time to delay
  * @retval None
  */
void delay_10us(u16 n_10us)
{
/* Init TIMER 4 */
  CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, ENABLE);

/* prescaler: / (2^0) = /1 */
  TIM2->PSCR = 0;

/* SYS_CLK_HSI_DIV1 Auto-Reload value: 16M / 1 = 16M, 16M / 100k = 160 */
  TIM2->ARRH = 0;
  TIM2->ARRL = 160;

/* Counter value: 10, to compensate the initialization of TIMER */
  TIM2->CNTRH = 0;
  TIM2->CNTRL = 10;

/* clear update flag */
  TIM2->SR1 &= ~TIM_SR1_UIF;

/* Enable Counter */
  TIM2->CR1 |= TIM_CR1_CEN;

  while(n_10us--)
  {
    while((TIM2->SR1 & TIM_SR1_UIF) == 0) ;
    TIM2->SR1 &= ~TIM_SR1_UIF;
  }

/* Disable Counter */
  TIM2->CR1 &= ~TIM_CR1_CEN;
  CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, DISABLE);

}

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
