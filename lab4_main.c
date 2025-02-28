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
 *  '*'->Enter/OK,
 *  '#'->Delete,
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
        //Select # of pieces (LCD msg and kb function)
        switch(state){
            case 0: //Get target count
                target_count=0;
                ReadKey();
                TakeKbAction();
                if((keyboard_value<0xA)&& key_pressed){
                    if(!target_count){ //there isn't a previous value
                        target_count=keyboard_value;
                    } else{
                        target_count+=keyboard_value;
                    }
                    tmp=keyboard_value;
                } else if((keyboard_value==0xF) && key_pressed){
                    target_count-=tmp;                    
                    //LCD handling
                } else if(keyboard_value==0xE){
                    if(target_count>0 && target_count<60) state=1; //Ok key
                }
                break;
            case 1:
                //Print LCD lines
                while(count<target_count){
                    //Button handling
                    if(COUNTER_BUTTON){
                        
                    }
                }
                break;
            case 2:

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
