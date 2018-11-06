#include "stm8l15x.h"
#include "board.h"
#include "tests.h"
int main( void )
{
  //CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);
      
	EXTI->CR1 = 0x0A;
	/* Init GPIO Port C PC1 pin input floating with interrupt */
	GPIOC->DDR &= 0xFD;		
	GPIOC->CR1 &= 0xFD;	
	GPIOC->CR2 |= 0x02;
//enableInterrupts();
//callbacks[0] = &lcdTest;
  //lcdTest();
        spiTest();
haltTest();
  while (1) {
haltTest();
  }
  return 0;
}


