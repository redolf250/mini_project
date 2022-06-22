/*
 * File:   main.c
 * Author: KEREN-HUPPUCH
 *
 * Created on April 23, 2022, 8:10 AM
 */

//Define Buttons
#define MB RB1 //The middle button 
#define LB RB0 //Left button 
#define RB RB2 //Right button 
#define UB RB3 //Upper Button 
#define BB RB4 //Bottom button 

/*Set the current value of date and time below*/
int sec = 00;
int min = 55;
int hour = 10;
int day = 06;
int month = 05;
int year = 18;
/*Time and Date Set*/

/*Vars for Alarm clock*/
char set_alarm = 0;
char trigger_alarm = 0;
char pos = 7;
char jump = 0;

char alarm_h0 = 0;
char alarm_h1 = 0;
char alarm_m0 = 0;
char alarm_m1 = 0;
int alarm_val[4] = {0, 0, 0, 0};
/*End of var declaration*/

 char sec_0;
 char sec_1;
 char min_0;
 char min_1;
 char hour_0;
 char hour_1;

#include <xc.h>
#include "lcd_commands.h"
#include "PIC16F877a_I2C.h"
#include "PIC16F877a_DS3231.h"



#define _XTAL_FREQ 20000000

#define RS RD0
#define RW RD1
#define EN RD2
#define BUZZER RD3 //Buzzer connection pin



#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = ON         // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3/PGM pin has PGM function; low-voltage programming enabled)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

unsigned char byte;

void lcd_data(unsigned char data) {
    PORTD = (data & 0xF0);
    RS = 1;
    RW = 0;
    EN = 1;
    __delay_ms(5);
    EN = 0;
    
    PORTD = ((data<<4) & 0xF0);
    RS = 1;
    RW = 0;
    EN = 1;
    __delay_ms(5);
    EN = 0;
}

void lcd_cmd(unsigned char cmd) {
    PORTD = (cmd & 0xF0);
    RS = 0;
    RW = 0;
    EN = 1;
    __delay_ms(5);
    EN = 0;
    
    PORTD = ((cmd<<4) & 0xF0);
    RS = 0;
    RW = 0;
    EN = 1;
    __delay_ms(5);
    EN = 0;
}

void lcd_string(unsigned char *str){
    unsigned int i;
    for (i = 0; str[i]!= '\0';i++) {
        lcd_data(str[i]);
    }
}

void ee_data(unsigned char dat){
    PORTD = dat;
    RS = 1;
    RW = 0;
    EN = 1;
    __delay_ms(5);
    EN = 0;
}

void lcd_init() {
    lcd_cmd(0x02);
    lcd_cmd(0x28);
    lcd_cmd(0x0C);
    lcd_cmd(0x06);
    lcd_cmd(0x01);
}

void Lcd_SetBit(char data_bit) //Based on the Hex value Set the Bits of the Data Lines
{
    if(data_bit& 1) 
        D4 = 1;
    else
        D4 = 0;

    if(data_bit& 2)
        D5 = 1;
    else
        D5 = 0;

    if(data_bit& 4)
        D6 = 1;
    else
        D6 = 0;

    if(data_bit& 8) 
        D7 = 1;
    else
        D7 = 0;
}

void Lcd_Cmd(char a)
{
    RS = 0;           
    Lcd_SetBit(a); //Incoming Hex value
    EN  = 1;         
        __delay_ms(4);
    EN  = 0;         
}

void Lcd_Set_Cursor(char a, char b)
{
    char temp,z,y;
    if(a== 1)
    {
      temp = FORCE_CURSOR_TO_FIRST_ROW + b - 1; //80H is used to move the curser
        z = temp>>4; //Lower 8-bits
        y = temp & 0x0F; //Upper 8-bits
        Lcd_Cmd(z); //Set Row
        Lcd_Cmd(y); //Set Column
    }
    else if(a== 2)
    {
        temp = FORCE_CURSOR_TO_SECOND_ROW + b - 1;
        z = temp>>4; //Lower 8-bits
        y = temp & 0x0F; //Upper 8-bits
        Lcd_Cmd(z); //Set Row
        Lcd_Cmd(y); //Set Column
    }else if(a== 3)
    {
        temp = FORCE_CURSOR_TO_THIRD_ROW + b - 1;
        z = temp>>4; //Lower 8-bits
        y = temp & 0x0F; //Upper 8-bits
        Lcd_Cmd(z); //Set Row
        Lcd_Cmd(y); //Set Column
    }else if(a== 4)
    {
        temp = FORCE_CURSOR_TO_FORTH_ROW + b - 1;
        z = temp>>4; //Lower 8-bits
        y = temp & 0x0F; //Upper 8-bits
        Lcd_Cmd(z); //Set Row
        Lcd_Cmd(y); //Set Column
    }
    
}

