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
unsigned char i = 0;
unsigned short cnt = 0;
double freq[8] = {261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25};
enum states{ start, off, offPress, on, onPress, up, upPress, down, downPress} state;

void tick(){
	switch(state){
		case start:
			state = off;
                        break;
		case off:
			cnt = 1;
			if(tmpA == 0x04){
				state = onPress;
				
			}
			else if(tmpA == 0x01){
                                state = upPress;
                        }
                        else if(tmpA == 0x02){
                                state = downPress;
                        }
			else{
				state = off;
			}
			break;
		case onPress:
			if(tmpA == 0x04){
				state = onPress;
			}
			else{
				state = on;
			}
			break;
		case on:
			cnt = 0;
			if(tmpA == 0x04){
				state = offPress;
			}	
			else if(tmpA == 0x01){
				state = upPress;
			}
			else if(tmpA == 0x02){
				state = downPress;
			}
			else{
				state = on;
			}
			break;
		case offPress:
			if(tmpA == 0x04){
				state = offPress;
			}
			else{
				state = off;
			}
			break;
		case upPress:
			if(tmpA == 0x01){
				state = upPress;
			}
			else{
				state = up;
			}
			break;
		case up:
			if(cnt % 2 == 1){
				state = off;
			}
			else if(cnt % 2 == 0){
				state = on;
			} 
			break;
		case downPress:
			if(tmpA == 0x02){
				state = downPress;
			}
			else{
				state = down;
			}
			break;
		case down:
			if(cnt % 2 == 1){
                                state = off;
                        }
                        else if(cnt %2 == 0){
                                state = on;
                        }
			break;

	}
	switch(state){
		case off:
			PWM_off();
			break;
		case onPress:
			PWM_on();
			break;
		case on:
			set_PWM(freq[i]);
			break;
		case offPress:
			break;
		case upPress:
			break;
		case up:
			if(i < 7){
				i++;
			}
			break;
		case downPress:
			break;
		case down:
			if(i > 0){
				i--;
			}
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
