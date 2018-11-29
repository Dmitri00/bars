#include "stm8l15x.h"
#include "stm8l15x_conf.h"
#include "board.h"
#include "tools.h"
#define RTCWUT_FREQ 32 /* Hz  (0.03125 ms) */
/* Problems:
 * 1) COUNT0,COUNT1 don't work together with BUTTONS
 * 2) PC[5] always produces interrupts
 * 3) BUTTON's interrupts aren't being set up by GPIO
 */
void RTC_init();
int main( void )
{
    
    // Both counters work properly
    //GPIO_Init(COUNT0_GPIO_PORT,COUNT0_GPIO_PIN, GPIO_Mode_In_FL_IT);
    //GPIO_Init(COUNT1_GPIO_PORT,COUNT1_GPIO_PIN, GPIO_Mode_In_FL_IT);
    //Set exti pin sensitivity

    //Buttons work, when counters are disabled. I don't know the reason
    GPIO_Init(BUTTON0_GPIO_PORT,BUTTON0_GPIO_PIN, GPIO_Mode_In_FL_IT);
    GPIO_Init(BUTTON1_GPIO_PORT,BUTTON1_GPIO_PIN, GPIO_Mode_In_FL_IT);

    EXTI_SetPinSensitivity (EXTI_Pin_7,EXTI_Trigger_Falling);
    EXTI_SetPinSensitivity (EXTI_Pin_6,EXTI_Trigger_Falling);
    
    GPIO_Init(LED1_GPIO_PORT,LED1_GPIO_PIN, GPIO_Mode_Out_PP_High_Slow);
    GPIO_Init(LED2_GPIO_PORT,LED2_GPIO_PIN, GPIO_Mode_Out_PP_High_Slow);
    GPIO_Init(LED3_GPIO_PORT,LED3_GPIO_PIN, GPIO_Mode_Out_PP_High_Slow);

    //For some reason PC[5] pin produces interrupt constantly
    GPIOE->CR2 &= ~( (uint8_t)GPIO_Pin_5);
    // I explored, that interrupt enable bits aren't being set properly.
    // It happends with buttos, but i will lieave all comands
    //GPIOD->CR2 |= ( (uint8_t)GPIO_Pin_6); // Interrupt enable of COUNT0
    //GPIOD->CR2 |= ( (uint8_t)GPIO_Pin_7); // Interrupt enable of COUNT1
    //GPIOE->CR2 |=  (uint8_t)GPIO_Pin_6; // Interrupt enable of BUT0
    //GPIOC->CR2 |=  (uint8_t)GPIO_Pin_7; // Interrupt enable of BUT1
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
    
    // Enable external 32.768 KHz oscillator
    CLK_LSEConfig (CLK_LSE_ON);
    while (CLK_GetFlagStatus(CLK_FLAG_LSERDY) == RESET) {}
    
    // Divide LSE totally by 64*16=1024 from 32768 Hz to 32 Hz 
    CLK_RTCClockConfig (CLK_RTCCLKSource_LSE, CLK_RTCCLKDiv_64);
    CLK_PeripheralClockConfig(CLK_Peripheral_RTC, ENABLE);
    RTC_WakeUpClockConfig (RTC_WakeUpClock_RTCCLK_Div16);
    
    RTC_WakeUpCmd(DISABLE); 
    // RTCWUT_FREQ equal to 32 Hz
    RTC_SetWakeUpCounter( RTCWUT_FREQ*5); //Set up timer for 5 secs
    RTC_ITConfig(RTC_IT_WUT, ENABLE);
    
}
