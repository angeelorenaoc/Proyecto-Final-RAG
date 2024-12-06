#include "main.h"
//Para arregalr descomentar funciones y borrar las del .c

absolute_time_t now, target, now1, target1;
const uint alarm1 = 1;
const uint alarm_text = 0;
uint8_t count_page = 0;

uint32_t gpio_interrup, mask_interrup;


void start_alarm(uint gpio, uint32_t mask_event){
  printf("---------------------- start alarm -------------------\n");
  now = get_absolute_time();
  target = delayed_by_ms(now, TIME_DEBOUNCE);
  hardware_alarm_set_target(alarm1, target);
  gpio_set_irq_enabled(16, GPIO_IRQ_EDGE_RISE, false);
  gpio_set_irq_enabled(17, GPIO_IRQ_EDGE_RISE, false);
  gpio_set_irq_enabled(18, GPIO_IRQ_EDGE_RISE, false);
  gpio_set_irq_enabled(19, GPIO_IRQ_EDGE_RISE, false);
  gpio_set_irq_enabled(20, GPIO_IRQ_EDGE_RISE, false);
  gpio_interrup = gpio;
  mask_interrup = mask_event;
  gpio_acknowledge_irq(gpio_interrup,mask_interrup);//Probar
}

void delay_show(uint alarm){
  if((menu.BITS.Show_Info && count_page < 4) || (menu.BITS.End && count_page < 5))
  {
    printf("------Alarma del texto ----\n");
    printf("pag: %d\n",count_page);
    Flags.BITS.CONFIG_ALARM = 1;
  } 
  else{
    count_page=0;
  }

}

void eleccion(uint alarm1){
  printf("Entra a la interrupcion, gpio: %x \n", gpio_interrup);
  gpio_set_irq_enabled_with_callback(16, GPIO_IRQ_EDGE_RISE, true, &start_alarm);
    switch (gpio_interrup)
    {
    case 16: //Rojo
      switch (menu.WORD) //Opciones de pagina
      {
      case 1: //Pasar al menú de opciones
        menu.BITS.Menu1 = 1;
        menu.BITS.Reset = 0;
        break;
      case 2: //Devolverse al estado de Reset
        menu.BITS.Reset = 1;
        menu.BITS.Menu1 = 0;
        break;
      case 4: //Ir a la despedida
        menu.BITS.End = 1;
        menu.BITS.Show_Info = 0;
        Flags.BITS.CONFIG_ALARM = 1;
        count_page = 0;
        break;
      case 8: //Volver al estado de reset
        menu.BITS.Reset = 1;
        menu.BITS.End = 0;
        break; 

      default:
        break;
      }
      break;

    case 17: //1
      menu.BITS.Menu1 = 0;
      menu.BITS.Show_Info = 1;
      opcion.WORD = 0;
      opcion.BITS.LED = 1;
      Flags.BITS.CONFIG_ALARM = 1;
      break;

    case 18: //2
      menu.BITS.Menu1 = 0;
      menu.BITS.Show_Info = 1;
      opcion.WORD = 0;
      opcion.BITS.GITA = 1;
      Flags.BITS.CONFIG_ALARM = 1;
      break;

    case 19: //3
      menu.BITS.Menu1 = 0;
      menu.BITS.Show_Info = 1;
      opcion.WORD = 0;
      opcion.BITS.LEB = 1;
      Flags.BITS.CONFIG_ALARM = 1;
      break;

    case 20: //4
      menu.BITS.Menu1 = 0;
      menu.BITS.Show_Info = 1;
      opcion.WORD = 0;
      opcion.BITS.LTS = 1;
      Flags.BITS.CONFIG_ALARM = 1;
      break;

    default:
      break;
    }
}

