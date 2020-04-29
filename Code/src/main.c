#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

xQueueHandle SendData;

void GPIOInit(void);
void USART1Init(void);
void SPI1Init(void);
void SPI_Tx(uint8_t adress, uint8_t data);
int8_t SPI_Rx(uint8_t adress);
void sendDataUSART(int data);
void writeLine(char *string);

void vTaskUSART1(void *arg);
void vTaskSPI1(void *arg);

int main() {

	GPIOInit();
	USART1Init();
	SPI1Init();
	
	SendData = xQueueCreate(10, sizeof(uint16_t));
	
	xTaskCreate(vTaskUSART1, "USATR1", 32, NULL, 1, NULL);
	xTaskCreate(vTaskSPI1, "SPI1", 32, NULL, 1, NULL);
	
	vTaskStartScheduler();
	
	while(1) {

	}
}

void vTaskUSART1(void *arg) {

	int16_t data_x;

	while(1) {

		
		if (uxQueueMessagesWaiting(SendData) != 0){

			xQueueReceive(SendData, &data_x, 0);
			writeLine("x = ");
			sendDataUSART(data_x);

			xQueueReceive(SendData, &data_x, 0);
			writeLine("y = ");
			sendDataUSART(data_x);

			writeLine("\r\n");	
		}
	}
	
}

void vTaskSPI1(void *arg) {
	
	int16_t x,y;
	
	for(int i=0;i<1000000;i++);

	SPI_Tx(0x20, 0x63);

	while(1)
	{
	
		x = SPI_Rx(0x29);
		y = SPI_Rx(0x2B);

		if (x < -20) GPIOD->ODR |= GPIO_ODR_ODR_12;
		else GPIOD->ODR &= ~GPIO_ODR_ODR_12;
		if (x > 20) GPIOD->ODR |= GPIO_ODR_ODR_14;
		else GPIOD->ODR &= ~GPIO_ODR_ODR_14;

		if (y < -20) GPIOD->ODR |= GPIO_ODR_ODR_15;
		else GPIOD->ODR &= ~GPIO_ODR_ODR_15;
		if (y > 20) GPIOD->ODR |= GPIO_ODR_ODR_13;
		else GPIOD->ODR &= ~GPIO_ODR_ODR_13;

		xQueueSend(SendData, &x, 0);
		xQueueSend(SendData, &y, 0);
		vTaskDelay(500);
	}
}

void sendDataUSART(int data) {
	
	int temp;

	if (data < 0) {
		temp = data * (-1);
		writeLine("-");
	} else temp = data;

	char text[3];
	text[1] = temp % 10 + '0';
	text[0] = temp/10 + '0';
	writeLine(text);
	writeLine("  ");

}

void GPIOInit(void) {

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;

	GPIOD->MODER |= GPIO_MODER_MODER12_0;
	GPIOD->OTYPER &= ~GPIO_OTYPER_OT_12;
	GPIOD->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR12_0;
	GPIOD->PUPDR |= GPIO_PUPDR_PUPDR12_0;

	GPIOD->MODER |= GPIO_MODER_MODER13_0;
	GPIOD->OTYPER &= ~GPIO_OTYPER_OT_13;
	GPIOD->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR13_0;
	GPIOD->PUPDR |= GPIO_PUPDR_PUPDR13_0;

	GPIOD->MODER |= GPIO_MODER_MODER14_0;
	GPIOD->OTYPER &= ~GPIO_OTYPER_OT_14;
	GPIOD->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR14_0;
	GPIOD->PUPDR |= GPIO_PUPDR_PUPDR14_0;

	GPIOD->MODER |= GPIO_MODER_MODER15_0;
	GPIOD->OTYPER &= ~GPIO_OTYPER_OT_15;
	GPIOD->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR15_0;
	GPIOD->PUPDR |= GPIO_PUPDR_PUPDR15_0;

}

void USART1Init(void) {

	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

	GPIOA->MODER |= GPIO_MODER_MODER2_1;
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT_2;
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR2_0;
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR2_0;
	GPIOA->AFR[0] |= 0x700;

	GPIOA->MODER &= ~GPIO_MODER_MODER3;
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR3_0;
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR3;
		
	USART2->BRR = 0x1117; 
	USART2->CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;

}

void SPI1Init(void) {

	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOEEN;

	
	GPIOA->MODER |= GPIO_MODER_MODER5_1 | GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1;
	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT_5 | GPIO_OTYPER_OT_6 | GPIO_OTYPER_OT_7);
	GPIOA->AFR[0] |= 0x55500000;

	GPIOE->MODER |= GPIO_MODER_MODER3_0;
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT_3;

	SPI1->CR1 |= SPI_CR1_CPHA | SPI_CR1_CPOL | SPI_CR1_MSTR | SPI_CR1_BR_1 | SPI_CR1_SSI | SPI_CR1_SSM;

	GPIOE->ODR |= GPIO_ODR_ODR_3;
	SPI1->CR1 |= SPI_CR1_SPE;
}


void SPI_Tx(uint8_t address, uint8_t data) {

	GPIOE->ODR &= ~GPIO_ODR_ODR_3;

	while(!(SPI1->SR & SPI_SR_TXE));
	SPI1->DR = address;
	while(!(SPI1->SR & SPI_SR_RXNE));
	SPI1->DR;

	while(!(SPI1->SR & SPI_SR_TXE));
	SPI1->DR = data;
	while(!(SPI1->SR & SPI_SR_RXNE));
	SPI1->DR;

	GPIOE->ODR |= GPIO_ODR_ODR_3;
  
}

int8_t SPI_Rx(uint8_t address) {

	GPIOE->ODR &= ~GPIO_ODR_ODR_3;

	address |= 0x80;

	while(!(SPI1->SR & SPI_SR_TXE));
	SPI1->DR = address;
	while(!(SPI1->SR & SPI_SR_RXNE));
	SPI1->DR;

	while(!(SPI1->SR & SPI_SR_TXE));
	SPI1->DR = 0x00;
	while(!(SPI1->SR & SPI_SR_RXNE));

	GPIOE->ODR |= GPIO_ODR_ODR_3;
	return SPI1->DR;
}

void writeLine(char *string) {
		int i = 0;

    while(string[i])
    {
				while (!(USART2->SR & USART_SR_TC));
				USART2->DR = string[i];
        i++;
    }
}


