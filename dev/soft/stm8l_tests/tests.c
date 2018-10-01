#include "stm8l15x.h"
#include "stm8l_discovery_lcd.h"
#include "delay.h"
#include "stm8l15x_flash.h"
#include "stm8l15x_clk.h"
#include "stm8l15x_gpio.h"
/* Private typedef -----------------------------------------------------------*/
typedef enum { FAILED = 0, PASSED = !FAILED} TestStatus;
/* Private define ------------------------------------------------------------*/
#define BLOCK_OPERATION    0    /* block 0 in data eeprom memory: address is 0x1000 */
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t GBuffer[FLASH_BLOCK_SIZE];
TestStatus OperationStatus;
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