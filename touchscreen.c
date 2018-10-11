/*
 * GccApplication13.c
 *
 * Created: 2018/9/15 9:41:32
 * Author : lzhof
 */ 
#define F_CPU 16000000L
#include <avr/io.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
//#include "uart.h"


uint16_t touchscreen_readXADC(void)
{
	DDRC &= 0xF0;
	DDRC |= (1 << DDC0)|(1 << DDC2);//A2,A0 digital output
	DDRC &= ~(1 << DDC3);//A3 analog input
	DDRC &= ~(1 << DDC1);//A1 analog input
	PORTC &= 0xF0;
	PORTC &= ~(1 << DDC3);PORTC &= ~(1 << DDC1); //A1,A3 high-z 
	PORTC |= (1 << PORTC2);//A2 high
	PORTC &= ~(1 << PORTC0);//A0 low
    ADMUX |= (1 << MUX0);ADMUX &= ~(1 << MUX1);ADMUX &= ~(1 << MUX2);ADMUX &= ~(1 << MUX3); //A1 analog input
	
	ADCSRA |= (1<<ADSC); //Start converting
	
	while(ADCSRA & (1<<ADSC));
	
	return ADC;
}

uint16_t touchscreen_readYADC(void)
{
	DDRC &= 0xF0;
	DDRC |= (1 << DDC1)|(1 << DDC3);//A3,A1 digital output
	DDRC &= ~(1 << DDC0);//A0 analog input
	DDRC &= ~(1 << DDC2);//A2 analog input
	PORTC &= 0xF0;
	PORTC &= ~(1 << DDC2);PORTC &= ~(1 << DDC0); //A0,A2 high-z 
	PORTC |= (1 << PORTC1);//A1 high
	PORTC &= ~(1 << PORTC3);//A3 low
	ADMUX |= (1 << MUX1);ADMUX &= ~(1 << MUX0);ADMUX &= ~(1 << MUX2);ADMUX &= ~(1 << MUX3); //A2 analog input
	
	ADCSRA |= (1<<ADSC); //Start converting
	
	while(ADCSRA & (1<<ADSC));
	
	return ADC;
}

void setupADC(void)
{
	ADCSRA |= ((1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0)); //prescaler at 128 so we have an 125Khz clock source
	//ADCSRA |= (1<<ADATE); //ADC Auto Trigger Enable
	ADCSRA |= (1<<ADEN); //ADC Enable
	//ADCSRB &= ~((1<<ADTS2)|(1<<ADTS1)|(1<<ADTS0)); //ADC Auto Trigger Source Selections(free running mode)
	ADMUX |= (1<<REFS0); ADMUX &= ~(1<<REFS1); // Voltage Referrence Selection (Avcc(+5v) as voltage reference)

}

uint8_t detect( void )
{
	ADMUX &= 0xF0;
	ADMUX |= (1 << MUX0);
	
	DDRC &= 0xF0;
	PORTC &= 0xF0;
	DDRC |= (1 << DDC2) | (1 << DDC0);//A2,A0 output
	PORTC &= ~(1 << DDC2);//A2 low
	PORTC &= ~(1 << DDC0);//A0 low
	
	DDRC &= ~(1 << DDC1);//A1 input
	DDRC &= ~(1 << DDC3);//A3 input
	PORTC |= (1 << DDC1);//A1 pull up
	PORTC &= ~(1 << DDC3);//A3 high-z
	ADCSRA |= (1<<ADSC); //Start converting
	
	while(ADCSRA & (1<<ADSC));
	
	return ADC<300;
	
	
}
