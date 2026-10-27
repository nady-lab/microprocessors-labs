#include <stm32l031xx.h>
#include <stdint.h>

void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode);
void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber);
void delay(volatile uint32_t dly);
void initClockHSI16(void);
void initADC(void);
int readADC(void);
uint32_t prbs(void);
void randomize(void);
uint32_t random(uint32_t lower,uint32_t upper);
void redOn(void);
void redOff(void);
void yellowOn(void);
void yellowOff(void);
void greenOn(void);
void greenOff(void);

int greenPressed(void);
int yellowPressed(void);
int redPressed(void);

void initSound(void);
void playNote(uint32_t freq);
int main()
{
    uint32_t choice=0;
    RCC->IOPENR |= (1 << 0) + (1 << 1);
    initClockHSI16();
	pinMode(GPIOA,0,1); // Make GPIOA bit 0 an output
	pinMode(GPIOA,1,1); // Make GPIOA bit 1 an output
	pinMode(GPIOA,2,1); // Make GPIOA bit 2 an output
    pinMode(GPIOA,7,3); // will use PA7 as an analog input to seed the random number generator
    pinMode(GPIOA,11,0);
    pinMode(GPIOB,4,0);
    pinMode(GPIOB,5,0);
    enablePullUp(GPIOA,11);
    enablePullUp(GPIOB,4);
    enablePullUp(GPIOB,5);
    initADC();
    initSound();
    randomize();

	while(1)
    {   
        choice=random(0,3);
        switch(choice) {
            case 0: {
                greenOn();
                break;
            }
            case 1: {
                yellowOn();
                break;
            }
            case 2: {
                redOn();
                break;
            }
        } 
        
        delay(200000);
        redOff();
        yellowOff();
        greenOff();

        while(redPressed() == 0 && yellowPressed() == 0 && greenPressed() == 0)
        {
            // do nothing
        }

        // save button choice
        int buttonChoice = 0;
        if (greenPressed())
        {
            buttonChoice = 0;
        }// end if
        else if (yellowPressed())
        {
            buttonChoice = 1;
        }// end else if
        else
        {
            buttonChoice = 2;
        }// end else

        if(buttonChoice == choice)
        {
            playNote(1000);
        }// end if
        else
        {
            playNote(400);
        }// end else

        delay(1000000);
        playNote(0);
    }
    return 0;
}
void redOn()
{
    GPIOA->ODR |= (1 << 2);
}
void redOff()
{
    GPIOA->ODR &= ~(1 << 2);
}
void yellowOn()
{
    GPIOA->ODR |= (1 << 1);
}
void yellowOff()
{
    GPIOA->ODR &= ~(1 << 1);
}
void greenOn()
{
    GPIOA->ODR |= (1 << 0);
}
void greenOff()
{
    GPIOA->ODR &= ~(1 << 0);
}

int greenPressed(void)
{
    if((GPIOA->IDR & (1 << 4)) == 0)
    {
        return 1;
    }// end if
    else
    {
        return 0;
    }// end else
}// end greenPressed()
int yellowPressed(void)
{
    if((GPIOA->IDR & (1 << 5)) == 0)
    {
        return 1;
    }// end if
    else
    {
        return 0;
    }// end else
}// end yellowPressed()
int redPressed(void)
{
    if((GPIOA->IDR & (1 << 11)) == 0)
    {
        return 1;
    }// end if
    else
    {
        return 0;
    }// end else
}// end redPressed()


void initSound()
{    
// Sound will be produced by sending the output from Timer 2, channel 4 out PB1
    pinMode(GPIOB,1,2); // select alternative function for PB1
    GPIOB->AFR[0] &= ~( (0b1111) << 1*4); // zero out alternate function bits
    GPIOB->AFR[0] |= ( (0b0101) << 1*4); // set required alternate function bits (AF5)
    
    RCC->APB1ENR |= (1 << 0);  // enable Timer 2
    TIM2->CR1 = 0; // Set Timer 2 to default values
	TIM2->CCMR2 = (1 << 14) + (1 << 13);
	TIM2->CCER |= (1 << 12);
	TIM2->PSC = 3;
	TIM2->ARR = 1UL;
	TIM2->CCR4 = TIM2->ARR/2;	
	TIM2->CNT = 0;
}
void playNote(uint32_t freq)
{
// Counter is running at 4MHz 
// Lowest possible frequency = 4000000/65536 = 61 Hz approx
    if (freq == 0)
    {
        TIM2->CR1 &= ~(1 << 0); // disable the counter
    }
	TIM2->ARR = (uint32_t)4000000/((uint32_t)freq); 
	TIM2->CCR4= TIM2->ARR/2;	
	TIM2->CNT = 0; // set the count to zero initially
	TIM2->CR1 |= (1 << 0); // and enable the counter
}
void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber)
{
	Port->PUPDR = Port->PUPDR &~(3u << BitNumber*2); // clear pull-up resistor bits
	Port->PUPDR = Port->PUPDR | (1u << BitNumber*2); // set pull-up bit
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
}
void delay(volatile uint32_t dly)
{
	while(dly--);	
}
uint32_t random(uint32_t lower,uint32_t upper)
{
    return (prbs()%(upper-lower))+lower;
}
uint32_t shift_register=0;
void randomize(void)
{
    // uses ADC noise values to seed the shift_register
    while(shift_register==0)
    {
        for (int i=0;i<10;i++)
        {
            shift_register+=(readADC()<<i);
        }
    }
}
uint32_t prbs()
{
	// This is an unverified 31 bit PRBS generator
	// It should be maximum length but this has not been verified 
	unsigned long new_bit=0;	

    new_bit= ((shift_register & (1<<27))>>27) ^ ((shift_register & (1<<30))>>30);
    new_bit= ~new_bit;
    new_bit = new_bit & 1;
    shift_register=shift_register << 1;
    shift_register=shift_register | (new_bit);
		
	return shift_register & 0x7fffffff; // return 31 LSB's 
}
void initClockHSI16()
{
    // Use the HSI16 clock as the system clock - allows operation down to 1.5V
    RCC->CR &= ~(1<<24);
    RCC->CR |= (1 << 0); // turn on HSI16 (16MHz clock)
    while ((RCC->CR & (1<<2))==0); // wait for HSI to be ready
    // set HSI16 as system clock source 
    RCC->CFGR |= (1<<0);
}

void initADC()
{

	
	// Turn on ADC 
	RCC->APB2ENR |= (1 << 9);		
    // Enable the reference voltage
	ADC->CCR |= (1 << 22);	
	// Begin ADCCalibration	
	ADC1->CR = ( 1 << 31);
	// Wait for calibration complete:  
	while ((ADC1->CR & ( 1 << 31)));
	// Select Channel 7
	ADC1->CHSELR |= (1 << 7);	
	// Enable the ADC
	ADC1->CR |= (1 << 0);  
}

int readADC()
{
	// Trigger a conversion
	ADC1->CR |=  (1 << 2);
	// Wait for End of Conversion
	while ( (ADC1->CR & 1 << 2));
	// return result
	return ADC1->DR;
}
