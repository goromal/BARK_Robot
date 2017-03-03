#ifndef INTERRUPTS_H
#define	INTERRUPTS_H

#include "pin_config.h"
/*
 * - Define all interrupt functions, effectively implementing the robot's
 *   state diagram.
 * - Define all supporting functions for the interrupt functions
 */
/*    States    */
// might want several sets of these for different things like steppers, etc.
enum {STATIONARY, C_SQUARE, CC_SQUARE} robot_state;
enum {FORWARD, LEFT, RIGHT, BACK} stepper_state;

void init_state()
{
    // Can set multiple initial states, as mentioned above...
    robot_state = STATIONARY;
    stepper_state = FORWARD;
}

/*    Interrupts    */

// STEPPER PWM SUPPORTS AND INTERRUPT
void _handle_cw_turn() // Handles turn transitions during a clockwise turn
{
    // A leg consists of a forward movement and right turn
    static int legs_left = 4; // Assumes creation of full square
    switch(stepper_state)
    {
        case FORWARD: // decrement legs_left and begin to turn
            stepper_state = RIGHT;
}

void _handle_ccw_turn() // same as _handle_cw_turn(), but counterclockwise
{
    static int legs_left = 4; // Assumes full square
    
}

#define FORWARD_CYCLES 400
#define TURN_CYCLES 300
void __attribute__((interrupt, no_auto_psv)) _OC1Interrupt(void)
{
    static int num_cycles = 0;
    _OC1IF = 0;
    num_cycles++;
    
    switch(robot_state)
    {
        case C_SQUARE:
        {
            switch(stepper_state)
            {
                case FORWARD:
                    if (num_cycles >= FORWARD_CYCLES)
                        _handle_cw_turn();
                    break;
                case RIGHT:
                    if (num_cycles >= TURN_CYCLES)
                        _handle_cw_turn();
                    break;
                default:
                    break; // ERROR
            }
            break;
        }
        case CC_SQUARE:
        {
            switch(stepper_state)
            {
                case FORWARD:
                    if (num_cycles >= FORWARD_CYCLES)
                        _handle_ccw_turn();
                    break;
                case LEFT:
                    if (num_cycles >= TURN_CYCLES)
                        _handle_ccw_turn();
                    break;
                default:
                    break; // ERROR
            }
            break;
        }
        default: 
            break; // ERROR
    }
    
    
}

#endif	/* INTERRUPTS_H */

