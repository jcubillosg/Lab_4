#ifndef LIBLCDXC8_H
#define	LIBLCDXC8_H

#include<xc.h>
#ifndef _XTAL_FREQ
#define _XTAL_FREQ 1000000
#endif
#ifndef Datos
#define Datos LATD
#endif
#ifndef RS
#define RS LATA3
#endif
#ifndef E
#define E LATA4
#endif
#ifndef LED_PIN
#define LED_PIN LATA1
#endif
#ifndef BACKLIGHT_PIN
#define BACKLIGHT_PIN LATA2
#endif
#ifndef COUNTER_BUTTON
#define COUNTER_BUTTON PORTA5
#endif

unsigned char interfaz;
unsigned char keyboard_value; //Stores key from 0x0 to 0xF. Default value: 0xFF
unsigned char count; //Current number of objects counter
unsigned char target_count; //Target number of objects counter
unsigned char key_pressed; //To check for idle states
unsigned char time_counter; //To count for idle time with Timer0 interrupt
unsigned char iter; //Iterable variable for loops
unsigned char tmp; //Temporary variable
unsigned char prev_button; //Pressed button flag
unsigned char state; //State machine

void Setup(void);
void EmergencyStop(void);
void CountStart(void);
void CountEnd(void);
void ConfiguraLCD(unsigned char);
void RetardoLCD(unsigned char);
void EnviaDato(unsigned char);
void InicializaLCD(void);
void HabilitaLCD(void);
void BorraLCD(void);
void CursorAInicio(void);
void ComandoLCD(unsigned char);
void EscribeLCD_c(unsigned char);
void EscribeLCD_n8(unsigned char, unsigned char);
void EscribeLCD_n16(unsigned int, unsigned char);
void EscribeLCD_d(double, unsigned char, unsigned char);
void MensajeLCD_Var(char *, char);
void DireccionaLCD(unsigned char);
void FijaCursorLCD(unsigned char,unsigned char);
void DesplazaPantallaD(void);
void DesplazaPantallaI(void);
void DesplazaCursorD(void);
void DesplazaCursorI(void);
void ReadKey(void);
void TakeKbAction(void); //performs action based on user input
void interrupt ISR(void);

void Setup(void){
    ADCON1=0x0F;
    CMCON=0x07;
    LATD=0x0;
    LATB=0x0F;
    TRISA=0b11000001;
    TRISB=0xF0; //Keyboard module <RB7:RB4> inputs
    TRISD=0x0; //LCD data <RD7:RD4> outputs
    //Flag settings
    state=0;
    key_pressed=0;
    time_counter=0; //Time starts at 0
    count=0; //Piece count initializes in 0
    keyboard_value=0xFF; //Initial Keyboard value
    //Set Timer0, prescaler 1:16 (for ~0.5ms period) and LED value
    LED_PIN=1;
    T1CON=0x9D;
    //T0CON=0b10000000; //Set Timer0 to Timer mode, 16-bit, rising edge, 1:8 prescaler (with 1MHz clk, the T0 interruption will happen approx. each 0x2*0xFF = 131072 bus cycles, approx. 0.524688 s)
    //ISR setup
    RBIF=0; //Clean Port B interrupt flag
    TMR1IF=0; //Clean Timer0 interrupt flag
    RBPU=0; //Port B internal pull ups
    RBIE=1; //Port B (RB7-RB4) interrupt enable
    TMR1IE=1; //Timer0 interrupt enable
    PEIE=1; //Enable peripheral interrupts
    GIE=1; //Global interrupt enable
    //LCD setup. Start after 40ms
    BACKLIGHT_PIN=1;
    ConfiguraLCD(0x4);
    InicializaLCD();
}

void EmergencyStop(void){
    TMR1ON=0; //Turns LED off
    LED_PIN=1;
    while(1){
        __nop();
    }
}

