#include "main.h"
/*FALTA PROBAR*/
absolute_time_t now, target, now1, target1, now_w, target_w;
const uint alarm_text = 0;
const uint alarm_menu = 1;
const uint alarm_antena = 2;
uint8_t count_page = 0;
volatile uint32_t gpio_interrup, mask_interrup;

uint32_t slice_num = 0 ;
uint32_t dutymask = 0;

/* Fin de carrera*/
uint32_t gpio_final_carrera = 0xf;
uint32_t init_time, delta_time;

/*LINEA*/
const uint32_t mask_motor = 0xF << 10;
const uint32_t gpio_linea = 0x3 << 6;
const uint32_t gpio_boton = 0x1F << 18;
bool band_right = false;
bool band_left = false; 
uint8_t verificacion = 0, pos_cand = 0; //Posición candidata para la triangulación 
uint sliceNumMotorA = 0, sliceNumMotorB = 0;

bt_data_t Datos;

volatile uint32_t gpios;
/*--------------------- ISR RAG --------------------------*/
void alarm_duty(uint alarm){
    // Calcula el duty de la seañl cada 10ms
    measure_duty_cycle(slice_num,&Balizas[Baliza-1].WORD);
    Flags.BITS.Capture_ADC = 0;
    ////printf("CAPTURANDO RSSI: %d, CR: %d\n", Balizas[Baliza-1].BITS.RSSI, CR);
}


/*--------------------------- ISR MENU ---------------------------*/

void gpio_isr(uint gpio, uint32_t mask_event){//Antirrebote
  gpios = gpio_get_all()&(3<<6|0xf);
  ////printf("Entra a la IRQ, gpio: %x \n", gpio);
  switch (gpios){
    case 1://Fin de carrera 1
      if(gpio_get(gpio)){
        menu.BITS.choque = 1;
      }
      else{
        menu.BITS.choque = 0;
      }
    break;
    case 2://Fin de carrera 2
      if(gpio_get(gpio)){
      menu.BITS.choque = 1;
      }
      else{
        menu.BITS.choque = 0;
      }

    break;
    case 4://Fin de carrera 3
      if(gpio_get(gpio)){
      menu.BITS.choque = 1;
      }
      else{
        menu.BITS.choque = 0;
      }
    break;
    case 8://Fin de carrera 4
      if(gpio_get(gpio)){
      menu.BITS.choque = 1;
      }
      else{
        menu.BITS.choque = 0;
      }
    break; 
    case (1<<6):
        ////printf("IZQUIERDA ..............\n");
        band_left = true;
        break;
    case (1<<7):
        ////printf("DERECHA ..............\n");
        band_right = true;
        break;    
    default://Botones del menú
      ////printf("Desahabilita la interrupcion............... \n");
      now = get_absolute_time();
      target = delayed_by_ms(now, TIME_DEBOUNCE);
      hardware_alarm_set_target(alarm_menu, target);
      gpio_set_irq_enabled(18, GPIO_IRQ_EDGE_RISE, false);
      gpio_set_irq_enabled(19, GPIO_IRQ_EDGE_RISE, false);
      gpio_set_irq_enabled(20, GPIO_IRQ_EDGE_RISE, false);
      gpio_set_irq_enabled(21, GPIO_IRQ_EDGE_RISE, false);
      gpio_set_irq_enabled(22, GPIO_IRQ_EDGE_RISE, false);
      break;
  }
  //mask_interrup = mask_event;
  //gpio_interrup = gpio;
  gpio_acknowledge_irq(gpio_interrup,mask_interrup);
  ////printf("---------------------- start alarm -------------------\n");
}

void delay_show(uint alarm){//Tiempo para el cambio de página
  if((menu.BITS.Show_Info2 && count_page < 4) || (menu.BITS.End && count_page < 5))
  {
    //printf("------Alarma del texto ----\n");
    //printf("pag: %d\n",count_page);
    Flags1.BITS.CONFIG_ALARM = 1;
  } 
  else{
    count_page=0;
  }

}

