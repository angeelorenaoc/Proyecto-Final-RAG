#include "main.h"
/*FALTA PROBAR*/
absolute_time_t now, target, now1, target1;
const uint alarm_menu = 1;
const uint alarm_text = 0;
uint8_t count_page = 0;
uint32_t gpio_interrup, mask_interrup;

/*OTRO CÓDIGO*/
uint32_t slice_num = 0 ;
uint32_t dutymask = 0;
uint32_t measured_duty_cycle = 0 ;
absolute_time_t now, target, now_w, target_w;
const uint alarm_antena = 2;
uint32_t mask_motor = 0x7 << 10;

/*--------------------- ISR RAG --------------------------*/
void alarm_duty(uint alarm){
    // Calcula el duty de la seañl cada 10ms
    measure_duty_cycle(slice_num,&Balizas[Baliza-1].WORD);
    Flags.BITS.Capture_ADC = 0;
    //printf("CAPTURANDO RSSI: %d, CR: %d\n", Balizas[Baliza-1].BITS.RSSI, CR);
}
/*void watchdog_irq(uint alarm){
    if(!Flags.BITS.Capture){
        now_w = get_absolute_time();
        target_w = delayed_by_ms(now, WATCHDOG_TIME);
        hardware_alarm_set_target(watchdog_XBEE, target_w);
        printf("Reenviando\n");
    }
}*/

/*--------------------------- ISR MENU ---------------------------*/

void start_alarm(uint gpio, uint32_t mask_event){//Antirrebote
  //printf("---------------------- start alarm -------------------\n");
  now = get_absolute_time();
  target = delayed_by_ms(now, TIME_DEBOUNCE);
  hardware_alarm_set_target(alarm_menu, target);
  gpio_set_irq_enabled(16, GPIO_IRQ_EDGE_RISE, false);
  gpio_set_irq_enabled(17, GPIO_IRQ_EDGE_RISE, false);
  gpio_set_irq_enabled(18, GPIO_IRQ_EDGE_RISE, false);
  gpio_set_irq_enabled(19, GPIO_IRQ_EDGE_RISE, false);
  gpio_set_irq_enabled(20, GPIO_IRQ_EDGE_RISE, false);
  gpio_interrup = gpio;
  mask_interrup = mask_event;
  gpio_acknowledge_irq(gpio_interrup,mask_interrup);//Probar
}

void delay_show(uint alarm){//Tiempo para el cambio de página
  if((menu.BITS.Show_Info2 && count_page < 4) || (menu.BITS.End && count_page < 5))
  {
    printf("------Alarma del texto ----\n");
    printf("pag: %d\n",count_page);
    Flags1.BITS.CONFIG_ALARM = 1;
  } 
  else{
    count_page=0;
  }

}

void eleccion(uint alarm1){//Botones
  //printf("Entra a la interrupcion, gpio: %x \n", gpio_interrup);
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
      case 16: //Ir a la despedida
        menu.BITS.End = 1;
        menu.BITS.Show_Info2 = 0;
        Flags1.BITS.CONFIG_ALARM = 1;
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
      Flags.BITS.Reset = 1;
      oledWriteString(&oled, 0,0,0,(char *)"Moviendome",FONT_8x8,0,1);
      //Flags1.BITS.CONFIG_ALARM = 1;
      break;

    case 18: //2
      menu.BITS.Menu1 = 0;
      menu.BITS.Show_Info = 1;
      opcion.WORD = 0;
      opcion.BITS.GITA = 1;
      Flags.BITS.Reset = 1;
      oledWriteString(&oled, 0,0,0,(char *)"Moviendome",FONT_8x8,0,1);
      //Flags1.BITS.CONFIG_ALARM = 1;
      break;

    case 19: //3
      menu.BITS.Menu1 = 0;
      menu.BITS.Show_Info = 1;
      opcion.WORD = 0;
      opcion.BITS.LEB = 1;
      Flags.BITS.Reset = 1;
      oledWriteString(&oled, 0,0,0,(char *)"Moviendome",FONT_8x8,0,1);
      //Flags1.BITS.CONFIG_ALARM = 1;
      break;

    case 20: //4
      menu.BITS.Menu1 = 0;
      menu.BITS.Show_Info = 1;
      opcion.WORD = 0;
      opcion.BITS.LTS = 1;
      Flags.BITS.Reset = 1;
      oledWriteString(&oled, 0,0,0,(char *)"Moviendome",FONT_8x8,0,1);
      //Flags1.BITS.CONFIG_ALARM = 1;
      break;

    default:
      break;
    }
    if(gpio_interrup!=16){
        gpio_set_irq_enabled(17, GPIO_IRQ_EDGE_RISE, false);
        gpio_set_irq_enabled(18, GPIO_IRQ_EDGE_RISE, false);
        gpio_set_irq_enabled(19, GPIO_IRQ_EDGE_RISE, false);
        gpio_set_irq_enabled(20, GPIO_IRQ_EDGE_RISE, false);
    }
}

