#include "stm8l15x.h"
#include "stm8l_discovery_lcd.h"
#include "delay.h"
#include "stm8l15x_flash.h"
#include "stm8l15x_clk.h"
#include "stm8l15x_gpio.h"
#include "stm8l15x_spi.h"
#include "stm8l15x_rtc.h"
#include "stm8l15x_tim4.h"
void Time_Show();
void Calendar_Init(void);
void LSE_StabTime(void);
/* Private typedef -----------------------------------------------------------*/
typedef enum { FAILED = 0, PASSED = !FAILED} TestStatus;
/* Private define ------------------------------------------------------------*/
#define BLOCK_OPERATION    0    /* block 0 in data eeprom memory: address is 0x1000 */
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t GBuffer[FLASH_BLOCK_SIZE];
TestStatus OperationStatus;
RTC_InitTypeDef   RTC_InitStr;
RTC_TimeTypeDef   RTC_TimeStr;
RTC_DateTypeDef   RTC_DateStr;
RTC_AlarmTypeDef  RTC_AlarmStr;
bool lcdTest(void) {
  LCD_GLASS_Init();
  uint8_t i;
  uint8_t test_string[7] = "LCD-OK";
  uint8_t test_char = 'X';
  for (i = 1; i <= LCD_SEGMENTS; i++) {
    LCD_GLASS_WriteChar(&test_char,0,0,i);
    delay_ms(500);
  }
  LCD_GLASS_DisplayString(test_string);
  delay_ms(5000);
  LCD_GLASS_Clear();
  LCD_GLASS_ScrollSentenceDef("**LCD is ok. Test Completed**");
  delay_ms(2000);
  LCD_GLASS_Clear();
  return 1;
}
bool btnTest_wait() {
  GPIO_Init(LED_GREEN_PORT, LED_GREEN_PIN, GPIO_Mode_Out_PP_Low_Fast);
  GPIO_Init(LED_BLUE_PORT, LED_BLUE_PIN, GPIO_Mode_Out_PP_Low_Fast);
  LCD_GLASS_ScrollSentenceDef("**Press button**");
  return 1;
}  
bool btnTest() {
  LCD_GLASS_ScrollSentenceDef("**Button is pressed**");  
  LCD_GLASS_Clear();
  delay_ms(500);
  
  GPIO_WriteBit(LED_BLUE_PORT,LED_BLUE_PIN,SET);
  GPIO_WriteBit(LED_GREEN_PORT,LED_GREEN_PIN,SET);

  
  LCD_GLASS_ScrollSentenceDef("**LED is on**");
  LCD_GLASS_Clear();
  delay_ms(500);
  
  GPIO_WriteBit(LED_BLUE_PORT,LED_BLUE_PIN,RESET);
  GPIO_WriteBit(LED_GREEN_PORT,LED_GREEN_PIN,RESET);
  LCD_GLASS_ScrollSentenceDef("**LED is off**");
  LCD_GLASS_Clear();
  delay_ms(500);
  return 1;
}  

