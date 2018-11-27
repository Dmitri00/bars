#include "stm8l15x.h"
#include "stm8l15x_conf.h"
#include "board.h"
#include "tools.h"
#define RTCWUT_FREQ 32 /* Hz  (0.03125 ms) */
// 
void RTC_init();
int main( void )
{
    
    // Both counters work properly, when
    GPIO_Init(COUNT0_GPIO_PORT,COUNT0_GPIO_PIN, GPIO_Mode_In_FL_IT);
    //GPIO_Init(COUNT1_GPIO_PORT,COUNT1_GPIO_PIN, GPIO_Mode_In_FL_IT);
    //Set exti pin sensitivity

    GPIO_Init(BUTTON0_GPIO_PORT,BUTTON0_GPIO_PIN, GPIO_Mode_In_FL_IT);

    //GPIO_Init(BUTTON1_GPIO_PORT,BUTTON1_GPIO_PIN, GPIO_Mode_In_FL_IT);

    EXTI_SetPinSensitivity (EXTI_Pin_7,EXTI_Trigger_Falling);
    EXTI_SetPinSensitivity (EXTI_Pin_6,EXTI_Trigger_Falling);
    
    GPIO_Init(LED1_GPIO_PORT,LED1_GPIO_PIN, GPIO_Mode_Out_PP_High_Slow);
    GPIO_Init(LED2_GPIO_PORT,LED2_GPIO_PIN, GPIO_Mode_Out_PP_High_Slow);
    GPIO_Init(LED3_GPIO_PORT,LED3_GPIO_PIN, GPIO_Mode_Out_PP_High_Slow);

    
    GPIOE->CR2 &= ~( (uint8_t)GPIO_Pin_5);
    GPIOD->CR2 |= ( (uint8_t)GPIO_Pin_6);
    //GPIOD->CR2 |= ( (uint8_t)GPIO_Pin_7);
    //GPIOE->CR2 |=  (uint8_t)GPIO_Pin_6;
    //GPIOC->CR2 |=  (uint8_t)GPIO_Pin_7;
    RTC_init();
    enableInterrupts();
    while(1) {
        if (RTC_GetWakeUpCounter() == 0)
            RTC_SetWakeUpCounter(RTCWUT_FREQ*5);
        RTC_WakeUpCmd(ENABLE); 
        halt();
    }
  return 0;
}

void RTC_init() {
    /* Enable RTC clock */
    
    CLK_LSEConfig (CLK_LSE_ON);
    while (CLK_GetFlagStatus(CLK_FLAG_LSERDY) == RESET) {}
    CLK_RTCClockConfig (CLK_RTCCLKSource_LSE, CLK_RTCCLKDiv_64);
    CLK_PeripheralClockConfig(CLK_Peripheral_RTC, ENABLE);
    RTC_WakeUpClockConfig (RTC_WakeUpClock_RTCCLK_Div16);

    /* Configure the RTC */
    //RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits );
    
    RTC_WakeUpCmd(DISABLE); 
    RTC_SetWakeUpCounter(RTCWUT_FREQ*5);
    RTC_ITConfig(RTC_IT_WUT, ENABLE);
    
}
