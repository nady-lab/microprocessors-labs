# microprocessors labs

lab work from my microprocessors module at TU Dublin. everything is written in C for the **STM32 Nucleo-L031K6** using PlatformIO and the CMSIS framework, so there's no HAL and no Arduino layer, just me reading and writing the chip's registers directly. helpers like `pinMode()` and `enablePullUp()` are ones i wrote myself.

## the labs

| folder | what it does |
|--------|--------------|
| `traffic_lights_l031` | three LEDs and a button make a traffic light. it sits on green, and when the button is pressed it goes to yellow and then red |
| `adc_pwm` | reads an analogue input with the ADC and uses the reading to set the duty cycle of a PWM signal, so turning a potentiometer changes the output |
| `serial_l031` | talks to a terminal over UART. every character you type is echoed back, typing `a` or `A` turns an LED on, anything else turns it off, and the output register is printed in hex |
| `simon` | a simplified simon game. a random LED lights up, you press the matching button, and a high note means you got it and a low note means you didn't |

### pins

| lab | pin | used for |
|-----|-----|----------|
| traffic lights | PA0 / PA1 / PA2 | green / yellow / red LEDs |
| | PB4 | button (pull-up enabled) |
| adc and pwm | PA3 | analogue input (ADC channel 3) |
| | PB1 | PWM output (Timer 2 channel 4), about 976 Hz at 12 bit |
| serial | PA2 / PA15 | UART TX / RX (USART2, 9600 baud), which is the ST-Link's virtual COM port |
| | PA0 | LED |
| simon | PA0 / PA1 / PA2 | green / yellow / red LEDs |
| | PA4 / PA5 / PA11 | green / yellow / red buttons (active low) |
| | PB1 | buzzer (Timer 2 channel 4) |
| | PA7 | floating analogue input, used as noise to seed the random numbers |

## build and flash

each folder is its own PlatformIO project, so build one at a time with `-d` (or `cd` into it):

```bash
git clone https://github.com/nady-lab/microprocessors-labs.git
cd microprocessors-labs

# build
pio run -d simon

# flash the board over USB (ST-Link)
pio run -d simon -t upload
```

swap `simon` for any of the other folder names. for the serial lab, open the monitor at 9600 baud after flashing:

```bash
pio device monitor -b 9600
```

## how each one works

**traffic lights:** each LED has its own on and off function that only touches its own bit of the output register, so changing one light never disturbs the others. the button is read from the input register on PB4.<br/>
**adc and pwm:** the ADC is calibrated and enabled, then each loop triggers a conversion, waits for it to finish and writes the 12-bit result straight into Timer 2's compare register. the timer counts to 4095, so the reading is the duty cycle. the clock is switched to the 16 MHz internal oscillator first.<br/>
**serial:** `eputchar`, `egetchar` and `eputs` poll the USART's status register for sending and receiving. `printDecimal` and `printHex` convert a number to text one digit at a time.<br/>
**simon:** random numbers come from a 31-bit shift register (a PRBS generator) seeded with noise from the ADC. sound is Timer 2 PWM with the period set from the note's frequency, at 50% duty.
