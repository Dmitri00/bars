/**
  ******************************************************************************
  * @file    GPIO/GPIO_Toggle/stm8l15x_it.c
  * @author  MCD Application Team
  * @version V1.5.2
  * @date    30-September-2014
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm8l15x_it.h"
#include "stm8l15x_conf.h"
#include "board.h"


/** @addtogroup STM8L15x_StdPeriph_Examples
  * @{
  */

/** @addtogroup GPIO_Toggle
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/*! Check whatever it is needed to work with i-th connected counter */
#define IS_ACTIVE_COUNTER(x) FLASH_ReadByte(COUNTERS_STATE_ADDR)&(0x01<<x) == 1
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
//* synchronization object to prevent flash io conflicts among handlers
CriticalSection_t flash_critsection = FREE;
uint8_t cb_num = 0;
CounterPacket_t packet;
uint8_t raw_packet[PACKET_SIZE];
/* Private function prototypes -----------------------------------------------*/
/*! Shortcut for measuring voltage from battery source */
uint32_t readBattADC();
/* Private functions ---------------------------------------------------------*/
uint32_t readBattADC() {
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
    return ADCData;
}
/* Public functions ----------------------------------------------------------*/

#ifdef _COSMIC_
/**
  * @brief Dummy interrupt routine
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(NonHandledInterrupt, 0)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}
#endif

/**
  * @brief TRAP interrupt routine
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER_TRAP(TRAP_IRQHandler)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}
/**
  * @brief FLASH Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(FLASH_IRQHandler, 1)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}
/**
  * @brief DMA1 channel0 and channel1 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(DMA1_CHANNEL0_1_IRQHandler, 2)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}
/**
  * @brief DMA1 channel2 and channel3 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(DMA1_CHANNEL2_3_IRQHandler, 3)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}
/**
  * @brief RTC wake up from halt (sleep) mode routine
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(RTC_CSSLSE_IRQHandler, 4)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
    /* disable wakeup counter and IT flag */
    RTC_WakeUpCmd(DISABLE);
    RTC_ClearITPendingBit(RTC_IT_WUT);
    
    /* prepare data packet for sending */
    while( flash_critsection == LOCKED) 
        {}
    flash_critsection = LOCKED;
    
    /* FLASH IO routine */
    FLASH_Unlock(FLASH_MemType_Data);
    /* Wait until Data EEPROM area unlocked flag is set*/
    while (FLASH_GetFlagStatus(FLASH_FLAG_DUL) == RESET)
    {}
    /* same sending routine for every counter */
    uint8_t i;
    for (i = 0; i < COUNTER_NUM; i++) 
        if (IS_ACTIVE_COUNTER(i)) {
            /* read 16-bit counter */ 
            /* counter data structures defined at board.h via #defines */
            packet.counter = 
              (FLASH_ReadByte(COUNTER_ADDR(i)+1))<<8
                + 
              FLASH_ReadByte(COUNTER_ADDR(i));
            packet.id = (FLASH_ReadByte(COUNTER0_ID_ADDR(i)+1))<<8
                + 
              FLASH_ReadByte(COUNTER0_ID_ADDR(i));
            /* Add to the packet info about battery level to show it on the webser screen */
            uint32_t battVoltage = readBattADC();
            packet.battery_state = battVoltage > BATTERY_HIGH_ADC_LEVEL ? HIGH : /* High battery level */
              ( battVoltage > BATTERY_MID_ADC_LEVEL ? MID : /* Mid battery level */
                  LOW );                                    /* Low battery level */
            memcpy(raw_packet, packet, PACKET_SIZE);
            //send packet via spi to esp
            
            //erase counter
            FLASH_WriteByte(COUNTER_ADDR(i),0);
            FLASH_WriteByte(COUNTER_ADDR(i)+1,0);
        }
    FLASH_Lock(FLASH_MemType_Data);
    /* prepare for sleep-mode */
    RTC_SetWakeUpCounter(PUBLICATION_PERIOD);
    RTC_WakeUpCmd(ENABLE);   
    
}
/**
  * @brief External IT PORTE/ PORTF and PVD Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTIE_F_PVD_IRQHandler, 5)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief External IT PORTB / PORTG Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTIB_G_IRQHandler, 6)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief External IT PORTD /PORTH Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTID_H_IRQHandler, 7)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief External IT PIN0 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI0_IRQHandler, 8)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief External IT PIN1 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI1_IRQHandler,9)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */

  EXTI_ClearITPendingBit(EXTI_IT_Pin1);
  
}

