#include <avr/interrupt.h>
#define BADISR 1

/**
 * E4E Weather Station code
 * Using Sparkfun SEN-08942
 * 5V
 * GND
 * A0 (wind vane)
 * D2 (anemometer)
 */

double angleVoltage = 0;
uint16_t speedIndex = 0;
double speedBuffer[15];
uint8_t flags = 0;
uint8_t eifr_copy = 0;
uint32_t windSpeedAvg = 0;

uint8_t windOffset = 0;

String direction = "";

void setup(void){
    Serial.begin(57600);
    Serial.println("E4E Weather Station v1.0b10");

	TCCR1A = 0;	// Clear timer 1 control register a
	TCCR1B |=  (1 << CS10);
	// use normal mode, set 1024 prescaler: 0.5 us @ 16 MHz
	EIMSK &= ~(1 << INT0);	// enable extermal interrupt on pin 2
	EICRA |= (1 << ISC01) | ( 1 << ISC00);	// Trigger INT0 on rising edge
	EIMSK |= (1 << INT0);	// enable extermal interrupt on pin 2
    sei();
	
	// Configure PORTD2 as input
	// write 1 to PORTD2 to activate pull-up resistor
	// DDD2 = 0
	DDRD &= ~(1 << DDD2);
	// PORTD2 = 1
	PORTD |= (1 << PORTD2);
	// PUD (MCUCR) = 0
	MCUCR &= ~(1 << PUD);
}
ISR(BADISR_vect){
    PORTB |= (1 << 7);
    flags ^= (1 << BADISR);
    eifr_copy = EIFR;
}

ISR(INT0_vect){	// Rising edge interrupt for RC_PPM
    speedBuffer[speedIndex++] = (79632.516 / TCNT1);    // add calculated speed
    TCNT1 = 0;
}

void loop(void){
    if(speedIndex == 15){
        uint8_t count = 0;
        for(int i = 0; i < 15; i++){
            if(speedBuffer[i] < 100){
                windSpeedAvg += speedBuffer[i];
                count++;
            }
        }
        windSpeedAvg /= count;
    }
    if(flags & (1 << BADISR)){
        Serial.print("BAD");
        Serial.println(eifr_copy);
        flags ^= (1 << BADISR);
        PORTB &= ~(1 << 7);
    }

    angleVoltage = analogRead(A0) / 1024.0 * 5;
    if(angleVoltage > 3.82 && angleVoltage < 3.86){
        direction = String(0 + windOffset);
    }
    if(angleVoltage > 1.97 && angleVoltage < 2.00){
        direction = String(23 + windOffset);
    }
    if(angleVoltage > 2.24 && angleVoltage < 2.27){
        direction = String(45 + windOffset);
    }
    if(angleVoltage > 0.39 && angleVoltage < 0.42){
        direction = String(68 + windOffset);
    }
    if(angleVoltage > 0.43 && angleVoltage < 0.46){
        direction = String(90 + windOffset);
    }
    if(angleVoltage > 0.30 && angleVoltage < 0.32){
        direction = String(113 + windOffset);
    }
    if(angleVoltage > 0.88 && angleVoltage < 0.91){
        direction = String(135 + windOffset);
    }
    if(angleVoltage > 0.60 && angleVoltage < 0.62){
        direction = String(158 + windOffset);
    }
    if(angleVoltage > 1.39 && angleVoltage < 1.41){
        direction = String(180 + windOffset);
    }
    if(angleVoltage > 1.18 && angleVoltage < 1.20){
        direction = String(203 + windOffset);
    }
    if(angleVoltage > 3.06 && angleVoltage < 3.09){
        direction = String(225 + windOffset);
    }
    if(angleVoltage > 2.91 && angleVoltage < 2.94){
        direction = String(248 + windOffset);
    }
    if(angleVoltage > 4.59 && angleVoltage < 4.63){
        direction = String(270 + windOffset);
    }
    if(angleVoltage > 4.02 && angleVoltage < 4.05){
        direction = String(293 + windOffset);
    }
    if(angleVoltage > 4.32 && angleVoltage < 4.35){
        direction = String(315 + windOffset);
    }
    if(angleVoltage > 3.41 && angleVoltage < 3.45){
        direction = String(338 + windOffset);
    }
    Serial.print("Direction: ");
	Serial.print(direction);
	Serial.print(" degrees\tSpeed: ");
    Serial.print(windSpeedAvg);
    Serial.println("MPH");
    speedIndex = 0;
}