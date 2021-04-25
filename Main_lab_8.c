//Archivo:	Main_lab_8.s
//dispositivo:	PIC16F887
//Autor:		Dylan Ixcayau
//Compilador:	XC8, MPLABX V5.45

//Programa:	Modulo ADC
//Hardware:	2 potenciometros, leds y displays

//Creado:	20 abril, 2021
//Ultima modificacion:  , 2021
// ----------------- Laboratorio No.8 ----------------------------------------

#pragma config FOSC=INTRC_NOCLKOUT //Oscilador interno sin salidas
#pragma config WDTE=OFF	           //WDT disabled (reinicio repetitivo del pic)
#pragma config PWRTE=ON	           //PWRT enabled (espera de 72ms al iniciar
#pragma config MCLRE=OFF	       //pin MCLR se utiliza como I/O
#pragma config CP=OFF		       //sin protección de código
#pragma config CPD=OFF		       //sin protección de datos
#pragma config BOREN=OFF	       //sin reinicio cuando el voltaje baja de 4v
#pragma config IESO=OFF	           //Reinicio sin cambio de reloj de interno a externo
#pragma config FCMEN=OFF	       //Cambio de reloj externo a interno en caso de falla
#pragma config LVP=OFF


 
//------------------ Configuración word2 --------------------------------------
#pragma config WRT=OFF	//Protección de autoescritura 
#pragma config BOR4V=BOR40V	//Reinicio abajo de 4V 
  
//----------------------Importancion de librerias-------------------------------
#include <xc.h>
#include <stdint.h>

#define _XTAL_FREQ 4000000

#define tesbit(var,bit)((var)&   (1<<(bit)))
#define tesbit(var,bit)((var)|=  (1<<(bit)))
#define tesbit(var,bit)((var)&=~ (1<<(bit)))

//-------------------Declaración de variables-----------------------------------
uint8_t cont = 0;   //Declaramos las variables de 8 bits y sin signo
uint8_t unidades = 0;
uint8_t decenas =  0;
uint8_t centenas=  0;
uint8_t banderas = 0;
uint8_t v1 = 0;

void setup(void);   //llamamos a las funciones desde el principio
int tabla(int v1);  //llamamamos a la tabla que regresara valores enteros
void Decimal(void); //Llamamos a la funcion de conversión a decimal

//--------------------------Interrupciones--------------------------------------
void __interrupt() interrupciones(void){
   if(T0IF == 1) {          //Revisamos si la bandera del timer0 esta levantada
        INTCONbits.T0IF = 0;         //Bajamos la bandera del timer0
        TMR0 = 236;                  //Le asignamos un valor al TMR0
        PORTE = 0;                   //Dejamos todo el puerto E apagado
        if (banderas == 0){          //Cambio entre displays
            v1 = unidades;          // El valor de unidades pasa a v1
            PORTD = tabla(v1);      //PORTD muestra el contador
            RE0 = 1;                //Encendemos el display correspondiente
            banderas = 1;           //Nos movemos un valor en banderas
            return;                 //regresamos
        }
        if (banderas == 1){
            v1 = decenas; 
            PORTD = tabla(v1);
            RE1 = 1;
            banderas = 2; 
            return; 
        }
        if (banderas == 2){
            v1 = centenas;
            PORTD = tabla(v1); 
            RE2 = 1;
            banderas = 0; 
            return; 
        }
    }
    
    if (PIR1bits.ADIF == 1)      //Revisamos la bandera del ADC
    {
        if (ADCON0bits.CHS == 0) //Si el canal es el 0
            PORTC = ADRESH;     //El valor en el ADRESH se pasa al puerto C
        else
            cont = ADRESH;  //Si el canal es diferente de 0 el valor a cont
        PIR1bits.ADIF = 0;  //Se baja la bandera del ADC
    }
    return;

}
//---------------------funcion principal----------------------------------------
void main(void) {
    setup();                //llamamos a las configuraciones de los puertos    
    __delay_us(50);         //Delay de 50us
    ADCON0bits.GO = 1;      
    while(1)                //Ciclo que se ejecuta siempre
    {
        Decimal();          //Llamamos a la conversion a decimal
        if (ADCON0bits.GO == 0)     //Si GO es 0
        {
            if (ADCON0bits.CHS == 0)    //Si el canal es el 0
                ADCON0bits.CHS = 1;     //Se cambia el canal a 1
            else
                ADCON0bits.CHS = 0;     //Si el canal es diferente de 0
                                        //Se pasa a uno
            __delay_us(50);     
            ADCON0bits.GO = 1;          //Se deja GO en 1
        }    
    }
    return;
}

