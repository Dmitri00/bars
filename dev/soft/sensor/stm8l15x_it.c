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
#include "tools.h"
#include "string.h"

/** @addtogroup STM8L15x_StdPeriph_Examples
  * @{
  */

/** @addtogroup GPIO_Toggle
  * @{
  */
// global mcu state variable, defined in main.c
extern MCUState_t mcu_state;
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
//* synchronization object to prevent flash io conflicts among handlers


    uint16_t raw_response;

/* Private function prototypes -----------------------------------------------*/
/*! Makes 3 trials to send spi_request
 * @retval first interval_packet with ready = 1, 0 after 3 unsuccessefull trials
*/
uint16_t get_spi_response(uint8_t rw, uint8_t trials);
/*! Shortcut for measuring voltage from battery source */
BatteryState_t convertBattState(uint32_t);
void* my_memcpy(void* dest, const void* src, int count);
/* Private functions ---------------------------------------------------------*/

BatteryState_t convertBattState(uint32_t voltage) {
  BatteryState_t state;
  if ( voltage >= BATTERY_HIGH_ADC_LEVEL)
    state = HIGH;
  else if (voltage >= BATTERY_MID_ADC_LEVEL)
    state = MID;
  else
    state = LOW;
  return state;
}
uint16_t get_spi_response(uint8_t rw, uint8_t trials) {

    SPIRequest_t request_struct;
    request_struct.rw = rw;
    request_struct.battery = (uint8_t)convertBattState(readBattADC());
    uint8_t raw_request;
    // convert bit field structure to raw bytes
    memcpy(&raw_request,&request_struct, 1);
    //send request to ESP
    uint16_t response = 0;
    uint8_t i;
    uint8_t success_flag = 0;
    for (i = 0; i < trials; i++) {
        // exchange first byte
        response = spi_exchange(raw_request)<<8;
        // exchange second byte
        // second spi_request byte is for filling
        response += spi_exchange(raw_request);
#ifdef BARS_BOARD
        if (INTERVAL_PACKET_RD(response) != 0) {
            success_flag = 1;
            break;
        }
        else
            delay_ms(SPI_RESP_TIMEOUT*1000);
#endif
#ifdef DISCOVERY_BOARD
        break;
#endif
   }
#ifdef BARS_BOARD
   return success_flag*response;
#endif
#ifdef DISCOVERY_BOARD
    return 0x1000;
#endif
}
    void* my_memcpy(void* dest, const void* src, int count) {
        char* dst8 = (char*)dest;
        char* src8 = (char*)src;

        while (count--) {
            *dst8++ = *src8++;
        }
        return dest;
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

    /* disable wakeup counter and IT flag */
  RTC_ClearITPendingBit(RTC_IT_WUT);
  RTC_WakeUpCmd(DISABLE); 
  //turn ESP on
  GPIO_WriteBit(ESP_CHEN_PORT,ESP_CHEN_PIN,SET);
  delay_ms(100);
    
    /* FLASH IO routine */
    FLASH_Unlock(FLASH_MemType_Data);
    /* Wait until Data EEPROM area unlocked flag is set*/
    while (FLASH_GetFlagStatus(FLASH_FLAG_DUL) == RESET)
    {}

    switch(mcu_state) {
        case CONFIG:
DEBUG("config");
            raw_response = get_spi_response(1,3);            
            if (raw_response != 0) {
                //memcpy(raw,&raw_response, sizeof(interval_packet));
                //interval_packet_p = (void*)raw;
                // Client sets publication interval in hours, so convert it to seconds
#ifdef BARS_BOARD
                uin32_t interval = (uint32_t)INTERVAL_PACKET_INTERVAL(raw_response) * 60 * 60
#endif
#ifdef DISCOVERY_BOARD
                uint32_t interval = (uint32_t)30;
#endif
                flash_write_int32((uint32_t)INTERVAL_ADDR,interval);
                RTC_SetWakeUpCounter(interval);
                mcu_state = OK;
                DEBUG(" -> OK");
            }
            else {
                mcu_state = ERROR_STATE;
            }
            break;
        case OK:
          DEBUG("OK");
            uint8_t i;    
            get_spi_response(0,1);
            uint32_t counters[2] = {0};
            for (i = 0; i < COUNTER_NUM; i++) {
                /* read 32-bit counter */ 
                /* counter data structures defined at board.h via #defines */
                counters[i] = flash_read_int32((uint32_t)COUNTER_ADDR(i));  
#ifdef BARS_BOARD
                spi_send_int32(counters[i]);
#endif
               GPIO_WriteBit(LED3_GPIO_PORT,LED3_GPIO_PIN, SET);//1
            delay_ms(200);
            GPIO_ToggleBits(LED3_GPIO_PORT,LED3_GPIO_PIN);//0
            delay_ms(200);
            }
            raw_response = get_spi_response(0,3);

            if (raw_response != 0) {
                  //memcpy(&interval_packet,&raw_response, sizeof(raw_response));
              uint16_t c = INTERVAL_PACKET_RD(raw_response);
                      if (c != 0) {
                        for (i = 0; i < COUNTER_NUM; i++) {
                            uint32_t updated_counter = flash_read_int32((uint32_t)COUNTER_ADDR(i));
                            updated_counter -= counters[i];
                            flash_write_int32((uint32_t)COUNTER_ADDR(i), updated_counter);
                        }
                      }
            }
            else {
                mcu_state = ERROR_STATE;
            }
            break;
        default:
            break;
    }
    //turn ESP off
    GPIO_WriteBit(ESP_CHEN_PORT,ESP_CHEN_PIN,RESET);
    RTC_WakeUpCmd(ENABLE); 
    //FLASH_Lock(FLASH_MemType_Data);



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
//delay_ms(300);
//    if(GPIO_ReadInputDataBit(COUNT0_GPIO_PORT,COUNT0_GPIO_PIN) != RESET) {
//        /* prevent flash io conflict with sending packet routine */
//DEBUG("count0+1");
//        /* FLASH IO routine */
//        FLASH_Unlock(FLASH_MemType_Data);
//        /* Wait until Data EEPROM area unlocked flag is set*/
//        while (FLASH_GetFlagStatus(FLASH_FLAG_DUL) == RESET)
//        {}
//        /* if counter0 is active (= first bit in flash[COUNTERS_STATE_ADDR] is set) */
//        /* read 32 counter */
//        uint32_t counter = flash_read_int32((uint32_t)COUNTER_ADDR(0));
//        /* update counter */
//        counter++;
//        flash_write_int32((uint32_t)COUNTER_ADDR(0), counter);
//        FLASH_Lock(FLASH_MemType_Data); 
//            GPIO_WriteBit(LED1_GPIO_PORT,LED1_GPIO_PIN, SET);//1
//            delay_ms(200);
//            GPIO_ToggleBits(LED1_GPIO_PORT,LED1_GPIO_PIN);//0
//            delay_ms(200);
   
//            
//        
//    }
    EXTI_ClearITPendingBit(EXTI_IT_Pin0);
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
//DEBUG("Battery info");
  

    if(GPIO_ReadInputDataBit (PBUTTON_GPIO_PORT,BUTTON0_GPIO_PIN) != RESET) {        
        //DEBUG("Battery high");
        if (readBattADC() >= BATTERY_HIGH_ADC_LEVEL) {
          
            GPIO_WriteBit(LED1_GPIO_PORT,LED1_GPIO_PIN, SET);//1
            delay_ms(200);
            GPIO_ToggleBits(LED1_GPIO_PORT,LED1_GPIO_PIN);//0
            delay_ms(200);
            #ifdef BARS_BOARD
            GPIO_ToggleBits(LED1_GPIO_PORT,LED1_GPIO_PIN);//1
            delay_ms(200);
            GPIO_ToggleBits(LED1_GPIO_PORT,LED1_GPIO_PIN);//0
            delay_ms(200);
            GPIO_ToggleBits(LED1_GPIO_PORT,LED1_GPIO_PIN);//1
            delay_ms(200);
            GPIO_ToggleBits(LED1_GPIO_PORT,LED1_GPIO_PIN);//0
            #endif
        }
        else {
          //DEBUG("battery low");
            GPIO_WriteBit(LED2_GPIO_PORT,LED2_GPIO_PIN, SET);//1
            delay_ms(200);
            #ifdef BARS_BOARD
            GPIO_ToggleBits(LED2_GPIO_PORT,LED2_GPIO_PIN);//0
            delay_ms(200);
            GPIO_ToggleBits(LED2_GPIO_PORT,LED2_GPIO_PIN);//1
            delay_ms(200);
            GPIO_ToggleBits(LED2_GPIO_PORT,LED2_GPIO_PIN);//0
            delay_ms(200);
            GPIO_ToggleBits(LED2_GPIO_PORT,LED2_GPIO_PIN);//1
            delay_ms(200);
            GPIO_ToggleBits(LED2_GPIO_PORT,LED2_GPIO_PIN);//0
            #endif
        }


  }
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
  * @brief Indicate battery voltage level: toggle LED1 if HIGH, else - LED2 on 
  * BUTTON0
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI5_IRQHandler, 13)
{
//DEBUG("Battery info");
    if(GPIO_ReadInputDataBit (BUTTON0_GPIO_PORT,BUTTON0_GPIO_PIN) != RESET) {        
        //DEBUG("Battery high");
        if (readBattADC() >= BATTERY_HIGH_ADC_LEVEL) {
          
            GPIO_WriteBit(LED1_GPIO_PORT,LED1_GPIO_PIN, SET);//1
            delay_ms(200);
            GPIO_ToggleBits(LED1_GPIO_PORT,LED1_GPIO_PIN);//0
            delay_ms(200);
            #ifdef BARS_BOARD
            GPIO_ToggleBits(LED1_GPIO_PORT,LED1_GPIO_PIN);//1
            delay_ms(200);
            GPIO_ToggleBits(LED1_GPIO_PORT,LED1_GPIO_PIN);//0
            delay_ms(200);
            GPIO_ToggleBits(LED1_GPIO_PORT,LED1_GPIO_PIN);//1
            delay_ms(200);
            GPIO_ToggleBits(LED1_GPIO_PORT,LED1_GPIO_PIN);//0
            #endif
        }
        else {
          //DEBUG("battery low");
            GPIO_WriteBit(LED2_GPIO_PORT,LED2_GPIO_PIN, SET);//1
            delay_ms(200);
            #ifdef BARS_BOARD
            GPIO_ToggleBits(LED2_GPIO_PORT,LED2_GPIO_PIN);//0
            delay_ms(200);
            GPIO_ToggleBits(LED2_GPIO_PORT,LED2_GPIO_PIN);//1
            delay_ms(200);
            GPIO_ToggleBits(LED2_GPIO_PORT,LED2_GPIO_PIN);//0
            delay_ms(200);
            GPIO_ToggleBits(LED2_GPIO_PORT,LED2_GPIO_PIN);//1
            delay_ms(200);
            GPIO_ToggleBits(LED2_GPIO_PORT,LED2_GPIO_PIN);//0
            #endif
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
    delay_ms(100);
    if(GPIO_ReadInputDataBit(COUNT0_GPIO_PORT,COUNT0_GPIO_PIN) != RESET) {
        /* prevent flash io conflict with sending packet routine */
DEBUG("count0+1");
        /* FLASH IO routine */
        FLASH_Unlock(FLASH_MemType_Data);
        /* Wait until Data EEPROM area unlocked flag is set*/
        while (FLASH_GetFlagStatus(FLASH_FLAG_DUL) == RESET)
        {}
        /* if counter0 is active (= first bit in flash[COUNTERS_STATE_ADDR] is set) */
        /* read 32 counter */
        uint32_t counter = flash_read_int32((uint32_t)COUNTER_ADDR(0));
        /* update counter */
        counter++;
        flash_write_int32((uint32_t)COUNTER_ADDR(0), counter);
        FLASH_Lock(FLASH_MemType_Data);   
            GPIO_WriteBit(LED1_GPIO_PORT,LED1_GPIO_PIN, SET);//1
            delay_ms(200);
            GPIO_ToggleBits(LED1_GPIO_PORT,LED1_GPIO_PIN);//0
            delay_ms(200);
 
            
        
    }
    

    EXTI_ClearITPendingBit(EXTI_IT_Pin6);

}

/**
  * @brief Interrupt handler for count1 on PD[7] and ClearButton (BUTTON1) on PC[7]
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI7_IRQHandler, 15)
{
  delay_ms(100);
   if(GPIO_ReadInputDataBit(COUNT1_GPIO_PORT,COUNT1_GPIO_PIN) != RESET) {
      DEBUG("count1+1");
        /* FLASH IO routine */
        FLASH_Unlock(FLASH_MemType_Data);
        /* Wait until Data EEPROM area unlocked flag is set*/
        while (FLASH_GetFlagStatus(FLASH_FLAG_DUL) == RESET)
        {}
        /* if counter1 is active (= first bit in flash[COUNTERS_STATE_ADDR] is set) */
        uint32_t counter = flash_read_int32((uint32_t)COUNTER_ADDR(1));
        /* update counter */
        counter++;
        flash_write_int32((uint32_t)COUNTER_ADDR(1), counter);
        
    }
   else  if (GPIO_ReadInputDataBit(BUTTON1_GPIO_PORT,BUTTON1_GPIO_PIN) != RESET) {
        mcu_state = CONFIG;
        
        DEBUG("->config");
    }

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