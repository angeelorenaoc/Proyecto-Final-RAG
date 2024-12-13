#include "control_BT.h"

bt_flags_t bt_flags; //! banderas para controlar el flujo de las funciones

const char buff_ok[] = "OK\n";      //! Mensanje para comprobar la función
const char buff_nok[] = "!OK\n";    //! Mensanje para informar de errores en la recepción
char buff_data[Nc_BUFFER_OUT];      //! Buffer contenedor de los mensajes
char buffer_BT_hour[Nc_HORA];       //! Buffer contenedor de la hora recibida
bt_data_t *data;                    //! Variable que almacena los datos


bool fired; //! Usado para comporbar el inicio de la comunicación

//* Tiempo de transmisión de datos
datetime_t alarm= {
        .year  = -1,
        .month = -1,
        .day   = -1,
        .dotw  = -1,
        .hour  = -1,
        .min   = -1,
        .sec   = RTC_FIRST_ALARM_TIME
    }; 
void bt_init(){
    /** \brief Inicialización los pines y modulos pertinentes como UART y RTC
     *  como tambien GPIOS con su respectifa funcion
     */
    datetime_t t = {
        .year  = 2024,
        .month = 12,
        .day   = 10,
        .dotw  = 3, // 0 is Sunday, so 3 is Wednesday
        .hour  = 11,
        .min   = 20,
        .sec   = 00
    }; //! Configuración inicial del RCT

    // Start the RTC
    rtc_init();
    rtc_set_datetime(&t);
    gpio_set_dir(ENABLE_BT_PIN,false);


    bt_init_uart();    
}

void bt_init_uart(){
    /** \brief Inicializacion del UART a usar para la comunicación
     * 
     */
    bt_flags.WORD = 0;
    uart_init(UART_BT, BAUD_RATE_BT);
    gpio_set_function(UART_BT_TX_PIN, UART_FUNCSEL_NUM(UART_BT, UART_BT_TX_PIN));
    gpio_set_function(UART_BT_RX_PIN, UART_FUNCSEL_NUM(UART_BT, UART_BT_RX_PIN));
    int __unused actual = uart_set_baudrate(UART_BT, BAUD_RATE_BT);

    uart_set_hw_flow(UART_BT, false, false);

    uart_set_format(UART_BT, DATA_BITS_BT, STOP_BITS_BT, PARITY);

    uart_set_fifo_enabled(UART_BT, false);
    int UART_IRQ = UART_BT == uart0 ? UART0_IRQ : UART1_IRQ;
    irq_set_exclusive_handler(UART_IRQ, bt_uart_irq);
    irq_set_enabled(UART_IRQ, true);
    uart_set_irq_enables(UART_BT, true, false);
}
inline void bt_deinit_uart(){
    /** 
     * \brief Desinicializa el uart 
      */
    uart_deinit(UART_BT);
}

void bt_uart_irq(){
    /**
     * \brief ISR para los datos recibidos a travez del UART, detecta la hora inicial recibida y demas datos enviados
     */
    char c = uart_getc(UART_BT);
    switch (bt_flags.WORD >> 6)
    {
    case 0:
        buffer_BT_hour[bt_flags.BITS.count_c] = c;
        bt_flags.BITS.count_c++;
    break;
    case 1:
        //printf("comando: %c\n",c);
        break;
    default:
        break;
    }
}
void alert_wait_alarm(uint alarm){
    /**
     * \brief ISR para reactivar la alarma que espera la respuesta de la interfaz
     */
    //printf("No se recibio respuesta de BT\n");
    bt_flags.BITS.alarm_timeout = 1;
};
bool bt_get_hour(){
    /**
     * \brief Espera la respuesta de la interfaz
     * \return Estdado de la conexión, (True) logro recbir la hora, (False) no logro establecer com
     */
    while(bt_flags.BITS.count_c<Nc_HORA){
        __wfi();
    }
    bt_flags.BITS.recep_hour = 1;
    fired = true;
    return true;
}

void bt_transt_buff(bt_data_t *datos){
    /**
     * \brief construye el buffer con los datos ingresados
     * \param datos variable con los datos a transmitir
     */

    sprintf(buff_data,"BTA:BTB:%dBLA:%dBLB:%dBLC:%dBLD:%dUA:%dUB:%dUC:%dUD:%d,A:%d,O:%dF\n",
                    datos->n_bat,
                    datos->bl_rssi[0],
                    datos->bl_rssi[1],
                    datos->bl_rssi[2],
                    datos->bl_rssi[3],
                    datos->sw[0],
                    datos->sw[1],
                    datos->sw[2],
                    datos->sw[3],
                    datos->pos_act,
                    datos->pos_obj);
    uart_puts(UART_BT,buff_data);
}
inline void bt_transmit_ok(){
    /**
     * \brief Transmite "OK" al BT
     */
    uart_puts(UART_BT,buff_ok);
}
inline void bt_transmit_nok(){
    /**
     * \brief Tranmite "NOK" al BT
     */
    uart_puts(UART_BT,buff_ok);
}
void bt_transmit_alarm(){ 
    /**
     * \brief ISR en la cual reactiva la alarma si se realizo una conexión adecuada
     */
    datetime_t t = {0};
    rtc_get_datetime(&t);
    alarm.sec = (t.sec+RTC_ALARM_TIME_S)%60;
    if(fired){
        bt_transt_buff(data);
        rtc_set_alarm(&alarm, &bt_transmit_alarm);
    }
}
void bt_set_data(bt_data_t *data_punt){
    /**
     * \brief Asigna el dato entrante a la variable local
     * \param data_punt Direccion con los datos
     */
    data = data_punt;
}