//-------------------------Tabla de valores decimales--------------------------
int tabla (int v1){     //La tabla
 int w;                 //Declaramos w de tipo int
 switch (v1)   {         //Entramos a un tipo de "Menu" que se navega con v1
 
     case 0 :           //Opción 0
         w = 0b00111111;    //Numero 0
         break; 
     case 1 :           //Opción 1
         w = 0b00000110;    //Numero 1
         break;         
     case 2 :           //Opción 2
         w = 0b01011011;    //Numero 2
         break;             
     case 3 :           //Opción 3
         w = 0b01001111;    //Numero 3
         break; 
     case 4 :           //Opción 4
         w = 0b01100110;    //Numero 4
         break;
     case 5 :           //Opción 5
         w = 0b01101101;    //Numero 5
         break;
     case 6 :           //Opción 6
         w = 0b01111101;    //Numero 6
         break;
     case 7 :           //Opción 7
         w = 0b00000111;    //Numero 7
         break;
     case 8 :           //Opción 8
         w = 0b01111111;    //Numero 8
         break;
     case 9 :           //Opción 9
         w = 0b01101111;    //Numero 9
            break;              //Rompemos el menu
        default:;
 }
 return w;
}

//-----------------------Convertidor a decimal----------------------------------
void Decimal(void){      //Funcion para conversion en decimal
    centenas = cont/100;            //Operacin para centenas
    decenas  = (cont - 100*centenas)/10;     //operacion para decenas
    unidades = (cont - 100*centenas - 10*decenas)/1;  //Operacion para unidades
}
//---------------- Configuraciones Generales ----------------------------------
void setup(void){
//-------  Asigancion de Pines Didigitales
    ANSEL  = 0x03;
    ANSELH = 0x00;
   
    TRISB = 0x00;
    TRISC = 0x00;         //Dejamos los los demas puertos Como salidas digitales
    TRISA = 0x03;
    TRISD = 0x00;
    TRISE = 0x00;
    
    
    PORTB = 0x00;        //Limpiamos todos los puertos
    PORTC = 0x00;
    PORTA = 0x00;
    PORTE = 0x00;
    PORTD = 0x00;
    
//-------------------- Configuracion del ADC -----------------------------------
    ADCON1bits.ADFM  = 0;       //Justificado a la izquierda
    ADCON1bits.VCFG0 = 0;       //Voltage de referencia VDD
    ADCON1bits.VCFG1 = 0;       //Voltage de referencia VSS
    
    ADCON0bits.ADCS  = 1;       //Fosc/8
    ADCON0bits.CHS   = 0;       //Empezamos con el canal 0
    __delay_us(100);
    
    ADCON0bits.ADON  = 1;       //ADC encendido
    
//-------------------- Configuraciones del Reloj -------------------------------
    OSCCONbits.IRCF2 = 1;
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF0 = 1;       //Reloj a 8MHz
    OSCCONbits.SCS   = 1;       //Oscilador interno

//------------------ Configuraciones de TMR0 -----------------------------------
    OPTION_REGbits.T0CS = 0;    //Ciclo de reloj interno
    OPTION_REGbits.PSA  = 0;    //Prescaler para el modulo de timer 0
    OPTION_REGbits.PS2  = 1;
    OPTION_REGbits.PS1  = 1;
    OPTION_REGbits.PS0  = 1;    //Prescaler a 256
  
   
//-------------------- Configuracion del las interrupciones --------------------
    INTCONbits.GIE  = 1 ;       //Habilitamos las interrupciones globales
    INTCONbits.PEIE = 1 ;       //Dejamos la  bandera levantada
    INTCONbits.T0IE = 1 ;       //Encendemos la interrupcion del timer0
    PIR1bits.ADIF = 0;          //Bandera de interrupcion del ADC
    PIE1bits.ADIE  = 1;          //Encendemos la bandera de interrupcion del ADC
    return;
}
