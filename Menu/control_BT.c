#include "control_BT.h"

bt_flags_t bt_flags;


const char buff_ok[] = "OK\n";
const char buff_nok[] = "!OK\n";
char buff_data[Nc_BUFFER_OUT];
char buffer_BT_hour[Nc_HORA];
bt_data_t *data;


bool fired;

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
    // Start on Wednesday 13th January 2021 11:20:00
    datetime_t t = {
        .year  = 2024,
        .month = 12,
        .day   = 10,
        .dotw  = 3, // 0 is Sunday, so 3 is Wednesday
        .hour  = 11,
        .min   = 20,
        .sec   = 00
    };

    // Start the RTC
    rtc_init();
    rtc_set_datetime(&t);
    gpio_set_dir(ENABLE_BT_PIN,false);


    bt_init_uart();    
}

void bt_init_uart(){
    bt_flags.WORD = 0;
    uart_init(UART_BT, BAUD_RATE_BT);
    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select
    gpio_set_function(UART_BT_TX_PIN, UART_FUNCSEL_NUM(UART_BT, UART_BT_TX_PIN));
    gpio_set_function(UART_BT_RX_PIN, UART_FUNCSEL_NUM(UART_BT, UART_BT_RX_PIN));
    int __unused actual = uart_set_baudrate(UART_BT, BAUD_RATE_BT);

    // Set UART flow control CTS/RTS, we don't want these, so turn them off
    uart_set_hw_flow(UART_BT, false, false);

    // Set our data format
    uart_set_format(UART_BT, DATA_BITS_BT, STOP_BITS_BT, PARITY);

    uart_set_fifo_enabled(UART_BT, false);
    int UART_IRQ = UART_BT == uart0 ? UART0_IRQ : UART1_IRQ;
     // And set up and enable the interrupt handlers
    irq_set_exclusive_handler(UART_IRQ, bt_uart_irq);
    irq_set_enabled(UART_IRQ, true);
    uart_set_irq_enables(UART_BT, true, false);
}
inline void bt_deinit_uart(){
    uart_deinit(UART_BT);
}

void bt_uart_irq(){
    char c = uart_getc(UART_BT);
    //printf("%c, flags: %x\n",c,bt_flags.WORD);
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
    //printf("No se recibio respuesta de BT\n");
    bt_flags.BITS.alarm_timeout = 1;
};
bool bt_get_hour(){
    /*int alarm_num = hardware_alarm_claim_unused(0);
    uint8_t trials = 0;
    if(alarm_num!=-1){
        //printf("Claim alarm");
        hardware_alarm_set_callback(alarm_num, &alert_wait_alarm);
    }
    else{
        uart_puts(UART_BT,buff_nok);
    }*/
    //printf("config alarm: %d\n", alarm_num);
    while(bt_flags.BITS.count_c<Nc_HORA){
        //if((alarm_num!=-1 && !gpio_get(ENABLE_BT_PIN)) || bt_flags.BITS.alarm_timeout){
        //    //printf("init alarm\n");
        //    absolute_time_t now = get_absolute_time();
        //    absolute_time_t target = delayed_by_ms(now, TIME_2_WAIT_HOUR_MS);
        //    hardware_alarm_set_target(alarm_num, target);
        //    //printf("config alarm OK\n");
        //    trials++;
        //}         
        //else if(trials>3){
        //    //printf("No se puede configurar una alarma\n");
        //    return false;
        //}
        //printf("Esperando recepcion\n");
        __wfi();
    }
    bt_flags.BITS.recep_hour = 1;
    //hardware_alarm_unclaim(alarm_num);
    fired = true;
    return true;
}

void bt_transt_buff(bt_data_t *datos){

   //printf("enviando\n");
    sprintf(buff_data,"BTA:BTB:%dBLA:%dBLB:%dBLC:%dBLD:%dUA:%dUB:%dUC:%dUD:%dF\n",
                    datos->n_bat,
                    datos->bl_rssi[0],
                    datos->bl_rssi[1],
                    datos->bl_rssi[2],
                    datos->bl_rssi[3],
                    datos->sw[0],
                    datos->sw[1],
                    datos->sw[2],
                    datos->sw[3]);
    uart_puts(UART_BT,buff_data);
}
inline void bt_transmit_ok(){
    uart_puts(UART_BT,buff_ok);
}
inline void bt_transmit_nok(){
    uart_puts(UART_BT,buff_ok);
}
void bt_transmit_alarm(){ 
    datetime_t t = {0};
    rtc_get_datetime(&t);
    alarm.sec = (t.sec+RTC_ALARM_TIME_S)%60;
    if(fired){
        //printf("Alarma configurada\n");
        bt_transt_buff(data);
        rtc_set_alarm(&alarm, &bt_transmit_alarm);
    }
}
void bt_set_data(bt_data_t *data_punt){
    data = data_punt;
}

