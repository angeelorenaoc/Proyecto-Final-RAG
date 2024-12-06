#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "ss_oled.h"
#include "UHD_1.h"
#include "hardware/pwm.h"
#include "pico/stdlib.h"
#include "hardware/structs/uart.h"
#include "hardware/uart.h"
#include "hardware/sync.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"

//Imagenes 
#include "Gestos.h"

#ifndef MENUS_H_
#define MENUDS_H_

typedef union{
    uint8_t WORD;
    struct{
        uint8_t Reset     :1;    // Estado de reset
        uint8_t Menu1     :1; //Muestra las opciones de los lugares, solo se habilitan las interrupciones en este punto
        uint8_t Show_Info :1;  //Muestra info del lugar
        uint8_t End       :1;  //Despedida 
    }BITS;      
}state_menu;

//Estructura para las opciones del menu1
typedef union
{
    uint8_t WORD;
    struct     {
        uint8_t LED     : 1; // Opcion 1
        uint8_t GITA    : 1; // Opcion 2
        uint8_t LEB     : 1; // Opcion 3 (Básicos)
        uint8_t LTS     : 1; // Opcion 4 (Señales)
    }BITS;
}opcion_t;

//Estructura para las opciones del menu1
typedef union
{
    uint8_t WORD;
    struct     {
        uint8_t CONFIG_ALARM    : 1; // Configuración de la alarma.
    }BITS;
}flags_t;

//Protoipos
void Menu1(SSOLED *);
void Info_Senales(SSOLED *, uint8_t);
void Info_Basicos(SSOLED *, uint8_t);
void Info_GITA(SSOLED *, uint8_t);
void Info_LED(SSOLED *, uint8_t);
void Despedida(SSOLED *, uint8_t);
#endif