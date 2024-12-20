
#include "pico/stdlib.h"
#include "hardware/dma.h"
#include "hardware/structs/uart.h"
#include "hardware/uart.h"
#include "hardware/sync.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "PWM_measure.h"


/// ---------------- Macros ---------------- ///
#define UART_ID uart1
#define BAUD_RATE 4800

// We are using pins 0 and 1, but see the GPIO function select table in the
// datasheet for information on which other pins can be used.
//UART
#define RESET_XBEE_PIN 7

#define UART_TX_PIN 8
#define UART_RX_PIN 9
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY    UART_PARITY_NONE


//BALIZAS
#define WATCHDOG_TIME 2000

#define N_BALIZAS 2
#define N_CRpb N_BALIZAS*10
#define N_OK 2

#define MEASURE_PIN 3
#define COUNT_MEASURE_TIME 10

//Motor
#define DIGITAL1 6
#define DIGITAL2 7
#define PIN_PWM_ENABLEA 12
#define PIN_PWM_ENABLEB 10
#define PWM_CHA             0
#define PWM_DIV_INTEGER     128
#define PWM_DIV_FRAC        0
#define PWM_TOP_VALUE       4095
#define PWM_DUTY_ZERO       1000


#define Umbral 990U

/// ---------------- Variables ---------------- ///
//BUFFERS
char BOK[N_OK]; // Buffer para recibir los OK del comando AT
uint8_t count = 0; // Contador que indica el número de bytes recibidos mediante el UART.
uint8_t Baliza = 0;
uint8_t CR = 0;


int ctrl_chan = 0;
static int chars_rxed = 0;
uint16_t Signal_Power = 0;
const char str[] = "+++";
const char comand[] = "ATND\r";
const uint8_t at_comand[5] = {0x41, 0x54, 0x4E, 0x44, 0x0D};

//Estructura para las banderas
typedef union{
    uint8_t WORD;
    struct{
        uint8_t Reset:        1; // Estado de reset1
        uint8_t OK :          1; // Recibe los OK2
        uint8_t Capture :     1; //Captura la trama4
        uint8_t Capture_ADC : 1; //Captura valor del ADC8
        uint8_t EndMsg      : 1; // Bandera de finalización de trama16
    }BITS;    
}state_flags;

typedef union
{
    uint32_t WORD;
    struct     {
        uint32_t RSSI  : 16; // Valor Pot señal
        uint32_t NI    : 8;  // Identificador
    }BITS;
}data_baliza_t;

data_baliza_t Balizas[N_BALIZAS];
state_flags Flags;

uint32_t RSSI[4]; 

char response[3];

// TRIANGULACIÓN
uint8_t state = 0;
uint8_t pos_act = '0', pos_obj = '1';

char UB[4] = {'0', '1', '2','3'};; //Ubicación de las balizas 

pwm_config cfg1, cfg2;

/// ---------------- Prototipos de funciones ---------------- ///
void Capture();
void Reset();
void Fin_Trama();
void Triangulación();
void Move();
