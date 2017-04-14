/*
 * File:   newmainXC16.c
 * Author: Andrew
 *
 * Created on March 25, 2017, 1:34 PM
 */


#include "xc.h"
#pragma config ICS = PGx3 // DEBUG
#pragma config FNOSC = FRC  // 8e6 hz Timer -> fcy = 4e6 hz
#define FCY                     4000000UL
#include <libpic30.h>       // For __delay functions

enum {INC, DEC} inc_state;
#define LOW_THRESH 275
#define HIGH_THRESH 1225
#define NEUTRAL 750
typedef enum {TRUE, FALSE} bools;

int main(void) {
    bools run = FALSE;
    int init = LOW_THRESH;
    
    T3CONbits.TON = 0;              // DISABLED at first
    T3CONbits.TCKPS = 0b01;         // 1:8 pre-scale
    T3CONbits.TCS = 0;              // Internal clock source
    
    OC3CON1 = 0b0000;           // Clear OC1 configuration bits
    OC3CON2 = 0b0000;           // Clear OC2 configuration bits 
    OC3CON1bits.OCTSEL = 0b001; // Use Timer 3
    OC3CON1bits.OCM = 0b110;    // Edge-aligned PWM mode
    OC3CON2bits.SYNCSEL = 0b11111;  // OC1 used for syncing
    
    OC3RS = 9999;  
    OC3R = init;
    
    T3CONbits.TON = 1; 
     
    inc_state = INC;
    __delay_ms(2000);
    
    if (run == TRUE)
    {
    while(1)
    {
        
        __delay_ms(2);
        switch(inc_state)
        {
            case INC:
                if (OC3R < HIGH_THRESH)
                    OC3R += 2;
                else
                {
                    OC3R = HIGH_THRESH;
                    __delay_ms(1000);
                    inc_state = DEC;
                }
                break;
            case DEC:
                if (OC3R > LOW_THRESH)
                    OC3R -= 2;
                else
                {
                    OC3R = LOW_THRESH;
                    __delay_ms(1000);
                    inc_state = INC;
                }
                break;
        }
        
    }
    }
    else
        while(1);
    return 0;
}
