#include <stdio.h>
#include "main.h"

uint32_t slice_num = 0;
uint32_t dutymask = 0;
uint32_t measured_duty_cycle = 0 ;
absolute_time_t now, target;
const uint alarm1 = 1;

void alarm_duty(uint alarm){
    // Calcula el duty de la seañl cada 10ms
    measure_duty_cycle(slice_num,&Balizas[Baliza-1].WORD);
    Flags.BITS.Capture_ADC = 0;
    printf("CAPTURANDO RSSI: %d, CR: %d\n", Balizas[Baliza-1].BITS.RSSI, CR);
}

int main() {
#ifndef uart_default
#warning dma/control_blocks example requires a UART
#else
    stdio_init_all();
    gpio_init(MEASURE_PIN);

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
    pwm_config cfg;
    init_pwm(&cfg);

    //Inicia en el estado de Reset
    Flags.WORD = 1;
    sleep_ms(2000);

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
            pwm_init(slice_num, &cfg, false);
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
            pos_act = Balizas[i].BITS.NI;
        }
    }    
}

inline void Move(){
    if (pos_act > pos_obj)
    {
        printf("Ir hacia atrás \n");
    }
    else if (pos_act < pos_obj)
    {
        printf("Ir hacia adelante \n");
    }
    else{
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
    //printf("El comando ha sido enviado \n");
}

void Fin_Trama(){// Hacer aquí la triangulación
    printf("-------------- FINALIZACION DE LA TRAMA --------------\n");
    for (uint8_t i = 0; i < N_BALIZAS; i++)
    {
        printf("El valor de la baliza %d es igual a %d \n", Balizas[i].BITS.NI, Balizas[i].BITS.RSSI);
    }
    Flags.WORD = 1;
}