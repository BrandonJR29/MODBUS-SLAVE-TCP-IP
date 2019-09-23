#include "pin_manager.h"
#include "clock.h"
#include "system.h"
#include "tmr2.h"
#include "interrupt_manager.h"
#include "traps.h"
#include "uart2.h"

void SYSTEM_Initialize(void)
{
    PIN_MANAGER_Initialize();
    INTERRUPT_Initialize();
    CLOCK_Initialize();
    UART2_Initialize();
    TMR2_Initialize();
}
