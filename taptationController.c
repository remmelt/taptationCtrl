#define F_CPU 1200000 // clock 1.2MHz, internal oscillator

//#include <stdbool.h>
//#include <avr/io.h>
//#include <util/delay.h>
#include <avr/interrupt.h>

#define LED        PB2
#define TEMPOSCALE PB3
#define DBLTIME    PB4

#define D1_1 0
#define D1_2 1
#define D3_4 2
#define ON   1
#define OFF  0

// there's just the one port on attiny13: PORTB
#define setBit(port, bit) { port |= (1 << bit); }
#define unsetBit(port, bit) { port &= ~(1 << bit); }
#define toggleBit(port, bit) { port ^= (1 << bit); }
#define setAsOutput(port) { setBit(DDRB, port); }
#define setAsInput(port) { unsetBit(DDRB, port); }

const int states[5][2] = {
	{ D1_1, OFF },  // 1/1
	{ D1_2, OFF },  // 1/2
	{ D1_2, ON  },  // 1/4
	{ D3_4, OFF },  // 3/4 (dotted eighth)
	{ D3_4, ON  },  // 3/8 (dotted sixteenth)
};

volatile int state = -1;

void toggleLedState(){
    toggleBit(PORTB, LED);
}

void setDblTime(int state){
    if (state == OFF){
        setAsInput(DBLTIME);
    } else {
        setAsOutput(DBLTIME);
        unsetBit(PORTB, DBLTIME);
    }
}

void setTempoScale(int state){
    switch (state) {
        case D1_1: default:
            setAsOutput(TEMPOSCALE);
            setBit(PORTB, TEMPOSCALE);
            break;
        case D1_2:
            setAsOutput(TEMPOSCALE);
            unsetBit(PORTB, TEMPOSCALE);
            break;
        case D3_4:
            setAsInput(TEMPOSCALE);
            break;
    }
}

void setState(int theState){
    toggleLedState();
    setTempoScale(states[theState][0]);
    setDblTime(states[theState][1]);
}

void advanceState(){
    state++;
    state %= 5;
    setState(state);
}

int main(void) {
    
    // General Interrupt Mask Register
    // INT0: enable external interrupt 0 (on PB1 / pin 6)
    // Attiny datasheet p47
    GIMSK = 1 << INT0;
    
    // Set interrupt sense control in MCU Control Register
    // ISC01,ISC00:
    // (0,0) low level,    (0,1) any change,
    // (1,0) falling edge, (1,1) rising edge
    // Attiny datasheet p47
    MCUCR = 1 << ISC01;
    
    // Set enable interrupts
    sei();
    
    setAsOutput(LED);
    setBit(PORTB, LED);
    
    advanceState();
    
    while(1){
    }
    
}

ISR (INT0_vect) {
    advanceState();
}
