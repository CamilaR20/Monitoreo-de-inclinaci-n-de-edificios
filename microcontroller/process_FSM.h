/* 
 * File:   process_FSM.h
 * Author: Pablo Mosquera
 *
 * Created on 30 de octubre de 2019, 11:30 AM
 */

#ifndef PROCESS_FSM_H
#define	PROCESS_FSM_H

#ifdef	__cplusplus
extern "C" {
#endif




#include "xc.h"
#include "p24FV32KA304.h"
#include "mcc_generated_files/system.h"
#include "mcc_generated_files/pin_manager.h"

    

typedef struct data_FSM data_FSM;
#define STATE_INITIAL           0
#define STATE_START_BYTE        1
#define STATE_SAVE_YEAR_BYTE    2
#define STATE_SAVE_BYTE_MONTH   3
#define STATE_SAVE_BYTE_DAY     4
#define STATE_SAVE_BYTE_HOUR    5    
#define STATE_SAVE_BYTE_MIN     6
#define STATE_SAVE_BYTE_SEC     7
#define STATE_SAVE_BYTE_CKS     8
#define STATE_COMPARE_CKS       9

#define SEND_INCLI_FLG  0b00000001
#define SEND_VPAN_FLG   0b00000010
#define SEND_VBAT_FLG   0b00000100
#define INIT_TMOUT_FLG  0b00001000
#define TMOUT_FLG       0b00010000
#define CHSUM_E         0b01000000
#define CONF_RTC_FLG    0b10000000



struct data_FSM{
    
    uint8_t byte_year;
    uint8_t byte_month;
    uint8_t byte_day;
    uint8_t byte_hour;
    uint8_t byte_min;
    uint8_t byte_sec;
    uint8_t byte_checksum;
    uint8_t byte_compare_checksum;
    uint8_t flags_internal;
    uint8_t current_state;
    uint8_t lp; 
    
};

void initialize_FSM(data_FSM *apt);     
void procesar_FSM(data_FSM *apt, uint8_t dato_serial);




#ifdef	__cplusplus
}
#endif

#endif	/* PROCESS_FSM_H */

