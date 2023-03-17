/* 
 * File:   main.c
 * Author: Administrator
 *
 * Created on 23 March 2022, 16:27
 */

// PIC16F684 Configuration Bit Settings

// 'C' source line config statements

// CONFIG
#pragma config FOSC = INTOSCIO  // Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA4/OSC2/CLKOUT pin, I/O function on RA5/OSC1/CLKIN)
#pragma config WDTE = ON        // Watchdog Timer Enable bit (WDT enabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON       // MCLR Pin Function Select bit (MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = ON       // Brown Out Detect (BOR enabled)
#pragma config IESO = ON        // Internal External Switchover bit (Internal External Switchover mode is enabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is enabled)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.
// 245.5 Hz PWM signal with variable duty cycle from 0% to 33.4% gives 73mA to 450mA
#include <xc.h>
#define _XTAL_FREQ 8000000

void PWM_Initialize()
{
  TRISC5 = 1; // make port pin on C as input 
  PR2 = 0xfb;  //frequency fine adjustment 
  CCP1M3 = 1; CCP1M2 = 1; CCP1M1 = 0; CCP1M0 = 0; P1M1 = 1; P1M0 = 0;  //Configure the CCP1CON register 
   TMR2IF = 0;
   T2CKPS1 = 1; T2CKPS0 = 1; TMR2ON = 1; //Configure the Timer module also helps in frequency adjustment
   while(!TMR2IF); //Wait until Timer2 overflows 
   TRISC5 = 0;   
}

void ADC_Initialize()
{
  ADCON0 = 0b10001001; //ADFM 0, Voltage ref is VDD and ADC ON  
  ADCON1 = 0b01010000; // Fosc/16 is selected
}

int ADC_Read()
{
  __delay_ms(2); //Acquisition time to charge hold capacitor
  GO_nDONE = 1; //Initializes A/D Conversion
  while(GO_nDONE); //Wait for A/D Conversion to complete
  return ((ADRESH<<8)+ADRESL); //Returns Result  
}

long map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void main(){
 
 int duty, val1;
 int val2, counter; 
 char flag2; 
 IRCF2 = 1; IRCF1 = 1; IRCF0 = 1;    // set frequency from 8MHz to 31kHz default is 4MHz 110
 TRISC = 0x00; //PORTC as output
 TRISA = 0xFF; //PORTA as input
 ANSEL = 4; // Turn off ADC

 ADC_Initialize();
 PWM_Initialize(); 
 
 while(1){
 
  if(RA5 == 1){
   duty = ADC_Read();
   val1 = map(duty, 0, 1023, 0, 160);
   DC1B1 = val1 & 2; //Store the 1st bit             // button not pressed 
   DC1B0 = val1 & 1; //Store the 0th bit
   CCPR1L = val1;// Store the remaining 8 bit   
   RC0 = 0;
   counter = 0;   
  }
  else{
   RC0 = 1;   
   val2 = map(counter, 0, 1023, 0, 80);
   DC1B1 = val2 & 2; //Store the 1st bit
   DC1B0 = val2 & 1; //Store the 0th bit
   CCPR1L = val2;// Store the remaining 8 bit
   
   if(counter == 0){
    flag2 = 0;  
   }
   else if(counter == 1023){
    flag2 = 1;                                       // button pressed
   }
   
   if(flag2){   
    counter--; 
   }
   else{
    counter++; 
   }             
  }
      
 }
    
} 



