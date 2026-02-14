#include <stdint.h>
#include <stm32l031xx.h>

void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode);
void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber);
void delay(volatile uint32_t dly);

// functions for the traffic light

void RedOn(void);		 // turns on the red LED without changing the others
void RedOff(void);		 // turns off the red LED without changing the others
void YellowOn(void);	 // turns on the yellow LED without changing the others
void YellowOff(void);	 // turns off the yellow LED without changing the others
void GreenOn(void);		 // turns on the green LED without changing the others
void GreenOff(void);	 // turns off the green LED without changing the others
int ButtonPressed(void); // returns if the button is pressed. 0 otherwise

int main()
{
	//uint32_t count = 0;
	RCC->IOPENR |= (1 << 0) + (1 << 1);
	pinMode(GPIOA, 0, 1); // Make GPIOA bit 0 an output
	pinMode(GPIOA, 1, 1); // Make GPIOA bit 1 an output
	pinMode(GPIOA, 2, 1); // Make GPIOA bit 2 an output

	pinMode(GPIOB, 3, 1); // Make GPIOB bit 3 an output
	pinMode(GPIOB, 4, 0); // Make GPIOB bit 4 an input

	enablePullUp(GPIOB, 4); // Turn on pull-up resistor for bit 4
	while (1)
	{
		// original code
		/*GPIOA->ODR = count;
		count++;
		delay(200000);*/

		// traffic light code
		if (ButtonPressed())
		{
			YellowOn();
			GreenOff();
			delay(400000);

			RedOn();
			YellowOff();
			delay(400000);
		} // end if
		else
		{
			RedOff();
			YellowOff();
			GreenOn();
		} // end else
	}
}
void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber)
{
	Port->PUPDR = Port->PUPDR & ~(3u << BitNumber * 2); // clear pull-up resistor bits
	Port->PUPDR = Port->PUPDR | (1u << BitNumber * 2);	// set pull-up bit
}
void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode)
{
	/*
	 */
	uint32_t mode_value = Port->MODER;
	Mode = Mode << (2 * BitNumber);
	mode_value = mode_value & ~(3u << (BitNumber * 2));
	mode_value = mode_value | Mode;
	Port->MODER = mode_value;
} // end pinMode

void delay(volatile uint32_t dly)
{
	while (dly--)
		;

} // end delay

void RedOn(void) // turns on the red LED without changing the others
{
	GPIOA->ODR = GPIOA->ODR | (1 << 2);
}// end RedOn()
void RedOff(void) // turns off the red LED without changing the others
{
	GPIOA->ODR = GPIOA->ODR & ~(1 << 2);
}// end RedOff()

void YellowOn(void) // turns on the yellow LED without changing the others
{
	GPIOA->ODR = GPIOA->ODR | (1 << 1);
}// end YellowOn()
void YellowOff(void) // turns off the yellow LED without changing the others
{
	GPIOA->ODR = GPIOA->ODR & ~(1 << 1);
}// end Yellow Off()

void GreenOn(void) // turns on the green LED without changing the others
{
	GPIOA->ODR = GPIOA->ODR | (1 << 0);
}// end GreenOn()
void GreenOff(void) // turns off the green LED without changing the others
{
	GPIOA->ODR = GPIOA->ODR & ~(1 << 0);
}// end GreenOff()

int ButtonPressed(void) // returns if the button is pressed. 0 otherwise
{
	if ((GPIOB->IDR & (1 << 4)))
	{
		return 1;
	} // end if
	else
	{
		return 0;
	} // end else
} // end ButtonPressed()
