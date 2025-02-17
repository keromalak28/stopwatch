/*
 * stopwatch.c
 *
 *  Created on: Feb 10, 2025
 *      Author: Kerolos Malak
 */
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
//define date type
unsigned char Seconds=0; // store Seconds
unsigned char Minutes=0; // store Minutes
unsigned char Hours=0; // store Hours
unsigned char paused=0; //  ISR-> 1 Pauses *INT1* , ISR->0 Resumes *INT2*
unsigned char Mode=0; // counting up (increment mode) and counting down (countdown mode)

// prototype function
void configration_pin();
void Buzzer();
void display();
void INT0_init();
void INT1_init();
void INT2_init();
void adjust_time();
void Timer1_CTC_INT();
//=====================================================================================================================//

ISR(INT0_vect){ // ISR for INT0
	TCNT1=0; // Reset timer counter
	Seconds=0;
	Minutes=0;
	Hours=0;
//	PORTD &=~(1<<PD0); // stop buzzer
	}

//=====================================================================================================================//

ISR(INT1_vect){ // ISR for INT1
 TCCR1B = 0;  // stop the timer
	paused=1; //set flag
}
//=====================================================================================================================//

ISR(INT2_vect){ // ISR for INT2
	TCCR1B |= (1<<CS12) | (1<<CS10) | (1<<WGM12) ; // start timer
	paused=0; // clear flag
}

//=====================================================================================================================//
ISR(TIMER1_COMPA_vect){
	   if(!paused) {
	        if (Mode == 0) { // up count
	            Seconds++;
	            PORTD |=(1<<PD4); //led on (red)
	            PORTD &=~(1<<PD5); // led on (yellow)
	            if (Seconds == 60) { Seconds = 0; Minutes++; }
	            if (Minutes == 60) { Minutes = 0; Hours++; }
	            if (Hours == 24) { Hours = 0; }
	        } else { // down count
	        	PORTD &=~(1<<PD4);
	            PORTD|=(1<<PD5); // led on (yellow)
	        	if (Seconds == 0) {
	                if (Minutes == 0) {
	                    if (Hours == 0) {
	                        TCCR1B = 0; // stop timer
	                        Buzzer(); // buzzer on
	                        return;
	                    } else { Hours--; Minutes = 59; Seconds = 59; }
	                } else { Minutes--; Seconds = 59; }
	            } else { Seconds--; }
	        }
	    }
	}
//=====================================================================================================================//

int main() {
    configration_pin();
    INT0_init();
    INT1_init();
    INT2_init();
    Timer1_CTC_INT();

    while (1) {
        if (!(PINB & (1 << PB7))) { //
//            _delay_ms(10);
            if (!(PINB & (1 << PB7))) {
                if (TCCR1B == 0) { //If it is temporarily stopped due to reaching zero
                    Mode = 0; // up counter
                    Seconds = 0;
                    Minutes = 0;
                    Hours = 0;
                    TCCR1B |= (1 << CS12) | (1 << CS10) | (1 << WGM12); // start timer
                } else {
                    Mode ^= 1; // toggle mode
                }
            }
        }

        adjust_time();
        display();
    }
}

//=====================================================================================================================//

void configration_pin(void){
	// out pins
	DDRD |=(1<<PD0) | (1<<PD4) | (1<<PD5); // (PD0) -> Buzzer , (PD4) -> Counting Up LED , (PD5)  -> Counting Down LED
	PORTD &=~(1<<PD0); // buzzer is off initially
	PORTD |=(1<<PD4);  // led counting up led on ( increment mode )
	PORTD &=~(1<<PD5); // counting down led (down mode)

	// PORTA for 7-segment display enable control (PD0-PD5)
	DDRD = 0x3F;
	// PORTA for 7-segment display decoder (PC0-PC3)
	DDRC = 0x0F;

	// input pins
	// pins (PB0-PB6) -> pull up resistors
    // PB0->inc.hour ,PB1->dec.hour ,PB4->inc.min, PB3->dec.min PB6->inc.sec PB5-> dec.sec
	DDRB &=~(1<<PB0) | (1<<PB1) | (1<<PB2) | (1<<PB3) | (1<<PB4) | (1<<PB5) | (1<<PB6);
	// pull up resistors -> 1 ,  press the button -> 0
	PORTB |= (1<<PB0) | (1<<PB1) | (1<<PB2) | (1<<PB3) | (1<<PB4) | (1<<PB5) | (1<<PB6);

	DDRB &=~(1<<PB7) | (1<<PB2);  // PB7 -> Toggles mode , PB2-> Resumes (INT2)
	PORTB |= (1<<PB7) | (1<<PB2); // Enable pull up

	DDRD &=~(1<<PD2); // PD2 -> pauses (INT0)
	PORTD |=(1<<PD2);  // Enable pull up *Reset*
	DDRD |= (1<<PD3); // external pull up INT1 *Pause*
}

