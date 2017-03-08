/*
 * File:   main.c
 * Authors: Andrew Torgesen
 *          Robert Monson
 *          Ben Himes
 *          Kade Roth
 *
 * Created on February 22, 2017, 4:57 PM
 */

#include "interrupts.h"

void interrupt_init()
{
    // Timers
    _T1IP = 4;
    _T1IE = 1;
    _T1IF = 0;
    // Toggle features on and off here; eventually all will be activated
    
    // STEPPER PWM
    _OC1IP = 4;
    _OC1IE = 1; // Will trigger once every cycle, or 1.8/4 degrees
    _OC1IF = 0;
}

int main(void) {
    initialize();
    set_robot_state(STATIONARY);
    interrupt_init();
    while(1);
    return 0;
}
