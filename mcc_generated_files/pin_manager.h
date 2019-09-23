#ifndef _PIN_MANAGER_H
#define _PIN_MANAGER_H
/**
    Section: Includes
*/
#include <xc.h>

/**
    Section: Device Pin Macros
*/
#define LED1        LATAbits.LATA1
#define TRISLED1    TRISAbits.TRISA1

#define LED0        LATAbits.LATA0
#define TRISLED0    TRISAbits.TRISA0
void PIN_MANAGER_Initialize (void);

#endif
