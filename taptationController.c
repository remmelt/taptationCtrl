#define F_CPU 1200000 // clock 1.2MHz, internal oscillator

//#include <stdbool.h>
//#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/power.h>

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

volatile uint8_t state = -1;

void toggleLedState(){
    toggleBit(PORTB, LED);
}

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
//    toggleLedState();
    setTempoScale(states[theState][0]);
    setDblTime(states[theState][1]);
}

void advanceState(){
    state++;
}

void blinkTimes(uint8_t times){
    setAsOutput(LED);
    unsetBit(PORTB, LED);
    uint8_t delayTimeMs = 100;
    for (uint8_t c = 1; c <= times; c++) {
        setBit(PORTB, LED);
        _delay_ms(delayTimeMs);
        unsetBit(PORTB, LED);
        _delay_ms(delayTimeMs);
        if(c % 2 == 0) {
            _delay_ms(delayTimeMs);
        }
    }
    _delay_ms(500);
}

int main(void) {
    
    // General Interrupt Mask Register
    // INT0: enable external interrupt 0 (on PB1 / pin 6)
    // Attiny datasheet p47
    setBit(GIMSK, INT0);
    
    // Set interrupt sense control in MCU Control Register
    // ISC01,ISC00:
    // (0,0) low level,    (0,1) any change,
    // (1,0) falling edge, (1,1) rising edge
    // Attiny datasheet p47
    MCUCR = 0; // 1 << ISC01;
    
    // TODO: Power down modules
    
    // Set the sleep mode
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
//    set_sleep_mode(SLEEP_MODE_IDLE);

    // Set enable interrupts
    sei();
    
    setAsOutput(LED);
    setBit(PORTB, LED);
    
    advanceState();
    
    while(1){
        state %= 5;
        setState(state);
        blinkTimes(state + 1);
        
        
        
        sleep_enable();
        
        setBit(GIMSK, INT0);

        sei();
        sleep_cpu();
        sleep_disable();
    }
    
}

ISR (INT0_vect) {
    sleep_disable();
    unsetBit(GIMSK, INT0);
//    PCMSK &= ~(1<<INT0);
    advanceState();
}
