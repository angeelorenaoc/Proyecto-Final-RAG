#include "PWM_measure.h"

void init_pwm(pwm_config *cfg){
    /*------CONFIGURACION PWM DUTY--------*/
    *cfg = pwm_get_default_config();
    pwm_config_set_clkdiv_mode(cfg, PWM_DIV_B_HIGH);
    pwm_config_set_clkdiv(cfg, 50); 
}
void measure_duty_cycle(uint slice, uint32_t *dato) {
    pwm_set_enabled(slice, false);
    //printf("%d, %d\n", slice, pwm_get_counter(slice));
    uint32_t counting_rate = clock_get_hz(clk_sys) /100;
    uint32_t duty = pwm_get_counter(slice)*5e4 / counting_rate;
    printf("Duty: %d\n",duty);
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
