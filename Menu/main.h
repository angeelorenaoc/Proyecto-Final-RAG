#include "control_BT.h"
#include "menus.h"
#include "PWM_measure.h"
#include "hardware/adc.h"


#ifndef MAIN_H_
#define MAIN_H_
//Macros
#define SDA_PIN 4
#define SCL_PIN 5
#define RESET_PIN -1
int rc;

static uint8_t ucBuffer[1024];
#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define TIME_DEBOUNCE 200
#define TIME_TEXT     7000
#define TIME_FIN     1000

#define ADC_MEASURE_PIN 26

//Botones
#define mask_boton 0x1F << 20 //Son 5 botones
state_menu menu;
opcion_t  opcion;
flags_t   Flags1;

//Pantalla
SSOLED oled;

//PROTOTIPOS
void initUHD(SSOLED *, state_menu *, uint8_t);
void Info_Place(opcion_t *);

/*------------------ DMA_CONTROL ----------------------*/
/// ---------------- Macros ---------------- ///
#define UART_ID uart1
#define BAUD_RATE 4800
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
#define WATCHDOG_TIME 2000

#define N_BALIZAS 2
#define N_CRpb N_BALIZAS*10
#define N_OK 2

#define MEASURE_PIN 15
#define COUNT_MEASURE_TIME 10

//Motor
#define PIN_PWM_ENABLE 10
#define PWM_CHA             0
#define PWM_DIV_INTEGER     128
#define PWM_DIV_FRAC        0
#define PWM_TOP_VALUE       4095
#define PWM_DUTY_ZERO       1000
#define PWM_ZERO 0
#define PWM_LEVEL 3500
#define PIN_PWM_ENABLEA 12
#define PIN_PWM_ENABLEB 10

#define Umbral 900U

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
        uint8_t Llego       : 1; //Indica cuando llega a la pos objetivo
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
uint8_t pos_act = 0, pos_obj = '1';

char UB[4] = {'0', '1', '2','3'}; //Ubicación de las balizas 

pwm_config cfg1, cfg2;

/// ---------------- Prototipos de funciones ---------------- ///
void Capture();
void Reset();
void Fin_Trama();
void Triangulación();
void Move();
void Corregir_adelante();
void Corregir_atras();
void actu_data();

#endif