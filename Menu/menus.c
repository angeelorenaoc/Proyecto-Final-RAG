#include <stdio.h>
#include "pico/stdlib.h"
#include "menus.h"

void Menu1(SSOLED *oled){
  /*
  \brief Muestra el menú de opciones de ubicación en la pantalla OLED
  \param oled puntero en el cual se encuentra la estructura de datos para la configuración de la OLED 
  */
    oledFill(oled, 0,1);
    oledSetContrast(oled, 127);
    oledWriteString(oled, 0,0,1,(char *)"  A DONDE   ", FONT_8x8, 0,1);
    oledWriteString(oled,0,0,2,(char*)  " QUIERES IR?  ", FONT_8x8, 0,1);
    oledWriteString(oled,0,0,4,(char *) "1. LED   ", FONT_8x8, 0,1);
    oledWriteString(oled,0,0,5,(char *) "2. GITA", FONT_8x8, 0,1);
    oledWriteString(oled,0,0,6,(char *) "3. Basicos", FONT_8x8, 0,1); 
    oledWriteString(oled,0,0,7,(char *) "4. Lab senales." , FONT_8x8, 0,1);
}
void Info_Senales(SSOLED *oled, uint8_t count){
  /*
  \brief Muestra la información en la pantalla OLED una vez llega al laboratorio de Señales
  \param oled puntero en el cual se encuentra la estructura de datos para la configuración de la OLED 
  \param count contador que indica el número de página.
  */
  switch (count)
  {
  case 1:
    oledFill(oled, 0,1);
    oledWriteString(oled, 0,0,0,(char *)"  BIENVENIDO AL ", FONT_8x8, 0,1);
    oledWriteString(oled,0,0,1,(char*)  "LAB DE TRATAMIEN-", FONT_8x8, 0,1);
    oledWriteString(oled,0,0,2,(char*)  " -TO DE SENALES", FONT_8x8, 0,1);
    oledWriteString(oled,0,0,4,(char *) "    Horarios:    ", FONT_8x8, 0,1);
    oledWriteString(oled,0,0,5,(char *) "        L-V         ", FONT_6x8, 0,1);
    oledWriteString(oled,0,0,6,(char *) "      6am-12pm      ", FONT_6x8, 0,1); 
    oledWriteString(oled,0,0,7,(char *) "      2pm-8pm       " , FONT_6x8, 0,1);
    break;

  case 2:
    oledFill(oled, 0,1);
    oledWriteString(oled, 0,0,1,(char *)"   En este lab se    ", FONT_6x8, 0,1);
    oledWriteString(oled,0,0,2,(char*)  "  procesan y adecuan ", FONT_6x8, 0,1);
    oledWriteString(oled,0,0,3,(char*)  "todas las senales EM", FONT_6x8, 0,1);
    oledWriteString(oled,0,0,4,(char *) "       para la      ", FONT_6x8, 0,1);
    oledWriteString(oled,0,0,5,(char *) "    implementacion  ", FONT_6x8, 0,1);
    oledWriteString(oled,0,0,6,(char *) "  de comunicaciones.", FONT_6x8, 0,1);
    break;
  case 3:
    oledFill(oled, 0,1);
    oledWriteString(oled, 0,0,1,(char *)"Dispondras de herra- ", FONT_6x8, 0,1);
    oledWriteString(oled,0,0,2,(char*)  " mientas como: osci- ", FONT_6x8, 0,1);
    oledWriteString(oled,0,0,3,(char*)  "loscopios, analizador", FONT_6x8, 0,1);
    oledWriteString(oled,0,0,4,(char *) "de RF y fibra optica,", FONT_6x8, 0,1);
    oledWriteString(oled,0,0,5,(char *) " entre otros equipos ", FONT_6x8, 0,1);
    oledWriteString(oled,0,0,6,(char *) " para el analisis de ", FONT_6x8, 0,1);
    oledWriteString(oled,0,0,7,(char *) "     senales.        ", FONT_6x8, 0,1); 
    break;

  default:
    break;
  }
}
void Info_Basicos(SSOLED *oled, uint8_t count){
  /*
  \brief Muestra la información en la pantalla OLED una vez llega al laboratorio de Básicos
  \param oled puntero en el cual se encuentra la estructura de datos para la configuración de la OLED 
  \param count contador que indica el número de página.
  */
  switch (count)
  {
  case 1:
    oledFill(oled, 0,1);
    oledWriteString(oled, 0,0,0,(char *)" BIENVENIDO AL ", FONT_8x8, 0,1);
    oledWriteString(oled,0,0,1,(char*)  "LAB ELECTRONICA", FONT_8x8, 0,1);
    oledWriteString(oled,0,0,2,(char*)  "BASICA \"BASICOS\"", FONT_8x8, 0,1);
    oledWriteString(oled,0,0,4,(char *) "   Horarios:    ", FONT_8x8, 0,1);
    oledWriteString(oled,0,0,5,(char *) "       L-V         ", FONT_6x8, 0,1);
    oledWriteString(oled,0,0,6,(char *) "     6am-12pm      ", FONT_6x8, 0,1); 
    oledWriteString(oled,0,0,7,(char *) "      2pm-8pm      ", FONT_6x8, 0,1);
    break;

  case 2:
    oledFill(oled, 0,1);
    oledWriteString(oled, 0,0,1,(char *)"   Este lab esta     ", FONT_6x8, 0,1);
    oledWriteString(oled,0,0,2,(char*)  " especializado en el", FONT_6x8, 0,1);
    oledWriteString(oled,0,0,3,(char*)  "  diseno y constru-", FONT_6x8, 0,1);
    oledWriteString(oled,0,0,4,(char *) "  ccion de HW; aqui  ", FONT_6x8, 0,1);
    oledWriteString(oled,0,0,5,(char *) "   encontraras las  ", FONT_6x8, 0,1);
    oledWriteString(oled,0,0,6,(char *) " herramientas para el", FONT_6x8, 0,1);
    oledWriteString(oled,0,0,7,(char *) "analisis de circuitos", FONT_6x8, 0,1);
    break;
  
  case 3:
    oledFill(oled, 0,1);
    oledWriteString(oled, 0,0,1,(char *)" Se dispone de herra-", FONT_6x8, 0,1);
    oledWriteString(oled,0,0,2,(char*)  " mientas como: CNC,  ", FONT_6x8, 0,1);
    oledWriteString(oled,0,0,3,(char*)  " fuentes, oscilosco- ", FONT_6x8, 0,1);
    oledWriteString(oled,0,0,4,(char *) " pios, estaciones de ", FONT_6x8, 0,1);
    oledWriteString(oled,0,0,5,(char *) "soldadura,y programas", FONT_6x8, 0,1);
    oledWriteString(oled,0,0,6,(char *) " para el desarrollo y", FONT_6x8, 0,1);
    oledWriteString(oled,0,0,7,(char *) "montaje de circuitos.", FONT_6x8, 0,1); 
    break;
  
  default:
    break;
  }
}
void Info_GITA(SSOLED *oled, uint8_t count){
  /*
  \brief Muestra la información en la pantalla OLED una vez llega al grupo de innvestigación de GITA
  \param oled puntero en el cual se encuentra la estructura de datos para la configuración de la OLED 
  \param count contador que indica el número de página.
  */
  switch (count)
  {
    case 1:
      oledFill(oled, 0,1);
      oledWriteString(oled, 0,0,0,(char *)"  BIENVENIDO AL ", FONT_8x8, 0,1);
      oledWriteString(oled,0,0,1,(char*)  "    GRUPO DE  ", FONT_8x8, 0,1);
      oledWriteString(oled,0,0,2,(char*)  "  INVESTIGACION    ", FONT_8x8, 0,1);
      oledWriteString(oled,0,0,4,(char *) "    GITA.     ", FONT_8x8, 0,1);
    break;

  case 2:
    oledFill(oled, 0,1);
    oledWriteString(oled, 0,0,1,(char *)"    Este grupo de    ", FONT_6x8, 0,1);
    oledWriteString(oled,0,0,2,(char*)  "    investigacion    ", FONT_6x8, 0,1);
    oledWriteString(oled,0,0,3,(char*)  " esta especializado  ", FONT_6x8, 0,1);
    oledWriteString(oled,0,0,4,(char *) "   en 3 líneas las   ", FONT_6x8, 0,1);
    oledWriteString(oled,0,0,5,(char *) "     cuales son:     ", FONT_6x8, 0,1);
    oledWriteString(oled,0,0,6,(char *) "     analisis de    ", FONT_6x8, 0,1); 
    oledWriteString(oled,0,0,7,(char *) "  patrones, optica  ", FONT_6x8, 0,1);
    break;

  case 3:
    oledFill(oled, 0,1);
    oledWriteString(oled, 0,0,1,(char *)"  y modelamiento ", FONT_6x8, 0,1);
    oledWriteString(oled,0,0,2,(char*)  "  de sistemas de ", FONT_6x8, 0,1);
    oledWriteString(oled,0,0,3,(char *) "   comunicacion  ", FONT_6x8, 0,1); 
    oledWriteString(oled,0,0,4,(char*)  "    Para más     ", FONT_6x8, 0,1);
    oledWriteString(oled,0,0,5,(char *) "   informacion,  ", FONT_6x8, 0,1);
    oledWriteString(oled,0,0,6,(char *) "     toque la    ", FONT_6x8, 0,1);
    oledWriteString(oled,0,0,7,(char *) "     puerta.     ", FONT_6x8, 0,1);
    
    break;
  default:
    break;
  }
}
void Info_LED(SSOLED *oled, uint8_t count){
  /*
  \brief Muestra la información en la pantalla OLED una vez llega al laboratorio de Eletrónica Digital
  \param oled puntero en el cual se encuentra la estructura de datos para la configuración de la OLED 
  \param count contador que indica el número de página.
  */
  switch (count)
  {
  case 1:
      oledFill(oled, 0,1);
      oledWriteString(oled, 0,0,0,(char *)"  BIENVENIDO AL ", FONT_8x8, 0,1);
      oledWriteString(oled,0,0,1,(char*)  " LAB ELECTRONICA ", FONT_8x8, 0,1);
      oledWriteString(oled,0,0,2,(char*)  "  DIGITAL (LED)    ", FONT_8x8, 0,1);
      oledWriteString(oled,0,0,4,(char *) "    Horarios:      ", FONT_8x8, 0,1);
      oledWriteString(oled,0,0,5,(char *) "    L - 8am a 6pm    ", FONT_6x8, 0,1);
      oledWriteString(oled,0,0,6,(char *) "  M,W,J - 8am a 7pm  ", FONT_6x8, 0,1); 
      oledWriteString(oled,0,0,7,(char *) "    V - 6am a 6pm    ", FONT_6x8, 0,1);
    break;

  case 2:
    oledFill(oled, 0,1);
    oledWriteString(oled, 0,0,1,(char *)"    Este Lab esta    ", FONT_6x8, 0,1);
    oledWriteString(oled,0,0,2,(char*)  "   especializado en  ", FONT_6x8, 0,1);
    oledWriteString(oled,0,0,3,(char*)  "    en la linea de   ", FONT_6x8, 0,1);
    oledWriteString(oled,0,0,4,(char *) "      digitales.     ", FONT_6x8, 0,1);
    oledWriteString(oled,0,0,5,(char *) "Encontraras diversos ", FONT_6x8, 0,1);
    oledWriteString(oled,0,0,6,(char *) "    tipos de MCUs,   ", FONT_6x8, 0,1); 
    oledWriteString(oled,0,0,7,(char *) "entre otros sistemas.", FONT_6x8, 0,1);
    break;

  case 3:
    oledFill(oled, 0,1);
    oledWriteString(oled, 0,0,1,(char *)"Ademas, herramientas ", FONT_6x8, 0,1);
    oledWriteString(oled,0,0,2,(char*)  "y materiales como CI,", FONT_6x8, 0,1);
    oledWriteString(oled,0,0,3,(char*)  "  sensores, modulos, ", FONT_6x8, 0,1);
    oledWriteString(oled,0,0,4,(char *) "   perifericos, y    ", FONT_6x8, 0,1);
    oledWriteString(oled,0,0,5,(char *) " componentes basicos ", FONT_6x8, 0,1);
    oledWriteString(oled,0,0,6,(char *) "   para el montaje   ", FONT_6x8, 0,1);
    oledWriteString(oled,0,0,7,(char *) "     de circuitos.   ", FONT_6x8, 0,1); 
    break;
  default:
    break;
  }
}
