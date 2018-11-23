#include "board.h"
#include "stm8l15x_conf.h"
#include "stm8l15x.h"

int main( void )
{
    GPIO_Init(PBUTTON_GPIO_PORT,PBUTTON_GPIO_PIN,GPIO_Mode_In_FL_IT);
    GPIO_Init(LED_GREEN_PORT,LED_GREEN_PIN,GPIO_Mode_Out_PP_Low_Fast);
    GPIO_Init(COUNTER_HIGH_PORT,COUNTER_HIGH_PIN,GPIO_Mode_Out_PP_Low_Fast);
    EXTI_SetPinSensitivity(EXTI_Pin_1,EXTI_Trigger_Rising);
    enableInterrupts();
    while(1)
    {
    }
    return 0;
}
