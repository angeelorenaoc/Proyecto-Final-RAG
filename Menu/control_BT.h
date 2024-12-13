/** \brief Controla la trasnmisión periodica y recepción de datos por medio de un modulo BT conectado a travez del UART para realizar telemetria hacia el SW Labview
 *  \author Jose Rivera
 */
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/structs/uart.h"
#include "hardware/timer.h"
#include "hardware/sync.h"
#include "hardware/irq.h"
#include "hardware/gpio.h"
#include "hardware/rtc.h"
#include "pico/util/datetime.h"


#ifndef CONTROL_BT_H_
#define CONTROL_BT_H_


#define UART_BT uart0               //! Identificador para usar la instancia del UART 0
#define BAUD_RATE_BT 9600           //! Rata de transmision serial
#define DATA_BITS_BT 8              //! Bits de datos de la configuración
#define STOP_BITS_BT 1              //! Numeros de bits de parada
#define PARITY UART_PARITY_NONE     //! Configura bit de paridad

#define Nc_HORA 4                   //! Numero de bits que contriene la hora transmitida por la interfaz
#define Nc_BUFFER_OUT 70            //! Cantidad de bits que contiene el mensaje a transmitir

#define TIME_2_WAIT_HOUR_MS 5000    //! Tiempo para esperar a recibir la Hora
#define ENABLE_BT_PIN 27            //! GPIO donde se conecta el ENABLE del BT

#define RTC_ALARM_TIME_S 3U         //! Tiempo de transmisión de los datos del buffer 
#define RTC_FIRST_ALARM_TIME 5U     //! Tiempo de inicio para realizar la primera transmisión


// We are using pins 0 and 1, but see the GPIO function select table in the
// datasheet for information on which other pins can be used.
#define UART_BT_TX_PIN 16           //! GPIO correspondiente a TX
#define UART_BT_RX_PIN 17           //! GPIO correspondiente a RX

//! banderas de tranmisión
typedef union {
    uint8_t WORD;
    struct{
        uint8_t count_c         : 5;
        uint8_t alarm_timeout   : 1;
        uint8_t recep_hour      : 1;
        uint8_t trans_buffer    : 1;
    }BITS;    
}bt_flags_t; //! Estructura con banderas para controlar el flujo de las funciones

typedef struct{
    uint16_t n_bat;
    uint16_t bl_rssi[4];
    uint8_t sw[4];
    uint8_t pos_act;
    uint8_t pos_obj;
}bt_data_t; //! Estructura contenedora de los datos relevantes para la telemetria


// Bandera de recepcion de hora actual

//Funciones de inicilizacion

// inicializa la comunicacion serial abilitando tambien irq de RX, habilita el pin de lectura de estado del modulo BT
void bt_init();

// inicializa el preriferico uart que estara conectado al modulo bt
void bt_init_uart();

// desinicializa el modulo uart
void bt_deinit_uart();

//Rutina de atención a Interrupciones
void bt_uart_irq();
void bt_set_data(bt_data_t *);

//Obtiene la Hora que sera transmitida al inicio del programa
bool bt_get_hour();
void bt_transt_buff(bt_data_t *);
void bt_transmit_ok();
void bt_transmit_nok();

void bt_transmit_alarm();



#endif