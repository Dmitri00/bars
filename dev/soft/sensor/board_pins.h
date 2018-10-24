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

/* gpio defines for button1 and counters simulation */
#define BUTTON1_HIGH_GPIO_PORT     GPIOC
#define BUTTON1_HIGH_GPIO_PIN      GPIO_Pin_6

#define COUNT0_HIGH_GPIO_PORT     GPIOD
#define COUNT0_HIGH_GPIO_PIN      GPIO_Pin_5
#define COUNT1_HIGH_GPIO_PORT     GPIOC
#define COUNT1_HIGH_GPIO_PIN      GPIO_Pin_0
/* end io simulation */

/*-------------------------*/
#define PBUTTON_GPIO_PORT          	GPIOC
#define USER_GPIO_PIN            GPIO_Pin_1

#define LED_GREEN_PORT             GPIOE
#define LED_GREEN_PIN              GPIO_Pin_7
  
#define LED_BLUE_PORT              GPIOC
#define LED_BLUE_PIN               GPIO_Pin_7

