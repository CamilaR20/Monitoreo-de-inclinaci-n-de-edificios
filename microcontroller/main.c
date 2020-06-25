
#include "xc.h"
#include "p24FV32KA304.h"
#include "mcc_generated_files/system.h"
#include "mcc_generated_files/mcc.h"
#include "nuestrostimers.h"
#include "rtc_epson.h"
#include "cola.h"
#include "process_FSM.h"
#include "Iirfilt_DF2SOS.h"


#define EEPROM_WRITE    0b00000001
#define EEPROM_RTC      0b00000010
#define TIMER1_INT      0b00000100     // Cada 2 ms 
#define INCL_OK         0b00010000 
#define TIMER2_INT      0b00100000
#define TIMER1_WMEM     0b01000000
#define TIMER1_TMO      0b10000000

// Tiempo para guardar datos en memoria
#define COUNT_SAVE_DATA     10     // El timer cuenta cada segundo, entonces el timer por software debe contar 900 para 15min
#define STANDBYM        0b00000001

Iir_2order iir1;
Iir_2order iir2;

volatile char banderas = 0b00000000;
volatile char banderas2 = 0b00000000;
//Acumuladores de ADC
uint16_t adc_incl = 0;
uint8_t dato_vpan = 0;
uint16_t dato_vpan_aux = 0;
uint8_t dato_vbat = 0;
uint16_t dato_vbat_aux = 0;
uint16_t adc_acum = 0;

uint16_t cont = 0;
uint16_t i[3];

int cont_adc = 1;


 
 
void TM1_INTERRUPT(void){           // Timer a 2ms
    banderas |= TIMER1_INT; 
    banderas |= TIMER1_TMO;
    banderas2 |= TIMER1_TMO;
    banderas |= TIMER1_WMEM;
}

void Low_compsumption (void){
    
    LED1_SetLow();
    LED2_SetLow();
    LED3_SetLow();
    LED4_SetLow();
    
    uint16_t int0 = IEC0;
    uint16_t int1 = IEC1;
    uint16_t int2 = IEC2;
    uint16_t int3 = IEC3;
    uint16_t int4 = IEC4;
    uint16_t int5 = IEC5;
    
    IEC0 = 0;
    IEC1 = 0;
    IEC2 = 0;
    IEC3 = 0;
    IEC4 = 0;
    IEC5 = 0;
    
    IEC0bits.U1RXIE = 1;
    
    Nop();
    Nop();
    Nop();
    
    asm("PWRSAV #1");
    
    Nop();
    
    Idle();
    
    
    IEC0 = int0;
    IEC1 = int1;
    IEC2 = int2;
    IEC3 = int3;
    IEC4 = int4;
    IEC5 = int5;
    
}

void TM2_INTERRUPT(void){           // Timer a 1s
    banderas |= TIMER2_INT;
}

void ADC1_CallBack(void){           // Muestrear inclinometro
    int sal_iir1 = 0;
    
    cont = ADC1_ConversionResultBufferGet(&i[0]);
    
    dato_vpan_aux = i[1];
    dato_vpan = dato_vpan_aux>>4;
    
    dato_vbat_aux = i[2];
    dato_vbat = dato_vbat_aux>>4;
    
    //sal_iir1 = calc_iir(i[0], &iir1);
    //adc_acum += calc_iir(sal_iir1, &iir2);

   cont_adc++;
   if(cont_adc >= 32){
      cont_adc = 1;
      adc_incl = adc_acum;
      adc_acum = 0;
      banderas |= INCL_OK;
   }
}



