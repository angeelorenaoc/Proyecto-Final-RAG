#include <stdio.h>
#include "main.h"

uint32_t slice_num = 0 ;
uint32_t dutymask = 0;
uint32_t measured_duty_cycle = 0 ;
absolute_time_t now, target, now_w, target_w;
const uint alarm1 = 1, watchdog_XBEE = 2;
uint32_t mask_motor = 0xF << 10;
const uint32_t gpio_linea = 0x3;
bool band_right = false;
bool band_left = false;
uint32_t pwm_level = 0;
uint32_t pwm_level_right = 3000;
uint32_t pwm_level_left = 3000;
uint sliceNumMotorA = 0, sliceNumMotorB = 0;

void corregir_mov(uint gpio, uint32_t events) {
    printf("El gpio que genera la interrupcion es: %d, y la mascara es: %x \n", gpio, events);
    if(events != GPIO_IRQ_EDGE_FALL){

        switch (gpio)
        {
        case 0:
            printf("IZQUIERDA ..............\n");
            band_left = true;
            break;
        case 1:
            printf("DERECHA ..............\n");
            band_right = true;
            break;    
        default:
            break;
        }
    }
    gpio_acknowledge_irq(gpio, events);
}

void alarm_duty(uint alarm){
    // Calcula el duty de la seañl cada 10ms
    measure_duty_cycle(slice_num,&Balizas[Baliza-1].WORD);
    Flags.BITS.Capture_ADC = 0;
    printf("CAPTURANDO RSSI: %d, CR: %d\n", Balizas[Baliza-1].BITS.RSSI, CR);
}
void watchdog_irq(uint alarm){
    if(!Flags.BITS.Capture){
        now_w = get_absolute_time();
        target_w = delayed_by_ms(now, WATCHDOG_TIME);
        hardware_alarm_set_target(watchdog_XBEE, target_w);
        printf("Reenviando\n");
    }

}