int main() {
  stdio_init_all();
  sleep_ms(4000);

  printf("Iniciando \n");
  //Configuración de los pines
  gpio_init_mask(mask_boton);

  gpio_pull_down(16);
  gpio_pull_down(17);
  gpio_pull_down(18);
  gpio_pull_down(19);
  gpio_pull_down(20);

  gpio_set_input_hysteresis_enabled(16,true);
  gpio_set_input_hysteresis_enabled(17,true);
  gpio_set_input_hysteresis_enabled(18,true);
  gpio_set_input_hysteresis_enabled(19,true);
  gpio_set_input_hysteresis_enabled(20,true);

  gpio_set_irq_enabled_with_callback(16, GPIO_IRQ_EDGE_RISE, true, &start_alarm);
  gpio_set_irq_enabled_with_callback(17, GPIO_IRQ_EDGE_RISE, true, &start_alarm);
  gpio_set_irq_enabled_with_callback(18, GPIO_IRQ_EDGE_RISE, true, &start_alarm);
  gpio_set_irq_enabled_with_callback(19, GPIO_IRQ_EDGE_RISE, true, &start_alarm);
  gpio_set_irq_enabled_with_callback(20, GPIO_IRQ_EDGE_RISE, true, &start_alarm);

  //Configuración de las alarmas
  //Alarma para el menu
  hardware_alarm_claim(alarm1);
  hardware_alarm_set_callback(alarm1, &eleccion);
  //Alarma para el texto 
  hardware_alarm_claim(alarm_text);
  hardware_alarm_set_callback(alarm_text, &delay_show);

  //Pantalla
  char szTemp[32];
  rc = oledInit(&oled, OLED_128x64, 0x3c, 0, 0, 1, SDA_PIN, SCL_PIN, RESET_PIN, 1000000L);
  //initUHD(&oled);

  menu.WORD = 1;//Parte del reset

  while (1)
  { 
    printf("Va en el menú: %x, op: %x\n", menu.WORD,opcion.WORD);
    if (Flags.BITS.CONFIG_ALARM)
    {
      //printf("Cambiando de pagina\n");
      now1 = get_absolute_time();
      if(menu.BITS.Show_Info){
        
        target1 = delayed_by_ms(now1, TIME_TEXT);
        hardware_alarm_set_target(alarm_text, target1);
      }
      else if (menu.BITS.End)
      {
  
        target1 = delayed_by_ms(now1, TIME_FIN);
        hardware_alarm_set_target(alarm_text, target1);
      }
      Flags.BITS.CONFIG_ALARM = 0;
      count_page++;
      
    }
    
    switch (menu.WORD)
    {
    case 1:
      //INICIO
      initUHD(&oled, &menu, 0);//Mostrar carita
      //Llevar esto a una función
      printf("Estado de reset\n");
      opcion.WORD = 0;
      break;    
    case 2:
      /* Menu 1*/
      //printf("------------------------ Menu 1 ------------------------\n");      
      gpio_set_irq_enabled_with_callback(17, GPIO_IRQ_EDGE_RISE, true, &start_alarm);
      gpio_set_irq_enabled_with_callback(18, GPIO_IRQ_EDGE_RISE, true, &start_alarm);
      gpio_set_irq_enabled_with_callback(19, GPIO_IRQ_EDGE_RISE, true, &start_alarm);
      gpio_set_irq_enabled_with_callback(20, GPIO_IRQ_EDGE_RISE, true, &start_alarm);

      //Mostrar el menu
      Menu1(&oled);
      break;
    case 4:
      /* Show Info*/
      //printf("------------------------ Mostrar info ------------------------\n");
      /*Esperar que la baliza haya llegado, la señal se da cuando la potencia de la 
      baliza supera el umbral*/

      //Info del lugar
      Info_Place(&opcion);
      if(count_page>3)
      {
        opcion.WORD = 0;
      }
      gpio_set_irq_enabled(17, GPIO_IRQ_EDGE_RISE, false);
      gpio_set_irq_enabled(18, GPIO_IRQ_EDGE_RISE, false);
      gpio_set_irq_enabled(19, GPIO_IRQ_EDGE_RISE, false);
      gpio_set_irq_enabled(20, GPIO_IRQ_EDGE_RISE, false);
      printf("Va en el menú: %x \n", menu.WORD);
      break;
    case 8:
      /* Final del recorrido*/
      //printf("------------------------ Adiós ------------------------\n");
      Despedida(&oled, count_page);
      break;

    default:
      menu.WORD = 1;
      break;
    }
    __wfi();
  }

  return 0;
}

void Despedida(SSOLED *oled, uint8_t count){
  printf("El valor del contador es: %d \n" , count);
  switch (count)
  {
  case 1:
    printf("Entró");
    oledFill(oled, 0,1);
    oledWriteString(oled, 0,0,1,(char *)"   ESTE ES EL", FONT_8x8, 0,1);
    oledWriteString(oled,0,0,2,(char*)  "    FIN DEL", FONT_8x8, 0,1);
    oledWriteString(oled,0,0,3,(char *) "   RECORRIDO.", FONT_8x8, 0,1);
    oledWriteString(oled,0,0,4,(char *) "    HA SIDO", FONT_8x8, 0,1);
    oledWriteString(oled,0,0,5,(char *) "   UN PLACER", FONT_8x8, 0,1); 
    oledWriteString(oled,0,0,6,(char *) "  ACOMPANARTE" , FONT_8x8, 0,1);
    break;
  case 2: 
    initUHD(oled, &menu, count_page);
    break;
  
  case 3:
    initUHD(oled, &menu, count_page);
    break;

  case 4:
    initUHD(oled, &menu, count_page);
    break;
  
  default:
    break;
  }
}
void Info_Place(opcion_t *op_m1){
  if (rc != OLED_NOT_FOUND){
    switch (op_m1->WORD)
    {
    case 1:
      Info_LED(&oled, count_page);
      break;

    case 2:
      Info_GITA(&oled, count_page);
    break;

    case 4:
      Info_Basicos(&oled, count_page);
      break;
    
    case 0x8:
      Info_Senales(&oled, count_page);
    break;

    default:
      break;
    }
  }

}
void initUHD(SSOLED *oled, state_menu *menu, uint8_t count){
  oledSetBackBuffer(oled, ucBuffer);
  switch (menu->WORD)
  {
  case 0x1:
    oledDumpBuffer(oled,PicandoOJO);
    //sleep_ms(3000);
    break;
  
  case 0x8:
    switch (count)
    {
    case 2:
      oledDumpBuffer(oled,Comenzar_Bostezar);
      break;
    
    case 3:
      oledDumpBuffer(oled,Bostezando);
      break;
    case 4:
      oledDumpBuffer(oled,Durmiendo);
      break;    
    default:
      break;
    }    
    break;
  
  default:
    break;
  }
}
