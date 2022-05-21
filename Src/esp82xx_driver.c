#include "esp82xx_driver.h"



#define GPIOAEN				(1U<<0)
#define UART2EN				(1U<<17)
#define UART1EN				(1U<<4)


#define CR1_UE				(1U<<13)
#define SYS_FREQ			16000000
#define APB1_CLK			SYS_FREQ
#define APB2_CLK			SYS_FREQ


#define UART_BAUDRATE		115200
#define CR1_TE				(1U<<3)
#define CR1_RE				(1U<<2)

#define SYSTICK_LOAD_VAL		16000
#define CTRL_ENABLE				(1U<<0)
#define CTRL_CLKSRC				(1U<<2)
#define CTRL_COUNTFLAG			(1U<<16)






static uint16_t compute_uart_bd(uint32_t periph_clk, uint32_t baudrate);



int __io_putchar(int ch)
{
	debug_uart_write(ch);
	return ch;
}

void debug_uart_init(void)
{
	/*Enable clock access to UART pins' GPIO port (PORT A)*/
	RCC->AHB1ENR |= GPIOAEN;

	/*Set PA2 mode to alternate function mode*/
	GPIOA->MODER &=~(1U<<4);
	GPIOA->MODER |= (1U<<5);

	/*Set PA3 mode to alternate function mode*/
	GPIOA->MODER &=~(1U<<6);
	GPIOA->MODER |= (1U<<7);

	/*Set PA2 alternate function type to UART_TX (AF07)*/
	GPIOA->AFR[0] |=(1U<<8);
	GPIOA->AFR[0] |=(1U<<9);
	GPIOA->AFR[0] |=(1U<<10);
	GPIOA->AFR[0] &=~(1U<<11);

	/*Set PA3 alternate function type to UART_RX (AF07)*/
	GPIOA->AFR[0] |=(1U<<12);
	GPIOA->AFR[0] |=(1U<<13);
	GPIOA->AFR[0] |=(1U<<14);
	GPIOA->AFR[0] &=~(1U<<15);

	/************Configure UART module*****************/

	/*Enable clock access to UART module*/
	RCC->APB1ENR |= UART2EN;

	/*Disable UART module*/
	USART2->CR1 &=~CR1_UE;

	/*Set UART baudrate*/
	USART2->BRR =  compute_uart_bd(APB1_CLK,UART_BAUDRATE);

	/*Set transfer direction*/
	USART2->CR1 = (CR1_TE | CR1_RE);

	/*Enable interrupt in the NVIC*/
	NVIC_EnableIRQ(USART2_IRQn);

	/*Enable UART module*/
	USART2->CR1 |= CR1_UE;


}


/*Pinout :
 * UART Module     :  	UART1
 * UART Pins   	   : 	PA9 = TX, PA10 = RX
 * esp pin		    	stm32f4 pin
 * --------------------------
 * ESP82XX RS  Pin : 	PA8
 * ESP82XX EN  Pin :	3.3V
 * ESP82XX IO1 Pin :	3.3V
 * ESP82XX IO2 Pin :	3.3V
 * ESP82XX VCC Pin :	3.3V
 * ESP82XX GND Pin :	GND
 * ESP82XX TX Pin  :	PA10(RX)
 * ESP82XX RX Pin  :	PA9(TX)
 * */


void esp_rs_pin_init(void)
{
	/*Enable clock access to GPIOA*/
	RCC->AHB1ENR |= GPIOAEN;

	/*Set PA8 as output pin*/
	GPIOA->MODER |=(1U<<16);
	GPIOA->MODER &=~(1U<<17);

}

void esp_rs_pin_enable(void)
{
	/*Set PA8 to high*/
	GPIOA->ODR |=(1U<<8);
}

void esp_uart_init(void)
{
	/*Enable clock access to GPIOA*/
	RCC->AHB1ENR |= GPIOAEN;

	/*Set PA9 and PA10 modes to alternate function*/

	/*PA9*/
	GPIOA->MODER &=~(1U<<18);
	GPIOA->MODER |= (1U<<19);

    /*PA10*/
	GPIOA->MODER &=~(1U<<20);
	GPIOA->MODER |= (1U<<21);

    /*Set alternate function type to UART1 AF07*/

	/*PA9*/
	GPIOA->AFR[1] |= (1U<<4);
	GPIOA->AFR[1] |= (1U<<5);
	GPIOA->AFR[1] |= (1U<<6);
	GPIOA->AFR[1] &= ~(1U<<7);

	/*PA10*/
	GPIOA->AFR[1] |= (1U<<8);
	GPIOA->AFR[1] |= (1U<<9);
	GPIOA->AFR[1] |= (1U<<10);
	GPIOA->AFR[1] &= ~(1U<<11);

	/************Configure UART module*****************/

	/*Enable clock access to UART1*/
	RCC->APB2ENR |= UART1EN;

	/*Disable UART module*/
	USART1->CR1 &=~CR1_UE;

	/*Set UART baudrate*/
	USART1->BRR =  compute_uart_bd(APB2_CLK,UART_BAUDRATE);

	/*Set transfer direction*/
	USART1->CR1 = (CR1_TE | CR1_RE);

	/*Enable interrupt in the NVIC*/
	NVIC_EnableIRQ(USART1_IRQn);

	/*Enable UART module*/
	USART1->CR1 |=CR1_UE;



}


void debug_uart_write(int ch)
{
	/*Make sure the transmit data register is empty*/
	while(!(USART2->SR & SR_TXE)){}

	/*Write to transmit data register*/
	USART2->DR  =  (ch & 0xFF);
}


void systick_delay_ms(uint32_t delay)
{

	/*Reload with number of clocks per millisecond*/
	SysTick->LOAD = SYSTICK_LOAD_VAL;

	/*Clear systick current value register */
	SysTick->VAL = 0;

	/*Enable systick and select internal clk src*/
	SysTick->CTRL = CTRL_CLKSRC |CTRL_ENABLE;

	for(int i = 0; i< delay ;i++)
	{
		/*Wait until the COUNTFLAG is set*/
		while((SysTick->CTRL &  CTRL_COUNTFLAG)== 0){}
	}

	SysTick->CTRL = 0;


}
static uint16_t compute_uart_bd(uint32_t periph_clk, uint32_t baudrate)
{
	return ((periph_clk + (baudrate/2U))/baudrate);
}