int main() {
#ifndef uart_default
#warning dma/control_blocks example requires a UART
#else
    stdio_init_all();
    sleep_ms(4000);

    gpio_init(MEASURE_PIN);
    gpio_init_mask(mask_motor | (1<<MEASURE_PIN)| gpio_linea);
    gpio_set_dir_out_masked(mask_motor | (1<<MEASURE_PIN)| (1<<RESET_XBEE_PIN));

    gpio_set_irq_enabled_with_callback(0, GPIO_IRQ_EDGE_RISE|GPIO_IRQ_EDGE_FALL, true, &corregir_mov);
    gpio_set_irq_enabled_with_callback(1, GPIO_IRQ_EDGE_RISE|GPIO_IRQ_EDGE_FALL, true, &corregir_mov);

    //------------------------------------ UART------------------------------------ 
    uart_init(UART_ID, BAUD_RATE);

    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select
    gpio_set_function(UART_TX_PIN, UART_FUNCSEL_NUM(UART_ID, UART_TX_PIN));
    gpio_set_function(UART_RX_PIN, UART_FUNCSEL_NUM(UART_ID, UART_RX_PIN));

    int __unused actual = uart_set_baudrate(UART_ID, BAUD_RATE);

    // Set UART flow control CTS/RTS, we don't want these, so turn them off
    uart_set_hw_flow(UART_ID, false, false);

    // Set our data format
    uart_set_format(UART_ID, DATA_BITS, STOP_BITS, PARITY);

    uart_set_fifo_enabled(UART_ID, true);
    
    // Select correct interrupt for the UART we are using
    int UART_IRQ = UART_ID == uart0 ? UART0_IRQ : UART1_IRQ;
     // And set up and enable the interrupt handlers
    irq_set_exclusive_handler(UART_IRQ, Capture);
    irq_set_enabled(UART_IRQ, true);

    // Now enable the UART to send interrupts - RX only
    uart_set_irq_enables(UART_ID, true, false);

    //Configuración de las alarmas
    hardware_alarm_claim(alarm1);
    hardware_alarm_set_callback(alarm1, &alarm_duty);

    //Inicialización del PWM
    //init_pwm(&cfg1, &cfg2);
    cfg1 = pwm_get_default_config();
    pwm_config_set_clkdiv_mode(&cfg1, PWM_DIV_B_HIGH);
    pwm_config_set_clkdiv(&cfg1, 50);

    // Enable PWM
    gpio_set_function(PIN_PWM_ENABLEA, GPIO_FUNC_PWM);
    // Determine the PWM slice connected to GPIO: PWM_GPIO_CHA
    sliceNumMotorA = pwm_gpio_to_slice_num(PIN_PWM_ENABLEA);
    // Set period for frequency divisor
    pwm_set_clkdiv_int_frac(sliceNumMotorA, PWM_DIV_INTEGER, PWM_DIV_FRAC); // What frequency enters to the PWM?
    // Set top (wrap) value (Determines the frequency)
    pwm_set_wrap(sliceNumMotorA, PWM_TOP_VALUE);
    // Set zero duty
    pwm_set_chan_level(sliceNumMotorA, PWM_CHA, PWM_DUTY_ZERO);
    // Enable PWM
    pwm_set_enabled(sliceNumMotorA, true); 
    
    //SERVO
    gpio_set_function(PIN_PWM_ENABLEB, GPIO_FUNC_PWM);
    // Determine the PWM slice connected to GPIO: PWM_GPIO_CHA
    sliceNumMotorB = pwm_gpio_to_slice_num(PIN_PWM_ENABLEB);
    // Set period for frequency divisor
    pwm_set_clkdiv_int_frac(sliceNumMotorB, PWM_DIV_INTEGER, PWM_DIV_FRAC); // What frequency enters to the PWM?
    // Set top (wrap) value (Determines the frequency)
    pwm_set_wrap(sliceNumMotorB, PWM_TOP_VALUE);
    // Set zero duty
    pwm_set_chan_level(sliceNumMotorB, PWM_CHA, PWM_DUTY_ZERO);
    // Enable PWM
    pwm_set_enabled(sliceNumMotorB, true); 

    //Inicia en el estado de Reset
    Flags.WORD = 1;

    Move();
    while (1)
    {
        if(!gpio_get(1) && !gpio_get(0)){
            printf("Va derecho.........\n");
            pwm_set_chan_level(sliceNumMotorB, PWM_CHA, PWM_DUTY_ZERO);
            pwm_set_chan_level(sliceNumMotorA, PWM_CHA, PWM_DUTY_ZERO);
        }

        else if (band_right)
        {    
            printf("Corrige derecha................. \n");
            pwm_set_chan_level(sliceNumMotorA, PWM_CHA, pwm_level_right);
            pwm_set_chan_level(sliceNumMotorB, PWM_CHA, pwm_level);
            band_right = false;
        }
        else if (band_left)
        {
            printf("Corrige izquierda................. \n");
            pwm_set_chan_level(sliceNumMotorB, PWM_CHA, pwm_level_left);
            pwm_set_chan_level(sliceNumMotorA, PWM_CHA, pwm_level);
            band_left = false;
        }
        
        __wfi();     
        //sleep_ms(1000);
    }
    
    while (true)
    {
        switch (Flags.WORD)
        {
        case 1:
            Reset();
            uart_puts(UART_ID, str);
            break;
        case 12:
            //Pasar a .h
            pwm_init(slice_num, &cfg1, false);
            //CONFIGURACIÓN ALARMA 10MS
            gpio_set_function(MEASURE_PIN, GPIO_FUNC_PWM);
            now = get_absolute_time();
            target = delayed_by_ms(now, COUNT_MEASURE_TIME);
            hardware_alarm_set_target(alarm1, target);
            slice_num = pwm_gpio_to_slice_num(MEASURE_PIN);
            pwm_set_enabled(slice_num, true);
            break;
        case 0x10:
            Fin_Trama();
            Triangulación();
            Move();           
            break;
        default:
            break;
        }
        if(!Flags.BITS.Reset){
         __wfi();
        }
    }
#endif
}

