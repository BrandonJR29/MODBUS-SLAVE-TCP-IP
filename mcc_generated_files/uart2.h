
#include <xc.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef enum
{
    /*  */
    SlaveAddress,
    Function,
    StartingAddress4HI, StartingAddress4LO, NoRegisters4Hi, NoRegisters4Lo, Crc4Hi, Crc4Lo,ByteCount,
            WrByte,

    EsperaSicronismo

}ModbusEstados;

typedef enum
{
    ReadCoils = 1, ReadDiscreteInput = 2, ReadHoldingRegister = 3, ReadInputRegister = 4,
            WriteSingleCoil = 5, WriteSingleRegister = 6, WriteMultipleCoils = 15,
            WriteMultipleRegister = 16
}ModbusFuncion;



typedef union
{
    uint16_t Val;
    struct
    {
        uint8_t LB;
        uint8_t HB;
    } byte;

} INT_VAL;


void UART2_Initialize(void);


