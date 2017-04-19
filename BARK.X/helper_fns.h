#ifndef HELPER_FNS_H
#define	HELPER_FNS_H

#include "pin_config.h"

/***********************************************************************/
/************************** Tunable Constants **************************/
/***********************************************************************/

#define B_CYCLES        75
#define F_CYCLES_1      100  // Determines how far it will drive with the solenoid armed
#define F_CYCLES_2      600  // How far it will drive with the solenoid disarmed
#define TURN_180_CYCLES 310  // Count for turning 180 degrees
#define IR_THRESHOLD    950
#define QRD_THRESHOLD   3500 //2200//2050 // QRD low threshold

/************************************************************************/
/********************* Helper Function Declarations *********************/
/************************************************************************/
typedef enum {TRUE, FALSE} my_bool;

typedef enum {ARMED, DISARMED} solenoid_state_t;
solenoid_state_t solenoid_state;

typedef enum {INIT_IDLE, TO_LEFT, TO_NEUTRAL, TO_RIGHT} servo_state_t;
servo_state_t shooter_servo_state;
servo_state_t trigger_servo_state;

typedef enum {STILL, FORWARD, LEFT, RIGHT, BACK} stepper_state_t;
stepper_state_t stepper_state;
int num_stepper_cycles;

typedef enum {NOT_SPINNING, SPINNING} dc_state_t;
dc_state_t dc_state;

typedef enum {READING, NOT_READING} sensor_state_t;
sensor_state_t sensor_state;
sensor_state_t button_state;

void _reset_timer_1(float count);
my_bool handle_timer_1();
void migrate_shooter_servo(int oc_count);
void check_trigger_position(int ocr_val);
void set_solenoid_state(solenoid_state_t state);
void set_shooter_servo_state(servo_state_t state);
void set_trigger_servo_state(servo_state_t state);
void set_button_state(sensor_state_t state);
void set_sensor_state(sensor_state_t state);
void set_stepper_state(stepper_state_t state);
void set_dc_state(dc_state_t state);
int _avg_ir_1(int new_val, my_bool new_measurement);
int _avg_ir_2(int new_val, my_bool new_measurement);
int _avg_ir_3(int new_val, my_bool new_measurement);
int _avg_qrd(int new_val, my_bool new_measurement);

/************************************************************************/
/**************** Subsystem State Transition Definitions ****************/
/************************************************************************/

void set_solenoid_state(solenoid_state_t state)
{
    solenoid_state = state;
    switch(state)
    {
        case ARMED:
            SOLENOID_TRIGGER = 1;
            break;
        case DISARMED:
            SOLENOID_TRIGGER = 0;
            break;
    }
}

void set_shooter_servo_state(servo_state_t state)
{
    shooter_servo_state = state;
    switch(state)
    {
        case INIT_IDLE:
            T3CONbits.TON = 1;
            migrate_shooter_servo(SHOOTER_SERVO_INIT);
            break;
        case TO_LEFT:
            T3CONbits.TON = 1;
            migrate_shooter_servo(SHOOTER_SERVO_LEFT);
            break;
        case TO_NEUTRAL:
            T3CONbits.TON = 1;
            migrate_shooter_servo(SHOOTER_SERVO_NEUTRAL);
            break;
        case TO_RIGHT:
            T3CONbits.TON = 1;
            migrate_shooter_servo(SHOOTER_SERVO_RIGHT);
            break;
    }
}

void set_trigger_servo_state(servo_state_t state)
{
    trigger_servo_state = state;
    switch(state)
    {
        case INIT_IDLE:
            T4CONbits.TON = 0;
            _OC2IE = 0;
            break;
        case TO_LEFT:
            T4CONbits.TON = 1;
            _OC2IE = 1;
            break;
        case TO_NEUTRAL:
            T4CONbits.TON = 1;
            _OC2IE = 1;
            break;
        case TO_RIGHT:
            T4CONbits.TON = 1;
            _OC2IE = 1;
            break;
    }
}

void set_button_state(sensor_state_t state)
{
    button_state = state;
    switch(state)
    {
        case READING:
            _CNIF = 0;
            _CNIE = 1;
            break;
        case NOT_READING:
            _CNIF = 0;
            _CNIE = 0;
            break;
    }
}

void set_sensor_state(sensor_state_t state)
{
    sensor_state = state;
    switch(state)
    {
        case READING:
            _avg_ir_1(0, TRUE);
            _avg_ir_2(0, TRUE);
            _avg_ir_3(0, TRUE);
            _avg_qrd(0, TRUE);
            _AD1IE = 1;
            _AD1IP = 6;
            _AD1IF = 0;
            break;
        case NOT_READING:
            _AD1IE = 0;
            _AD1IP = 6;
            _AD1IF = 0;
            break;
    }
}

