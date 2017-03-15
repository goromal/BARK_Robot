#ifndef PIN_CONFIG_H
#define	PIN_CONFIG_H

#include <xc.h>
//#pragma config ICS = PGx3 // DEBUG
#pragma config FNOSC = FRC  // 8e6 hz Timer -> fcy = 4e6 hz
/*
 * - Initialize pins
 * - Initialize pwm
 * - Initialize adc
 * ...
 */
#define TRIGGER_NOTICE          _LATB14
#define STEPPER_DIR_LEFT        _LATB9
#define STEPPER_DIR_RIGHT       _LATB8
#define SOLENOID_TRIGGER        _LATB7

void pin_init()
{
    // Configure digital / analog 
    ANSA = 0x00; // No analog pins used on main PIC
    ANSB = 0x00;
    
    // Configure inputs / outputs
    _TRISA0 = 1;
    _TRISA1 = 1;
    _TRISB0 = 0;
    _TRISB1 = 0;
    _TRISB2 = 1;
    _TRISA2 = 1;
    _TRISA3 = 1;
    _TRISB4 = 1;
    _TRISA4 = 1;
    _TRISB15 = 1;
    _TRISB14 = 0;
    _TRISA6 = 0;
    _TRISB9 = 0;
    _TRISB8 = 0;
    _TRISB7 = 0;
}

// 1500 for 1 kHz, increase to slow down proportionally
#define SHOOTER_SERVO_PWM_RATE 1500
#define STEPPER_MOTOR_PWM_RATE 5000
#define SHOOTER_DCMOT_PWM_RATE 1500

void pwm_init()
{
    // Using OC2, Pin 4 (RB0) for Shooter Servo PWM
    OC2CON1 = 0b0000;           // Clear OC1 configuration bits
    OC2CON2 = 0b0000;           // Clear OC2 configuration bits 
    OC2CON1bits.OCTSEL = 0b111; // Use System Clock
    OC2CON1bits.OCM = 0b110;    // Edge-aligned PWM mode
    OC2CON2bits.SYNCSEL = 0b11111;  // OC1 used for syncing
    
    OC2RS = 2*SHOOTER_SERVO_PWM_RATE;    
    OC2R = 0;
    
    // Using OC1, Pin 14 (RA6) for Stepper Motor PWM
    // will sync with Timer 2
    
    T2CONbits.TON = 0;              // DISABLED at first
    T2CONbits.TCKPS = 0b00;         // 1:1 pre-scale
    T2CONbits.T32 = 0;              // 16-bit timer
    T2CONbits.TCS = 0;              // Internal clock source
    OC1CON1 = 0b0000;               // Clear OC1 configuration bits
    OC1CON2 = 0b0000;               // Clear OC2 configuration bits 
    OC1CON1bits.OCTSEL = 0b000;     // Use Timer2
    OC1CON1bits.OCM = 0b110;        // Edge-aligned PWM mode
    OC1CON2bits.SYNCSEL = 0b11111;  // OC1 used for syncing
    
    OC1RS = 2*STEPPER_MOTOR_PWM_RATE; 
    OC1R = 0;
    _OC1IP = 4;
    _OC1IE = 0;                     // DISABLED at first
    _OC1IF = 0;
    
    
    // Using OC3, Pin 5 (RB1) for Shooter DC Motor PWM
    OC3CON1 = 0b0000;           // Clear OC1 configuration bits
    OC3CON2 = 0b0000;           // Clear OC2 configuration bits 
    OC3CON1bits.OCTSEL = 0b111; // Use System Clock
    OC3CON1bits.OCM = 0b110;    // Edge-aligned PWM mode
    OC3CON2bits.SYNCSEL = 0b11111;  // OC1 used for syncing
    
    OC3RS = 2*SHOOTER_DCMOT_PWM_RATE;    
    OC3R = 0;
}

void timer_config()
{
    // TIMER 1
    T1CONbits.TON = 0; // begins off
    T1CONbits.TCS = 0; // internal clock source
    T1CONbits.TCKPS = 11; // 1:256 Pre-scaling
    TMR1 = 0;
}

void adc_init()
{
    // No ADC on this PIC
}

void initialize() // include all above functions
{
    pin_init();
    pwm_init();
    timer_config();
    adc_init();
}

#endif	/* PIN_CONFIG_H */