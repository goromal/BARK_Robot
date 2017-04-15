#ifndef INTERRUPTS_H
#define	INTERRUPTS_H

#include "helper_fns.h"

/*************************************************************************/
/******************** State Diagram Transition Outputs *******************/
/*************************************************************************/

typedef enum {INIT, LOC_CORN, TURN_AR, BAC_CORN, T_DROP, LET_ROLL, TO_MID_1, 
              MID_MID, TO_MID_2, WAIT, SHOOTING} robot_state_t;
robot_state_t robot_state;

void set_robot_state(robot_state_t state) // called in the main() function
{
    robot_state = state;
    switch(state)
    {
        case INIT: // Subsystem initializations
            set_trigger_servo_state(TO_NEUTRAL);
            set_dc_state(NOT_SPINNING);
            set_solenoid_state(DISARMED);
            set_stepper_state(STILL);
            set_sensor_state(NOT_READING);
            set_button_state(NOT_READING);
            set_shooter_servo_state(INIT_IDLE);
            _reset_timer_1(1);
            break;
        case LOC_CORN: // Spinning to locate the dispenser
            set_stepper_state(RIGHT);
            set_sensor_state(READING);
            break;
        case TURN_AR: // Turn so that back faces the dispenser
            set_stepper_state(LEFT);
            set_sensor_state(NOT_READING);
            break;
        case BAC_CORN: // Back into dispenser corner
            set_stepper_state(BACK);
            set_button_state(READING);
            break;
        case T_DROP: // Drop ball collector tower
            set_button_state(NOT_READING);
            set_solenoid_state(ARMED);
            break;
        case LET_ROLL:
            set_stepper_state(STILL);
            _reset_timer_1(2); // to let the balls roll away before driving
            break;
        case TO_MID_1:
            //OC1RS = 2*STEPPER_MOTOR_PWM_SLOW; // Drive away more slowly
            set_stepper_state(FORWARD);
            break;
        case MID_MID:
            set_stepper_state(STILL);
            _reset_timer_1(2);
            break;
        case TO_MID_2: // Drive the rest of the way to the middle of the arena
            set_stepper_state(FORWARD);
            set_solenoid_state(DISARMED);
            break;
        case WAIT: // Wait for an active goal to be detected
            set_stepper_state(STILL);
            set_shooter_servo_state(TO_NEUTRAL);
            set_sensor_state(READING);
            break;
        case SHOOTING: // Begin shooting and remain attentive to goal changes
            set_trigger_servo_state(TO_LEFT);
            set_dc_state(SPINNING); // CAUSING THE SHAKES (?))
            break;
    }
}

/************************************************************************/
/****************************** Interrupts ******************************/
/************************************************************************/

// TIMER
void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void)
{
    _T1IF = 0;
    switch(robot_state)
    {
        case INIT:
            if (handle_timer_1() == TRUE)
                set_robot_state(LOC_CORN);
            break;
        case LET_ROLL:
            if (handle_timer_1() == TRUE)
                set_robot_state(TO_MID_1);
            break;
        case MID_MID:
            if (handle_timer_1() == TRUE)
                set_robot_state(TO_MID_2);
            break;
        case SHOOTING: // In a deflection state
            if (handle_timer_1() == TRUE)
                set_sensor_state(READING);
            break;
    }
}

// BUTTONS
void __attribute__((interrupt, no_auto_psv)) _CNInterrupt(void)
{
    _CNIF = 0;

    switch(robot_state)
    {
        case BAC_CORN:
            if (BUTTON_1 == 1 && BUTTON_2 == 1)
                set_robot_state(T_DROP);
            break;
    }
}

// STEPPERS
void __attribute__((interrupt, no_auto_psv)) _OC1Interrupt(void)
{
    _OC1IF = 0;
    switch(robot_state)
    {
        case TURN_AR:
            num_stepper_cycles++;
            if (num_stepper_cycles >= TURN_180_CYCLES)
                set_robot_state(BAC_CORN);
            break;
        case T_DROP:
            num_stepper_cycles++;
            if (num_stepper_cycles >= B_CYCLES)
                set_robot_state(LET_ROLL);
            break;
        case TO_MID_1:
            num_stepper_cycles++;
            if (num_stepper_cycles >= F_CYCLES_1)
                set_robot_state(MID_MID);
            break;
        case TO_MID_2:
            num_stepper_cycles++;
            if (num_stepper_cycles >= F_CYCLES_2)
                set_robot_state(WAIT);
            break;
    }
}

