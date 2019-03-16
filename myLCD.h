#include<avr/io.h>
#include<util/delay.h>
#include<avr/pgmspace.h>
#define F_CPU 7372800UL
// define 2 macro to set and clear bit
#define sbi(sfr,bit) sfr|= _BV(bit)
#define cbi(sfr,bit) sfr&= ~(_BV(bit))
// define connection between LCD and uC
#define EN 2
#define RW 1
#define RS 0
#define DATA_O      PORTC
#define DDR_DATA    DDRC
#define DATA_I      PINC
#define CTRL        PORTC
#define DDR_CTRL    DDRC
// Function prototype
char Read2Nib();
void write2Nib(uint8_t chr);
void wait_LCD();
void init_LCD();
void home_LCD();
void putnum_LCD (uint8_t num);
void print_LCD(char* str);
void putChar_LCD (uint8_t chr);
void clr_LCD();
void move_LCD(uint8_t,uint8_t);
 
// read 2 nibble from LCD
char Read2Nib()
{
 
  char HNib,LNib;  // temporary data
  DDR_DATA&=0x0F; // set high nibble portB to be input
  sbi(CTRL,EN); //Enable LCD
  
  HNib=DATA_I & 0xF0; //get high nibble data
  cbi(CTRL,EN); //make a falling edge

  sbi(CTRL,EN); //Enable LCD
  LNib= (DATA_I & 0xF0)>>4;
  cbi(CTRL,EN);

  return(HNib|LNib);
}

void write2Nib(uint8_t chr)
{
uint8_t HNib,LNib,temp;

temp= DATA_O & 0x0F; //mask the high bit
//seperate high nib and low nib
HNib=chr & 0xF0;
LNib=(chr<<4)& 0xF0;
//write the high nibble first
DATA_O=(temp|HNib);
sbi(CTRL,EN);
cbi(CTRL,EN);
//write the second nibble then
DATA_O=(temp|LNib);
sbi(CTRL,EN);
cbi(CTRL,EN);
}

void wait_LCD()
{/*char temp_val;
  while(1)
  {
  cbi(CTRL,RS); // define the data is instruction
  sbi(CTRL,RW); // the data goes from LCD to AVR
  temp_val=Read2Nib();
  if (temp_val & (1<<7))
  break;
  }
  cbi(CTRL,RW); // end the read operation, turn back to normal mode
DDR_DATA|=0xF0;// ready to out */
_delay_ms(10);
}

void init_LCD()
{
DDR_CTRL|=0x07; // output on CTRL pin
DDR_DATA|=0xF0; // output on data pin to send out instruction
// Function set
cbi(CTRL,RS); // sending instruction
cbi(CTRL,RW); // the data goes from uP to LCD
cbi(CTRL,EN); // make a low be4 sending data
sbi(CTRL,EN); // setting the EN high
DATA_O|=0x20;
cbi(CTRL,EN);
wait_LCD();
write2Nib(0x28); // choosing 4 bit mode,2line,5x8 font
wait_LCD();
// Display control
cbi(CTRL,RS);
write2Nib(0x0C);
wait_LCD();
// Entry mode set
cbi(CTRL,RS);
write2Nib(0x06);
wait_LCD();


}

void home_LCD()
{
cbi(CTRL,RS); //sending instruction
cbi(CTRL,RW); //data from uP to LCD
write2Nib(0x02);
wait_LCD();
}

void move_LCD(uint8_t y,uint8_t x)
{
uint8_t add;
add=64*(y-1)+(x-1)+0x80;
cbi(CTRL,RS);//sending command
cbi(CTRL,RW);//data from up to LCD
write2Nib(add);
wait_LCD();
}

void clr_LCD()
{
cbi(CTRL,RS); //sending instruction
cbi(CTRL,RW); //data from uP to LCD
write2Nib(0x01);
wait_LCD();
}

void putChar_LCD (uint8_t chr)
{
sbi(CTRL,RS); //sending data
cbi(CTRL,RW); //data from uP to LCD
write2Nib(chr);
wait_LCD();
}

void print_LCD(char* str)
{unsigned char i=0;
while(str[i]!='\0') 
{putChar_LCD(str[i]);
i++;}
}

void print_LCD_P(char* str)
{unsigned char i=0;
while (pgm_read_byte(&str[i])!='\0')
{putChar_LCD(pgm_read_byte(&str[i]));
i++;
}
}

void putnum_LCD (uint8_t num)
{uint8_t temp=0x30;
uint8_t hundredth;
uint8_t tenth;
uint8_t unit;
// seperate number
hundredth=num/100;
num%=100;
tenth=num/10;
unit=num%10;
hundredth&=0x0F;
tenth&=0x0F;
unit&=0x0F;
sbi(CTRL,RS); //sending data
cbi(CTRL,RW); //data from uP to LCD
if (hundredth)
{hundredth=hundredth|temp;
tenth=tenth|temp;
unit=unit|temp;
 write2Nib(hundredth);
 wait_LCD();
 sbi(CTRL,RS); //sending data
 cbi(CTRL,RW); //data from uP to LCD
 write2Nib(tenth);
 wait_LCD();
sbi(CTRL,RS); //sending data
cbi(CTRL,RW); //data from uP to LCD
 write2Nib(unit);
 wait_LCD();
 }
else if (tenth)
{tenth=tenth|temp;
 unit=unit|temp;
 write2Nib(tenth);
 wait_LCD();
 sbi(CTRL,RS); //sending data
cbi(CTRL,RW); //data from uP to LCD
 write2Nib(unit);
 wait_LCD();
}
else 
 {
 unit=unit|temp;
 write2Nib(unit);
 wait_LCD();
 }
}
void welcome_LCD()
{
print_LCD("ECE 411 Project");
_delay_ms(1000);

for (int i=0;i<=10;i++)
{
 move_LCD(2,i);
 putChar_LCD(255);
 move_LCD(2,11);
 putnum_LCD(10*i);
 putChar_LCD(0x25);
 _delay_ms(100);
 }
_delay_ms(1000);
clr_LCD();
}

//void main(void){}
