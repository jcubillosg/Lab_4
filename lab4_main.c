/* Requirements:
 * 4-bit connection with LCD 
 * 1. Welcome sentence for 5s. This message will have:
 * - Developer defined characters
 * 2. Ask user: "How many pieces do you wanna count for?"
 * - The "OK" key will set the count to whatever value
 *   the user specified. Write back the keys pressed
 * - Value will go from 1 to 59
 * 3. Print at any point how many pieces are currently counted
 *    and the count goal
 * 4. Print a message whenever the goal has been reached and
 *    wait until the user presses the "OK" key to go back to 
 *    step 2
 * 5. Set the 4x4 matrix keyboard to have: 
 *    - Digits from 0-9
 *    - An "OK" key
 *    - A delete key for the user input
 *    - A reset count key for the current count
 *    - An emergency stop key with a specific message
 *    - A count finish key that sends user to message 2
 *    - Backlight on/off
 * 6. After idle for 10s, turn off backlight; after 20s idle,
 *    turn off LCD
 */

/* Definitions: 
 * 4-Bit interface via Port D <RD7:RD4>
 * Keyboard keys function mapping:
 * ['0-9'->0-9, 
 *  '#'->Enter/OK,
 *  '*'->Delete,
 *  'A'->Reset_count,
 *  'B'->Emergency_stop,
 *  'C'->Count_end,
 *  'D'->Backlight_onoff]
 *
 *
 * */

#include <xc.h>
#include "LibLCDXC8_mod.h"

#pragma config FOSC=INTOSC_EC
#pragma config WDT=OFF
#pragma config LVP=OFF
#pragma config PBADEN=OFF

void main(void) {
    Setup(); //Hello msg
    while(1){
        /*
        if(cnt%16==0) ComandoLCD(0xC0);        
        EscribeLCD_c(cnt+'0');
        cnt++; 
        */
        /*
        ReadKey();
        TakeKbAction();
        if((keyboard_value<0xA)&&key_pressed){
            EscribeLCD_c('0'+keyboard_value);
            __delay_ms(1000);
            key_pressed=0;
            cnt++;
        }
        if(cnt%16==0) ComandoLCD(0xC0);
        */
        //Select # of pieces (LCD msg and kb function)
        switch(state){
            case 0: //Get target count
                MensajeLCD_Var("Piezas a contar:",0,0);
                while(!state){
                    ReadKey();
                    TakeKbAction();
                    if(key_pressed){
                        if(keyboard_value<0xA){
                            if(!target_count){
                                ComandoLCD(0xC0);
                                target_count=keyboard_value;
                                EscribeLCD_c('0'+keyboard_value);
                            } else if(target_count < 0xA){
                                target_count=keyboard_value + target_count*10;
                                EscribeLCD_c('0'+keyboard_value);
                            }
                            key_pressed=0;
                            __delay_ms(1000);
                        } else if(keyboard_value==0xE && target_count){
                            ComandoLCD(0x10); //Shifts cursor left
                            EscribeLCD_c(' '); //Writes whitespace
                            ComandoLCD(0x10); //Shifts cursor left
                            target_count/=10;
                            key_pressed=0;
                            __delay_ms(1000);
                        } else if(keyboard_value==0x0F){
                            if(target_count && (target_count<60)){
                                state=1;
                                key_pressed=0;
                                __delay_ms(500);
                            }
                        }
                    }
                }
                break;
            case 1:
                //Print LCD lines
                BorraLCD();
                MensajeLCD_Var("Objetivo:",0,0);
                EscribeLCD_c('0'+target_count/10);
                EscribeLCD_c('0'+target_count%10);
                MensajeLCD_Var("Cuenta:",1,0);
                EscribeLCD_c('0'+count/10);
                EscribeLCD_c('0'+count%10);
                while(count<target_count){
                    ReadKey();
                    TakeKbAction();
                    if(COUNTER_BUTTON){
                        if(button_pressed){
                            count++;
                            button_pressed=0;
                            time_counter=0;
                            BACKLIGHT_PIN=1;
                            MensajeLCD_Var("Cuenta:",1,0);
                            EscribeLCD_c('0'+count/10);
                            EscribeLCD_c('0'+count%10);
                            __delay_ms(500);
                            while(COUNTER_BUTTON);
                        } else{
                            __delay_ms(50);
                            button_pressed = COUNTER_BUTTON;
                        }
                    }
                }
                state=2;
                break;
            case 2:
                BorraLCD();
                MensajeLCD_Var("Cuenta completa!",0,1);
                MensajeLCD_Var("Presione OK.",1,1);
                keyboard_value=0xFF;
                while(keyboard_value!=0x0F){
                    KeyRead();
                    TakeKbAction();
                }
                state=0;
                break;
            default:
                state=0;
                break;
        }
        /*
        ReadKey();
        TakeKbAction();
        if((keyboard_value<0xA)&&(key_pressed)){
            key_pressed = 0;
            Datos=keyboard_value;
            __delay_ms(2000);
        }
        */
    }
}
