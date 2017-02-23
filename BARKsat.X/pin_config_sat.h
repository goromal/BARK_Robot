#ifndef PIN_CONFIG_SAT_H
#define	PIN_CONFIG_SAT_H

#include <xc.h>
#pragma config FNOSC = FRC  // 8e6 hz Timer -> fcy = 4e6 hz
/*
 * - Initialize pins
 * - Initialize pwm
 * - Initialize adc
 * ...
 */

void pin_init()
{
    
}

void pwm_init()
{
    
}

void adc_init()
{
    
}

void initialize() // include all above functions
{
    pin_init();
    pwm_init();
    adc_init();
}

#endif	/* PIN_CONFIG_SAT_H */