void ConfiguraLCD(unsigned char a){
	if(a==4 | a ==8)
		interfaz=a;	
}
void EnviaDato(unsigned char a){
	if(interfaz==4){
		Datos=(Datos & 0b00001111) | (a & 0b11110000);
		HabilitaLCD();
		RetardoLCD(1);
		Datos=(Datos & 0b00001111) | (a<<4);
		//HabilitaLCD();
		//RetardoLCD(1);
	}else if(interfaz==8){
		Datos=a;
	}	
}
void InicializaLCD(void){
//Función que inicializa el LCD caracteres
	RS=0;
	if(interfaz==4)
		Datos=(Datos & 0b00001111) | 0x30;
	else	
		Datos=0x3F;
	HabilitaLCD();
	RetardoLCD(1); //15ms
	if(interfaz==4)
		Datos=(Datos & 0b00001111) | 0x30;
	else	
		Datos=0x3F;
	HabilitaLCD();
	RetardoLCD(3); //100us
	if(interfaz==4)
		Datos=(Datos & 0b00001111) | 0x30;
	else	
		Datos=0x3F;
	HabilitaLCD();
	RetardoLCD(4); //40us
	if(interfaz==4){
		Datos=(Datos & 0b00001111) | 0x20;
		HabilitaLCD();
		RetardoLCD(4);
		EnviaDato(0x2F);
		HabilitaLCD();
		RetardoLCD(4);
	}
	BorraLCD();
	EnviaDato(0xF);
	HabilitaLCD();
	RetardoLCD(4);
    MensajeLCD_Var("Hola Usuario!",0);
    __delay_ms(5000);
}
void HabilitaLCD(void){
//Función que genera los pulsos de habilitación al LCD 	
	E=1;
	__delay_us(40);
    //Delay1TCY();
	E=0;
}
void BorraLCD(void){
//Función que borra toda la pantalla	
	RS=0;
	EnviaDato(0x01);
	HabilitaLCD();
	RetardoLCD(2);
}
void CursorAInicio(){
//Función que lleva el cursor a la primera posición o la de la
//primera línea mas a la izquierda	
    ComandoLCD(0x02);    
}	
void ComandoLCD(unsigned char a){
//Función que envia cualquier comando al LCD
	RS=0;
	if(a==1)
		BorraLCD();
	else{	
		EnviaDato(a);
		HabilitaLCD();
		RetardoLCD(4);
	}		
}	