bool flashTest() {
    uint32_t add, startadd, stopadd = 0;
  uint8_t newval = 0xAA;
  uint8_t i = 0;
  OperationStatus = PASSED;
    /* High speed internal clock prescaler */
  CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);

  /* Define flash programming Time*/
  FLASH_SetProgrammingTime(FLASH_ProgramTime_Standard);

  FLASH_Unlock(FLASH_MemType_Program);
  /* Wait until Flash Program area unlocked flag is set*/
  while (FLASH_GetFlagStatus(FLASH_FLAG_PUL) == RESET)
  {}

  /* Unlock flash data eeprom memory */
  FLASH_Unlock(FLASH_MemType_Data);
  /* Wait until Data EEPROM area unlocked flag is set*/
  while (FLASH_GetFlagStatus(FLASH_FLAG_DUL) == RESET)
  {}

  /* Fill the buffer in RAM */
  for (i = 0; i < FLASH_BLOCK_SIZE; i++)
  {
    GBuffer[i] = newval;
  }
  /* This function is executed from RAM */
  FLASH_ProgramBlock(BLOCK_OPERATION, FLASH_MemType_Data, FLASH_ProgramMode_Standard, GBuffer);
  
  /* Wait until End of high voltage flag is set*/
  while (FLASH_GetFlagStatus(FLASH_FLAG_HVOFF) == RESET)
  {}
  /* Check the programmed block */
  startadd = FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS + ((uint16_t)BLOCK_OPERATION * (uint16_t)FLASH_BLOCK_SIZE);
  stopadd = startadd + (uint16_t)FLASH_BLOCK_SIZE;
  for (add = startadd; add < stopadd; add++)
      {
        if (FLASH_ReadByte(add) != newval)
        {
          /* Error */
          OperationStatus = FAILED;
          /* OperationStatus = PASSED, if the data written/read to/from Flash program memory is correct */
          /* OperationStatus = FAILED, if the data written/read to/from Flash program memory is corrupted */
            LCD_GLASS_ScrollSentenceDef("**Flash error**"); /*FAIL: write error */            
            delay_ms(2000);
        }
      }
  /* Erase block 0 and verify it */
  /* This function is executed from RAM */
  FLASH_EraseBlock(BLOCK_OPERATION, FLASH_MemType_Data);

  /* Wait until End of high voltage flag is set*/
  while (FLASH_GetFlagStatus(FLASH_FLAG_HVOFF) == RESET)
  {}

  for (add = startadd; add < stopadd; add++)
      {
        if (FLASH_ReadByte(add) != 0x00)
        {
          /* Error */
          OperationStatus = FAILED;
          /* OperationStatus = PASSED, if the data written/read to/from Flash program memory is correct */
          /* OperationStatus = FAILED, if the data written/read to/from Flash program memory is corrupted */
            LCD_GLASS_ScrollSentenceDef("**Flash error**"); /*FAIL: write error */            
            delay_ms(2000);
        }
      }

  /* Pass */
  
  /* OperationStatus = PASSED, if the data written/read to/from Flash program memory is correct */
  /* OperationStatus = FAILED, if the data written/read to/from Flash program memory is corrupted */
  if (OperationStatus == PASSED)
  {
    LCD_GLASS_ScrollSentenceDef("**Flash OK**");          
    delay_ms(2000);
  }
}
void spiTest(void) {
/* Pcap01_SPI Periph clock enable */
   CLK_PeripheralClockConfig(CLK_Peripheral_SPI1, ENABLE);
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
      
SPI_Cmd(SPI1, ENABLE); 
  uint8_t tx;
  uint8_t errno = 0;
  for (tx=0; tx <= (uint8_t)250; tx++) {
    while (SPI_GetFlagStatus(SPI1, SPI_FLAG_TXE) == RESET)
    {}
    SPI_SendData(SPI1, tx);
    while (SPI_GetFlagStatus(SPI1, SPI_FLAG_RXNE) == RESET)
    {}
  
    errno += tx !=SPI_ReceiveData(SPI1) ? 1 : 0;
  }
  
  while (SPI_GetFlagStatus(SPI1,SPI_FLAG_TXE) == RESET) ;
  while (SPI_GetFlagStatus(SPI1,SPI_FLAG_BSY) == SET) ;
  SPI_Cmd(SPI1, DISABLE);
  CLK_PeripheralClockConfig (CLK_Peripheral_SPI1, DISABLE);
  LCD_GLASS_Init();
  if (!errno) 
    LCD_GLASS_ScrollSentenceDef("**SPI OK**");
  else
    LCD_GLASS_ScrollSentenceDef("**SPI ERROR**");
  
    
}
bool rtcTest() {
  /* Enable LSE */
  CLK_LSEConfig(CLK_LSE_ON);
  /* Wait for LSE clock to be ready */
  while (CLK_GetFlagStatus(CLK_FLAG_LSERDY) == RESET);
  /* wait for 1 second for the LSE Stabilisation */
  LSE_StabTime();

  /* Select LSE (32.768 KHz) as RTC clock source */
  CLK_RTCClockConfig(CLK_RTCCLKSource_LSE, CLK_RTCCLKDiv_1);

  CLK_PeripheralClockConfig(CLK_Peripheral_RTC, ENABLE);
  Time_Show();
}
void Time_Show() {
  /* Wait until the calendar is synchronized */
  while (RTC_WaitForSynchro() != SUCCESS);

  /* Get the current Time*/
  RTC_GetTime(RTC_Format_BCD, &RTC_TimeStr);

  /* Fill the LCDString fields with the current Time*/
  uint8_t ch = ((RTC_TimeStr.RTC_Hours & 0xF0) >> 4) + ASCII_NUM_0;
  LCD_GLASS_WriteChar(&ch,0,0,1);
  ch = (RTC_TimeStr.RTC_Hours & 0x0F) + ASCII_NUM_0;
  LCD_GLASS_WriteChar(&ch, 0,1,2);
  ch = ((RTC_TimeStr.RTC_Minutes & 0xF0) >> 4) + ASCII_NUM_0;
  LCD_GLASS_WriteChar(&ch, 0,0,3);
  ch = (RTC_TimeStr.RTC_Minutes & 0x0F) + ASCII_NUM_0;
  LCD_GLASS_WriteChar(&ch, 0,1,4);

  ch = ((RTC_TimeStr.RTC_Seconds & 0xF0) >> 4) + ASCII_NUM_0;
  LCD_GLASS_WriteChar(&ch, 0,0,5);
  ch = (RTC_TimeStr.RTC_Seconds & 0x0F) + ASCII_NUM_0;
  LCD_GLASS_WriteChar(&ch, 0,0,6);

}

