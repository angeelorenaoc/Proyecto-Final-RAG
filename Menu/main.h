#include "menus.h"

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
#define TIME_DEBOUNCE 100
#define TIME_TEXT     5000
#define TIME_FIN     1000

//Botones
#define mask_boton 0x1F << 20 //Son 5 botones
state_menu menu;
opcion_t  opcion;
flags_t   Flags;

//Pantalla
SSOLED oled;

//PROTOTIPOS
void initUHD(SSOLED *, state_menu *, uint8_t);
void Info_Place(opcion_t *);

#endif