int main() {
  stdio_init_all();
  sleep_ms(4000);

  printf("Iniciando \n");
  //Configuración de los pines
  gpio_init_mask(mask_boton|MEASURE_PIN);
  gpio_init_mask(mask_motor | (1<<PIN_PWM_ENABLE) | (1<<MEASURE_PIN));
  gpio_set_dir_out_masked(mask_motor| (1<<PIN_PWM_ENABLE) | (1<<MEASURE_PIN)| (1<<RESET_XBEE_PIN));


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
  hardware_alarm_claim(alarm_menu);
  hardware_alarm_set_callback(alarm_menu, &eleccion);
  //Alarma para el texto 
  hardware_alarm_claim(alarm_text);
  hardware_alarm_set_callback(alarm_text, &delay_show);

  //------------------------------------ UART------------------------------------ 
    uart_init(UART_ID, BAUD_RATE);

    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select
    gpio_set_function(UART_TX_PIN, UART_FUNCSEL_NUM(UART_ID, UART_TX_PIN));
    gpio_set_function(UART_RX_PIN, UART_FUNCSEL_NUM(UART_ID, UART_RX_PIN));

    int __unused actual = uart_set_baudrate(UART_ID, BAUD_RATE);

    // Set UART flow control CTS/RTS, we don't want these, so turn them off
    uart_set_hw_flow(UART_ID, false, false);

    // Set our data format
    uart_set_format(UART_ID, DATA_BITS, STOP_BITS, PARITY);

    uart_set_fifo_enabled(UART_ID, true);
    
    // Select correct interrupt for the UART we are using
    int UART_IRQ = UART_ID == uart0 ? UART0_IRQ : UART1_IRQ;
     // And set up and enable the interrupt handlers
    irq_set_exclusive_handler(UART_IRQ, Capture);
    irq_set_enabled(UART_IRQ, true);

    // Now enable the UART to send interrupts - RX only
    uart_set_irq_enables(UART_ID, true, false);

    //Inicialización del PWM
    //init_pwm(&cfg1, &cfg2);
    cfg1 = pwm_get_default_config();
    pwm_config_set_clkdiv_mode(&cfg1, PWM_DIV_B_HIGH);
    pwm_config_set_clkdiv(&cfg1, 50);

    // Enable PWM
    gpio_set_function(PIN_PWM_ENABLE, GPIO_FUNC_PWM);
    // Determine the PWM slice connected to GPIO: PWM_GPIO_CHA
    uint sliceNum = pwm_gpio_to_slice_num(PIN_PWM_ENABLE);
    // Set period for frequency divisor
    pwm_set_clkdiv_int_frac(sliceNum, PWM_DIV_INTEGER, PWM_DIV_FRAC); // What frequency enters to the PWM?
    // Set top (wrap) value (Determines the frequency)
    pwm_set_wrap(sliceNum, PWM_TOP_VALUE);
    // Set zero duty
    pwm_set_chan_level(sliceNum, PWM_CHA, PWM_DUTY_ZERO);
    // Enable PWM
    pwm_set_enabled(sliceNum, true);


  //Configuración de la alarma
  if(hardware_alarm_is_claimed(alarm_antena)){
    printf("NO ES POSIBLE USAR ESTA ALARMA \n");
  }
  else{
    hardware_alarm_claim(alarm_antena);
    hardware_alarm_set_callback(alarm_antena, &alarm_duty);
    //printf("Configurar alarma antena \n");
  }

  //Pantalla
  char szTemp[32];
  rc = oledInit(&oled, OLED_128x64, 0x3c, 0, 0, 1, SDA_PIN, SCL_PIN, RESET_PIN, 1000000L);
  //initUHD(&oled);
  //printf("Va aquí \n");
  menu.WORD = 1;//Parte del reset
  //Flags.WORD = 1;

  while (1)
  { 
    //printf("Va en el menú: %x, op: %x\n", menu.WORD,opcion.WORD);
    
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
      switch (Flags.WORD){
        case 1:
            Reset();
            uart_puts(UART_ID, str);
            break;
        case 12:
            //Pasar a .h
            pwm_init(slice_num, &cfg1, false);
            //CONFIGURACIÓN ALARMA 10MS
            gpio_set_function(MEASURE_PIN, GPIO_FUNC_PWM);
            now = get_absolute_time();
            target = delayed_by_ms(now, COUNT_MEASURE_TIME);
            hardware_alarm_set_target(alarm_antena, target);
            slice_num = pwm_gpio_to_slice_num(MEASURE_PIN);
            pwm_set_enabled(slice_num, true);
            break;
        case 0x10:
            Fin_Trama();
            Triangulación();
            Move();           
            break;
        default:
            break;
      }
      //printf("El estado de la alarma de texto es: %x \n", Flags1.BITS.CONFIG_ALARM);
      //Info del lugar
      if(Flags.BITS.Llego){
        menu.BITS.Show_Info2 = 1;
        menu.BITS.Show_Info = 0;
      }

      break;
    case 16:
          Info_Place(&opcion);
          if(count_page>3)
          {
            opcion.WORD = 0;
          }
          printf("Va en el menú: %x \n", menu.WORD);
    break;
    case 8:
      /* Final del recorrido*/
      //printf("------------------------ Adiós ------------------------\n");
      Despedida(&oled, count_page);
      Flags.BITS.Llego = 0;
      break;

    default:
      menu.WORD = 1;
      break;
    }
    if (Flags1.BITS.CONFIG_ALARM)
    {
      //printf("Cambiando de pagina\n");
      now1 = get_absolute_time();
      if(menu.BITS.Show_Info2){
        
        target1 = delayed_by_ms(now1, TIME_TEXT);
        hardware_alarm_set_target(alarm_text, target1);
      }
      else if (menu.BITS.End)
      {
  
        target1 = delayed_by_ms(now1, TIME_FIN);
        hardware_alarm_set_target(alarm_text, target1);
      }
      Flags1.BITS.CONFIG_ALARM = 0;
      count_page++;
    }
    if(!Flags.BITS.Reset){
         __wfi();
    }
  }

  return 0;
}
/// ---------------- Funciones otro código ---------------- ///

