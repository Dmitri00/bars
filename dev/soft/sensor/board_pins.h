/* Buttons */
#define BUTTON0_GPIO_PORT          	PBUTTON_GPIO_PORT
#define BUTTON0_GPIO_PIN          	GPIO_Pin_1
#define BUTTON1_GPIO_PORT          	GPIOC
#define BUTTON1_GPIO_PIN          	GPIO_Pin_7
#define BUTTON2_GPIO_PORT          	GPIOE
#define BUTTON2_GPIO_PIN          	GPIO_Pin_6

/* Counter switch */
#define COUNT0_GPIO_PORT                GPIOD
#define COUNT0_GPIO_PIN                 GPIO_Pin_6

#define COUNT1_GPIO_PORT                GPIOD
#define COUNT1_GPIO_PIN                 GPIO_Pin_7

/* LEDs */
#define LED1_GPIO_PORT                  LED_GREEN_PORT
#define LED1_GPIO_PIN                   LED_GREEN_PIN
#define LED2_GPIO_PORT                  LED_GREEN_PORT
#define LED2_GPIO_PIN                   LED_GREEN_PIN
#define LED3_GPIO_PORT                  LED_BLUE_PORT
#define LED3_GPIO_PIN                   LED_BLUE_PIN
/* unused leds*/
#define LED4_GPIO_PORT                  GPIOE
#define LED4_GPIO_PIN                   GPIO_Pin_0
#define LED5_GPIO_PORT                  GPIOE
#define LED5_GPIO_PIN                   GPIO_Pin_1
/* end unused leds */

/* SPI */
#define SPI_PORT                   GPIOB
#define SPI_NSS_PIN                GPIO_Pin_4
#define SPI_SCK_PIN                GPIO_Pin_5
#define SPI_MOSI_PIN               GPIO_Pin_6
#define SPI_MISO_PIN               GPIO_Pin_7

/* USART */
#define USART_TX_PORT              GPIOC
#define USART_TX_PIN              GPIO_Pin_3


/* ESP CHIP enable */
#define ESP_CHEN_PORT               GPIOD
#define ESP_CHEN_PIN                GPIO_Pin_3
#define ESP_SLEEP_PORT              GPIOF
#define ESP_SLEEP_PIN               GPIO_Pin_0
#define ESP_RESET_PORT              GPIOD
#define ESP_RESET_PIN               GPIO_Pin_4


/*-------------------------*/
#define PBUTTON_GPIO_PORT          	GPIOC
#define USER_GPIO_PIN            GPIO_Pin_1

#define LED_GREEN_PORT             GPIOE
#define LED_GREEN_PIN              GPIO_Pin_7
  
#define LED_BLUE_PORT              GPIOC
#define LED_BLUE_PIN               GPIO_Pin_7

