#include "board_flash.h"

void flash_save(uint16_t block, uint8_t* buffer) {
  uint8_t i = 0;
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

  /* This function is executed from RAM */
  FLASH_ProgramBlock(block, FLASH_MemType_Data, FLASH_ProgramMode_Standard, buffer);
  
  /* Wait until End of high voltage flag is set*/
  while (FLASH_GetFlagStatus(FLASH_FLAG_HVOFF) == RESET)
  {}
 }
 
 
 void flash_get(uint16_t block, uint8_t* buffer) {
    uint32_t add, startadd, stopadd = 0;
    uint8_t i = 0;
    
    startadd = FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS + ((uint16_t)block * (uint16_t)FLASH_BLOCK_SIZE);
    stopadd = startadd + (uint16_t)FLASH_BLOCK_SIZE;
    for (add = startadd; add < stopadd; add++)
      buffer[i++] = FLASH_ReadByte(add);

   }