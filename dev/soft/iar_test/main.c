#include "board.h"
#include "stm8l15x_conf.h"

uint8_t buffer[FLASH_BLOCK_SIZE];
BoardConfig_t conf;


int main( void )         // ???????? ?????????
{
  CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);
  uint8_t fillvall=0xaa,i;
  conf.publicationPeriod = 6;
  conf.switchCounter = 0;
  buffer[0] = conf.publicationPeriod;
  buffer[1] = conf.switchCounter;
  for(i=3; i < FLASH_BLOCK_SIZE; i++)
    buffer[i] = fillvall;
  /* Unlock flash data eeprom memory */
flash_save(CONFIG,buffer);

uint8_t str[6]="Damon ";
str[0]=FLASH_ReadByte(0x0100);
  str[5]=0;
  LCD_GLASS_Init();
  LCD_GLASS_DisplayString(str);
  
  

  GPIO_Init(LED_GREEN_PORT, LED_GREEN_PIN, GPIO_Mode_Out_PP_Low_Fast);
	EXTI->CR1 = 0x0A;
	/* Init GPIO Port C PC1 pin input floating with interrupt */
	GPIOC->DDR &= 0xFD;		
	GPIOC->CR1 &= 0xFD;	
	GPIOC->CR2 |= 0x02;
enableInterrupts();


  while(1)              // ??????????? ????
  {
        
        ;
  }
}
