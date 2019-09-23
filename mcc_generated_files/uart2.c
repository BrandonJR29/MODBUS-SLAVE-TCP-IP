#include "uart2.h"
#include "CRC.h"
#include "pin_manager.h"
#include <math.h>
#include "adc1.h"


#define SlaveID   1

INT_VAL dirIn;
INT_VAL NoIn;
INT_VAL Crc;
uint8_t WrIn;
int j,i;
bool f1,f2;//banderas 
ADC1_CHANNEL canal = ADC1_MAX_CHANNEL_COUNT; //canal de ADC
ModbusEstados estadoRx = SlaveAddress;
ModbusFuncion modoFuncion;
uint8_t buffRx[100], buffTx[100],n;
uint8_t *pint;
uint8_t contTx,contRx;
INT_VAL InputRegister[3];//Medidores ADC
INT_VAL HoldingRegister[4];//Estados de RTU y Volumen
INT_VAL DiscreteInputs[1];//Sensor de Rebose
INT_VAL Coils[1];//Valvulas
int coils[6];//vector de estados de bobinas especificas


//funcion de obtencion de estados de bobinas
void getVecCoils(){
    if(LATBbits.LATB6==1){
        coils[0]=1;
    }
    else{
        coils[0]=0;
    }
    if(LATBbits.LATB7==1){
        coils[1]=1;
    }
    else{
        coils[1]=0;
    }
    if(LATBbits.LATB8==1){
        coils[2]=1;
    }
    else{
        coils[2]=0;
    }
    if(LATBbits.LATB9==1){
        coils[3]=1;
    }
    else{
        coils[3]=0;
    }
    if(LATBbits.LATB10==1){
        coils[4]=1;
    }
    else{
        coils[4]=0;
    }
    if(LATBbits.LATB11==1){
        coils[5]=1;
    }
    else{
        coils[5]=0;
    }
}

//cambio de bobinas segun valor y direccion
void changeCoil(uint16_t direccion,uint16_t value){
    if(value==0xFF00){
        value=1;
    }
    else{
        value=0;
    }
    switch (direccion){
        case 0:
            LATBbits.LATB6=value;
        break;
        
        case 1:
            LATBbits.LATB7=value;
        break;
        
        case 2:
            LATBbits.LATB8=value;
        break;
        
        case 3:
            LATBbits.LATB9=value;
        break;
        case 4:
            LATBbits.LATB10=value;
        break;
        case 5:
            LATBbits.LATB11=value;
        break;
    }

}

//obtener decimal de bobinas "comodidad"

uint16_t getDecByCoilVec(uint16_t cuantos, uint16_t direccion){
    getVecCoils();
    uint16_t result;
    result=0;
    j=0;
    for (i=direccion;i<(direccion+cuantos);i++){
        result= result + coils[i]*pow(2,j);
        j++;
    }
    return result;
}


void UART2_Initialize (void)
{
   // STSEL 1; IREN disabled; PDSEL 8N; UARTEN enabled; RTSMD disabled; USIDL disabled; WAKE disabled; ABAUD disabled; LPBACK disabled; BRGH enabled; RXINV disabled; UEN TX_RX; 
   U2MODE = (0x8008 & ~(1<<15));  // disabling UARTEN bit   
   // UTXISEL0 TX_ONE_CHAR; UTXINV disabled; OERR NO_ERROR_cleared; URXISEL RX_ONE_CHAR; UTXBRK COMPLETED; UTXEN disabled; ADDEN disabled; 
   U2STA = 0x0000;
   // BaudRate = 9600; Frequency = 16000000 Hz; BRG 416; 
   U2BRG = 0x01A0;
   
   IFS1bits.U2RXIF = false;	// Clear the Recieve Interrupt Flag
   IEC1bits.U2RXIE = 1;     // 1 Enable Recieve Interrupts
 
    //Make sure to set LAT bit corresponding to TxPin as high before UART initialization
   U2MODEbits.UARTEN = 1;  // enabling UART ON bit
   U2STAbits.UTXEN = 1;
   
   IFS1bits.U2TXIF = false;	// Clear the Transmit Interrupt Flag 
   IEC1bits.U2TXIE = 1;	// 1 Enable Transmit Interrupts

}


