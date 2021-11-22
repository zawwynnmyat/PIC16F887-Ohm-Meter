/*
 * File:   newmain.c
 * Author: Zaw Myat
 *
 * Created on December 27, 2020, 10:00 AM
 */

#define _XTAL_FREQ  8000000
// CONFIG1
#pragma config FOSC = EXTRC_CLKOUT// Oscillator Selection bits (RC oscillator: CLKOUT function on RA6/OSC2/CLKOUT pin, RC on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON       // RE3/MCLR pin function select bit (RE3/MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = ON       // Brown Out Reset Selection bits (BOR enabled)
#pragma config IESO = ON        // Internal External Switchover bit (Internal/External Switchover mode is enabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is enabled)
#pragma config LVP = ON         // Low Voltage Programming Enable bit (RB3/PGM pin has PGM function, low voltage programming enabled)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define RS RD2
#define EN RD3
#define D4 RD4
#define D5 RD5
#define D6 RD6
#define D7 RD7

unsigned int adcRead(unsigned char channel){
    int a,b, c;
    if(channel>7)return 0;
    ADCON0 = ADCON0 & 0xC5;
    ADCON0 = ADCON0 | (channel << 3);
    __delay_ms(2);
    ADCON0bits.GO_DONE = 1;
    while(ADCON0bits.GO_DONE);
    a = ADRESH;
    a = a<<2;
    b = ADRESL;
    b = b >>6;
    c = a| b;
    return c;

}

void Lcd_Port(char a)
{
	if(a & 1)
		D4 = 1;
	else
		D4 = 0;

	if(a & 2)
		D5 = 1;
	else
		D5 = 0;

	if(a & 4)
		D6 = 1;
	else
		D6 = 0;

	if(a & 8)
		D7 = 1;
	else
		D7 = 0;
}
void Lcd_Cmd(char a)
{
	RS = 0;             // => RS = 0
	Lcd_Port(a);
	EN  = 1;             // => E = 1
        __delay_ms(4);
        EN  = 0;             // => E = 0
}

Lcd_Clear()
{
	Lcd_Cmd(0);
	Lcd_Cmd(1);
}

void Lcd_Set_Cursor(char a, char b)
{
	char temp,z,y;
	if(a == 1)
	{
	  temp = 0x80 + b - 1;
		z = temp>>4;
		y = temp & 0x0F;
		Lcd_Cmd(z);
		Lcd_Cmd(y);
	}
	else if(a == 2)
	{
		temp = 0xC0 + b - 1;
		z = temp>>4;
		y = temp & 0x0F;
		Lcd_Cmd(z);
		Lcd_Cmd(y);
	}
}

void Lcd_Init()
{
  Lcd_Port(0x00);
   __delay_ms(20);
  Lcd_Cmd(0x03);
	__delay_ms(5);
  Lcd_Cmd(0x03);
	__delay_ms(11);
  Lcd_Cmd(0x03);

  Lcd_Cmd(0x02);
  Lcd_Cmd(0x02);
  Lcd_Cmd(0x08);
  Lcd_Cmd(0x00);
  Lcd_Cmd(0x0C);
  Lcd_Cmd(0x00);
  Lcd_Cmd(0x06);
}

void Lcd_Write_Char(char a)
{
   char temp,y;
   temp = a&0x0F;
   y = a&0xF0;
   RS = 1;             
   Lcd_Port(y>>4);           
   EN = 1;
   __delay_us(40);
   EN = 0;
   Lcd_Port(temp);
   EN = 1;
   __delay_us(40);
   EN = 0;
}

void Lcd_Write_String(char *a)
{
	int i;
	for(i=0;a[i]!='\0';i++)
	   Lcd_Write_Char(a[i]);
}

/*
 * Join Hexa Dev's C Language Class
 *  to understand the follwing 3 functions
 *  
 */
void reverse(char* str, int len) 
{ 
    int i = 0, j = len - 1, temp; 
    while (i < j) { 
        temp = str[i]; 
        str[i] = str[j]; 
        str[j] = temp; 
        i++; 
        j--; 
    } 
} 
  
int intToStr(int x, char str[], int d) 
{ 
    int i = 0; 
    while (x) { 
        str[i++] = (x % 10) + '0'; 
        x = x / 10; 
    } 
  
    while (i < d) 
        str[i++] = '0'; 
  
    reverse(str, i); 
    str[i] = '\0'; 
    return i; 
} 
void ftoa(float n, char* res, int afterpoint) 
{ 
    int ipart = (int)n; 
    float fpart = n - (float)ipart; 
    int i = intToStr(ipart, res, 0); 
    if (afterpoint != 0) { 
        res[i] = '.'; 
        fpart = fpart * pow(10, afterpoint); 
        intToStr((int)fpart, res + i + 1, afterpoint); 
    } 
} 
  
float v1, v2, v3;
 char str[10];
void main(void) {
    
    ADCON0 = 0x81;
    ADCON1 = 0x02;
    
    TRISD = 0x00;
    Lcd_Init();
    
    Lcd_Set_Cursor(1,1);
    Lcd_Write_String("Resistance (ohm) - "); 
    
    while(1) {
        
        v1=adcRead(0);
        v2 = (v1 * 5) / 1024 ;
        v3 = (v2 * 10000) / (5 - v2);
        ftoa(v3, str, 2); 
        
        Lcd_Set_Cursor(2,2);
        Lcd_Write_String(str); 
    }
}
