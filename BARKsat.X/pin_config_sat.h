#ifndef PIN_CONFIG_SAT_H
#define	PIN_CONFIG_SAT_H

#include <xc.h>
#pragma config FNOSC = FRC  // 8e6 hz Timer -> fcy = 4e6 hz
/*
 * - Initialize pins
 * - Initialize pwm
 * - Initialize adc
 * ...
 */
// ROT_VAL 4-3-2-1
#define ROT_VAL_4               _RB4
#define ROT_VAL_3               _RA3
#define ROT_VAL_2               _RA2
#define ROT_VAL_1               _RB2
#define ROT_VAL_FLAG            _RA4
#define DISCARD_FLAG            _RB15

void pin_init()
{
    // Configure digital / analog 
    ANSA = 0x00;
    ANSB = 0x00;
    _ANSA0 = 1;
    _ANSA1 = 1;
    _ANSB0 = 1;
    _ANSB12 = 1;
    
    // Configure inputs / outputs
    _TRISA0 = 1;
    _TRISA1 = 1;
    _TRISB0 = 1;
    _TRISB2 = 0;
    _TRISA2 = 0;
    _TRISA3 = 0;
    _TRISB4 = 0;
    _TRISA4 = 0;
    _TRISB15 = 0;
    _TRISB14 = 1;
    _TRISB12 = 1;
    _TRISA6 = 0;
    
    // Configure initial values
    ROT_VAL_4 = 0;
    ROT_VAL_3 = 0;
    ROT_VAL_2 = 0;
    ROT_VAL_1 = 0;
    ROT_VAL_FLAG = 0;
    DISCARD_FLAG = 0;
    //...
}

void pwm_init()
{
    // 1500 for 1 kHz, increase to slow down
    int TRIGGER_SERVO_PWM_RATE = 1500; 
    
    // Using OC1, Pin 14 (RA6) for Trigger Servo PWM
    OC1CON1 = 0b0000;           // Clear OC1 configuration bits
    OC1CON2 = 0b0000;           // Clear OC2 configuration bits 
    OC1CON1bits.OCTSEL = 0b111; // Use System Clock
    OC1CON1bits.OCM = 0b110;    // Edge-aligned PWM mode
    OC1CON2bits.SYNCSEL = 0b11111;  // OC1 used for syncing
    
    OC1RS = 2*TRIGGER_SERVO_PWM_RATE;              
    OC1R = TRIGGER_SERVO_PWM_RATE;                // 50% Duty cycle
}

void adc_init()
{
    //...
}

void initialize() // include all above functions
{
    pin_init();
    pwm_init();
    adc_init();
}

#endif	/* PIN_CONFIG_SAT_H */