/**
  * @brief External IT PIN2 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI2_IRQHandler, 10)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief External IT PIN3 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI3_IRQHandler, 11)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief External IT PIN4 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI4_IRQHandler, 12)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief External IT PIN5 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI5_IRQHandler, 13)
{

    if(GPIO_ReadInputDataBit(BUTTON0_GPIO_PORT,BUTTON0_GPIO_PIN) == SET) {        
        
        if (readBattADC() > BATTERY_HIGH_ADC_LEVEL) {
            GPIO_WriteBit(LED1_GPIO_PORT,LED1_GPIO_PIN, SET);//1
            delay_um(200);
            GPIO_ToggleBits(LED1_GPIO_PORT,LED1_GPIO_PIN);//0
            delay_um(200);
            GPIO_ToggleBits(LED1_GPIO_PORT,LED1_GPIO_PIN);//1
            delay_um(200);
            GPIO_ToggleBits(LED1_GPIO_PORT,LED1_GPIO_PIN);//0
            delay_um(200);
            GPIO_ToggleBits(LED1_GPIO_PORT,LED1_GPIO_PIN);//1
            delay_um(200);
            GPIO_ToggleBits(LED1_GPIO_PORT,LED1_GPIO_PIN);//0
        }
        else {
            GPIO_WriteBit(LED2_GPIO_PORT,LED2_GPIO_PIN, SET);//1
            delay_um(200);
            GPIO_ToggleBits(LED2_GPIO_PORT,LED2_GPIO_PIN);//0
            delay_um(200);
            GPIO_ToggleBits(LED2_GPIO_PORT,LED2_GPIO_PIN);//1
            delay_um(200);
            GPIO_ToggleBits(LED2_GPIO_PORT,LED2_GPIO_PIN);//0
            delay_um(200);
            GPIO_ToggleBits(LED2_GPIO_PORT,LED2_GPIO_PIN);//1
            delay_um(200);
            GPIO_ToggleBits(LED2_GPIO_PORT,LED2_GPIO_PIN);//0
        }


  }
  EXTI_ClearITPendingBit(EXTI_IT_Pin5);
}

/**
  * @brief Interrupt routine for input signal from COUNT0.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI6_IRQHandler, 14)
{
    
    if(GPIO_ReadInputDataBit(COUNT0_GPIO_PORT,COUNT0_GPIO_PIN) == SET) {
        /* prevent flash io conflict with sending packet routine */
        while( flash_critsection == LOCKED) 
        {}
        flash_critsection = LOCKED;
        /* FLASH IO routine */
        FLASH_Unlock(FLASH_MemType_Data);
        /* Wait until Data EEPROM area unlocked flag is set*/
        while (FLASH_GetFlagStatus(FLASH_FLAG_DUL) == RESET)
        {}
        /* if counter0 is active (= first bit in flash[COUNTERS_STATE_ADDR] is set) */
        if(IS_ACTIVE_COUNTER(0)) {
            /* read 16 counter */
            uint16_t counter = 
              (FLASH_ReadByte(COUNTER_ADDR(0)+1))<<8
                + 
              FLASH_ReadByte(COUNTER_ADDR(0));
            /* update counter */
            counter++;
            /* to explicit type conversion to uint8_t (= LOW(counter*0x00ff)) */
            FLASH_ProgramByte(COUNTER_ADDR(0),(uint8_t)counter);        
            while (FLASH_GetFlagStatus(FLASH_FLAG_EOP) == RESET)
            {}
            FLASH_ProgramByte(COUNTER_ADDR(0)+1,(uint8_t)(counter>>8));
            FLASH_Lock(FLASH_MemType_Data);    
            
        }
        /* free flash */
        flash_critsection = FREE;
        
    }
    EXTI_ClearITPendingBit(EXTI_IT_Pin6);
}

