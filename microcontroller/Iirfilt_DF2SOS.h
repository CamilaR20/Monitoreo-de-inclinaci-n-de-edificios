/* 
 * File:   Iirfilt_DF2SOS.h
 * Author: macar
 *
 * Created on November 11, 2019, 2:41 PM
 */

#ifndef IIRFILT_DF2SOS_H
#define	IIRFILT_DF2SOS_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include "xc.h"
#include "p24FV32KA304.h" 

// Definicion de variable tipo estructura para las constantes del filtro
typedef struct Iir_2order {
   float a1, k1, a2, a3, b1, b2, b3;
   float wn1, wn2;
} Iir_2order;

// Funcion para inicializar los valores de un filtro de segundo orden (para orden 4 se usan dos de orden 2 en cascada)
void init_iir(Iir_2order*ap_iir, float k1, float a1, float a2, float a3, float b1, float b2, float b3);

int calc_iir(int x_int, Iir_2order *ap_iir);


#ifdef	__cplusplus
}
#endif

#endif	/* IIRFILT_DF2SOS_H */

