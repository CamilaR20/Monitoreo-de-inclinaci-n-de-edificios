/* 
 * File:   rtc_epson.h
 * Author: Pablo Mosquera
 *
 * Created on October 22, 2019, 3:11 PM
 */

#ifndef RTC_EPSON_H
#define	RTC_EPSON_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "xc.h"
#include "p24FV32KA304.h"    
    
#define BIT0 0b00000001
#define BIT1 0b00000010
#define BIT2 0b00000100
#define BIT3 0b00001000
#define BIT4 0b00010000
#define BIT5 0b00100000
#define BIT6 0b01000000

uint8_t Obtener(uint8_t dato_frtc); 

    
#ifdef	__cplusplus
}
#endif

#endif	/* RTC_EPSON_H */

