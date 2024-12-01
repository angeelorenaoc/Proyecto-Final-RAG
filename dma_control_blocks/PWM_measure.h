#include <stdio.h>
#include "pico/stdlib.h"
#include <stdint.h>
#include <stdlib.h> 
#include "hardware/sync.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "hardware/timer.h"
#include "hardware/irq.h"



#ifndef FuncPollInt_h_
#define FuncPollInt_h_

//extern pwm_config cfg;
void init_pwm(pwm_config *);
void measure_duty_cycle(uint,uint32_t *);
void PWM_config_alarm();
#endif