void EscribeLCD_c(unsigned char a){
//Función que escribe un caracter en la pantalla
//a es un valor en codigo ascii
//Ejemplo EscribeLCD_c('A');
	RS=1;
	EnviaDato(a);
	HabilitaLCD();
	RetardoLCD(4);
}
void EscribeLCD_n8(unsigned char a,unsigned char b){
//Función que escribe un número positivo de 8 bits en la pantalla
//a es el número a escribir, el cual debe estar en el rango de 0 a 255
//b es el número de digitos que se desea mostrar empezando desde las unidades
//Ejemplo EscribeLCD_n8(204,3);	
    unsigned char centena,decena,unidad;
	RS=1;
	switch(b){
		case 1: unidad=a%10;
				EnviaDato(unidad+48);
				HabilitaLCD();
				RetardoLCD(4);
				break;
		case 2:	decena=(a%100)/10;
				unidad=a%10;
				EnviaDato(decena+48);
				HabilitaLCD();
				RetardoLCD(4);
				EnviaDato(unidad+48);
				HabilitaLCD();
				RetardoLCD(4);
				break;
		case 3: centena=a/100;
				decena=(a%100)/10;
				unidad=a%10;
				EnviaDato(centena+48);
				HabilitaLCD();
				RetardoLCD(4);
				EnviaDato(decena+48);
				HabilitaLCD();
				RetardoLCD(4);
				EnviaDato(unidad+48);
				HabilitaLCD();
				RetardoLCD(4);
				break;
		default: break;
	}
}
void EscribeLCD_n16(unsigned int a,unsigned char b){
//Función que escribe un número positivo de 16 bits en la pantalla
//a es el número a escribir, el cual debe estar en el rango de 0 a 65535
//b es el número de digitos que se desea mostrar empezando desde las unidades
//Ejemplo EscribeLCD_n16(12754,5);	
    unsigned char decena,unidad;
	unsigned int centena,millar;
    RS=1;
	switch(b){
		case 1: unidad=a%10;
                EscribeLCD_c(unidad+48);
				break;
		case 2:	decena=(a%100)/10;
				unidad=a%10;
				EscribeLCD_c(decena+48);
                EscribeLCD_c(unidad+48);
				break;
		case 3: centena=(a%1000)/100;
                decena=(a%100)/10;
				unidad=a%10;
                EscribeLCD_c(centena+48);
				EscribeLCD_c(decena+48);
                EscribeLCD_c(unidad+48);
				break;
		case 4: millar=(a%10000)/1000;
                centena=(a%1000)/100;
                decena=(a%100)/10;
				unidad=a%10;
                EscribeLCD_c(millar+48);
				EscribeLCD_c(centena+48);
				EscribeLCD_c(decena+48);
                EscribeLCD_c(unidad+48);
				break;
		case 5: EscribeLCD_c(a/10000 +48);
				millar=(a%10000)/1000;
                centena=(a%1000)/100;
                decena=(a%100)/10;
				unidad=a%10;
                EscribeLCD_c(millar+48);
				EscribeLCD_c(centena+48);
				EscribeLCD_c(decena+48);
                EscribeLCD_c(unidad+48);
				break;
		default: break;
	}	
}
void EscribeLCD_d(double num, unsigned char digi, unsigned char digd){
	
}
void MensajeLCD_Var(char* a, char line){
//Función que escribe una cadena de caracteres variable en la pantalla
//a es una cadena de caracteres guardada en una variable *char
//Ejemplo: char aux[4]="Hola"; MensajeLCD_Var(aux);
    iter = 0;
    if(line == 1){ComandoLCD(0xC0);} else{CursorAInicio();}
    while((*a)!=0){
        EscribeLCD_c(*a);
        a++;
        iter++;
    }
    for(iter; iter<=0xF; iter++){
        EscribeLCD_c('\b');
    }
}
void DireccionaLCD(unsigned char a){
//Función que ubica el cursor en una posición especificada
//a debe ser una dirección de 8 bits valida de la DDRAM o la CGRAM	
	RS=0;
	EnviaDato(a);
	HabilitaLCD();
	RetardoLCD(4);
}
void FijaCursorLCD(unsigned char fila,unsigned char columna){
//Función que ubica el cursor en una fila y columna especificadas
//fila es un valor positivo que especifica la posición del cursor de 1 a 4
//columna es un valor positivo que especifica la posición del cursor de 1 a 80
//Si el display es de una fila, Ej: 8x1, tiene 80 columnas
//Si el display es de dos filas, Ej: 20x2, tiene 40 columnas
//Si el display es de cuatro filas, Ej: 20x4, tiene 20 columnas
	
}
void DesplazaPantallaD(void){
//Función que desplaza una sola vez la pantalla a la derecha	
  RS=0;
  EnviaDato(0x1C);
  HabilitaLCD();
  RetardoLCD(4);	
}
void DesplazaPantallaI(void){
//Función que desplaza una sola vez la pantalla a la izquierda
  RS=0;
  EnviaDato(0x18);
  HabilitaLCD();
  RetardoLCD(4);	
	
}
void DesplazaCursorD(void){
//Función que desplaza una sola vez la pantalla a la derecha
	
  RS=0;
  EnviaDato(0x14);
  HabilitaLCD();
  RetardoLCD(4);	
}
void DesplazaCursorI(void){
//Función que desplaza una sola vez la pantalla a la izquierda
	
  RS=0;
  EnviaDato(0x10);
  HabilitaLCD();
  RetardoLCD(4);	
}		
void RetardoLCD(unsigned char a){
	switch(a){
		case 1: __delay_ms(15);
                //Delay100TCYx(38); //Retardo de mas de 15 ms
				break;
		case 2: __delay_ms(1);
                __delay_us(640);
                //Delay10TCYx(41); //Retardo de mas de 1.64 ms
				break;
		case 3: __delay_us(100);
                //Delay10TCYx(3);	//Retardo de mas de 100 us
				break;
		case 4: __delay_us(40);
                //Delay10TCYx(1); //Retardo de mas de 40 us
				break;
		default:
				break;
	}
}


void ReadKey(void){
    LATB=0x07;
    __delay_ms(10);
    LATB=0x0B;
    __delay_ms(10);
    LATB=0x0D;
    __delay_ms(10);
    LATB=0x0E;
    __delay_ms(10);
    LATB=0x0F;
}

void TakeKbAction(void){
    if(key_pressed){
        switch(keyboard_value){
            case 0xA:
				key_pressed=0;
                count=0;
				__delay_ms(10);
                break;
            case 0xB:
				key_pressed=0;
                LED_PIN=1;
                EmergencyStop();
				__delay_ms(10);
                break;
            case 0xC:
				key_pressed=0; 
                state=2;
                break;
				__delay_ms(10);
                break;
            case 0xD:
				key_pressed=0;
                BACKLIGHT_PIN=~BACKLIGHT_PIN;
				__delay_ms(10);
                break;
            default:
                break;
        }
    }
}