void eleccion(uint alarm1){//Botones
  gpios = (gpio_get_all()>>18) & 0x1f;
  //printf("Entra a la interrupcion, gpio: %x \ngpios: %x\nmenu: %x\n", gpio_interrup,gpios, menu.WORD);
    switch (gpios)
    {
    case 1: //Rojo
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

    case 2: //1
      menu.BITS.Menu1 = 0;
      menu.BITS.Show_Info = 1;
      opcion.WORD = 0;
      opcion.BITS.LED = 1;
      Flags.BITS.Reset = 1;
      oledWriteString(&oled, 0,0,0,(char *)"Moviendome",FONT_8x8,0,1);
      gpio_set_irq_enabled(6, GPIO_IRQ_EDGE_RISE|GPIO_IRQ_EDGE_FALL, true);
      gpio_set_irq_enabled(7, GPIO_IRQ_EDGE_RISE|GPIO_IRQ_EDGE_FALL, true);
      pos_obj = '0';
      //Flags1.BITS.CONFIG_ALARM = 1;
      break;

    case 4: //2
      menu.BITS.Menu1 = 0;
      menu.BITS.Show_Info = 1;
      opcion.WORD = 0;
      opcion.BITS.GITA = 1;
      Flags.BITS.Reset = 1;
      oledWriteString(&oled, 0,0,0,(char *)"Moviendome",FONT_8x8,0,1);
      gpio_set_irq_enabled(6, GPIO_IRQ_EDGE_RISE|GPIO_IRQ_EDGE_FALL, true);
      gpio_set_irq_enabled(7, GPIO_IRQ_EDGE_RISE|GPIO_IRQ_EDGE_FALL, true);
      pos_obj = '1';
      //Flags1.BITS.CONFIG_ALARM = 1;
      break;

    case 8: //3
      menu.BITS.Menu1 = 0;
      menu.BITS.Show_Info = 1;
      opcion.WORD = 0;
      opcion.BITS.LEB = 1;
      Flags.BITS.Reset = 1;
      oledWriteString(&oled, 0,0,0,(char *)"Moviendome",FONT_8x8,0,1);
      gpio_set_irq_enabled(6, GPIO_IRQ_EDGE_RISE|GPIO_IRQ_EDGE_FALL, true);
      gpio_set_irq_enabled(7, GPIO_IRQ_EDGE_RISE|GPIO_IRQ_EDGE_FALL, true);
      pos_obj = '2';
      //Flags1.BITS.CONFIG_ALARM = 1;
      break;

    case 16: //4
      menu.BITS.Menu1 = 0;
      menu.BITS.Show_Info = 1;
      opcion.WORD = 0;
      opcion.BITS.LTS = 1;
      Flags.BITS.Reset = 1;
      oledWriteString(&oled, 0,0,0,(char *)"Moviendome",FONT_8x8,0,1);
      gpio_set_irq_enabled(6, GPIO_IRQ_EDGE_RISE|GPIO_IRQ_EDGE_FALL, true);
      gpio_set_irq_enabled(7, GPIO_IRQ_EDGE_RISE|GPIO_IRQ_EDGE_FALL, true);
      pos_obj = '3';
      //Flags1.BITS.CONFIG_ALARM = 1;
      break;

    default:
      break;
    }
    if(gpios&0x1e){
        gpio_set_irq_enabled(19, GPIO_IRQ_EDGE_RISE, false);
        gpio_set_irq_enabled(20, GPIO_IRQ_EDGE_RISE, false);
        gpio_set_irq_enabled(21, GPIO_IRQ_EDGE_RISE, false);
        gpio_set_irq_enabled(22, GPIO_IRQ_EDGE_RISE, false);
    }
    gpio_set_irq_enabled(18, GPIO_IRQ_EDGE_RISE, true);
}

