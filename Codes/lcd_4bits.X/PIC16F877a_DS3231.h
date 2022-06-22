/*
 * File: Header file to use DS3231 RTC module with PIC16F877A
 * Author: B.Aswinth Raj
 * Created on 5 May, 2018, 10:06 PM
 * Connect the RTC module to SDA and SCK pins of the PIC.
 * This header needs the PIC16f877a_I2C.h header to compile
 */


//PIN 18 -> RC3 -> SCL
//PIN 23 -> RC4 ->SDA


/****** Functions for RTC module *******/

int  BCD_2_DEC(int to_convert)
{
   return (to_convert >> 4) * 10 + (to_convert & 0x0F); 
}

int DEC_2_BCD (int to_convert)
{
   return ((to_convert / 10) << 4) + (to_convert % 10);
}

void Set_Time_Date()
{
   I2C_Begin();       
   I2C_Write(0xD0); 
   I2C_Write(0);  
   I2C_Write(DEC_2_BCD(sec)); //update sec
   I2C_Write(DEC_2_BCD(min)); //update min
   I2C_Write(DEC_2_BCD(hour)); //update hour
   I2C_Write(1); //ignore updating day
   I2C_Write(DEC_2_BCD(day)); //update date
   I2C_Write(DEC_2_BCD(month)); //update month
   I2C_Write(DEC_2_BCD(year)); //update year
   I2C_End();
}

void Update_Current_Date_Time()
{
   //START to Read
   I2C_Begin();       
   I2C_Write(0xD0); 
   I2C_Write(0);    
   I2C_End(); 
   
  //READ
   I2C_Begin();
   I2C_Write(0xD1);                              // Initialize data read
   sec = BCD_2_DEC(I2C_Read(1));    
   min = BCD_2_DEC(I2C_Read(1));   // Read sec from register 
   hour = BCD_2_DEC(I2C_Read(1));  
   I2C_Read(1);
   day = BCD_2_DEC(I2C_Read(1));  
   month = BCD_2_DEC(I2C_Read(1));  
   year = BCD_2_DEC(I2C_Read(1));  
   I2C_End(); 
    
  //END Reading  
    I2C_Begin();
    I2C_Write(0xD1);                              // Initialize data read
    I2C_Read(1);    
    I2C_End(); 

}
