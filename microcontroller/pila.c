
#include "pila.h"

void Init_Pila(Pila * st_pila){ 
    st_pila->stack_pointer = -1;
} 

// Retorna 1 si el stack esta lleno 
int Pila_Llena(Pila * st_pila){ 
    return (st_pila->stack_pointer == TAM_PILA-1); 
} 
  
// Retorna 1 si el stack esta vacio
int Pila_Vacia(Pila * st_pila){ 
    return (st_pila->stack_pointer == -1); 
} 
  
// Guardar dato en pila (PUSH)
void Pila_push(Pila * st_pila, uint8_t dato) 
{ 
    if (Pila_Llena(st_pila)) 
        return;
    st_pila->array_pila[++st_pila->stack_pointer] = dato; 
} 
  
// Sacar dato de la pila (POP)
uint8_t Pila_pop(Pila * st_pila) 
{ 
    if (Pila_Vacia(st_pila)) 
        return 0; 
    return (st_pila->array_pila[st_pila->stack_pointer--]); 
} 
