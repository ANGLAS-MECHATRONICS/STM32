#include <stm32f446xx.h>
#include <stm32f446xx_gpio.h>

void delay(uint32_t cnt){
	while(cnt)
		cnt--;
}

GPIO_handle_t LED;

void GPIO_LED_Init(){
	LED.pGPIOx = GPIOA;
	LED.GPIO_config.GPIO_Mode = GPIO_MODE_OUT;
	LED.GPIO_config.GPIO_OType = GPIO_OType_PP;
 	LED.GPIO_config.GPIO_Pin = GPIO_PIN_5;
	LED.GPIO_config.GPIO_PuPd = GPIO_PuPd_NONE;
	LED.GPIO_config.GPIO_Speed = GPIO_SPEED_LOW;

	GPIO_Init(&LED);
}

int main(void){

	GPIO_LED_Init();

    while(1){
    	GPIO_TogglePin(LED.pGPIOx, GPIO_PIN_5);
    	delay(500000);

    }
}