void __attribute__ ( ( interrupt, no_auto_psv ) ) _U2TXInterrupt ( void )
{
    
    pint++;
    if(--contTx>0) U2TXREG = *pint; 
    
    IFS1bits.U2TXIF = false;
}


void __attribute__ ( ( interrupt, no_auto_psv ) ) _U2RXInterrupt( void )
{
  
    uint8_t auxRx;
    TMR2 = 0x00;
    auxRx = U2RXREG;
    
    switch(estadoRx)
    {
        case SlaveAddress:
            LED1=0;  // LED1  OFF 
            if(auxRx == SlaveID){
                n=0;
                buffRx[n++]  = auxRx;
                estadoRx = Function;
            }
            else estadoRx =  EsperaSicronismo;

        break;
        
        case Function:
            buffRx[n++]  = auxRx;
            modoFuncion = auxRx;
            switch(auxRx)
            {  
                case 1:
                    estadoRx = StartingAddress4HI;
                
                break;
                
                case 2:
                    estadoRx = StartingAddress4HI;
                    
                
                break;
                         
                case 3:
                    
                    estadoRx = StartingAddress4HI;
                    
                break;
                
                case 4:
                    estadoRx =  StartingAddress4HI;
                break;
                
                case 5:
                    
                    estadoRx =  StartingAddress4HI;
                    
                break;
                
                case 6:
                    
                    estadoRx =  StartingAddress4HI;
                    
                break;
                         
                case 15:
                    //aun no se habilitan estas funciones
                    //mensaje de error de funcion solicitada
                    
                    f2=1; //bandera casos multiples bobinas
                    estadoRx = StartingAddress4HI;
                break;
                
                case 16:
                    //aun no se habilitan estas funciones
                    //mensaje de error de funcion solicitada
                    f2=1;
                    estadoRx =  StartingAddress4HI;
                
                break; 
                
                default:
                    estadoRx =  EsperaSicronismo;
                break;    
            
            }
        break;        
        case StartingAddress4HI:
            
            buffRx[n++]  = auxRx;
            dirIn.byte.HB = auxRx;
            estadoRx =  StartingAddress4LO;
            
        break;
        
        case StartingAddress4LO:
            
            buffRx[n++]  = auxRx;
            dirIn.byte.LB = auxRx;
            estadoRx =  NoRegisters4Hi;       
           
        break;        
        case NoRegisters4Hi:
            
            buffRx[n++]  = auxRx;
            NoIn.byte.HB = auxRx;
            estadoRx =  NoRegisters4Lo;
            
        break;
        
        case NoRegisters4Lo:
            
            buffRx[n++]  = auxRx;
            NoIn.byte.LB = auxRx;
            estadoRx=(f2==0) ?  Crc4Hi: ByteCount;
            //compruebo si es casos multiples
            
        break;
        
        
        case ByteCount://en este caso va a ser siempre un solo byte
            buffRx[n++] = auxRx;
            f2=0;
            estadoRx = WrByte;
        break;
        
            
        case WrByte://byte de escritura
            //en este caso solo se necesita un byte
            buffRx[n++]=auxRx;
            WrIn = auxRx;//almaceno byte
            estadoRx = Crc4Hi;
            break;
               
        
        case Crc4Hi:
            
            buffRx[n++]  = auxRx;
            Crc.byte.HB = auxRx;
            estadoRx =  Crc4Lo;
            
        break;
        
        case Crc4Lo:
            
            buffRx[n++]  = auxRx;
            Crc.byte.LB = auxRx;
            estadoRx =  EsperaSicronismo;
            //CRC
            if(CRC16 (buffRx, n)==0){
                
                // datos buenos crear respuesta  
                
                LED0=!LED0; // LED0 cambia cada vez que pasa
                
                switch(modoFuncion){
                    case ReadCoils:
                        if(dirIn.Val+NoIn.Val>6){
                            f1=true;
                            break;//fuera de rango
                        }
                        Coils[0].Val=getDecByCoilVec(NoIn.Val,dirIn.Val);
                        buffTx[0]=buffRx[0];//Numero de Slave
                        buffTx[1]=buffRx[1];//Funcion
                        buffTx[2] = 1;
                        buffTx[3]= Coils[0].byte.LB;
                        buffTx[4]= Coils[0].byte.HB;
                        Crc.Val=CRC16(buffTx,3 + buffTx[2]);//Calcula el CRC
                        buffTx[3 + buffTx[2]]=Crc.byte.LB;
                        buffTx[4 + buffTx[2]]=Crc.byte.HB;
                        contTx= 5 + buffTx[2];
                        pint=buffTx;                
                        U2TXREG = *pint;
                        
                    break;
                    
                    case ReadDiscreteInput:
                        if(dirIn.Val+NoIn.Val>0x2712 || dirIn.Val<0x2711){
                            f1=true;
                            break;//fuera de rango
                        }                        
                        buffTx[0]=buffRx[0];//Numero de esclavo
                        buffTx[1]=buffRx[1];//Funcion
                        buffTx[2] = 1;
                        buffTx[3] = PORTBbits.RB5;
                        Crc.Val=CRC16(buffTx,4);
                        buffTx[3 + buffTx[2]]=Crc.byte.LB;
                        buffTx[4 + buffTx[2]]=Crc.byte.HB;
                        contTx= 5 + buffTx[2];
                        pint=buffTx;                
                        U2TXREG = *pint;
                        
                    break;
                    
                    case ReadHoldingRegister:
                        if(dirIn.Val+NoIn.Val>0x9C45 || dirIn.Val<0x9C41){
                            f1=true;
                            break;//fuera de rango
                        }
                        //falta rutina para holding register
                        buffTx[0]=buffRx[0];
                        buffTx[1]=buffRx[1];
                        buffTx[2]=NoIn.Val*2;
                        j=0;
                        for (i = (dirIn.Val-0x9C41); i < (dirIn.Val-0x9C41) + NoIn.Val; i++){
                        buffTx[3+j*2] = HoldingRegister[i].byte.HB;
                        buffTx[4+j*2] = HoldingRegister[i].byte.LB;
                        j++;
                        }
                        Crc.Val=CRC16(buffTx,3+NoIn.Val*2);
                        buffTx[3+NoIn.Val*2]=Crc.byte.LB;
                        buffTx[4+NoIn.Val*2]=Crc.byte.HB;
                
                        // transmite respuesta
                        contTx= 5 + NoIn.Val*2;
                        pint=buffTx;                
                        U2TXREG = *pint;
                    break;
                    
                    case ReadInputRegister:
                        if(dirIn.Val+NoIn.Val>0x7534 || dirIn.Val < 0x7531){
                            f1=true;
                            break;//fuera de rango
                        }
                        buffTx[0]=buffRx[0];
                        buffTx[1]=buffRx[1];
                        buffTx[2]=NoIn.Val*2;
                         j=0;
                        for (i = (dirIn.Val-0x7531); i < (dirIn.Val-0x7531) + NoIn.Val; i++){
                            canal=(dirIn.Val-0x7531)+2;//de acuerdo a los canales del ADC
                            InputRegister[i].Val=AD1Conversion(canal);
                        buffTx[3+j*2] = InputRegister[i].byte.HB;
                        buffTx[4+j*2] = InputRegister[i].byte.LB;
                        j++;
                        }
                        Crc.Val=CRC16(buffTx,3+NoIn.Val*2);
                        buffTx[3+NoIn.Val*2]=Crc.byte.LB;
                        buffTx[4+NoIn.Val*2]=Crc.byte.HB;
                
                        // transmite respuesta
                        contTx= 5 + NoIn.Val*2;
                        pint=buffTx;                
                        U2TXREG = *pint;
                    break;
                    
                    case WriteSingleCoil:
                        if(dirIn.Val>6){
                            f1=true;
                            break;//fuera de rango
                        }
                        
                        buffTx[0]=buffRx[0];
                        buffTx[1]=buffRx[1];
                        buffTx[2]=buffRx[2];
                        buffTx[3]=buffRx[3];
                        changeCoil(dirIn.Val,NoIn.Val);
                        buffTx[4]=NoIn.byte.HB;
                        buffTx[5]=NoIn.byte.LB;
                        Crc.Val=CRC16(buffTx,6);
                        buffTx[6]=Crc.byte.LB;
                        buffTx[7]=Crc.byte.HB;
                
                        // transmite respuesta
                        contTx= 8;
                        pint=buffTx;                
                        U2TXREG = *pint;
                        
                        
                    break;
                    
                    case WriteSingleRegister:
                        if((dirIn.Val-0x9C41)>4){
                            f1=true;
                            break;//fuera de rango
                        }
                        
                        buffTx[0]=buffRx[0];
                        buffTx[1]=buffRx[1];
                        buffTx[2]=buffRx[2];
                        buffTx[3]=buffRx[3];
                        HoldingRegister[dirIn.Val].Val=NoIn.Val;
                        buffTx[4]=HoldingRegister[dirIn.Val].byte.HB;
                        buffTx[5]=HoldingRegister[dirIn.Val].byte.LB;
                        Crc.Val=CRC16(buffTx,6);
                        buffTx[6]=Crc.byte.LB;
                        buffTx[7]=Crc.byte.HB;
                
                        // transmite respuesta
                        contTx= 8;
                        pint=buffTx;                
                        U2TXREG = *pint;
                        
                    break;
                    
                    case WriteMultipleCoils:
                        
                        buffTx[0]=buffRx[0];
                        buffTx[1]=buffRx[1]+ 123;
                        buffTx[2]=1;
                        Crc.Val=CRC16(buffTx,3);
                        buffTx[3]=Crc.byte.LB;
                        buffTx[4]=Crc.byte.HB;
                        contTx= 5;
                        pint=buffTx;                
                        U2TXREG = *pint;
                        
                        
                        /*
                        buffTx[0]=buffRx[0];
                        buffTx[1]=buffRx[1];
                        buffTx[2]=buffRx[2];
                        buffTx[3]=buffRx[3];
                        buffTx[4]=buffRx[4];
                        buffTx[5]=buffRx[5];
                        Crc.Val=CRC16(buffTx,6);
                        buffTx[6]=Crc.byte.LB;
                        buffTx[7]=Crc.byte.HB;
                        contTx= 8;
                        pint=buffTx;                
                        U2TXREG = *pint;
                         */
                    break;
                    
                    case WriteMultipleRegister:
                        //mensaje de error
                        buffTx[0]=buffRx[0];
                        buffTx[1]=buffRx[1]+ 123;
                        buffTx[2]=1;
                        Crc.Val=CRC16(buffTx,3);
                        buffTx[3]=Crc.byte.LB;
                        buffTx[4]=Crc.byte.HB;
                        contTx= 5;
                        pint=buffTx;                
                        U2TXREG = *pint;
                        
                    break;   
                }
                if(f1==true){//registro desconocido
                    buffTx[0]=buffRx[0];
                    buffTx[1]=buffRx[1]+128;
                    buffTx[2]=2;
                    Crc.Val=CRC16(buffTx,3);
                    buffTx[3]=Crc.byte.LB;
                    buffTx[4]=Crc.byte.HB;
                    f1=false;
                    contTx= 5;
                    pint=buffTx;                
                    U2TXREG = *pint;
                }
                

            }           
        break;
        
        case EsperaSicronismo:
            // este estado no hace nada espera Timer 2 lo saque de aqui
            
        break;
        
//        case 100:
//           
//            
//            
//        break;        
//                      
    }
    
    
    IFS1bits.U2RXIF = false;
}

void __attribute__ ( ( interrupt, no_auto_psv ) ) _U2ErrInterrupt ( void )
{
    if ((U2STAbits.OERR == 1))
    {
        U2STAbits.OERR = 0;
    }

    IFS4bits.U2ERIF = false;
}

/**
  End of File
*/