//=====================================================================================================================//


void Buzzer(void){
	PORTD |=(1<<PD0);
	_delay_ms(3000);
	PORTD &=~(1<<PD0);
}

//=====================================================================================================================//

void display() {
    unsigned char segments[6] = {0b00100000, 0b00010000, 0b00001000, 0b00000100, 0b00000010, 0b00000001};
    unsigned char values[6] = {Seconds % 10, Seconds / 10, Minutes % 10, Minutes / 10, Hours % 10, Hours / 10};

    for (int i = 0; i < 6; i++) {
        PORTA = segments[i]; // PA0-PA5
        PORTC = (PORTC & 0xF0) | (values[i] & 0x0F); // display the value in PORTC
        _delay_ms(2);
    }
}


//=====================================================================================================================//

void INT0_init(){ // Reset *INT0*//
	SREG |=(1<<7);  // Global interrupt enable//
	MCUCR |=(1<<ISC01); // interrupt with falling edge//
	GICR |=(1<<INT0); // module interrupt enable //
}

//=====================================================================================================================//

void INT1_init(){ // Pauses *INT1*
	SREG |=(1<<7); // Global interrupt enable//
	MCUCR |=(1<<ISC11) | (1<<ISC10); // interrupt with Rising edge//
	GICR |=(1<<INT1); // module interrupt enable //
}

//=====================================================================================================================//

void INT2_init(){ // Resumes *INT2*
	SREG |=(1<<7); // Global interrupt enable//
	MCUCR |=(1<<ISC2);  // interrupt with falling edge//
	GICR |=(1<<INT2); // module interrupt enable //
}

//=====================================================================================================================//
// control button
void adjust_time() {
// increment hours
	if (!(PINB & (1 << PB1))) {
        _delay_ms(10); //(Debounce)
        if (!(PINB & (1 << PB1))) {
            while (!(PINB & (1 << PB1))); // check the pin
            Hours++;
            if (Hours >= 24) Hours = 0;
        }
    }

// decrement hours
	if (!(PINB & (1 << PB0))) {
        _delay_ms(20);
        if (!(PINB & (1 << PB0))) {
            while (!(PINB & (1 << PB0))); //
            if (Hours > 0) Hours--;
        }
    }

    // inc.min
    if (!(PINB & (1 << PB4))) {
        _delay_ms(20);
        if (!(PINB & (1 << PB4))) {
            while (!(PINB & (1 << PB4))); // wait -> press button
            Minutes++;
            if (Minutes >= 60) Minutes = 0;
        }
    }

    // dec.min
    if (!(PINB & (1 << PB3))) {
        _delay_ms(20);
        if (!(PINB & (1 << PB3))) {
            while (!(PINB & (1 << PB3)));
            if (Minutes > 0) Minutes--;
        }
    }

    // inc.sec
    if (!(PINB & (1 << PB6))) {
        _delay_ms(20);
        if (!(PINB & (1 << PB6))) {
            while (!(PINB & (1 << PB6))); //
            Seconds++;
            if (Seconds >= 60) Seconds = 0;
        }
    }

    // dec.sec
    if (!(PINB & (1 << PB5))) {
        _delay_ms(20);
        if (!(PINB & (1 << PB5))) {
            while (!(PINB & (1 << PB5))); //
            if (Seconds > 0) Seconds--;
        }
    }
}


//==================================================================//
void Timer1_CTC_INT(){
    TCNT1 = 0;
    TCCR1A = 0;
    TCCR1B = (1 << WGM12) | (1 << CS10) | (1 << CS12);
    OCR1A = 15625;
    TIMSK |= (1 << OCIE1A);
    SREG |= (1 << 7);
}
//=====================================================================================================================//





