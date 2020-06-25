/* 
 * File:   pila.h
 * Author: macar
 *
 * Created on October 30, 2019, 8:35 PM
 */

#ifndef PILA_H
#define	PILA_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include "xc.h"
#include "p24FV32KA304.h"  

#define TAM_PILA  8

// Estructura para la pila 
typedef struct Pila Pila;
struct Pila { 
    int stack_pointer;
    uint8_t array_pila[TAM_PILA]; 
}; 

void Init_Pila(Pila * st_pila);

// Retorna 1 si el stack esta lleno 
int Pila_Llena(Pila * st_pila);

// Retorna 1 si el stack esta vacio
int Pila_Vacia(Pila * st_pila);
  
// Guardar dato en pila (PUSH)
void Pila_push(Pila * st_pila, uint8_t dato) ;
  
// Sacar dato de la pila (POP)
uint8_t Pila_pop(Pila * st_pila);


#ifdef	__cplusplus
}
#endif

#endif	/* PILA_H */