void Capture(){
    char c = uart_getc(UART_ID);
    //printf("Banderas : %x", Flags.WORD);
    switch (Flags.WORD & (0x3<<1))
    {
    case 2:
        // Recepción de OK
        BOK[count] = c;
        break;

    case 4:
        // Recibiendo trama de cada una de las balizas
        //printf("CR: %d, %c\n", CR,c);
        if(c != 0x0D && CR%10 == 3){
            // Captura y almacena el ID de la baliza
            Balizas[Baliza].BITS.NI = c;
            Baliza++;
        }
        if(c == 0x0D){
            if(CR == N_CRpb){
                //Indica Gin de trama
                Flags.BITS.EndMsg = 1;
                Flags.BITS.Capture = 0;
            }
            CR++;            
        }
        break;
    default:
        break;
    }
    if(count == N_OK && Flags.BITS.OK){
        //Final del OK recibido

        Flags.BITS.Capture = 1;
        Flags.BITS.OK = 0;
        count = 0;
        uart_puts(UART_ID, at_comand);
    }
    if(CR%10 == 2  && Flags.BITS.Capture){
        // Habilita el calculo del duty del duty 
        Flags.BITS.Capture_ADC = 1;
    }
    count++;//printf("\n");
}

inline void Triangulación(){
    for (uint8_t i = 0; i < sizeof(UB); i++)
    {
        if((Balizas[i].BITS.RSSI) > Umbral){
            printf("Baliza: %x \n", Balizas[i].WORD);
            if(Balizas[i].BITS.NI>47){
            pos_act = Balizas[i].BITS.NI;
            printf("La pos despues de la triangulacion es: %d \n", pos_act);
            }   
        }
    }    
}

inline void Move(){
    //Pin 10: Enable
    //Pin 11 : Salida A
    //Pin 12 : Salida B
    printf("La pos actual y obj son: %d, %d \n", pos_act, pos_obj);
    if (pos_act > pos_obj)
    {
        //pwm_set_enabled(slice_num_motor, true);
        gpio_put_masked(mask_motor, mask_motor & (0x1 << 11));
        printf("Ir hacia atrás \n");
        Flags.BITS.Llego = 0;//creo que esto se puede poner al principio
    }
    else if (pos_act < pos_obj)
    {
        //pwm_set_enabled(slice_num_motor, true);
        gpio_put_masked(mask_motor, mask_motor & (0x1 << 12));
        printf("Ir hacia adelante, banderas Xbee %x, banderas OLED %x\n",Flags.WORD,menu.WORD);
        Flags.BITS.Llego = 0;
    }
    else{
        //pwm_set_enabled(slice_num_motor, false);
        gpio_put_masked(mask_motor, 0x0);
        printf("Quieto \n"); 
        Flags.BITS.Llego = 1;
        Flags1.BITS.CONFIG_ALARM = 1;
        Flags.BITS.Reset = 0;
    } 
}

void Reset(){
    for (uint8_t i = 0; i < N_BALIZAS; i++)
    {
        Balizas[i].WORD = 0; 
    }
    count = 0;
    CR = 0;
    Flags.WORD = 2;
    Baliza = 0;
    printf("El comando ha sido enviado \n");
}

void Fin_Trama(){// Hacer aquí la triangulación
    printf("-------------- FINALIZACION DE LA TRAMA --------------\n");
    for (uint8_t i = 0; i < N_BALIZAS; i++)
    {
        printf("El valor de la baliza %d es igual a %d \n", Balizas[i].BITS.NI, Balizas[i].BITS.RSSI);
    }
    Flags.WORD = 1;
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