void Lcd_Start()
{
  Lcd_SetBit(0x00);
  for(int i=1065244; i<=0; i--)  NOP();  
  Lcd_Cmd(0x03);
    __delay_ms(5);
  Lcd_Cmd(0x03);
    __delay_ms(11);
  Lcd_Cmd(0x03); 
  Lcd_Cmd(0x02); //02H is used for Return home -> Clears the RAM and initializes the LCD
  Lcd_Cmd(0x02); //02H is used for Return home -> Clears the RAM and initializes the LCD
  Lcd_Cmd(0x08); //Select Row 1
  Lcd_Cmd(0x00); //Clear Row 1 Display
  Lcd_Cmd(0x0C); //Select Row 2
  Lcd_Cmd(0x00); //Clear Row 2 Display
  Lcd_Cmd(0x06);
}

void lcd_clear(){
    lcd_cmd(CLEAR_SCREEN);
}
void Lcd_Print_Char(char data)  //Send 8-bits through 4-bit mode
{
   char Lower_Nibble,Upper_Nibble;
   Lower_Nibble = data&0x0F;
   Upper_Nibble = data&0xF0;
   RS = 1;             // => RS = 1
   Lcd_SetBit(Upper_Nibble>>4);             //Send upper half by shifting by 4
   EN = 1;
   __delay_ms(40);//for(int i=2130483; i<=0; i--)  NOP(); 
   EN = 0;
   Lcd_SetBit(Lower_Nibble); //Send Lower half
   EN = 1;
   __delay_ms(40);//for(int i=2130483; i<=0; i--)  NOP();
   EN = 0;
}

void Lcd_Print_String(char *a)
{
    int i;
    for(i=0;a[i]!='\0';i++)
       Lcd_Print_Char(a[i]);  //Split the string using pointers and call the Char function 
}


void readDataFromEEPROM(unsigned char address){
    EEADR = address;
    EECON1bits.EEPGD = 0;
    EECON1bits.RD = 1;
    byte = EEDATA;  
}

void writeDataToEEPROM(unsigned char data, unsigned char address){
    EEADR = address;
    EEDATA = data;
    EECON1bits.EEPGD = 0;
    EECON1bits.WREN = 1;
    EECON2 = 0x55;
    EECON2 = 0xAA;
    EECON1bits.WR = 1;
    while(PIR2bits.EEIF==0);
    PIR2bits.EEIF=0;    
}

void display_current_time(){
    //Split the into char to display on lcd
         sec_0 = sec % 10;
         sec_1 = (sec / 10);
         min_0 = min % 10;
         min_1 = min / 10;
         hour_0 = hour % 10;
         hour_1 = hour / 10;

        //Display the Current Time on the LCD screen
        Lcd_Set_Cursor(2,1);
        Lcd_Print_String("Time: ");
        Lcd_Print_Char(hour_1 + '0');
        Lcd_Print_Char(hour_0 + '0');
        Lcd_Print_Char(':');
        Lcd_Print_Char(min_1 + '0');
        Lcd_Print_Char(min_0 + '0');
        Lcd_Print_Char(':');
        Lcd_Print_Char(sec_1 + '0');
        Lcd_Print_Char(sec_0 + '0');
}

void display_stop_time(){
        Lcd_Set_Cursor(3, 1);
        Lcd_Print_String("Stop: ");
        Lcd_Print_Char(alarm_val[0] + '0');
        Lcd_Print_Char(alarm_val[1] + '0');
        Lcd_Print_Char(':');
        Lcd_Print_Char(alarm_val[2] + '0');
        Lcd_Print_Char(alarm_val[3] + '0');
        Lcd_Print_Char(':');
        Lcd_Print_Char('0');
        Lcd_Print_Char('0');
        __delay_ms(50);
}

void display_relay_status(){
     Lcd_Set_Cursor(4, 1);
     Lcd_Print_String("Relay status: ");
}

void display_current_date(){
    char year_0 = year % 10;
    char year_1 = year / 10;
    char month_0 = month % 10;
    char month_1 = month / 10;
    char day_0 = day % 10;
    char day_1 = day / 10;
    
    Lcd_Set_Cursor(1,13);
    Lcd_Print_Char(day_1  + '0');
    Lcd_Print_Char(day_0  + '0');
    Lcd_Print_Char('-');
    Lcd_Print_Char(month_1  + '0');
    Lcd_Print_Char(month_0  + '0');
    Lcd_Print_Char('-');
    Lcd_Print_Char(year_1  + '0');
    Lcd_Print_Char(year_0  + '0');
}

