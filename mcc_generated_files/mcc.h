#ifndef MCC_H
#define	MCC_H
#include <xc.h>
#include "system.h"
#include "clock.h"
#include "pin_manager.h"
#include <stdint.h>
#include <stdbool.h>
#include "tmr2.h"
#include "interrupt_manager.h"
#include "traps.h"
#include "uart2.h"

#ifndef _XTAL_FREQ
#define _XTAL_FREQ  32000000UL
#endif

void OSCILLATOR_Initialize(void) __attribute__((deprecated ("\nThis will be removed in future MCC releases. \nUse CLOCK_Initialize (void) instead. ")));


#endif	/* MCC_H */
/**
 End of File
*/