int main(void)
{
    SYSTEM_Initialize();
            
    TMR1_Start();
    TMR1_SetInterruptHandler(TM1_INTERRUPT);
    TMR2_Start();
    TMR2_SetInterruptHandler(TM2_INTERRUPT);
    
    UART1_Initialize();
    
    U1MODEbits.USIDL = 0;
    DSCONbits.DSEN = 0;
 
    ADC1_Initialize ();
    ADC1_ChannelSelect(ADC1_CHANNEL_AN4);
    ADC1_Start();
   
    banderas = 0b00000000;
   
    init_iir(&iir1, 0.991153597831726, 1, -1.432918429374695, 0.982220232486725, 1, -1.458052396774292, 1);
    init_iir(&iir2, 0.991153597831726, 1, -1.457277178764343, 0.982550680637360, 1, -1.458052396774292, 1);
    
    
    Cola cola_inclitx;                      // Cola para los datos del inclinometro
    Init_Cola(&cola_inclitx);
    
    Cola cola_memtx;                      // Cola para los datos de memoria
    Init_Cola(&cola_memtx);
    
    Tm_Periodico Tiempo_REEPROM_RTC;          // Tiempo de espera antes de leer la EEPROM despues de escribirla
    Tm_Periodico Tiempo_REEPROM;             // Tiempo de espera antes de leer la EEPROM despues de escribirla
    Tm_Periodico Tiempo_WEEPROM;             // Tiempo para guardar datos (15 minutos)
    Tm_Periodico Tiempo_RXTMOUT;             // Tiempo para timeout de la maquina de estados RX
    
    Tm_Inicie_periodico (&Tiempo_WEEPROM, COUNT_SAVE_DATA); // Para 15 min 
    
    data_FSM stRX_FSM;              // Estructura para maquina de estados de recepcion de datos por serial
    initialize_FSM(&stRX_FSM);
    
    // Variables
     // Para separar en 2 bytes el dato del inclinometro  y enviar \n
    uint8_t incl_LSB = 0;       
    uint8_t incl_MSB = 0;
    uint8_t caracter1 = 0;
    uint8_t caracter2 = 255;
    uint8_t dato_enviar = 0; // Variable para enviar último dato de la Cola (FIFO)
    
    // Para separar los datos del inclinometro a guardar en memoria en 2 bytes
    uint8_t dato_mem_LSB = 0; 
    uint8_t dato_mem_MSB = 0;
    
    uint8_t dato_RX = 0; // Variable para leer dato recibido RX
    
    int ad_i2c_mem = 0b1010000;                   // Direccion i2c de la memoria
    uint16_t pos_mem = 0x0000;                    // Contador para la posición de memoria
    uint8_t data_i2c_mem[3] = {0x00, 0x00, 0x00};       // Dirección donde se guarda el dato (2 primeros bytes), dato
            
    int ad_i2c_rtc = 0b0110010;                   // Direccion i2c del rtc
    uint8_t sec[2] = {0b0000000, 0b00000000};    // Datos para inicializar el i2c (default)
    uint8_t min[2] = {0b00000001, 0b01010010};    // Primer byte del arreglo: dirección del registro del rtc
    uint8_t hour[2] = {0b00000010, 0b00010101};
    uint8_t day[2] = {0b00000100,0b00100100};
    uint8_t month[2] = {0b00000011,0b00010000};
    uint8_t year[2] = {0b00000110, 0b00011001};
    
    i2c_writeNBytes(ad_i2c_rtc, &sec, 2); // Inicializar rtc con valores por default
    i2c_writeNBytes(ad_i2c_rtc, &min, 2);  
    i2c_writeNBytes(ad_i2c_rtc, &hour, 2); 
    i2c_writeNBytes(ad_i2c_rtc, &day, 2);  
    i2c_writeNBytes(ad_i2c_rtc, &month, 2); 
    i2c_writeNBytes(ad_i2c_rtc, &year, 2); 
    
    uint8_t rtc2mem[5] = {0,0,0,0,0};       // Para guardar Timestamp en la memoria
    uint8_t cont_rtc2mem = 0;
    
    uint8_t pos_mem_read = 0;
    uint8_t data_r[2] = {0,0}; // Datos que se leen de la memoria
    
    long int j=200000;
    

    
    while (1){

        // Envia datos si la cola de memoria no esta vacía
        if(!Cola_Vacia(&cola_memtx)){         
                if(UART1_IsTxReady()){
                    dato_enviar = Cola_pop(&cola_memtx);
                    UART1_Write(dato_enviar);
                } 
            }
        //----------------------------------------------------------------------
        
        // Inicializar rtc con valores que dio el usuario por comando
        if(stRX_FSM.flags_internal & CONF_RTC_FLG){
            banderas &= ~EEPROM_WRITE;
            sec[1] = stRX_FSM.byte_sec;
            min[1] = stRX_FSM.byte_min;
            hour[1] = stRX_FSM.byte_hour;
            day[1] = stRX_FSM.byte_day;
            month[1] = stRX_FSM.byte_month;
            year[1] = stRX_FSM.byte_year;
            
            i2c_writeNBytes(ad_i2c_rtc, &sec, 2); 
            i2c_writeNBytes(ad_i2c_rtc, &min, 2);  
            i2c_writeNBytes(ad_i2c_rtc, &hour, 2); 
            i2c_writeNBytes(ad_i2c_rtc, &day, 2);  
            i2c_writeNBytes(ad_i2c_rtc, &month, 2); 
            i2c_writeNBytes(ad_i2c_rtc, &year, 2);
            
            
            stRX_FSM.flags_internal &= ~CONF_RTC_FLG;
            cont_rtc2mem = 0;                              // Variables para escribir Timestamp en memoria
            pos_mem = 0;
            rtc2mem[0] = year[1];//Obtener(year[1]);                 // Convertir datos del formato del RTC en integers 
            rtc2mem[1] = month[1];//Obtener(month[1]);
            rtc2mem[2] = day[1];//Obtener(day[1]);
            rtc2mem[3] = hour[1];//Obtener(hour[1]);
            rtc2mem[4] = min[1];//Obtener(min[1]);
            banderas |= EEPROM_RTC;                   // A partir de este momento se escriben los datos en la memoria cada 15 min 
            
        }
        //-----------------------------------------------------------------------

        // Enviar datos almacenados en la memoria si el usuario lo indica por comando
        if(stRX_FSM.flags_internal & SEND_INCLI_FLG){
            if(pos_mem_read < pos_mem){
                while(j){
                    j--;
                }
                data_i2c_mem[0] = pos_mem_read>>8;                   // Posicion de memoria MSB
                data_i2c_mem[1] = pos_mem_read;                      // Posicion de memoria LSB
                i2c_writeNBytes(ad_i2c_mem, &data_i2c_mem, 2);       // Selective reading
                i2c_readNBytes(ad_i2c_mem, &data_r, 2);
                Cola_push(&cola_memtx, data_r[0]);                   // Los datos que lee los va poniendo en la cola
                Cola_push(&cola_memtx, data_r[1]);
                pos_mem_read = pos_mem_read + 2;
            }
            else{
                Cola_push(&cola_memtx, caracter1);
                Cola_push(&cola_memtx, caracter2);
                stRX_FSM.flags_internal &= ~SEND_INCLI_FLG;
                banderas2 |= STANDBYM;
                pos_mem_read = 0;
            }
            
            if(!Cola_Vacia(&cola_memtx)){         // Enviar datos de la cola de memoria
                if(UART1_IsTxReady()){
                    dato_enviar = Cola_pop(&cola_memtx);
                    UART1_Write(dato_enviar);
                }
            }
            
        }
        
        
        // Enviar dato del panel si el usuario lo pide por comando
        else if(stRX_FSM.flags_internal & SEND_VPAN_FLG){
            
            LED3_Toggle();
            while (!UART1_IsTxReady()){}
            UART1_Write(dato_vpan);
            stRX_FSM.flags_internal &= ~SEND_VPAN_FLG;
            banderas2 |= STANDBYM;
             
        }
        
        // Enviar dato de la bateria si el usuario lo pide por comando
        else if(stRX_FSM.flags_internal & SEND_VBAT_FLG){
            while (!UART1_IsTxReady()){}
            UART1_Write(dato_vbat); 
            stRX_FSM.flags_internal &= ~SEND_VBAT_FLG;   
            banderas2 |= STANDBYM;
        }
        
        else if(stRX_FSM.lp == 1){
            stRX_FSM.lp = 0;
            Low_compsumption();
        }
        
        else if(banderas2 & STANDBYM){
            LED1_SetHigh();
            if(UART1_IsRxReady()){                  // Hay dato en Rx, entonces volver a mandar datos del inclinometro
                banderas2 &= ~STANDBYM;
            }
        }
        
        // Enviar datos del inclinometro (c/32 datos muestreados a 500Hz) y atender con la FSM los comandos que llegan 
        else{
            if(banderas & INCL_OK){                 // Si ya se acumularon los datos del ADC
                banderas &= ~INCL_OK;               // separar en 2 bytes y guardar datos en la cola
                incl_LSB = (uint8_t)adc_incl;                // incluyendo el \n para mandar
                incl_MSB = (uint8_t)(adc_incl>>8);             // si es necesario: pasar a ascii antes de guardar en la cola
                Cola_push(&cola_inclitx, incl_LSB);
                Cola_push(&cola_inclitx, incl_MSB);
                Cola_push(&cola_inclitx, caracter1);
                Cola_push(&cola_inclitx, caracter2);
            }
        
            if(!Cola_Vacia(&cola_inclitx)){         // Enviar datos de la cola del inclinometro
                if(UART1_IsTxReady()){
                    dato_enviar = Cola_pop(&cola_inclitx);
                    UART1_Write(dato_enviar);
                } 
            }
        
            if(UART1_IsRxReady()){                  // Hay dato en Rx, leerlo y procesar estado
                dato_RX = UART1_Read();
                procesar_FSM(&stRX_FSM, dato_RX);
            }
        
            if(stRX_FSM.flags_internal & INIT_TMOUT_FLG){       // Inicie timeout de RX
                stRX_FSM.flags_internal &= ~INIT_TMOUT_FLG;
                Tm_Baje_timeout (&Tiempo_RXTMOUT);
                Tm_Inicie_timeout(&Tiempo_RXTMOUT,15000);         // 500 para 1s
            }
        
            if(banderas & TIMER1_INT){                          // Procesar timeout de Rx si la fsm ya lo indico
                banderas &= ~TIMER1_INT;
                Tm_Procese_tiempo (&Tiempo_RXTMOUT);
            }
        
            if(Tm_Hubo_timeout (&Tiempo_RXTMOUT)){          // Si hubo timeout llama funcion de fsm para modificar la estructura
                Tm_Baje_timeout (&Tiempo_RXTMOUT);
                stRX_FSM.flags_internal |= TMOUT_FLG;
                procesar_FSM(&stRX_FSM, dato_RX);
            }
        
            if(stRX_FSM.flags_internal & CHSUM_E){          // Si hubo error en el Checksum de Rx se envia E por serial
                if(UART1_IsTxReady()){                      // Cuando lo indica la fsm con la bandera
                    stRX_FSM.flags_internal &= ~CHSUM_E;
                    UART1_Write(0b01000101);
                    banderas2 |= STANDBYM;
                } 
            }    
        }
        
        //-----------------------------------------------------------------------
        
        // Escribir datos del RTC en las primeras posiciones de memoria
        if(banderas & EEPROM_RTC){                         
            data_i2c_mem[0] = pos_mem>>8;                   // Posicion de memoria MSB
            data_i2c_mem[1] = pos_mem;                      // Posicion de memoria LSB
            data_i2c_mem[2] = rtc2mem[cont_rtc2mem];        // Dato que se escribirá en memoria (year, month, day, hour, min)
            i2c_writeNBytes(ad_i2c_mem, &data_i2c_mem, 3);  // Escribir primer byte en memoria
            pos_mem++;
            cont_rtc2mem++;
            Tm_Inicie_timeout(&Tiempo_REEPROM_RTC, 4);
            banderas &= ~EEPROM_RTC;
            if(cont_rtc2mem == 5){
                Tm_Baje_timeout (&Tiempo_REEPROM_RTC);
                banderas &= ~EEPROM_RTC;
                banderas |= EEPROM_WRITE;
            }   
        }
            
        if(banderas2 & TIMER1_TMO){
                banderas2 &= ~TIMER1_TMO;
                Tm_Procese_tiempo (&Tiempo_REEPROM_RTC); 
            }
        
        if(Tm_Hubo_timeout (&Tiempo_REEPROM_RTC)){
            Tm_Baje_timeout (&Tiempo_REEPROM_RTC);
            banderas |= EEPROM_RTC;
        }
        //----------------------------------------------------------------------
        
        
        // Despues de escribir en las primeras posiciones de memoria el Timestamp, se permite escribir en las 
        //siguientes posiciones de memoria cada 15 min un dato del inclinometro (dividido en 2 bytes)
        if(banderas & EEPROM_WRITE){
            if(banderas & TIMER2_INT){                       // Timer 2 cada 1s para contar 15 min
                banderas &= ~TIMER2_INT;
                Tm_Procese_tiempo (&Tiempo_WEEPROM);
            }
        
            if(Tm_Hubo_periodico (&Tiempo_WEEPROM)){         // Si se cumple periodo de 15 min guardar dato en memoria
                LED3_Toggle();
                Tm_Baje_periodico (&Tiempo_WEEPROM);
                dato_mem_LSB = incl_LSB;
                dato_mem_MSB = incl_MSB;
                data_i2c_mem[2] = dato_mem_LSB;
                data_i2c_mem[0] = pos_mem>>8;                   // Posicion de memoria MSB
                data_i2c_mem[1] = pos_mem;                          // Posicion de memoria LSB
                i2c_writeNBytes(ad_i2c_mem, &data_i2c_mem, 3);
                pos_mem++;
                Tm_Inicie_timeout(&Tiempo_REEPROM,3);           // Tiempo que debe pasar antes de escribir el otro byte
            }
        
            if(banderas & TIMER1_TMO){
                banderas &= ~TIMER1_TMO;
                Tm_Procese_tiempo (&Tiempo_REEPROM); 
            }
        

            if(Tm_Hubo_timeout (&Tiempo_REEPROM)){
                Tm_Baje_timeout (&Tiempo_REEPROM);
                data_i2c_mem[2] = dato_mem_MSB;
                data_i2c_mem[0] = pos_mem>>8;                   // Posicion de memoria MSB
                data_i2c_mem[1] = pos_mem;                      // Posicion de memoria LSB
                i2c_writeNBytes(ad_i2c_mem, &data_i2c_mem, 3);
                pos_mem++;
            }
        }
        
        //----------------------------------------------------------------------

        // Encender LEDs con botones
        if(BOTON1_GetValue()){                  // Si se oprime el boton 1, prender LED1
            LED1_Toggle();
        }  
        
        
        // Envia datos si la cola de memoria no esta vacía
        if(!Cola_Vacia(&cola_memtx)){         // Enviar datos de la cola de memoria
                if(UART1_IsTxReady()){
                    dato_enviar = Cola_pop(&cola_memtx);
                    UART1_Write(dato_enviar);
                } 
            }
        
        
        //----------------------------------------------------------------------
        
//        switch (stRX_FSM.current_state){
//        
////////////////////////////////////////////////////////////////////////////////// CASE S0
//        case STATE_INITIAL :  
//            
//            LED1_SetLow();
//            LED2_SetLow();
//            LED3_SetLow();
//            LED4_SetHigh();
//        
//        break;
//        
////////////////////////////////////////////////////////////////////////////////// CASE S1       
//        case STATE_START_BYTE : 
//            
//            LED1_SetLow();
//            LED2_SetLow();
//            LED3_SetHigh();
//            LED4_SetLow();
//                  
//        break;
//       
////////////////////////////////////////////////////////////////////////////////// CASE S2         
//        case STATE_SAVE_YEAR_BYTE : 
//            
//            LED1_SetLow();
//            LED2_SetLow();
//            LED3_SetHigh();
//            LED4_SetHigh();
//
//        break;
//        
////////////////////////////////////////////////////////////////////////////////// CASE S3         
//        case STATE_SAVE_BYTE_MONTH : 
//            
//            LED1_SetLow();
//            LED2_SetHigh();
//            LED3_SetLow();
//            LED4_SetLow();
//            
//        break;
//        
////////////////////////////////////////////////////////////////////////////////// CASE S4         
//        case STATE_SAVE_BYTE_DAY : 
//            
//            LED1_SetLow();
//            LED2_SetHigh();
//            LED3_SetLow();
//            LED4_SetHigh();
//     
//        break;
//        
////////////////////////////////////////////////////////////////////////////////// CASE S5         
//        case STATE_SAVE_BYTE_HOUR : 
//
//            LED1_SetLow();
//            LED2_SetHigh();
//            LED3_SetHigh();
//            LED4_SetLow();
//            
//        break;
//        
////////////////////////////////////////////////////////////////////////////////// CASE S6        
//        case STATE_SAVE_BYTE_MIN : 
//            
//            LED1_SetLow();
//            LED2_SetHigh();
//            LED3_SetHigh();
//            LED4_SetHigh();
//
//        break;
//        
////////////////////////////////////////////////////////////////////////////////// CASE S7         
//        case STATE_SAVE_BYTE_SEC : 
//            
//            LED1_SetHigh();
//            LED2_SetLow();
//            LED3_SetLow();
//            LED4_SetLow();
//
//        break;
//        
////////////////////////////////////////////////////////////////////////////////// CASE S8         
//        case STATE_SAVE_BYTE_CKS : 
//            
//            LED1_SetHigh();
//            LED2_SetLow();
//            LED3_SetLow();
//            LED4_SetHigh();
//            
//        break;
//                //////////////////////////////////////////////////////////////////////////////// DEFAULT            
//
//       
//    }
//               
            
    }
    

    return 1;
}

