#ifndef INTERRUPTS_H
#define	INTERRUPTS_H

#include "pin_config.h"
/*
 * - Define all interrupt functions, effectively implementing the robot's
 *   state diagram.
 * - Define all supporting functions for the interrupt functions
 */

/*    States    */
typedef enum {STATIONARY, C_SQUARE, CC_SQUARE} robot_state_t;
robot_state_t robot_state;

typedef enum {STILL, FORWARD, LEFT, RIGHT, BACK} stepper_state_t;
stepper_state_t stepper_state;

/*    Necessary Declarations    */
void _reset_timer_1(int count);
void set_stepper_state(stepper_state_t state);
void set_robot_state(robot_state_t state);
void _handle_cw_turn();
void _handle_ccw_turn();

/*    Hierarchical state transitions and state-pin relationships    */
void set_stepper_state(stepper_state_t state)
{
    stepper_state = state;
    switch(state)
    {
        case STILL:
            OC1R = 0;
            STEPPER_DIR_LEFT = 0;
            STEPPER_DIR_RIGHT = 0;
            break;
        case FORWARD:
            OC1R = STEPPER_MOTOR_PWM_RATE; // 50% duty cycle for stepper motors
            STEPPER_DIR_LEFT = 1; // counter clockwise
            STEPPER_DIR_RIGHT = 0; // clockwise
            break;
        case LEFT:
            //OC1R = STEPPER_MOTOR_PWM_RATE;==============================
            OC1R = .5*STEPPER_MOTOR_PWM_RATE;
            STEPPER_DIR_LEFT = 0;
            STEPPER_DIR_RIGHT = 0;
            break;
        case RIGHT:
            OC1R = STEPPER_MOTOR_PWM_RATE;
            STEPPER_DIR_LEFT = 1;
            STEPPER_DIR_RIGHT = 1;
            break;
        case BACK:
            OC1R = STEPPER_MOTOR_PWM_RATE;
            STEPPER_DIR_LEFT = 0;
            STEPPER_DIR_RIGHT = 1;
            break;
    }
}

#define STATIONARYCOUNT 0xFFFF
void set_robot_state(robot_state_t state) // called in the main() function
{
    robot_state = state;
    switch(state)
    {
        case STATIONARY:
            set_stepper_state(STILL);
            _reset_timer_1(STATIONARYCOUNT);
            TRIGGER_NOTICE = 0; //
            SOLENOID_TRIGGER = 0; //
            break;
        case C_SQUARE:
            set_stepper_state(FORWARD);
            break;
        case CC_SQUARE:
            set_stepper_state(FORWARD);
            break;
    }
}

/*    Interrupts    */

// TIMER SUPPORTS AND INTERRUPTS
// Only resort to using timer 2, 3, etc if timer 1 is theoretically supposed to be in use
void _reset_timer_1(int count) // Reset timer with specific period
{
    TMR1 = 0;
    PR1 = count;
    T1CONbits.TON = 1; // Only turning off is necessary outside of this function
}

void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void)
{
    _T1IF = 0;

    switch(robot_state)
    {
        case STATIONARY:
            T1CONbits.TON = 0;
            set_robot_state(C_SQUARE);
            break;
        default:
            break;
    }
    
}

// STEPPER PWM SUPPORTS AND INTERRUPT
void _handle_cw_turn() // Handles (choreographed) turn transitions during a clockwise turn
{
    // A leg consists of a forward movement and right turn
    static int legs_left = 4; // Assumes creation of full square
    switch(stepper_state)
    {
        case FORWARD: // decrement legs_left and begin to turn
            set_stepper_state(RIGHT);
            legs_left--;
            break;
        case RIGHT:
            if (legs_left == 0)
            {
                set_robot_state(CC_SQUARE);
                legs_left = 4;
            }
            else
                set_stepper_state(FORWARD);
            break;
    }
}

void _handle_ccw_turn() // same as _handle_cw_turn(), but counterclockwise
{
    static int legs_left = 4; // Assumes full square
    switch(stepper_state)
    {
        case FORWARD: // decrement legs_left and begin to turn
            set_stepper_state(LEFT);
            legs_left--;
            break;
        case LEFT:
            if (legs_left == 0)
            {
                set_robot_state(STATIONARY);
                legs_left = 4;
            }
            else
                set_stepper_state(FORWARD);
            break;
    }
}

#define FORWARD_CYCLES 1000
#define TURN_CYCLES 900
void __attribute__((interrupt, no_auto_psv)) _OC1Interrupt(void)
{
    static int num_cycles = 0;
    _OC1IF = 0;
    num_cycles++;
    
    switch(robot_state)
    {
        case C_SQUARE:
            switch(stepper_state)
            {
                case FORWARD:
                    if (num_cycles >= FORWARD_CYCLES)
                    {
                        _handle_cw_turn();
                        num_cycles = 0;
                    }
                    break;
                case RIGHT:
                    if (num_cycles >= TURN_CYCLES)
                    {
                        _handle_cw_turn();
                        num_cycles = 0;
                    }
                    break;
                default:
                    break; // ERROR
            }
            break;
        case CC_SQUARE:
            switch(stepper_state)
            {
                case FORWARD:
                    if (num_cycles >= FORWARD_CYCLES)
                    {
                        _handle_ccw_turn();
                        num_cycles = 0;
                    }
                    break;
                case LEFT:
                    if (num_cycles >= TURN_CYCLES)
                    {
                        _handle_ccw_turn();
                        num_cycles = 0;
                    }
                    break;
                default:
                    break; // ERROR
            }
            break;
        default: 
            break; // ERROR
    }
}

#endif	/* INTERRUPTS_H */

