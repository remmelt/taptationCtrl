#define F_CPU 1200000 // clock 1.2MHz, internal oscillator

//#include <stdbool.h>
//#include <avr/io.h>
//#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
//#include <avr/power.h>

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

const uint8_t states[5][2] = {
	{ D1_1, OFF },  // 1/1
	{ D1_2, OFF },  // 1/2
	{ D1_2, ON  },  // 1/4
	{ D3_4, OFF },  // 3/4 (dotted eighth)
	{ D3_4, ON  },  // 3/8 (dotted sixteenth)
};

volatile uint8_t state = 0;
volatile uint8_t travel = 0;

void setDblTime(uint8_t state){
    if (state == OFF){
        setAsInput(DBLTIME);
    } else {
        setAsOutput(DBLTIME);
        unsetBit(PORTB, DBLTIME);
    }
}

void setTempoScale(uint8_t state){
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

void setState(uint8_t theState){
    setTempoScale(states[theState][0]);
    setDblTime(states[theState][1]);
}

int main(void) {
    
    // General Interrupt Mask Register
    // INT0: enable external interrupt 0 (on PB1 / pin 6)
    // Attiny datasheet p46
    setBit(GIMSK, PCIE);
    setBit(PCMSK, PCINT1);
    
    // TODO: Power down modules
    
    // Set the sleep mode
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    
//    setAsOutput(LED);
//    setBit(PORTB, LED);
        
    while(1){
//          blinkTimes(state + 1);
        
        state %= 5;
        setState(state);

        sleep_enable();
        
        // Set enable interrupts
        sei();
        
        sleep_cpu();
        sleep_disable();
    }
    
}

ISR (PCINT0_vect) {
    cli();
    travel = !travel;
    if(!travel) {
        state++;
    }
}
