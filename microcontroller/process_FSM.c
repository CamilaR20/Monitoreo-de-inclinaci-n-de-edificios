
#include "process_FSM.h"




void procesar_FSM(data_FSM *apt, uint8_t dato_serial){
    int sum = 0;
    switch (apt->current_state){
        
//////////////////////////////////////////////////////////////////////////////// CASE S0
        case STATE_INITIAL :  

                      
            if(dato_serial == 0b00100011) //Dato serial igual a #
            {
                apt->flags_internal |= INIT_TMOUT_FLG;
                apt->current_state = STATE_START_BYTE;
            }
            else
            {
                apt->current_state = STATE_INITIAL;
            }
        
        break;
        
//////////////////////////////////////////////////////////////////////////////// CASE S1       
        case STATE_START_BYTE : 

            if(apt->flags_internal & TMOUT_FLG){                // Primero revisa si hubo timeout, si no revisa el dato recibido
               apt->flags_internal &= ~TMOUT_FLG;
               apt->current_state = STATE_INITIAL;              // En el caso que si, va al estado inicial
            }
            
            else{
                apt->flags_internal |= INIT_TMOUT_FLG;          // Sube la bandera para empezar a contar el timeout
                if(dato_serial == 0b01000000) //Dato serial = @
                {
                    apt->flags_internal |= SEND_INCLI_FLG;
                    apt->current_state = STATE_INITIAL;
                }

                else if(dato_serial == 0b00101011)//Dato serial igual a +
                {
                    apt->lp = 1;
                    apt->current_state = STATE_INITIAL;
                }

                else if(dato_serial == 0b00100101) //Dato serial igual a %
                {
                    apt->flags_internal |= SEND_VPAN_FLG;
                    apt->current_state = STATE_INITIAL;
                }
                
                else if(dato_serial == 0b00100100) //Dato serial igual a $
                {
                    apt->flags_internal |= SEND_VBAT_FLG;
                    apt->current_state = STATE_INITIAL;
                }

                else if(dato_serial == 0b00100110) // Dato serial igual a &
                {
                    apt->current_state = STATE_SAVE_YEAR_BYTE;
                }

                else
                {
                    apt->current_state = STATE_INITIAL;
                }
            }
                        
        break;
       
//////////////////////////////////////////////////////////////////////////////// CASE S2         
        case STATE_SAVE_YEAR_BYTE : 
            

            if(apt->flags_internal & TMOUT_FLG){                // Primero revisa si hubo timeout, si no revisa el dato recibido
               apt->flags_internal &= ~TMOUT_FLG;
               apt->current_state = STATE_INITIAL;              // En el caso que si, va al estado inicial
            }
    
            else{
                apt->flags_internal |= INIT_TMOUT_FLG;
                apt->byte_year = dato_serial;
                apt->current_state = STATE_SAVE_BYTE_MONTH; 
            }

        break;
        
//////////////////////////////////////////////////////////////////////////////// CASE S3         
        case STATE_SAVE_BYTE_MONTH : 
            

            if(apt->flags_internal & TMOUT_FLG){                // Primero revisa si hubo timeout, si no revisa el dato recibido
               apt->flags_internal &= ~TMOUT_FLG;
               apt->current_state = STATE_INITIAL;              // En el caso que si, va al estado inicial
            }
    
            else{
                apt->flags_internal |= INIT_TMOUT_FLG;
                apt->byte_month = dato_serial;
                apt->current_state = STATE_SAVE_BYTE_DAY; 
            }

            
        break;
        
//////////////////////////////////////////////////////////////////////////////// CASE S4         
        case STATE_SAVE_BYTE_DAY : 
            
            if(apt->flags_internal & TMOUT_FLG){                // Primero revisa si hubo timeout, si no revisa el dato recibido
               apt->flags_internal &= ~TMOUT_FLG;
               apt->current_state = STATE_INITIAL;              // En el caso que si, va al estado inicial
            }
    
            else{
                apt->flags_internal |= INIT_TMOUT_FLG;
                apt->byte_day = dato_serial;
                apt->current_state = STATE_SAVE_BYTE_HOUR; 
            }
     
        break;
        
//////////////////////////////////////////////////////////////////////////////// CASE S5         
        case STATE_SAVE_BYTE_HOUR : 
            
            if(apt->flags_internal & TMOUT_FLG){                // Primero revisa si hubo timeout, si no revisa el dato recibido
               apt->flags_internal &= ~TMOUT_FLG;
               apt->current_state = STATE_INITIAL;              // En el caso que si, va al estado inicial
            }
    
            else{
                apt->flags_internal |= INIT_TMOUT_FLG;
                apt->byte_hour = dato_serial;
                apt->current_state = STATE_SAVE_BYTE_MIN; 
            }
            
        break;
        
//////////////////////////////////////////////////////////////////////////////// CASE S6        
        case STATE_SAVE_BYTE_MIN : 
            
            if(apt->flags_internal & TMOUT_FLG){                // Primero revisa si hubo timeout, si no revisa el dato recibido
               apt->flags_internal &= ~TMOUT_FLG;
               apt->current_state = STATE_INITIAL;              // En el caso que si, va al estado inicial
            }
    
            else{
                apt->flags_internal |= INIT_TMOUT_FLG;
                apt->byte_min = dato_serial;
                apt->current_state = STATE_SAVE_BYTE_SEC; 
            }         
            
        break;
        
//////////////////////////////////////////////////////////////////////////////// CASE S7         
        case STATE_SAVE_BYTE_SEC : 

            if(apt->flags_internal & TMOUT_FLG){                // Primero revisa si hubo timeout, si no revisa el dato recibido
               apt->flags_internal &= ~TMOUT_FLG;
               apt->current_state = STATE_INITIAL;              // En el caso que si, va al estado inicial
            }
    
            else{
                apt->flags_internal |= INIT_TMOUT_FLG;
                apt->byte_sec = dato_serial;
                apt->current_state = STATE_SAVE_BYTE_CKS; 
            }              
            
        break;
        
//////////////////////////////////////////////////////////////////////////////// CASE S8         
        case STATE_SAVE_BYTE_CKS : 
            
            if(apt->flags_internal & TMOUT_FLG){                // Primero revisa si hubo timeout, si no revisa el dato recibido
               apt->flags_internal &= ~TMOUT_FLG;
               apt->current_state = STATE_INITIAL;              // En el caso que si, va al estado inicial
            }
    
            else{
            
                apt->byte_checksum = dato_serial;
                sum = apt->byte_year + apt->byte_month + apt->byte_day + apt->byte_hour + apt->byte_min + apt->byte_sec;
            
                if(apt->byte_checksum == sum)                   // Si checksum OK sube bandera para configurar RTC
                {
                    apt->flags_internal |= CONF_RTC_FLG;
                    apt->current_state = STATE_INITIAL;
                }
                else                                            // Si checksum tiene error sube bandera de error para que 
                {                                               // en el main se envie E por serial
                    apt->flags_internal |= CHSUM_E;
                    apt->current_state = STATE_INITIAL;
                }
                
            }
            
        break;
                //////////////////////////////////////////////////////////////////////////////// DEFAULT            
        default :
            
        apt->current_state = STATE_INITIAL;   
        
        
        break;
       
    }
   
}



void initialize_FSM(data_FSM *apt){
    
    apt->current_state = 0;
    apt->byte_year = 0;
    apt->byte_month = 0;
    apt->byte_day = 0;
    apt->byte_hour = 0;
    apt->byte_min = 0;
    apt->byte_sec = 0;
    apt->byte_checksum = 0;
    apt->byte_compare_checksum = 0;
    apt->flags_internal = 0b00000000;
    apt->lp = 0;
}
