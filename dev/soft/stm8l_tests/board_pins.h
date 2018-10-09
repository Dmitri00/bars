#define GPIO_HIGH(a,b) a->ODR|=b
#define GPIO_LOW(a,b) a->ODR&=~b
#define GPIO_TOGGLE(a,b) a->ODR^=b

#define BUTTON_GPIO_PORT          	GPIOC
#define USER_GPIO_PIN             	GPIO_Pin_1

#define LED_GREEN_PORT             GPIOE
#define LED_GREEN_PIN              GPIO_Pin_7
  
#define LED_BLUE_PORT              GPIOC
#define LED_BLUE_PIN               GPIO_Pin_7
#define CTN_GPIO_PORT              GPIOC
#define CTN_CNTEN_GPIO_PIN         GPIO_Pin_4
#define WAKEUP_GPIO_PORT           GPIOE
#define ICC_WAKEUP_GPIO_PIN        GPIO_Pin_6
#define ICC_WAKEUP_EXTI_PIN        EXTI_Pin_3

#define SPI_PORT                   GPIOB
#define SPI_NSS_PIN                GPIO_Pin_4
#define SPI_SCK_PIN                GPIO_Pin_5
#define SPI_MOSI_PIN               GPIO_Pin_6
#define SPI_MISO_PIN               GPIO_Pin_7

#define CONFIG CONFIG_BLOCK
#define CONFIG_BLOCK 0