void interrupt ISR(void){
    if(RBIF){
        __delay_ms(50); //Debouncing
        switch(PORTB){
            case 0x77: //R1, CE
                keyboard_value = 0x1;
                key_pressed=1;
                TMR1ON=1;
                BACKLIGHT_PIN=1;
                time_counter=0;
                __delay_ms(200);
				break;
            case 0x7B: //RE, CD
                keyboard_value = 0x2;
                key_pressed=1;
                TMR1ON=1;
                BACKLIGHT_PIN=1;
                time_counter=0;
                __delay_ms(200);
				break;
            case 0x7D: //RE, C3
                keyboard_value = 0x3;
                key_pressed=1;
                TMR1ON=1;
                BACKLIGHT_PIN=1;
                time_counter=0;
                __delay_ms(200);
				break;
            case 0x7E: //RE, CB
                keyboard_value = 0xA;
                key_pressed=1;
                TMR1ON=1;
                BACKLIGHT_PIN=1;
                time_counter=0;
                __delay_ms(200);
				break;
            case 0xB7: //RD, CE
                keyboard_value = 0x4;
                key_pressed=1;
                TMR1ON=1;
                BACKLIGHT_PIN=1;
                time_counter=0;
                __delay_ms(200);
				break;
            case 0xBB: //RD, CD
                keyboard_value = 0x5;
                key_pressed=1;
                TMR1ON=1;
                BACKLIGHT_PIN=1;
                time_counter=0;
                __delay_ms(200);
				break;
            case 0xBD: //RD, C3
                keyboard_value = 0x6;
                key_pressed=1;
                TMR1ON=1;
                BACKLIGHT_PIN=1;
                time_counter=0;
                __delay_ms(200);
				break;
            case 0xBE: //RD, CB
                keyboard_value = 0xB;
                key_pressed=1;
                TMR1ON=1;
                BACKLIGHT_PIN=1;
                time_counter=0;
                __delay_ms(200);
				break;
            case 0xD7: //R3, CE
                keyboard_value = 0x7;
                key_pressed=1;
                TMR1ON=1;
                BACKLIGHT_PIN=1;
                time_counter=0;
                __delay_ms(200);
				break;
            case 0xDB: //R3, CD
                keyboard_value = 0x8;
                key_pressed=1;
                TMR1ON=1;
                BACKLIGHT_PIN=1;
                time_counter=0;
                __delay_ms(200);
				break;
            case 0xDD: //R3, C3
                keyboard_value = 0x9;
                key_pressed=1;
                TMR1ON=1;
                BACKLIGHT_PIN=1;
                time_counter=0;
                __delay_ms(200);
				break;
            case 0xDE: //R3, CB
                keyboard_value = 0xC;
                key_pressed=1;
                TMR1ON=1;
                BACKLIGHT_PIN=1;
                time_counter=0;
                __delay_ms(200);
				break;
            case 0xE7: //RB, CE
                keyboard_value = 0xE;
                key_pressed=1;
                TMR1ON=1;
                BACKLIGHT_PIN=1;
                time_counter=0;
                __delay_ms(200);
				break;
            case 0xEB: //RB, CD
                keyboard_value = 0x0;
                key_pressed=1;
                TMR1ON=1;
                BACKLIGHT_PIN=1;
                time_counter=0;
                __delay_ms(200);
				break;
            case 0xED: //RB, C3
                keyboard_value = 0xF;
                key_pressed=1;
                TMR1ON=1;
                BACKLIGHT_PIN=1;
                time_counter=0;
                __delay_ms(200);
				break;
            case 0xEE: //RB, CB
                keyboard_value = 0xD;
                key_pressed=1;
                TMR1ON=1;
                time_counter=0;
                __delay_ms(200);
				break;
            default:
                //keyboard_value = 0xFF;
                break;
        }
        RBIF=0;
    } else if(TMR1IF){
        LED_PIN=~LED_PIN;
        TMR1IF=0;
        time_counter++;
        if(time_counter>=40){
            TMR1ON=0;
            LED_PIN=0;
        } else if(time_counter>=20){
            BACKLIGHT_PIN=0;
        }
    }
}

#endif	/* LIBLCDXC8_H */
