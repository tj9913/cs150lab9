/*	Author: terry
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

void set_PWM(double frequency){
	static double current_frequency;
	if(frequency != current_frequency){ 
		if(!frequency){ TCCR3B &= 0x08; }
		else{ TCCR3B |= 0x03; }
	
		if(frequency < 0.954){ OCR3A = 0xFFFF; }
	
		else if(frequency > 31250){ OCR3A = 0x0000; }

		else{ OCR3A = (short)(8000000 / (128 * frequency)) - 1;}
	
		TCNT3 = 0;
		current_frequency = frequency;
	}
}

void PWM_on(){
	TCCR3A = (1 << COM3A0);
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
	set_PWM(0);
}

void PWM_off(){
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}

unsigned char tmpA = 0x00;
enum states{ start, off, c4, d4, e4} state;

void tick(){
	switch(state){
		case start:
			state = off;
			break;
		case off:
			if(tmpA == 0x01){
				state = c4;
			}
			else if(tmpA == 0x02){
				state = d4;
			}
			else if(tmpA == 0x04){
				state = e4;
			}
			else{
				state = off;
			}
			break;
		case c4:
			if(tmpA == 0x01){
				state = c4;
			}
			else{
				state = off;
			}
			break;
		case d4:
			if(tmpA == 0x02){
				state = d4;
			}
			else{
				state = off;
			}
			break;
		case e4:
			if(tmpA == 0x04){
                                state = e4;
                        }
                        else{
                                state = off;
                        }
                        break;
	}
	switch(state){
		case off:
			set_PWM(0);
			break;
		case c4:
			set_PWM(261.63);
			break;
		case d4:
			set_PWM(293.66);
			break;
		case e4:
			set_PWM(329.63);
			break;
			
	}
}
int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;
    PWM_on();
    state = off;
    /* Insert your solution below */
    while (1) {
	tmpA = ~PINA;
	tick();
    }
    return 1;
}