// TRIGGER SERVO
void __attribute__((interrupt, no_auto_psv)) _OC2Interrupt(void)
{
    _OC2IF = 0;
    switch(trigger_servo_state)
    {
        case TO_NEUTRAL:
            if (OC2R == TRIGGER_SERVO_NEUTRAL)
                set_trigger_servo_state(INIT_IDLE);
            else
                check_trigger_position(TRIGGER_SERVO_NEUTRAL);
            break;
        case TO_LEFT:
            if (OC2R == TRIGGER_SERVO_LEFT)
                set_trigger_servo_state(TO_RIGHT);
            else
                check_trigger_position(TRIGGER_SERVO_LEFT);
            break;
        case TO_RIGHT:
            if (OC2R == TRIGGER_SERVO_RIGHT)
                set_trigger_servo_state(TO_LEFT);
            else
                check_trigger_position(TRIGGER_SERVO_RIGHT);
            break;
    }
}

// IR SENSORS
void __attribute__((interrupt, no_auto_psv)) _ADC1Interrupt(void)
{   
    static int avg_ir_1 = 0;
    static int avg_ir_2 = 0;
    static int avg_ir_3 = 0;
    static int avg_qrd = 0;
    
    switch (robot_state)
    {
        case LOC_CORN:
            avg_ir_1 = _avg_ir_1(IR_1_BUF, FALSE);
            
            if (avg_ir_1 > IR_THRESHOLD)
                set_robot_state(TURN_AR);
            break;
        case WAIT: // For initial orientation to goal 
            avg_ir_1 = _avg_ir_1(IR_1_BUF, FALSE);
            avg_ir_2 = _avg_ir_2(IR_2_BUF, FALSE);
            avg_ir_3 = _avg_ir_3(IR_3_BUF, FALSE);
            
            if (avg_ir_1 > IR_THRESHOLD)
            {
                if (shooter_servo_state != TO_NEUTRAL)
                    set_shooter_servo_state(TO_NEUTRAL);
                _avg_ir_2(0, TRUE);
                _avg_ir_3(0, TRUE);
                set_robot_state(SHOOTING);
            }
            else if (avg_ir_2 > IR_THRESHOLD)
            {
                if (shooter_servo_state != TO_LEFT)
                    set_shooter_servo_state(TO_LEFT);
                _avg_ir_1(0, TRUE);
                _avg_ir_3(0, TRUE);
                set_robot_state(SHOOTING);
            }
            else if (avg_ir_3 > IR_THRESHOLD)
            {
                if (shooter_servo_state != TO_RIGHT)
                    set_shooter_servo_state(TO_RIGHT);
                _avg_ir_1(0, TRUE);
                _avg_ir_2(0, TRUE);
                set_robot_state(SHOOTING);
            }
            break;
        case SHOOTING:
            avg_ir_1 = _avg_ir_1(IR_1_BUF, FALSE);
            avg_ir_2 = _avg_ir_2(IR_2_BUF, FALSE);
            avg_ir_3 = _avg_ir_3(IR_3_BUF, FALSE);
            
            if (avg_ir_1 > IR_THRESHOLD && shooter_servo_state != TO_NEUTRAL)
            {
                _avg_ir_2(0, TRUE);
                _avg_ir_3(0, TRUE);
                set_shooter_servo_state(TO_NEUTRAL);
            }
            if (avg_ir_2 > IR_THRESHOLD && shooter_servo_state != TO_LEFT)
            {
                _avg_ir_1(0, TRUE);
                _avg_ir_3(0, TRUE);
                set_shooter_servo_state(TO_LEFT);
            }
             if (avg_ir_3 > IR_THRESHOLD && shooter_servo_state != TO_RIGHT)
             {
                _avg_ir_1(0, TRUE);
                _avg_ir_2(0, TRUE);
                set_shooter_servo_state(TO_RIGHT);
             }
            
            // check for black ball
            avg_qrd = _avg_qrd(QRD_BUF, FALSE);
            
            if (avg_qrd > QRD_THRESHOLD_L && avg_qrd < QRD_THRESHOLD_H)
            {
                set_sensor_state(NOT_READING);
                set_shooter_servo_state(INIT_IDLE);
                _reset_timer_1(2); // Deflect for 2 seconds
            }
            break;
    }
    _AD1IF = 0;
}

#endif	/* INTERRUPTS_H */

