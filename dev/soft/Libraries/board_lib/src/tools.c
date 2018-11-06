#include "tools.h"
uint8_t spi_exchange(uint8_t send) {

   
    SPI_Cmd(SPI1, ENABLE); 
    while (SPI_GetFlagStatus(SPI1, SPI_FLAG_TXE) == RESET)
    {}
    // this spi_request is informative byte
    SPI_SendData(SPI1, send);
    while (SPI_GetFlagStatus(SPI1, SPI_FLAG_RXNE) == RESET)
    {}
    uint8_t receive = SPI_ReceiveData(SPI1);
    SPI_Cmd(SPI1, DISABLE); 
    
    return receive;
    
}
void spi_send_int32(uint32_t data) {
  CLK_PeripheralClockConfig(CLK_Peripheral_SPI1,ENABLE);
    SPI_Cmd(SPI1, ENABLE); 
    uint8_t i;
    for (i = 0; i < 4; i++) {
    while (SPI_GetFlagStatus(SPI1, SPI_FLAG_TXE) == RESET)
    {}
    
    SPI_SendData(SPI1, (uint8_t)data);
    while (SPI_GetFlagStatus(SPI1, SPI_FLAG_RXNE) == RESET)
    {}
    data = data>>8;
    }
    SPI_Cmd(SPI1, DISABLE); 
    CLK_PeripheralClockConfig(CLK_Peripheral_SPI1,DISABLE);
}
uint32_t readBattADC() {
#ifdef BARS_BOARD
    CLK_PeripheralClockConfig(CLK_Peripheral_ADC1, ENABLE);
    /* Enable ADC1 */
    ADC_Cmd(ADC1, ENABLE);
    /* Enable ADC1 Channel used for IDD measurement */
    ADC_ChannelCmd(ADC1, ADC_IDD_MEASUREMENT_CHANNEL, ENABLE);

    ADC_SoftwareStartConv(ADC1);
    /* Wait until End-Of-Convertion */
    while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == 0)
    {}

    /* Get conversion value in uV */
    uint32_t ADCData = (uint32_t)((uint32_t)ADC_GetConversionValue(ADC1) *(uint32_t)ADC_CONVERT_RATIO_uV) ;
    /* Convert uV to mV */
    ADCData = ADCData / (uint32_t)1000;
    ADC_Cmd(ADC1, DISABLE);
    CLK_PeripheralClockConfig(CLK_Peripheral_ADC1, DISABLE);
    return ADCData;
#endif
#ifdef DISCOVERY_BOARD
    return BATTERY_MAX_ADC_LEVEL + 1;
#endif
}
uint32_t flash_read_int32(uint32_t addr) {
    int8_t i;
    uint32_t data=0;
    for (i=0; i <4; i++) {
        uint8_t read = FLASH_ReadByte(addr+(3-i));
        data += read<<(8*(3-i));
    }
    return data;
}
void flash_write_int32(uint32_t addr, uint32_t data) {
    /* explicit type conversion to uint8_t (= LOW(counter*0x00ff)) */
    FLASH_ProgramByte(addr,(uint8_t)data);        
    //while (FLASH_GetFlagStatus(FLASH_FLAG_EOP) == RESET)
    //{}
    FLASH_ProgramByte(addr+1,(uint8_t)(data>>8));
    //while (FLASH_GetFlagStatus(FLASH_FLAG_EOP) == RESET)
    //{}
    FLASH_ProgramByte(addr+2,(uint8_t)(data>>16));
    //while (FLASH_GetFlagStatus(FLASH_FLAG_EOP) == RESET)
    //{}
    FLASH_ProgramByte(addr+3,(uint8_t)(data>>24));
    //while (FLASH_GetFlagStatus(FLASH_FLAG_EOP) == RESET)
    //{}
}
uint16_t flash_read_int16(uint32_t addr) {
    uint32_t counter =
    (FLASH_ReadByte(addr+1))<<8
        +
    FLASH_ReadByte(addr);
    return counter;
}
void flash_write_int16(uint32_t addr, uint16_t data) {
    /* explicit type conversion to uint8_t (= LOW(counter*0x00ff)) */
    FLASH_ProgramByte(addr,(uint8_t)data);        
    //while (FLASH_GetFlagStatus(FLASH_FLAG_EOP) == RESET)
    //{}
    FLASH_ProgramByte(addr+1,(uint8_t)(data>>8));
    //while (FLASH_GetFlagStatus(FLASH_FLAG_EOP) == RESET)
    //{}
}
void send_usart(const char* message) {
  //CLK_PeripheralClockConfig(CLK_Peripheral_USART1, ENABLE);
  USART_Cmd( USART1,  ENABLE);  
  uint8_t i=0;
  while(message[i]) {
      while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET) {}
      USART_SendData8(USART1, message[i++]);
  }
  
    
  //CLK_PeripheralClockConfig(CLK_Peripheral_USART1, DISABLE);
  USART_Cmd( USART1,  DISABLE);  
}
