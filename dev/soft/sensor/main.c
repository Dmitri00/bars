#include "stm8l15x.h"
#include "stm8l15x_conf.h"
#include "board.h"
int main( void )
{
    RTC_init();
    GPIO_init();
    SPI_init();
    ADC_init();
    while(true) {
        /* Enter Halt mode*/
        halt();
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
    GPIO_Init(BUTTON0_GPIO_PORT,BUTTON0_GPIO_PIN, GPIO_Mode_In_FL_IT);
    GPIO_Init(LED1_GPIO_PORT,LED1_GPIO_PIN, GPIO_Mode_Out_PP_Low_Fast);
    GPIO_Init(LED2_GPIO_PORT,LED2_GPIO_PIN, GPIO_Mode_Out_PP_Low_Fast);
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
    ADC_Init(ADC1, ADC_ConversionMode_Continuous, ADC_Resolution_6Bit, ADC_Prescaler_2);

    /* ADC channel used for IDD measurement */
    ADC_SamplingTimeConfig(ADC1, BATT_VOLTAGE_MEASUREMENT_CHANNEL, ADC_SamplingTime_384Cycles);
    /* Disable SchmittTrigger for ADC_Channel, to save power */
    ADC_SchmittTriggerConfig(ADC1, ADC_IDD_MEASUREMENT_CHANNEL, DISABLE);
        /* Start ADC1 Conversion using Software trigger*/

    
}
    



