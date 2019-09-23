#ifndef _INTERRUPT_MANAGER_H
#define _INTERRUPT_MANAGER_H


void INTERRUPT_Initialize(void);

inline static void INTERRUPT_GlobalEnable(void)
{
    __builtin_enable_interrupts();
}

inline static void INTERRUPT_GlobalDisable(void)
{
    __builtin_disable_interrupts();
}


#endif