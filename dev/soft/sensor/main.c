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
int main( void )
{
#ifdef DISCOVERY_BOARD
  //LCD_GLASS_Init();

#endif
    //DEBUG(" Starting up ");
    RTC_init();
    GPIO_init();
    SPI_init();
    ADC_init();
    /* FLASH IO routine */
    FLASH_Unlock(FLASH_MemType_Data);
    /* Wait until Data EEPROM area unlocked flag is set*/
    while (FLASH_GetFlagStatus(FLASH_FLAG_DUL) == RESET)
    {}
    flash_write_int32(INTERVAL_ADDR,(uint32_t)0);
    flash_write_int32(COUNTER_ADDR(0), (uint32_t)0);
    flash_write_int32(COUNTER_ADDR(1), (uint32_t)0);
    DEBUG(" Periph configured ");

    enableInterrupts();
    while(1) {
        /* Enter Halt mode*/
      switch (mcu_state) {
        case OK:
          DEBUG(" main:ok ");
            RTC_SetWakeUpCounter(flash_read_int32(INTERVAL_ADDR));
            RTC_WakeUpCmd(ENABLE); 
            //halt();
            break;
        case CONFIG:
          DEBUG(" main:config ");
            // do first configuration
            RTC_SetWakeUpCounter(1);
            RTC_WakeUpCmd(ENABLE); 
            //halt();
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

  /* Configures the RTC */
  RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);
  RTC_ITConfig(RTC_IT_WUT, ENABLE);
  RTC_SetWakeUpCounter(PUBLICATION_PERIOD);
}
void GPIO_init() {

    GPIO_Init(COUNT0_GPIO_PORT,COUNT0_GPIO_PIN, GPIO_Mode_In_FL_IT);
    //GPIO_Init(COUNT1_GPIO_PORT,COUNT1_GPIO_PIN, GPIO_Mode_In_FL_IT);
    EXTI_SetPinSensitivity (COUNT0_GPIO_PIN,EXTI_Trigger_Rising);
    GPIO_Init(BUTTON0_GPIO_PORT,BUTTON0_GPIO_PIN, GPIO_Mode_In_FL_IT);
    EXTI_SetPinSensitivity (BUTTON0_GPIO_PIN,EXTI_Trigger_Rising);
    GPIO_Init(BUTTON1_GPIO_PORT,BUTTON1_GPIO_PIN, GPIO_Mode_In_FL_IT);
    
    GPIO_Init(LED1_GPIO_PORT,LED1_GPIO_PIN, GPIO_Mode_Out_PP_Low_Fast);
    GPIO_Init(LED2_GPIO_PORT,LED2_GPIO_PIN, GPIO_Mode_Out_PP_Low_Fast);
    GPIO_Init(LED3_GPIO_PORT,LED3_GPIO_PIN, GPIO_Mode_Out_PP_Low_Fast);
#ifdef DISCOVERY_BOARD
    GPIO_Init(COUNT0_HIGH_GPIO_PORT,COUNT0_HIGH_GPIO_PIN, GPIO_Mode_Out_PP_Low_Fast);
    GPIO_WriteBit(COUNT0_HIGH_GPIO_PORT,COUNT0_HIGH_GPIO_PIN,SET);
    
    GPIO_Init(COUNT1_HIGH_GPIO_PORT,COUNT1_HIGH_GPIO_PIN, GPIO_Mode_Out_PP_Low_Fast);
    GPIO_WriteBit(COUNT1_HIGH_GPIO_PORT,COUNT1_HIGH_GPIO_PIN,SET);
    
    GPIO_Init(BUTTON1_HIGH_GPIO_PORT,BUTTON1_HIGH_GPIO_PIN, GPIO_Mode_Out_PP_Low_Fast);
    GPIO_WriteBit(BUTTON1_HIGH_GPIO_PORT,BUTTON1_HIGH_GPIO_PIN,SET);
#endif

}
void SPI_init() {
  
    //SPI_CLOCK:PB5, SPI_MOSI: PB6, SPI_MISO: PB7  
    GPIO_Init(GPIOB, GPIO_Pin_5, GPIO_Mode_Out_PP_High_Fast);  
    GPIO_Init(GPIOB, GPIO_Pin_6, GPIO_Mode_Out_PP_High_Fast);  
    GPIO_Init(GPIOB, GPIO_Pin_7, GPIO_Mode_In_PU_No_IT);  
    /* CS */  
    GPIO_Init(GPIOB , GPIO_Pin_4, GPIO_Mode_Out_PP_High_Fast);  
   
    //GPIO_WriteBit(GPIOB , GPIO_Pin_4, SET);
     
    SPI_Init(SPI1, SPI_FirstBit_MSB, SPI_BaudRatePrescaler_2, SPI_Mode_Master,  
            SPI_CPOL_Low, SPI_CPHA_1Edge,   
            SPI_Direction_2Lines_FullDuplex, SPI_NSS_Soft, 0x07); 
}
void ADC_init() {
#ifdef BARS_BOARD
    ADC_Init(ADC1, ADC_ConversionMode_Continuous, ADC_Resolution_6Bit, ADC_Prescaler_2);

    /* ADC channel used for IDD measurement */
    ADC_SamplingTimeConfig(ADC1, BATT_VOLTAGE_MEASUREMENT_CHANNEL, ADC_SamplingTime_384Cycles);
    /* Disable SchmittTrigger for ADC_Channel, to save power */
    ADC_SchmittTriggerConfig(ADC1, ADC_IDD_MEASUREMENT_CHANNEL, DISABLE);
        /* Start ADC1 Conversion using Software trigger*/
#endif
#ifdef DISCOVERY_BOARD
    ;
#endif

    
}
    



