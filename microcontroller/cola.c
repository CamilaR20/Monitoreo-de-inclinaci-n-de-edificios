
#include "cola.h"


void Init_Cola(Cola * st_cola){ 
    st_cola->push_pointer = -1;
    st_cola->pop_pointer = -1;
} 

// Retorna 1 si la cola esta llena
int Cola_Llena(Cola * st_cola){ 
    return (st_cola->push_pointer == TAM_PILA-1); 
} 
  
// Retorna 1 si el stack esta vacio
int Cola_Vacia(Cola * st_cola){ 
    return (st_cola->pop_pointer == -1); 
} 
  
// Guardar dato en cola (PUSH)
void Cola_push(Cola * st_cola, uint8_t dato) 
{ 
    if (Cola_Llena(st_cola)){ 
        return;
    }
    if(st_cola->pop_pointer == -1){
        st_cola->pop_pointer = 0;
    }
    st_cola->array_cola[++st_cola->push_pointer] = dato;  
} 
  
// Sacar dato de la cola (POP, FIFO)
uint8_t Cola_pop(Cola * st_cola) 
{ 
    int aux_pointer = st_cola->pop_pointer;
    if (Cola_Vacia(st_cola)){ 
        return 0; 
    }
    st_cola->pop_pointer++;
    if(st_cola->pop_pointer > st_cola->push_pointer){
        st_cola->pop_pointer = -1;
        st_cola->push_pointer = -1;
    }
    return (st_cola->array_cola[aux_pointer]); 
} 