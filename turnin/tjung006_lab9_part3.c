/*	Author: terry
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
//Demo: https://drive.google.com/open?id=1QMBgMCkrfwGpP2G7oOiH11A-UHt27w-1
#include <avr/io.h>
#include <avr/interrupt.h>
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

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn(){
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;

	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}

void TimerOff(){
	TCCR1B = 0x00;
}

void TimerISR(){
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect){
	_avr_timer_cntcurr--;
	if(_avr_timer_cntcurr == 0){
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet(unsigned long M){
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

double song[12] = {329.63, 293.66, 261.63, 293.66, 0.0, 0.0, 261.63, 293.66, 261.63, 329.63, 261.63,0.0};
unsigned char length = 0x0C;
unsigned char i = 0;
unsigned char tmpA = 0x00;
enum States {start, off, play, change, wait}state;
unsigned char temp = 0x00;

void Tick() {
        switch(state) { 
                case start:
                        state = off;
                        break;

                case off:
                        if (tmpA == 0x01) {
                                state = play;
                        } else {
                                state = off;
                        }
                        break;

                case play:
                        set_PWM(song[i]);
			if(i >= length){
				state = wait;
			}
			else if(i < length){
				state = change;
			}
                        break;

                case change:
                        i++;
                        if (i >= length) {
                                state = wait;
                        } else if (i < length) {
                                state = play;
                        }
                        break;

                case wait:
                        if (tmpA == 0x01) {
                                state = wait;
                        } else {
                                state = off;
                        }
                        break;
		default:
			break;
        }

 	 switch(state){ 
                case off:
			i = 0;
                        set_PWM(0);
                        break;
                case wait:
                        i = 0;
                        break;

        }
}

int main(void) {
        DDRA = 0x00; PORTA = 0xFF;
        DDRB = 0xFF; PORTB = 0x00;
        PWM_on();

        state = start;
        TimerSet(200);
        TimerOn();

        i = 0;
        while(1) {
		tmpA = ~PINA;
                Tick();
                while (!TimerFlag);
                TimerFlag = 0;
        }
        PWM_off();
        return 1;
}

