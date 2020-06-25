#include "rtc_epson.h"


//uint8_t Calcular(uint8_t dato_fn) //le ingreso la estructura
//{
//    uint8_t dato_frtc;
//    
//    
//      
//	return dato_frtc;
//}

uint8_t Obtener(uint8_t dato_frtc) 
{
    uint8_t dato_fn = 0;
            
    if(dato_frtc & BIT0){
        dato_fn = 1;
    }
    
    if(dato_frtc & BIT1){
        dato_fn = dato_fn + 2;
    }
          
    if(dato_frtc & BIT2){
        dato_fn = dato_fn + 4;
    }
    
    if(dato_frtc & BIT3){
        dato_fn = dato_fn + 8;
    }
    
    if(dato_frtc & BIT4){
        dato_fn = dato_fn + 10;
    }
    
    if(dato_frtc & BIT5){
        dato_fn = dato_fn + 20;
    }
    
    if(dato_frtc & BIT6){
        dato_fn = dato_fn + 40;
        
    }
    
    
	return dato_fn;//activo la bandera cuando hubo periodo
}