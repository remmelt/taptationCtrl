# Taptation Digital Controller

Digital controller for the fabulous [Taptation](http://www.diystompboxes.com/wpress/?p=157) tap tempo unit.

The [PT2399 Interface Application Note](http://www.thetonegod.com/tech/datasheets/taptation_pt2399.pdf) for the Taptation tells me to switch the five different tempo states with two switches, (on|off|on) and (on|off). I think this is not very elegant because two states are equal, also I wanted to see if this could be done with a pushbutton.

Turns out it's fairly easy by using the three states of an AVR digital pin (HIGH, LOW and high impedance/input).

I'm using a [debounce circuit using NAND ports](http://www.robotroom.com/DebouncedCounter.html) which works very well but adds a couple of parts. My button is very bouncy so it's a fair tradeoff.

         attiny13
          ------
         -1    8- Vcc
     PB3 -2    7-
     PB4 -3    6- INT0
     GND -4    5-
          ------

PB3 is the Tempo Scale pin (three states), PB4 is the Double Time pin. These should be connected to pins 12 and 11 of the Taptation, respectively. INT0 is the interrupt used with the button.