void set_stepper_state(stepper_state_t state)
{
    stepper_state = state;
    switch(state)
    {
        case STILL:
            T2CONbits.TON = 0; 
            _OC1IE = 0;
            OC1R = 0;
            STEPPER_DIR_RIGHT = 0;
            STEPPER_DIR_LEFT = 0;
            break;
        case FORWARD:
            T2CONbits.TON = 1; 
            _OC1IE = 1;
            OC1R = STEPPER_MOTOR_PWM_RATE; // 50% duty cycle for stepper motors
            STEPPER_DIR_RIGHT = 0; // clockwise
            STEPPER_DIR_LEFT = 1; // counter clockwise
            break;
        case LEFT:
            T2CONbits.TON = 1; 
            _OC1IE = 1;
            OC1R = STEPPER_MOTOR_PWM_RATE;
            STEPPER_DIR_RIGHT = 0;
            STEPPER_DIR_LEFT = 0;
            break;
        case RIGHT:
            T2CONbits.TON = 1; 
            _OC1IE = 1;
            OC1R = STEPPER_MOTOR_PWM_RATE;
            STEPPER_DIR_RIGHT = 1;
            STEPPER_DIR_LEFT = 1;
            break;
        case BACK:
            T2CONbits.TON = 1; 
            _OC1IE = 1;
            OC1R = STEPPER_MOTOR_PWM_RATE;
            STEPPER_DIR_RIGHT = 1;
            STEPPER_DIR_LEFT = 0;
            break;
    }
    num_stepper_cycles = 0;
}

void set_dc_state(dc_state_t state)
{
    dc_state = state;
    switch(state)
    {
        case NOT_SPINNING:
            DC_TRIGGER = 0;
            break;
        case SPINNING:
            DC_TRIGGER = 1;
            break;
    }
}

/**************************************************************************/
/**************************** Helper Functions ****************************/
/**************************************************************************/

void migrate_shooter_servo(int oc_count) // perhaps while loop will interrupt other functions (good...)
{
    while (OC3R != oc_count)
    {
        if (OC3R > oc_count)
            OC3R -= 1;
        else
            OC3R += 1;
        __delay_ms(1);
    }
}

void _reset_timer_1(float seconds) // Reset timer with specific period
{
    // Truncates to integer
    timer_1.extra_cycles = 1.0 * T1_COUNT_MULT * seconds / MAX_TIMER_COUNT;
    
    TMR1 = 0;
    // Set count for the first cycle (the only non-full cycle if extra_cycles >= 1)
    PR1 = T1_COUNT_MULT * seconds - timer_1.extra_cycles * MAX_TIMER_COUNT;
    _T1IE = 1;
    T1CONbits.TON = 1; // Only turning off is necessary outside of this function
}

my_bool handle_timer_1()
{
    if (timer_1.extra_cycles > 0)
    {
        timer_1.extra_cycles--;
        return FALSE;
    }
    else
    {
        T1CONbits.TON = 0;
        TMR1 = 0;
        _T1IE = 0;
        return TRUE;
    }
}

#define SERVO_INCREMENT 10
void check_trigger_position(int ocr_val)
{
    if (OC2R > ocr_val)
        OC2R -= SERVO_INCREMENT;
    else
        OC2R += SERVO_INCREMENT;
}

int _avg_ir_1(int new_val, my_bool new_measurement)
{
    static int RunSum = 0;
    static int Buffer[32] = {0};
    static unsigned char Newest = 0;
    static unsigned char Oldest = 1;
    if (new_measurement == TRUE)
    {
        int i = 0;
        for (i = 0; i < 32; i++)
            Buffer[i] = 0;
        RunSum = 0;
        Newest = 0;
        Oldest = 1;
    }
    // update running sum: remove oldest value and add in newest value
    RunSum = RunSum - Buffer[Oldest] + new_val;
    // put the new value into the buffer
    Buffer[Newest] = new_val;
    // update indices by incrementing, modulo 32
    Newest = (Newest + 1) & 0x1F;
    Oldest = (Oldest + 1) & 0x1F;
    // divide by 32 and return
    return (RunSum >> 5);
}
int _avg_ir_2(int new_val, my_bool new_measurement)
{
    static int RunSum = 0;
    static int Buffer[32] = {0};
    static unsigned char Newest = 0;
    static unsigned char Oldest = 1;
    if (new_measurement == TRUE)
    {
        int i = 0;
        for (i = 0; i < 32; i++)
            Buffer[i] = 0;
        RunSum = 0;
        Newest = 0;
        Oldest = 1;
    }
    RunSum = RunSum - Buffer[Oldest] + new_val;
    Buffer[Newest] = new_val;
    Newest = (Newest + 1) & 0x1F;
    Oldest = (Oldest + 1) & 0x1F;
    return (RunSum >> 5);
}
int _avg_ir_3(int new_val, my_bool new_measurement)
{
    static int RunSum = 0;
    static int Buffer[32] = {0};
    static unsigned char Newest = 0;
    static unsigned char Oldest = 1;
    if (new_measurement == TRUE)
    {
        int i = 0;
        for (i = 0; i < 32; i++)
            Buffer[i] = 0;
        RunSum = 0;
        Newest = 0;
        Oldest = 1;
    }
    RunSum = RunSum - Buffer[Oldest] + new_val;
    Buffer[Newest] = new_val;
    Newest = (Newest + 1) & 0x1F;
    Oldest = (Oldest + 1) & 0x1F;
    return (RunSum >> 5);
}

int _avg_qrd(int new_val, my_bool new_measurement)
{
    static int RunSum = 0;
    static int Buffer[4] = {0};
    static unsigned char Newest = 0;
    static unsigned char Oldest = 1;
    if (new_measurement == TRUE)
    {
        int i = 0;
        for (i = 0; i < 4; i++)
            Buffer[i] = 0;
        RunSum = 0;
        Newest = 0;
        Oldest = 1;
    }
    RunSum = RunSum - Buffer[Oldest] + new_val;
    Buffer[Newest] = new_val;
    Newest = (Newest + 1) & 0x03;
    Oldest = (Oldest + 1) & 0x03;
    return (RunSum >> 2);
}

#endif	/* HELPER_FNS_H */