/**
  * @brief External IT PIN7 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI7_IRQHandler, 15)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  if(GPIO_ReadInputDataBit(COUNT1_GPIO_PORT,COUNT1_GPIO_PIN) == SET) {
        /* prevent flash io conflict with sending packet routine */
        while( flash_critsection == LOCKED) 
        {}
        flash_critsection = LOCKED;
        /* FLASH IO routine */
        FLASH_Unlock(FLASH_MemType_Data);
        /* Wait until Data EEPROM area unlocked flag is set*/
        while (FLASH_GetFlagStatus(FLASH_FLAG_DUL) == RESET)
        {}
        /* if counter0 is active (= first bit in flash[COUNTERS_STATE_ADDR] is set) */
        if(IS_ACTIVE_COUNTER(1)) {
            /* read 16 counter */
            uint16_t counter = 
              (FLASH_ReadByte(COUNTER_ADDR(1)+1))<<8
                + 
              FLASH_ReadByte(COUNTER_ADDR(1));
            /* update counter */
            counter++;
            /* to explicit type conversion to uint8_t (= LOW(counter*0x00ff)) */
            FLASH_ProgramByte(COUNTER_ADDR(1),(uint8_t)counter);        
            while (FLASH_GetFlagStatus(FLASH_FLAG_EOP) == RESET)
            {}
            FLASH_ProgramByte(COUNTER_ADDR(1)+1,(uint8_t)(counter>>8));
            FLASH_Lock(FLASH_MemType_Data);   
        }
        /* free flash */
        flash_critsection = FREE;
        EXTI_ClearITPendingBit(EXTI_IT_Pin7);
}
/**
  * @brief LCD /AES Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(LCD_AES_IRQHandler, 16)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}
/**
  * @brief CLK switch/CSS/TIM1 break Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(SWITCH_CSS_BREAK_DAC_IRQHandler, 17)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief ADC1/Comparator Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(ADC1_COMP_IRQHandler, 18)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief TIM2 Update/Overflow/Trigger/Break /USART2 TX Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TIM2_UPD_OVF_TRG_BRK_USART2_TX_IRQHandler, 19)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief Timer2 Capture/Compare / USART2 RX Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TIM2_CC_USART2_RX_IRQHandler, 20)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}


/**
  * @brief Timer3 Update/Overflow/Trigger/Break Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TIM3_UPD_OVF_TRG_BRK_USART3_TX_IRQHandler, 21)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}
/**
  * @brief Timer3 Capture/Compare /USART3 RX Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TIM3_CC_USART3_RX_IRQHandler, 22)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}
/**
  * @brief TIM1 Update/Overflow/Trigger/Commutation Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TIM1_UPD_OVF_TRG_COM_IRQHandler, 23)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}
/**
  * @brief TIM1 Capture/Compare Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TIM1_CC_IRQHandler, 24)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief TIM4 Update/Overflow/Trigger Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TIM4_UPD_OVF_TRG_IRQHandler, 25)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}
/**
  * @brief SPI1 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(SPI1_IRQHandler, 26)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief USART1 TX / TIM5 Update/Overflow/Trigger/Break Interrupt  routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(USART1_TX_TIM5_UPD_OVF_TRG_BRK_IRQHandler, 27)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief USART1 RX / Timer5 Capture/Compare Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(USART1_RX_TIM5_CC_IRQHandler, 28)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief I2C1 / SPI2 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(I2C1_SPI2_IRQHandler, 29)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @}
  */

/**
  * @}
  */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/