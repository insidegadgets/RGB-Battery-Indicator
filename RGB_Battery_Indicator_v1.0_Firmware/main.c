/*
 RGB Battery Indicator
 Version: 1.0
 Author: Alex from insideGadgets (www.insidegadgets.com)
 Created: 14/05/2020
 Last Modified: 14/05/2020
 
 Suitable for monitoring LiPo batteries
 
 */

#define F_CPU 1000000

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include "setup.c"

int main(void) {
	setup();
	
	uint8_t rgbColour = 0;
	
	while (1) {
		// 5 samples over 25ms
		uint16_t batteryVoltageLevel = 0;
		for (uint8_t x = 0; x < 5; x++) {
			batteryVoltageLevel += adc_read(batteryADCPin, ADC_REF_1_1V);
			_delay_ms(5);
		}
		batteryVoltageLevel = batteryVoltageLevel / 5;
		
		
		int greenHysterious = 0;
		int blueHysterious = 0;
		if (rgbColour == 2) { // Green
			greenHysterious = -10;
		}
		else if (rgbColour == 1) { // Blue
			greenHysterious = 10;
		}
		else { // Red
			blueHysterious = 10;
		}
		
		
		// Green
		if (batteryVoltageLevel >= (650 + greenHysterious)) { // 3.8v
			PORTB &= ~(1<<RGB_GREEN);
			PORTB |= (1<<RGB_RED);
			PORTB |= (1<<RGB_BLUE);
			rgbColour = 2;
		}
		
		// Blue
		else if (batteryVoltageLevel >= (580 + blueHysterious)) { // 3.3v
			PORTB &= ~(1<<RGB_BLUE);
			PORTB |= (1<<RGB_RED);
			PORTB |= (1<<RGB_GREEN);
			rgbColour = 1;
		}
		
		// Red
		else {
			PORTB &= ~(1<<RGB_RED);
			PORTB |= (1<<RGB_BLUE);
			PORTB |= (1<<RGB_GREEN);
			rgbColour = 0;
		}
		
		watchdog_sleep(T1S);
	}
	
	
	return 0;
}

EMPTY_INTERRUPT(ADC_vect);
EMPTY_INTERRUPT(WDT_vect);
