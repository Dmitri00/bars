#include "stm8l15x.h"
#include "stm8l15x_conf.h"
#include "board.h"
#include "tools.h"
/* Firmware for verification of Ext. interrupts on buttons and counters */
int main( void )
{

    GPIO_Init(COUNT0_GPIO_PORT,COUNT0_GPIO_PIN, GPIO_Mode_In_FL_IT);
    GPIO_Init(COUNT1_GPIO_PORT,COUNT1_GPIO_PIN, GPIO_Mode_In_FL_IT);
    //Set exti pin sensitivity

    //GPIO_Init(BUTTON0_GPIO_PORT,BUTTON0_GPIO_PIN, GPIO_Mode_In_FL_IT);

    //GPIO_Init(BUTTON1_GPIO_PORT,BUTTON1_GPIO_PIN, GPIO_Mode_In_FL_IT);

    EXTI_SetPinSensitivity (EXTI_Pin_7,EXTI_Trigger_Falling);
    EXTI_SetPinSensitivity (EXTI_Pin_6,EXTI_Trigger_Falling);
    
    GPIO_Init(LED1_GPIO_PORT,LED1_GPIO_PIN, GPIO_Mode_Out_PP_High_Slow);
    GPIO_Init(LED2_GPIO_PORT,LED2_GPIO_PIN, GPIO_Mode_Out_PP_High_Slow);
    GPIO_Init(LED3_GPIO_PORT,LED3_GPIO_PIN, GPIO_Mode_Out_PP_High_Slow);

    
    GPIOE->CR2 &= ~( (uint8_t)GPIO_Pin_5);
    GPIOD->CR2 |= ( (uint8_t)GPIO_Pin_6);
    GPIOD->CR2 |= ( (uint8_t)GPIO_Pin_7);
    //GPIOE->CR2 |=  (uint8_t)GPIO_Pin_6;
    //GPIOC->CR2 |=  (uint8_t)GPIO_Pin_7;
    enableInterrupts();
    while(1) {
       
    }
  return 0;
}


