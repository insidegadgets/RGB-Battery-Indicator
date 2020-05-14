/*
 RGB Battery Indicator
 Version: 1.0
 Author: Alex from insideGadgets (www.insidegadgets.com)
 Created: 14/05/2020
 Last Modified: 14/05/2020

 */

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

#define LOW 0
#define HIGH 1
#define false 0
#define true 1

// Watchdog timeouts to values
#define T16MS 0
#define T32MS 1
#define T64MS 2
#define T128MS 3
#define T250MS 4
#define T500MS 5
#define T1S 6
#define T2S 7
#define T4S 8
#define T8S 9

#define ADC_REF_VCC 0
#define ADC_REF_1_1V 1

#define batteryADCPin 3

#define RGB_RED PB0
#define RGB_GREEN PB1
#define RGB_BLUE PB2

// Put the AVR in power down sleep mode
void system_sleep(void) {
	set_sleep_mode(SLEEP_MODE_PWR_DOWN); // Set sleep mode
	sleep_mode(); // System sleeps here
}

// Setup watchdog timed sequence
void setup_watchdog(uint8_t wdDelay) {
	if (wdDelay > 7) {
		wdDelay = wdDelay & 7;
		wdDelay |= (1<<5); // Set WDP3 bit
	}
	
	MCUSR &= ~(1<<WDRF); // Turn off flag
	WDTCR |= (1<<WDCE) | (1<<WDE); // Start timed sequence
	WDTCR = wdDelay | (1<<WDTIE); // Set new watchdog timeout value
}

// Turn off the Watchdog
void turn_off_watchdog(void) {
	cli(); // Turn interrupts off
	MCUSR &= ~(1<<WDRF);
	WDTCR |= (1<<WDCE) | (1<<WDE);
	WDTCR = 0;
	sei(); // Turn interrupts back on
}

// Watchdog sleep function - setup, sleep and turn off
void watchdog_sleep(uint8_t timeout) { 
	setup_watchdog(timeout);
	system_sleep();
	turn_off_watchdog();
}

// ADC Read
int adc_read(uint8_t pin, uint8_t reference) {
	uint8_t low, high;
	
	// Set the pin and ref
	if (reference == ADC_REF_1_1V) {
		ADMUX = (1<<REFS0) | (pin & 0x3F);
	}
	else {
		ADMUX = (pin & 0x3F);
	}
	
	set_sleep_mode(SLEEP_MODE_ADC); // Set sleep mode ADC
	sleep_mode(); // System sleeps here
	
	// we have to read ADCL first; doing so locks both ADCL
	// and ADCH until ADCH is read.  reading ADCL second would
	// cause the results of each conversion to be discarded,
	// as ADCL and ADCH would be locked when it completed.
	low = ADCL;
	high = ADCH;
	
	// combine the two bytes
	return (high << 8) | low;
}

// Setup the AVR
void setup(void) {
	// Set RGB outputs and all off
	DDRB |= (1<<PB2) | (1<<PB1) | (1<<PB0);
	PORTB |= (1<<PB2) | (1<<PB1) | (1<<PB0);
	
	// Wait a bit
	_delay_ms(250);
	
	// Set ADC prescale factor to 8
	// 1 MHz / 8 = 125 KHz, inside the desired 50-200 KHz range.
	sbi(ADCSRA, ADPS1);
	sbi(ADCSRA, ADPS0);
	
	// Enable ADC/interrupt
	sbi(ADCSRA, ADEN);
	sbi(ADCSRA, ADIE);
	
	sei(); // Turn on interrupts
}
