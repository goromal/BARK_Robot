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
    // Toggle features on and off here; eventually all will be activated
}

int main(void) {
    initialize();
    interrupt_init();
    while(1);
    return 0;
}
