/*
 * File:   main.c
 * Authors: Andrew Torgesen
 *          Robert Monson
 *          Ben Himes
 *          Kade Roth
 * 
 * For Mechatronics.
 *
 * Created on February 22, 2017, 4:57 PM
 */

#include "interrupts.h"

int main(void) {
    initialize();
    set_robot_state(INIT);
    while(1);
    return 0;
}