int main() {
  stdio_init_all();
  sleep_ms(4000);

  //printf("Iniciando \n");
  //Configuración de los pines
  gpio_init_mask(mask_motor | (1<<PIN_PWM_ENABLE) | (1<<MEASURE_PIN) | gpio_final_carrera | gpio_linea | gpio_boton);

  gpio_set_dir_out_masked(mask_motor| (1<<PIN_PWM_ENABLE) | (1<<MEASURE_PIN));

  gpio_pull_down(0);
  gpio_pull_down(1);
  gpio_pull_down(2);
  gpio_pull_down(3);
  gpio_pull_down(18);
  gpio_pull_down(19);
  gpio_pull_down(20);
  gpio_pull_down(21);
  gpio_pull_down(22);

  gpio_set_input_hysteresis_enabled(0, true);
  gpio_set_input_hysteresis_enabled(1, true);
  gpio_set_input_hysteresis_enabled(2, true);
  gpio_set_input_hysteresis_enabled(3, true);
  gpio_set_input_hysteresis_enabled(18,true);
  gpio_set_input_hysteresis_enabled(19,true);
  gpio_set_input_hysteresis_enabled(20,true);
  gpio_set_input_hysteresis_enabled(21,true);
  gpio_set_input_hysteresis_enabled(22,true);

  /*-------------------------------------- FINAL DE CARRERA --------------------------------------*/
  gpio_set_irq_enabled_with_callback(0, GPIO_IRQ_EDGE_RISE|GPIO_IRQ_EDGE_FALL, false, &gpio_isr);
  gpio_set_irq_enabled_with_callback(1, GPIO_IRQ_EDGE_RISE|GPIO_IRQ_EDGE_FALL, false, &gpio_isr);
  gpio_set_irq_enabled_with_callback(2, GPIO_IRQ_EDGE_RISE|GPIO_IRQ_EDGE_FALL, false, &gpio_isr);
  gpio_set_irq_enabled_with_callback(3, GPIO_IRQ_EDGE_RISE|GPIO_IRQ_EDGE_FALL, false, &gpio_isr);

  /*-------------------------------------- SENSOR DE LINEA --------------------------------------*/
  gpio_set_irq_enabled_with_callback(6, GPIO_IRQ_EDGE_RISE|GPIO_IRQ_EDGE_FALL, false, &gpio_isr);
  gpio_set_irq_enabled_with_callback(7, GPIO_IRQ_EDGE_RISE|GPIO_IRQ_EDGE_FALL, false, &gpio_isr);

  /*-------------------------------------- BOTONES --------------------------------------*/
  gpio_set_irq_enabled_with_callback(18, GPIO_IRQ_EDGE_RISE, false, &gpio_isr);
  gpio_set_irq_enabled_with_callback(19, GPIO_IRQ_EDGE_RISE, false, &gpio_isr);
  gpio_set_irq_enabled_with_callback(20, GPIO_IRQ_EDGE_RISE, false, &gpio_isr);
  gpio_set_irq_enabled_with_callback(21, GPIO_IRQ_EDGE_RISE, false, &gpio_isr);
  gpio_set_irq_enabled_with_callback(22, GPIO_IRQ_EDGE_RISE, false, &gpio_isr);

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

  gpio_set_function(MEASURE_PIN, GPIO_FUNC_PWM);
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

  //Inicialización de PWMs
  //init_pwm(&cfg1, &cfg2);
  cfg1 = pwm_get_default_config();
  pwm_config_set_clkdiv_mode(&cfg1, PWM_DIV_B_HIGH);
  pwm_config_set_clkdiv(&cfg1, 50);

  // ENABLE PWM MOTOR A
  gpio_set_function(PIN_PWM_ENABLEA, GPIO_FUNC_PWM);
  // Determine the PWM slice connected to GPIO: PWM_GPIO_CHA
  sliceNumMotorA = pwm_gpio_to_slice_num(PIN_PWM_ENABLEA);
  // Set period for frequency divisor
  pwm_set_clkdiv_int_frac(sliceNumMotorA, PWM_DIV_INTEGER, PWM_DIV_FRAC); // What frequency enters to the PWM?
  // Set top (wrap) value (Determines the frequency)
  pwm_set_wrap(sliceNumMotorA, PWM_TOP_VALUE);
  // Set zero duty
  pwm_set_chan_level(sliceNumMotorA, PWM_CHA, PWM_DUTY_ZERO);
  // Enable PWM
  pwm_set_enabled(sliceNumMotorA, true); 
  
  //Motor B
  gpio_set_function(PIN_PWM_ENABLEB, GPIO_FUNC_PWM);
  // Determine the PWM slice connected to GPIO: PWM_GPIO_CHA
  sliceNumMotorB = pwm_gpio_to_slice_num(PIN_PWM_ENABLEB);
  // Set period for frequency divisor
  pwm_set_clkdiv_int_frac(sliceNumMotorB, PWM_DIV_INTEGER, PWM_DIV_FRAC); // What frequency enters to the PWM?
  // Set top (wrap) value (Determines the frequency)
  pwm_set_wrap(sliceNumMotorB, PWM_TOP_VALUE);
  // Set zero duty
  pwm_set_chan_level(sliceNumMotorB, PWM_CHA, PWM_DUTY_ZERO);
  // Enable PWM
  pwm_set_enabled(sliceNumMotorB, true); 

  //Inicia en el estado de Reset
  Flags.WORD = 0;
  menu.WORD = 1;//Parte del reset
  slice_num = pwm_gpio_to_slice_num(MEASURE_PIN);


  //Configuración de la alarma
  if(hardware_alarm_is_claimed(alarm_antena)){
    //printf("NO ES POSIBLE USAR ESTA ALARMA \n");
  }
  else{
    hardware_alarm_claim(alarm_antena);
    hardware_alarm_set_callback(alarm_antena, &alarm_duty);
    ////printf("Configurar alarma antena \n");
  }
   adc_init();
    // Make sure GPIO is high-impedance, no pullups etc
    adc_gpio_init(ADC_MEASURE_PIN);
    // Select ADC input 0 (GPIO26)
    adc_select_input(0);
  //Pantalla
  char szTemp[32];
  rc = oledInit(&oled, OLED_128x64, 0x3c, 0, 0, 1, SDA_PIN, SCL_PIN, RESET_PIN, 1000000L);
  // Inicializacion de bluethoot 
  bt_init();
  bt_set_data(&Datos);

  //Inicializacion de los datos
  Datos.n_bat = adc_read();
  Datos.bl_rssi[0] = 000; 
  Datos.bl_rssi[1] = 000;
  Datos.bl_rssi[2] = 000; 
  Datos.bl_rssi[3] = 000;  
  Datos.sw[0] = 0;
  Datos.sw[1] = 0;
  Datos.sw[2] = 0;
  Datos.sw[3] = 0;

  // obtiene la hora y la configura
  bool check = bt_get_hour();
  if (check)
  {
    bt_transmit_ok();
  }
  else{
    bt_transmit_nok();
  }
  
  // tramsite la inforcacion cada cierto tiempo
  bt_transmit_alarm();

  while (1)
  { 

    ////printf("Va en el menú: %x, op: %x, flags : %x\n", menu.WORD,opcion.WORD, Flags.WORD);
    switch (menu.WORD)
    {
    case 1:
      //INICIO
      gpio_set_irq_enabled_with_callback(18, GPIO_IRQ_EDGE_RISE, true, &gpio_isr);
      initUHD(&oled, &menu, 0);//Mostrar carita
      //Llevar esto a una función
      //printf("Estado de reset\n");
      opcion.WORD = 0;
      Flags.BITS.Reset=0;
      //bt_transt_buff(&Datos);
      break;    
    case 2:
      /* Menu 1*/
      ////printf("------------------------ Menu 1 ------------------------\n");      
      gpio_set_irq_enabled(19, GPIO_IRQ_EDGE_RISE, true);
      gpio_set_irq_enabled(20, GPIO_IRQ_EDGE_RISE, true);
      gpio_set_irq_enabled(21, GPIO_IRQ_EDGE_RISE, true);
      gpio_set_irq_enabled(22, GPIO_IRQ_EDGE_RISE, true);

      //Mostrar el menu
      Menu1(&oled);
      Flags.BITS.Reset=0;
      break;
    case 4:
      /* Show Info*/
      ////printf("------------------------ Mostrar info ------------------------\n");

      switch (Flags.WORD){
        /*----------------------------------- TRIANGULACIÓN -----------------------------------*/
        case 1:
            Reset();
            uart_puts(UART_ID, str);
            break;
        case 12:
            //Pasar a .h
            pwm_init(slice_num, &cfg1, false);
            //CONFIGURACIÓN ALARMA 10MS
            now = get_absolute_time();
            target = delayed_by_ms(now, COUNT_MEASURE_TIME);
            hardware_alarm_set_target(alarm_antena, target);
            pwm_set_enabled(slice_num, true);
            break;
        case 0x10:
            Fin_Trama();
            Triangulación();
            Move();      
            actu_data();
            break;
        default:
            break;
      }
      if (!Flags.BITS.Reset || !Flags.BITS.OK)
      {
        ////printf("Correccion...................\n");
        Move(); 
      }
      
      ////printf("El estado de la alarma de texto es: %x \n", Flags1.BITS.CONFIG_ALARM);
      //Info del lugar
      if(Flags.BITS.Llego){
        menu.BITS.Show_Info2 = 1;
        menu.BITS.Show_Info = 0; 
      }

      break;

    case 0x24: //choque de objeto
      Colision(&oled);
      actu_data();
      Move();
    break;

    case 16:
          Info_Place(&opcion);
          if(count_page>3)
          {
            opcion.WORD = 0;
          }
          ////printf("Va en el menú: %x \n", menu.WORD);
    break;
    case 8:
      /* Final del recorrido*/
      ////printf("------------------------ Adiós ------------------------\n");
      Despedida(&oled, count_page);
      Flags.BITS.Llego = 0;
      break;

    default:
      menu.WORD = 1;
      break;
    }
    if (Flags1.BITS.CONFIG_ALARM)
    {
      ////printf("Cambiando de pagina\n");
      now1 = get_absolute_time();
      if(menu.BITS.Show_Info2){
        
        target1 = delayed_by_ms(now1, TIME_TEXT);
        hardware_alarm_set_target(alarm_text, target1);
      }
      else if (menu.BITS.End)
      {
        /*target1 = delayed_by_ms(now1, TIME_FIN);
        hardware_alarm_set_target(alarm_text, target1);*/
        if(count_page == 1){
          target1 = delayed_by_ms(now1, TIME_TEXT);
        }
        else{
          target1 = delayed_by_ms(now1, TIME_FIN);
        }
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
    ////printf("Banderas : %x", Flags.WORD);
    switch (Flags.WORD & (0x3<<1))
    {
    case 2:
        // Recepción de OK
        BOK[count] = c;
        break;

    case 4:
        // Recibiendo trama de cada una de las balizas
        ////printf("CR: %d, %c\n", CR,c);
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
    if(CR%10 == 6  && Flags.BITS.Capture){
        // Habilita el calculo del duty del duty 
        Flags.BITS.Capture_ADC = 1;
    }
    count++;////printf("\n");
}

inline void Triangulación(){
    for (uint8_t i = 0; i < sizeof(UB); i++)
    {
      //printf("La pos actual es: %d, y la pos objetivo es: %d\n", pos_act, pos_obj);
        if((Balizas[i].BITS.RSSI) > Umbral){
          if(Balizas[i].BITS.NI>47){//47 se refiere al char
            if(Balizas[i].BITS.NI == pos_cand){
              pos_act = pos_cand; 
            }
            else{
              pos_cand = Balizas[i].BITS.NI;
            }
              //printf("Baliza: %x \n", Balizas[i].WORD);
            ////printf("La pos despues de la triangulacion es: %d \n", pos_act);
          }   
        }        
    }    
}

inline void Move(){
    //Pin 10: Enable
    //Pin 11 : Salida A
    //Pin 12 : Salida B
    ////printf("La pos actual es: %d, y la pos objetivo es: %d\n", pos_act, pos_obj);
    if (pos_act != 0)
    {
      if (pos_act > pos_obj)
      {
          //pwm_set_enabled(slice_num_motor, true);
          gpio_put_masked(mask_motor & (0x5 << 11), mask_motor & (0x1 << 11));
          ////printf("Ir hacia atrás \n");
          Flags.BITS.Llego = 0;//creo que esto se puede poner al principio
          Corregir_atras();
      }
      else if (pos_act < pos_obj)
      {
          gpio_put_masked(mask_motor & (0x5 << 11), mask_motor & (0x1 << 13));
          /*gpio_put(13,true);
          gpio_put(11, false);*/
          ////printf("Ir hacia adelante, y el valor de los gpios son: %d, %d\n",gpio_get(11), gpio_get(13));
          Flags.BITS.Llego = 0;
          Corregir_adelante();
          
      }
      else{
          //pwm_set_enabled(slice_num_motor, false);
          gpio_put_masked(mask_motor & (0x5 << 11), 0x0);
          //printf("Se deshabilita la interrupcion \n"); 
          Flags.BITS.Llego = 1;
          Flags1.BITS.CONFIG_ALARM = 1;
          Flags.BITS.Reset = 0;
          gpio_set_irq_enabled(6, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, false);
          gpio_set_irq_enabled(7, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, false);
      } 
      if(menu.BITS.choque){
        gpio_put_masked(mask_motor & (0x5 << 11), 0x0);
        //printf("Choque \n");
      }
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
    //printf("El comando ha sido enviado \n");
}

void Fin_Trama(){// Hacer aquí la triangulación
    //printf("-------------- FINALIZACION DE LA TRAMA --------------\n");
    for (uint8_t i = 0; i < N_BALIZAS; i++)
    {
        //printf("El valor de la baliza %d es igual a %d \n", Balizas[i].BITS.NI, Balizas[i].BITS.RSSI);
    }
    Flags.WORD = 1;
}
void Despedida(SSOLED *oled, uint8_t count){
  //printf("El valor del contador es: %d \n" , count);
  switch (count)
  {
  case 1:
    //printf("Entró");
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
void Corregir_adelante(){
  if(!gpio_get(6) && !gpio_get(7)){
      ////printf("Va derecho.........\n");
      pwm_set_chan_level(sliceNumMotorB, PWM_CHA, PWM_DUTY_ZERO);
      pwm_set_chan_level(sliceNumMotorA, PWM_CHA, PWM_DUTY_ZERO);
      band_right = false;
      band_left = false;
  }

  else if (band_right)
  {    
      ////printf("Corrige derecha................. \n");
      pwm_set_chan_level(sliceNumMotorA, PWM_CHA, PWM_ZERO);
      pwm_set_chan_level(sliceNumMotorB, PWM_CHA, PWM_LEVEL);
      band_right = false;
  }
  else if (band_left)
  {
      ////printf("Corrige izquierda................. \n");
      pwm_set_chan_level(sliceNumMotorB, PWM_CHA, PWM_ZERO);
      pwm_set_chan_level(sliceNumMotorA, PWM_CHA, PWM_LEVEL);

      band_left = false;
  }
}
void Corregir_atras(){
  if(!gpio_get(6) && !gpio_get(7)){
      ////printf("Va derecho.........\n");
      pwm_set_chan_level(sliceNumMotorB, PWM_CHA, PWM_DUTY_ZERO);
      pwm_set_chan_level(sliceNumMotorA, PWM_CHA, PWM_DUTY_ZERO);
  }

  else if (band_right)
  {    
      ////printf("Corrige derecha................. \n");
      pwm_set_chan_level(sliceNumMotorA, PWM_CHA, PWM_LEVEL);
      pwm_set_chan_level(sliceNumMotorB, PWM_CHA, PWM_ZERO);
      band_right = false;
  }
  else if (band_left)
  {
      ////printf("Corrige izquierda................. \n");
      pwm_set_chan_level(sliceNumMotorB, PWM_CHA, PWM_LEVEL);
      pwm_set_chan_level(sliceNumMotorA, PWM_CHA, PWM_ZERO);

      band_left = false;
  }
}
void actu_data(){
  uint32_t values = gpio_get_all();
  Datos.n_bat = adc_read();
  Datos.sw[0] = values&1;
  Datos.sw[1] = values&(1<<1);
  Datos.sw[2] = values&(1<<2);
  Datos.sw[3] = values&(1<<3);
  for(uint8_t i = 0; i<N_BALIZAS; i++){
    Datos.bl_rssi[(Balizas[i].BITS.NI-48)]=Balizas[i].BITS.RSSI;
  }
}