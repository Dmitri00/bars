#include "stm8l15x.h"
#include "stm8l15x_conf.h"
#include "board.h"
#include "tools.h"

/*! global state variable to exchange info between main.c and interrupts */
MCUState_t mcu_state = CONFIG;
void RTC_init();
void GPIO_init();
void SPI_init();
void ADC_init();
void ITC_init();


int main( void )
{

#ifdef DISCOVERY_BOARD

//USART_init();
#endif

    DEBUG("1");
   
    GPIO_init();
    SPI_init();
    ADC_init();
    ITC_init();

    /* FLASH IO routine */
    FLASH_Unlock(FLASH_MemType_Data);
    /* Wait until Data EEPROM area unlocked flag is set*/
    while (FLASH_GetFlagStatus(FLASH_FLAG_DUL) == RESET)
    {}
    flash_write_int32(INTERVAL_ADDR,(uint32_t)0);
    flash_write_int32(COUNTER_ADDR(0), (uint32_t)0);
    flash_write_int32(COUNTER_ADDR(1), (uint32_t)0);
    
    GPIO_ToggleBits(LED1_GPIO_PORT,LED3_GPIO_PIN);//1
    delay_ms(200);
    GPIO_ToggleBits(LED2_GPIO_PORT,LED3_GPIO_PIN);//0
    delay_ms(200);
    GPIO_ToggleBits(LED3_GPIO_PORT,LED3_GPIO_PIN);//1
    delay_ms(200);
#ifdef STM8L05X_MD_VL
    GPIO_ToggleBits(LED4_GPIO_PORT,LED3_GPIO_PIN);//0
    delay_ms(200);
    GPIO_ToggleBits(LED5_GPIO_PORT,LED3_GPIO_PIN);//0
    delay_ms(200);
#endif
    GPIO_ToggleBits(LED1_GPIO_PORT,LED3_GPIO_PIN);//1
    delay_ms(200);
    GPIO_ToggleBits(LED2_GPIO_PORT,LED3_GPIO_PIN);//0
    delay_ms(200);
    GPIO_ToggleBits(LED3_GPIO_PORT,LED3_GPIO_PIN);//1
    delay_ms(200);
#ifdef STM8L05X_MD_VL
    GPIO_ToggleBits(LED4_GPIO_PORT,LED3_GPIO_PIN);//0
    delay_ms(200);
    GPIO_ToggleBits(LED5_GPIO_PORT,LED3_GPIO_PIN);//0
    delay_ms(200);
#endif

    enableInterrupts();
    while(1) {
        /* Enter Halt mode*/
      switch (mcu_state) {
        case OK:
            DEBUG(" main:ok ");        
            
            halt();
            
            break;
        case CONFIG:
          DEBUG(" main:config ");
            // do first configuration always in case of reconfiguration state
          RTC_DeInit();
             RTC_init();
             RTC_WakeUpCmd(ENABLE); 
            halt();
            

            break;
        case ERROR_STATE:
            // toogle led, delay
            // wait for CLR
          DEBUG(" main:error ");
            GPIO_WriteBit(LED3_GPIO_PORT,LED3_GPIO_PIN, SET);//1
            delay_ms(200);
            GPIO_ToggleBits(LED3_GPIO_PORT,LED3_GPIO_PIN);//0
            delay_ms(200);
            break;
        default:
            break;
      }
    }
  return 0;
}
void RTC_init() {
    /* Enable RTC clock */
    CLK_PeripheralClockConfig(CLK_Peripheral_RTC, ENABLE);

    /* Configure the RTC */
    RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits );
    
    RTC_WakeUpCmd(DISABLE); 
    RTC_SetWakeUpCounter(1);
    RTC_ITConfig(RTC_IT_WUT, ENABLE);
    
}
void GPIO_init() {

    GPIO_Init(COUNT0_GPIO_PORT,COUNT0_GPIO_PIN, GPIO_Mode_In_FL_IT);
    GPIO_Init(COUNT1_GPIO_PORT,COUNT1_GPIO_PIN, GPIO_Mode_In_FL_IT);
    //Set exti pin sensitivity
    EXTI_SetPinSensitivity (EXTI_Pin_7,EXTI_Trigger_Rising);
    EXTI_SetPinSensitivity (EXTI_Pin_6,EXTI_Trigger_Rising);
    EXTI_SetPinSensitivity (EXTI_Pin_5,EXTI_Trigger_Rising);
    EXTI_SetPinSensitivity (EXTI_Pin_1,EXTI_Trigger_Rising);
    EXTI_SetPinSensitivity (EXTI_Pin_0,EXTI_Trigger_Rising);

    GPIO_Init(BUTTON0_GPIO_PORT,BUTTON0_GPIO_PIN, GPIO_Mode_In_FL_IT);
    GPIO_Init(BUTTON1_GPIO_PORT,BUTTON1_GPIO_PIN, GPIO_Mode_In_FL_IT);
    
    GPIO_Init(LED1_GPIO_PORT,LED1_GPIO_PIN, GPIO_Mode_Out_PP_Low_Fast);
    GPIO_Init(LED2_GPIO_PORT,LED2_GPIO_PIN, GPIO_Mode_Out_PP_Low_Fast);
    GPIO_Init(LED3_GPIO_PORT,LED3_GPIO_PIN, GPIO_Mode_Out_PP_Low_Fast);
    
    GPIO_Init(ESP_CHEN_PORT,ESP_CHEN_PIN, GPIO_Mode_Out_PP_High_Fast);
    GPIO_Init(ESP_SLEEP_PORT,ESP_SLEEP_PIN, GPIO_Mode_Out_PP_High_Fast);
    GPIO_Init(ESP_RESET_PORT,ESP_RESET_PIN, GPIO_Mode_Out_PP_High_Fast);
    


}
void SPI_init() {
    CLK_PeripheralClockConfig(CLK_Peripheral_SPI1, ENABLE);
    //SPI_CLOCK:PB5, SPI_MOSI: PB6, SPI_MISO: PB7   NSS:PB4
    GPIO_Init(GPIOB, GPIO_Pin_5, GPIO_Mode_Out_PP_High_Fast);  
    GPIO_Init(GPIOB, GPIO_Pin_6, GPIO_Mode_Out_PP_High_Fast);  
    GPIO_Init(GPIOB, GPIO_Pin_7, GPIO_Mode_In_PU_No_IT);  
    /* CS */  
    GPIO_Init(GPIOB , GPIO_Pin_4, GPIO_Mode_Out_PP_High_Fast);  
    
    SPI_Init(SPI1, SPI_FirstBit_MSB, SPI_BaudRatePrescaler_2, SPI_Mode_Master,  
        SPI_CPOL_Low, SPI_CPHA_1Edge,   
        SPI_Direction_2Lines_FullDuplex, SPI_NSS_Soft, 0x07);

}
void ADC_init() {
    CLK_PeripheralClockConfig(CLK_Peripheral_ADC1, ENABLE);
    ADC_Init(ADC1, ADC_ConversionMode_Continuous, ADC_Resolution_6Bit, ADC_Prescaler_2);

    /* ADC channel used for IDD measurement */
    ADC_SamplingTimeConfig(ADC1, BATT_VOLTAGE_MEASUREMENT_CHANNEL, ADC_SamplingTime_384Cycles);
    /* Disable SchmittTrigger for ADC_Channel, to save power */
    ADC_SchmittTriggerConfig(ADC1, BATT_VOLTAGE_MEASUREMENT_CHANNEL, DISABLE);
        /* Start ADC1 Conversion using Software trigger*/


    
}
    


void ITC_init() {
  /* sending data routine. It may take some time, so allow to process other 
    interrupts */
  ITC_SetSoftwarePriority(RTC_IRQn, ITC_PriorityLevel_1);

  /* push button for battery status */
  ITC_SetSoftwarePriority(EXTI1_IRQn ,ITC_PriorityLevel_2); //clone at the dev board
  ITC_SetSoftwarePriority(EXTI5_IRQn ,ITC_PriorityLevel_2);
  
  // highest priority for the counter pins
  ITC_SetSoftwarePriority(EXTI6_IRQn ,ITC_PriorityLevel_3);
  ITC_SetSoftwarePriority(EXTI7_IRQn ,ITC_PriorityLevel_3);
}

