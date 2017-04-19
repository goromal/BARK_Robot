#ifndef PIN_CONFIG_H
#define	PIN_CONFIG_H

#include <xc.h>
#pragma config ICS = PGx3 // DEBUG
#pragma config OSCIOFNC = OFF
#pragma config FNOSC = FRC  // 8e6 hz Timer -> fcy = 4e6 hz
#define FCY                     4000000UL
#include <libpic30.h>       // For __delay functions

#define DC_TRIGGER              _LATA2
#define STEPPER_DIR_LEFT        _LATB9
#define STEPPER_DIR_RIGHT       _LATB8
#define SOLENOID_TRIGGER        _LATB7
#define IR_1_BUF                ADC1BUF12   // Pin 15, FRONT
#define IR_2_BUF                ADC1BUF11   // Pin 16, LEFT
#define IR_3_BUF                ADC1BUF10   // Pin 17, RIGHT
#define QRD_BUF                 ADC1BUF14   // Pin 8, Black Ball
#define BUTTON_1                _RA0
#define BUTTON_2                _RA1

void pin_init()
{
    // Configure digital / analog 
    ANSA = 0x00;
    ANSB = 0x00;
    _ANSA3 = 1;
    _ANSB14 = 1;
    _ANSB13 = 1;
    _ANSB12 = 1;
    
    LATA = 0;
    LATB = 0;
    // Configure inputs / outputs (unused configured as inputs)
    _TRISA0 = 1;
    _TRISA1 = 1;
    _TRISB0 = 0;
    _TRISB1 = 0;
    _TRISB2 = 1;
    _TRISA2 = 0;
    _TRISA3 = 1;
    _TRISB4 = 1;
    _TRISA4 = 1;
    _TRISB15 = 1;
    _TRISB14 = 1;
    _TRISB13 = 1;
    _TRISB12 = 1;
    _TRISA6 = 0;
    _TRISB9 = 0;
    _TRISB8 = 0;
    _TRISB7 = 0;
    
    // Configure CN Interrupt for Buttons (only)
    _CN2IE = 1;
    _CN2PDE = 1;
    _CN3IE = 1;
    _CN3PDE = 1;
    _CNIP = 6;
    _LATA1 = 0;
    _LATA0 = 0;
}

// 1500 for 1 kHz, increase to slow down proportionally
#define STEPPER_MOTOR_PWM_RATE  8000
#define STEPPER_MOTOR_PWM_SLOW  10000
#define SHOOTER_SERVO_PWM_PER   9999  
#define SHOOTER_SERVO_INIT      988
#define SHOOTER_SERVO_NEUTRAL   750
#define SHOOTER_SERVO_LEFT      275
#define SHOOTER_SERVO_RIGHT     1225
#define TRIGGER_SERVO_PWM_PER   9999
#define TRIGGER_SERVO_NEUTRAL   750
#define TRIGGER_SERVO_LEFT      500
#define TRIGGER_SERVO_RIGHT     1000

void pwm_init()
{
    // Using OC2, Pin 4 (RB0) for Trigger Servo PWM
    // will sync with Timer 4
    
    T4CONbits.TON = 0;              // DISABLED at first
    T4CONbits.TCKPS = 0b01;         // 1:8 pre-scale
    T4CONbits.TCS = 0;              // Internal clock source
    
    OC2CON1 = 0b0000;           // Clear OC1 configuration bits
    OC2CON2 = 0b0000;           // Clear OC2 configuration bits 
    OC2CON1bits.OCTSEL = 0b010; // Use Timer 4
    OC2CON1bits.OCM = 0b110;    // Edge-aligned PWM mode
    OC2CON2bits.SYNCSEL = 0b11111;  // OC1 used for syncing
    
    OC2RS = TRIGGER_SERVO_PWM_PER;  
    OC2R = TRIGGER_SERVO_NEUTRAL;
    _OC2IP = 4;
    _OC2IE = 0;                     // interrupt DISABLED at first
    _OC2IF = 0;
    
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
    _OC1IE = 0;                     // interrupt DISABLED at first
    _OC1IF = 0;
    
    
    // Using OC3, Pin 5 (RB1) for Shooter Servo Motor PWM
    // Will sync with Timer 3

    T3CONbits.TON = 0;              // DISABLED at first
    T3CONbits.TCKPS = 0b01;         // 1:8 pre-scale
    T3CONbits.TCS = 0;              // Internal clock source
    
    OC3CON1 = 0b0000;           // Clear OC1 configuration bits
    OC3CON2 = 0b0000;           // Clear OC2 configuration bits 
    OC3CON1bits.OCTSEL = 0b001; // Use Timer 3
    OC3CON1bits.OCM = 0b110;    // Edge-aligned PWM mode
    OC3CON2bits.SYNCSEL = 0b11111;  // OC1 used for syncing
    
    OC3RS = SHOOTER_SERVO_PWM_PER;  
    OC3R = SHOOTER_SERVO_INIT;
}

#define MAX_TIMER_COUNT     65535   // For 16-bit timer
#define T1_COUNT_MULT       15625   // For chosen oscillator and pre-scale

typedef struct {
   int extra_cycles;
} timer_t;

timer_t timer_1;
timer_t timer_2;

void timer_config()
{
    // TIMER 1
    T1CONbits.TON = 0; // begins off
    T1CONbits.TCS = 0; // internal clock source
    T1CONbits.TCKPS = 11; // 1:256 Pre-scaling
    TMR1 = 0;
    _T1IP = 5;
    _T1IE = 1;
    _T1IF = 0;
}

void adc_init()
{
	// use AVdd for positive reference
	_PVCFG = 0b00;	//              AD1CON2<15:14>, pg. 212-213 datasheet
	// use AVss for negative reference
	_NVCFG = 0;		//              AD1CON2<13>
	// make sure Tad is at least 600ns, see Table 29-41 datasheet
	_ADCS = 0b00000010;	// Tad = 3*Tcy = 750 ns > 600 ns
                        //          AD1CON3<7:0>, pg. 213 datasheet
	// use auto-convert
	_SSRC = 0b0111;		//          AD1CON1<7:4>, pg. 211 datasheet
	// use auto-sample
	_ASAM = 1;          //          AD1CON1<2>
	// choose a sample time >= 1 Tad, see Table 29-41 datasheet
	_SAMC = 0b00001;	//          AD1CON3<12:8>
	// scan inputs
	_CSCNA = 1;		//              AD1CON2<10>
	// choose which channels to scan, e.g. for ch AN12, set _CSS12 = 1;
	_CSS14 = 1;		// Pin 8 = AN14 AD1CSSH/L, pg. 217
    _CSS12 = 1;     // Pin 15 = AN12
    _CSS11 = 1;     // Pin 16 = AN11
    _CSS10 = 1;     // Pin 17 = AN10
	// set 12-bit resolution
	_MODE12 = 1;		//          AD1CON1<10>
	// use absolute decimal format
	_FORM = 0b00;		//          AD1CON1<9:8>
	// load results into buffer determined by converted channel, e.g. ch AN12 
    // results appear in ADC1BUF12
	_BUFREGEN = 1;        // AD1CON2<11>
	// interrupt rate should reflect number of analog channels used, e.g. if 
    // 5 channels, interrupt every 5th sample
	_SMPI = 0b11; // at every fourth completion   AD1CON2<6:2>
	_ADON = 1;		//              AD1CON1<15>
}

void initialize() // include all above functions
{
    pin_init();
    pwm_init();
    timer_config();
    adc_init();
}

#endif	/* PIN_CONFIG_H */