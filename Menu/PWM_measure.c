#include "PWM_measure.h"
//uint32_t slice_num_motor = 0;
/*void init_pwm(pwm_config *cfg1 ,pwm_config *cfg2){

    *cfg1 = pwm_get_default_config();
    pwm_config_set_clkdiv_mode(cfg1, PWM_DIV_B_HIGH);
    pwm_config_set_clkdiv(cfg1, 50);

    // Enable PWM
    *cfg2 = pwm_get_default_config(); 
    pwm_config_set_clkdiv(cfg2, 10);
    pwm_config_set_wrap(cfg2, 1e3);
    gpio_set_function(PIN_PWM_ENABLE, GPIO_FUNC_PWM);
    slice_num_motor = pwm_gpio_to_slice_num(PIN_PWM_ENABLE);
    pwm_set_chan_level(slice_num_motor, PWM_CHA, PWM_DUTY_ZERO);
    pwm_init(slice_num_motor, cfg2, true);
}*/
void measure_duty_cycle(uint slice, uint32_t *dato) {
    pwm_set_enabled(slice, false);
    //printf("%d, %d\n", slice, pwm_get_counter(slice));
    uint32_t counting_rate = clock_get_hz(clk_sys) /100;
    uint32_t duty = pwm_get_counter(slice)*5e4 / counting_rate;
    //printf("Duty: %d\n",duty);
    *dato |= (uint16_t)duty;
}
/*void PWM_config_alarm(){
    pwm_init(slice_num, &cfg, false);
    //CONFIGURACIÓN ALARMA 10MS
    gpio_set_function(MEASURE_PIN, GPIO_FUNC_PWM);
    now = get_absolute_time();
    target = delayed_by_ms(now, COUNT_MEASURE_TIME);
    hardware_alarm_set_target(alarm1, target);
    slice_num = pwm_gpio_to_slice_num(MEASURE_PIN);
    pwm_set_enabled(slice_num, true);

}*/