void Calendar_Init(void)
{
  RTC_InitStr.RTC_HourFormat = RTC_HourFormat_24;
  RTC_InitStr.RTC_AsynchPrediv = 0x7F;
  RTC_InitStr.RTC_SynchPrediv = 0x00FF;
  RTC_Init(&RTC_InitStr);

  RTC_DateStructInit(&RTC_DateStr);
  RTC_DateStr.RTC_WeekDay = RTC_Weekday_Friday;
  RTC_DateStr.RTC_Date = 13;
  RTC_DateStr.RTC_Month = RTC_Month_May;
  RTC_DateStr.RTC_Year = 11;
  RTC_SetDate(RTC_Format_BIN, &RTC_DateStr);

  RTC_TimeStructInit(&RTC_TimeStr);
  RTC_TimeStr.RTC_Hours   = 01;
  RTC_TimeStr.RTC_Minutes = 00;
  RTC_TimeStr.RTC_Seconds = 00;
  RTC_SetTime(RTC_Format_BIN, &RTC_TimeStr);


  RTC_AlarmStructInit(&RTC_AlarmStr);
  RTC_AlarmStr.RTC_AlarmTime.RTC_Hours   = 01;
  RTC_AlarmStr.RTC_AlarmTime.RTC_Minutes = 01;
  RTC_AlarmStr.RTC_AlarmTime.RTC_Seconds = 00;
  RTC_AlarmStr.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay;
  RTC_SetAlarm(RTC_Format_BIN, &RTC_AlarmStr);

  RTC_ITConfig(RTC_IT_ALRA, ENABLE);
  RTC_AlarmCmd(ENABLE);
}

void LSE_StabTime(void)
{

  CLK_PeripheralClockConfig(CLK_Peripheral_TIM4, ENABLE);

  /* Configure TIM4 to generate an update event each 1 s */
  TIM4_TimeBaseInit(TIM4_Prescaler_16384, 123);
  /* Clear update flag */
  TIM4_ClearFlag(TIM4_FLAG_Update);

  /* Enable TIM4 */
  TIM4_Cmd(ENABLE);

  /* Wait 1 sec */
  while ( TIM4_GetFlagStatus(TIM4_FLAG_Update) == RESET );

  TIM4_ClearFlag(TIM4_FLAG_Update);

  /* Disable TIM4 */
  TIM4_Cmd(DISABLE);

  CLK_PeripheralClockConfig(CLK_Peripheral_TIM4, DISABLE);
}
void haltTest() {
  /* Configures the LCD GLASS relative GPIO port IOs and LCD peripheral */
  LCD_GLASS_Init();

  /* Enable RTC clock */
  CLK_PeripheralClockConfig(CLK_Peripheral_RTC, ENABLE);

  /* Configures the RTC */
  RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);
  RTC_ITConfig(RTC_IT_WUT, ENABLE);

  /* Enable general Interrupt*/
  enableInterrupts();


  uint8_t i = 3;
  while (i-- > 0)
  {
    /*Display " STM8L " string on LCD glass in scrolling mode*/
    LCD_GLASS_DisplayString("STM8L");
    delay_ms(3000);
    /* Configures the LCD Blink mode*/
    LCD_BlinkConfig(LCD_BlinkMode_AllSEG_AllCOM, LCD_BlinkFrequency_Div64);

    /* Display "  HALT " string on LCD glass before entering in Halt mode*/
    LCD_GLASS_DisplayString("LPMODE");

    /* Waiting Delay */
    delay_ms(5000);

    /* Configures the LCD Blink mode*/
    LCD_BlinkConfig(LCD_BlinkMode_Off, LCD_BlinkFrequency_Div64);

    /* RTC will wake-up from halt every 20second*/
    RTC_SetWakeUpCounter(10);
    RTC_WakeUpCmd(ENABLE);

    /* Display "LP MODE" string on LCD glass before entering in Halt mode*/
    LCD_GLASS_DisplayString("HALT");

    /* Enter Halt mode*/
    halt();

    RTC_WakeUpCmd(DISABLE);
  }
}