/// ---------------- Funciones ---------------- ///

void Capture(){
    char c = uart_getc(UART_ID);
    //printf("Banderas : %x", Flags.WORD);
    switch (Flags.WORD & (0x3<<1))
    {
    case 2:
        // Recepción de OK
        BOK[count] = c;
        break;

    case 4:
        // Recibiendo trama de cada una de las balizas
        //printf("CR: %d, %c\n", CR,c);
        if(c != 0x0D && CR%10 == 3){
            // Captura y almacena el ID de la baliza
            Balizas[Baliza].BITS.NI = c;
            Baliza++;
        }
        if(c == 0x0D){
            if(CR == N_CRpb){
                //Indica Gin de trama
                Flags.BITS.EndMsg = 1;
                Flags.BITS.Capture = 0;
            }
            CR++;            
        }
        break;
    default:
        break;
    }
    if(count == N_OK && Flags.BITS.OK){
        //Final del OK recibido

        Flags.BITS.Capture = 1;
        Flags.BITS.OK = 0;
        count = 0;
        uart_puts(UART_ID, at_comand);
    }
    if(CR%10 == 2  && Flags.BITS.Capture){
        // Habilita el calculo del duty del duty 
        Flags.BITS.Capture_ADC = 1;
    }
    count++;//printf("\n");
}

inline void Triangulación(){
    for (uint8_t i = 0; i < sizeof(UB); i++)
    {
        if((Balizas[i].BITS.RSSI) > Umbral){
            printf("Baliza: %x \n", Balizas[i].WORD);
            if(Balizas[i].BITS.NI>47){
            pos_act = Balizas[i].BITS.NI;
            printf("La pos despues de la triangulacion es: %d \n", pos_act);
            }   
        }
    }    
}

inline void Move(){
    //Pin 10: Enable
    //Pin 11 : Salida A
    //Pin 12 : Salida B
    //printf("La pos actual y obj son: %d, %d \n", pos_act, pos_obj);
    pwm_set_chan_level(sliceNumMotorA, PWM_CHA, PWM_DUTY_ZERO);
    pwm_set_chan_level(sliceNumMotorB, PWM_CHA, PWM_DUTY_ZERO);
    if (pos_act > pos_obj)
    {
        //pwm_set_enabled(slice_num_motor, true);
        gpio_put_masked(mask_motor & (0x5 << 11), mask_motor & (0x1 << 11));
        printf("Ir hacia atrás \n, y la mascara es: %x, y el valor es: %x \n", mask_motor & (0x5 << 11), mask_motor & (0x1 << 11));
    }
    else if (pos_act < pos_obj)
    {
        //pwm_set_enabled(slice_num_motor, true);
        gpio_put_masked(mask_motor & (0x5 << 11), mask_motor & (0x1 << 13));
        printf("Ir hacia adelante \n, y la mascara es: %x, y el valor es: %x \n", mask_motor & (0x5 << 11), mask_motor & (0x1 << 13));
    }
    else{
        //pwm_set_enabled(slice_num_motor, false);
        gpio_put_masked(mask_motor & (0x5 << 11), 0x0);
        printf("Quieto \n"); 
    } 
}

void Reset(){
    for (uint8_t i = 0; i < N_BALIZAS; i++)
    {
        Balizas[i].WORD = 0; 
    }
    count = 0;
    CR = 0;
    Flags.WORD = 2;
    Baliza = 0;
    printf("El comando ha sido enviado \n");
}

void Fin_Trama(){// Hacer aquí la triangulación
    printf("-------------- FINALIZACION DE LA TRAMA --------------\n");
    for (uint8_t i = 0; i < N_BALIZAS; i++)
    {
        printf("El valor de la baliza %d es igual a %d \n", Balizas[i].BITS.NI, Balizas[i].BITS.RSSI);
    }
    Flags.WORD = 1;
}