void get_controls(){
    
      //Use middle button to check if alarm has to be set 
        if (MB == 0 && set_alarm == 0) { //If middle button is pressed and alarm is not turned on 
            while (!MB); //Wait till button is released
            set_alarm = 1; //start setting alarm value 
        }
        if (MB == 0 && set_alarm == 1) { //If middle button is pressed and alarm is not turned off 
            while (!MB); //Wait till button is released
            set_alarm = 0; //stop setting alarm value 
        }


        //If alarm has to be navigate through each digit
        if (set_alarm == 1) {
            if (LB == 0) { //If left button is pressed
                while (!LB); //Wait till button is released
                pos--; //Then move the cursor to left
            }
            if (RB == 0) { //If right button is pressed 
                while (!RB); //Wait till button is released
                pos++; //Move cursor to right
            }

            if (pos >= 9) //eliminate ":" symbol if cursor reaches there
            {
                jump = 1; //Jump over the ":" symbol 
            } else
                jump = 0; //get back to normal movement


            if (UB == 0) { //If upper button is pressed
                while (!UB); //Wait till button is released
                alarm_val[(pos - 7)]++; //Increase that particular char value 
            }
            if (BB == 0) { //If lower button is pressed
                while (!UB); //Wait till button is released
                alarm_val[(pos - 7)]--; //Decrease that particular char value 
            }

            Lcd_Set_Cursor(3, pos + jump);
            Lcd_Print_Char(95); //Display "_" to indicate cursor position
        }

        //IF alarm is set Check if the set value is equal to current value 
        if (set_alarm == 0 && alarm_val[0] == hour_1 && alarm_val[1] == hour_0 && alarm_val[2] == min_1 && alarm_val[3] == min_0)
            trigger_alarm = 1; //Turn on trigger if value match

        if(trigger_alarm == 0){
            PORTCbits.RC0 = 1;
            Lcd_Set_Cursor(4, 15);
            Lcd_Print_String("ON");
        }else{
            PORTCbits.RC0 = 0;
            Lcd_Set_Cursor(4, 15);
            Lcd_Print_String("OFF");
        }
        
        __delay_ms(50);//Update interval 

}

void init_adc(){
    ADCON0 = 0b11000100; 
    ADCON1 = 0b11001110;
}

unsigned int get_adc(){
    unsigned int ADC_Results = 0;
    while(ADCON0bits.GO);
    ADC_Results = ADRESL;
    ADC_Results|= ((unsigned int)ADRESH) << 8;
    return ADC_Results;
}

void main(void){
    
    TRISCbits.TRISC0 = 0;
    PORTCbits.RC0 = 1;
    TRISD=0x00;
    TRISB = 0xFF; //Switches are declared as input pins
    OPTION_REG = 0b00000000; //Enable pull up Resistor on port B for switches
    BUZZER = 0; //Turn of buzzer
    lcd_init();
    
    I2C_Initialize(100); //Initialize I2C Master with 100KHz clock

    //Remove the below line once time and date is set for the first time.
    //Set_Time_Date(); //set time and date on the RTC module 
    
    Lcd_Set_Cursor(2,6);
    Lcd_Print_String("Welcome to");
    Lcd_Set_Cursor(3,7);
    Lcd_Print_String("rTronics");
     __delay_ms(1500);
      lcd_clear();
        
    while(1){
        
        
         
         //Digits[3] = (Value % 10) + '0';
         //Value /= 10;
         //Digits[2] = (Value % 10) + '0';
         //Value /= 10;
         //Digits[1] = (Value % 10) + '0';
         //Value /= 10;
         //Digits[0] = Value + '0';
         
         Update_Current_Date_Time(); //Read the current date and time from RTC module
         display_current_date();
         display_current_time();
         display_stop_time();
         display_relay_status(); 
         get_controls();
     
          ADCON0bits.GO =1;
        
         unsigned char value = get_adc();    //The value to convert
         unsigned char Digits[4];
         unsigned char newDigits[4];
 
         //Lcd_Set_Cursor(1,1);
         Digits[0] = value/1000;
         value = value - Digits[0]*1000;
         Digits[1] = value/100;
         value = value - Digits[1]*100;
         Digits[2] = value/10;
         Digits[3] = value%10;
         Lcd_Set_Cursor(1,1);
         for(int i=0;i<=4;i++){
             newDigits[i]=(Digits[i]+48);
         }
         lcd_data(Digits[0]+48);
         //Lcd_Print_String(newDigits);
        //writeDataToEEPROM(7,0x00);
        //readDataFromEEPROM(0x00);
        //lcd_cmd(0x80);
        //lcd_string("EEPROM: "); 
        //lcd_cmd(0xC0);
        //lcd_data(byte+0x30);
    
        
    } 
    return;
}
