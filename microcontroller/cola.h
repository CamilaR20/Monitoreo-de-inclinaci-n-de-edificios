/* 
 * File:   cola.h
 * Author: macar
 *
 * Created on November 3, 2019, 3:16 PM
 */

#ifndef COLA_H
#define	COLA_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "xc.h"
#include "p24FV32KA304.h"  
    
#define TAM_PILA  12
    
// Estructura para la cola
typedef struct Cola Cola;
struct Cola { 
    int push_pointer;
    int pop_pointer;
    uint8_t array_cola[TAM_PILA]; 
}; 
  
    
void Init_Cola(Cola * st_cola);
int Cola_Vacia(Cola * st_cola);
void Cola_push(Cola * st_cola, uint8_t dato);
uint8_t Cola_pop(Cola * st_cola);



#ifdef	__cplusplus
}
#endif

#endif	/* COLA_H */

