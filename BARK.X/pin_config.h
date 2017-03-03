#ifndef PIN_CONFIG_H
#define	PIN_CONFIG_H

#include <xc.h>
#pragma config FNOSC = FRC  // 8e6 hz Timer -> fcy = 4e6 hz
/*
 * - Initialize pins
 * - Initialize pwm
 * - Initialize adc
 * ...
 */
#define TRIGGER_NOTICE          _RB14
#define STEPPER_DIR_LEFT        _RB9
#define STEPPER_DIR_RIGHT       _RB8
#define SOLENOID_TRIGGER        _RB7

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
    
    // Configure initial values
    TRIGGER_NOTICE = 0;
    //...
    SOLENOID_TRIGGER = 0;
    // STEPPER DIR CONFIGS: ***********************
}

void pwm_init()
{
    // 1500 for 1 kHz, increase to slow down
    int SHOOTER_SERVO_PWM_RATE = 1500;      
    int STEPPER_MOTOR_PWM_RATE = 1500;
    int SHOOTER_DCMOT_PWM_RATE = 1500;
    
    // Using OC2, Pin 4 (RB0) for Shooter Servo PWM
    OC2CON1 = 0b0000;           // Clear OC1 configuration bits
    OC2CON2 = 0b0000;           // Clear OC2 configuration bits 
    OC2CON1bits.OCTSEL = 0b111; // Use System Clock
    OC2CON1bits.OCM = 0b110;    // Edge-aligned PWM mode
    OC2CON2bits.SYNCSEL = 0b11111;  // OC1 used for syncing
    
    OC2RS = 2*SHOOTER_SERVO_PWM_RATE;              
    OC2R = SHOOTER_SERVO_PWM_RATE;                // 50% Duty cycle
    
    // Using OC1, Pin 14 (RA6) for Stepper Motor PWM
    OC1CON1 = 0b0000;           // Clear OC1 configuration bits
    OC1CON2 = 0b0000;           // Clear OC2 configuration bits 
    OC1CON1bits.OCTSEL = 0b111; // Use System Clock
    OC1CON1bits.OCM = 0b110;    // Edge-aligned PWM mode
    OC1CON2bits.SYNCSEL = 0b11111;  // OC1 used for syncing
    
    OC1RS = 2*STEPPER_MOTOR_PWM_RATE;              
    OC1R = STEPPER_MOTOR_PWM_RATE;                // 50% Duty cycle
    
    // Using OC3, Pin 5 (RB1) for Shooter DC Motor PWM
    OC3CON1 = 0b0000;           // Clear OC1 configuration bits
    OC3CON2 = 0b0000;           // Clear OC2 configuration bits 
    OC3CON1bits.OCTSEL = 0b111; // Use System Clock
    OC3CON1bits.OCM = 0b110;    // Edge-aligned PWM mode
    OC3CON2bits.SYNCSEL = 0b11111;  // OC1 used for syncing
    
    OC3RS = 2*SHOOTER_DCMOT_PWM_RATE;              
    OC3R = SHOOTER_DCMOT_PWM_RATE;                // 50% Duty cycle
}

void adc_init()
{
    // No ADC on this PIC
}

void initialize() // include all above functions
{
    pin_init();
    pwm_init();
    adc_init();
}

#endif	/* PIN_CONFIG_H */