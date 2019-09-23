#include <stdint.h>
#include "xc.h"
#include "clock.h"

void CLOCK_Initialize(void)
{
    //Temporarily switch to FRC (without PLL), so we can safely change the PLL settings,
    //in case we had previously been already running from the PLL.
    __builtin_write_OSCCONH((uint8_t) (OSCCONH & 0xF8));
    if(OSCCONbits.COSC != OSCCONbits.NOSC)
    {
        __builtin_write_OSCCONL((uint8_t) (OSCCONL & 0xFF));            //Initiate clock switching operation.
        while(OSCCONbits.OSWEN != 0);    //Wait for switching complete.
        while (OSCCONbits.LOCK != 1);    //Wait till PLL is locked
    }
    // RCDIV FRC/1; DOZE 1:8; DOZEN disabled; ROI disabled; 
    CLKDIV = 0x3000;
    // TUN Center frequency; 
    OSCTUN = 0x00;
    // ADC1MD enabled; T3MD enabled; T4MD enabled; T1MD enabled; U2MD enabled; T2MD enabled; U1MD enabled; SPI2MD enabled; SPI1MD enabled; T5MD enabled; I2C1MD enabled; 
    PMD1 = 0x00;
    // OC5MD enabled; IC5MD enabled; IC4MD enabled; IC3MD enabled; OC1MD enabled; IC2MD enabled; OC2MD enabled; IC1MD enabled; OC3MD enabled; OC4MD enabled; 
    PMD2 = 0x00;
    // PMPMD enabled; RTCCMD enabled; CMPMD enabled; CRCPMD enabled; I2C2MD enabled; 
    PMD3 = 0x00;
    // NOSC FRCPLL; SOSCEN disabled; OSWEN Switch is Complete; 
    __builtin_write_OSCCONH((uint8_t) (0x01));
    __builtin_write_OSCCONL((uint8_t) (0x01));
    // Wait for Clock switch to occur
//    while (OSCCONbits.OSWEN != 0);
//    while (OSCCONbits.LOCK != 1);
}
