
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
#define UART_TX_PIN 8
#define UART_RX_PIN 9
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY    UART_PARITY_NONE

//ADC
#define ADC_GPIO_CH0        26
#define ADC_CH0             0
#define ADC_MIN_READVALUE   30

//BALIZAS
#define N_BALIZAS 1
#define N_CRpb N_BALIZAS*10
#define N_OK 2

#define MEASURE_PIN 3
#define COUNT_MEASURE_TIME 10

#define Umbral 990

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
uint8_t pos_act, pos_obj = '0';

char UB[4] = {'0', '1', '2','3'};; //Ubicación de las balizas 

/// ---------------- Prototipos de funciones ---------------- ///
void Capture();
void Reset();
void Fin_Trama();
void Triangulación();
void Move();
