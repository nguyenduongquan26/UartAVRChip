#include <avr/io.h>
#include <avr/interrupt.h>
#include "myLCD.h"
#include <util/delay.h>
/*Khai bao chan keypad */
#define KEYPAD_DDR      DDRC
#define KEYPAD_PORT     PORTC
#define KEYPAD_PIN      PINC

/*Khai bao UART */
#define F_CPU 7372800UL
#define USART_BAUDRATE 57600UL
#define BAUD_PRESCALE ((F_CPU/(USART_BAUDRATE<<4))-1)

/* Khai bao chan SET va chan User button */
#define SET PORTE
#define BUTT PINB
#define BUTTPIN 5
#define dem     16// Kich thuoc bo dem nhan
#define SIZE    20//Kich thuoc bo dem nhan 
unsigned char d[10];// Mang de luu gia tri keypad


unsigned char k, m, h,bit;
unsigned char buffer[SIZE];  //Bo dem nhan
unsigned char scan_code[4]={0x0E,0x0D,0x0B,0x07};
unsigned char ascii_code[4][4] ={ '1','2','3','A',
                            '4','5','6','B',
                            '7','8','9','C',
                            '*','0','#','D'};

void clear(unsigned char p, unsigned char q[]){  //Ham xoa bo nho dem
    unsigned char z;
    for(z=0;z<p;z++)
	q[z]='\0';
}

void xuly(){  //Ham xu ly du lieu nhan duoc qua UART
    clr_LCD();
    print_LCD("Nhan Noi Dung: "); 
	for (int h=0;h<k+1;h++)
	{      
    move_LCD(2,h);//Dong 2
    putChar_LCD(buffer[h]); //Xuat du lieu trong buffer ra LCD
	}
}

                                        
unsigned char  key;
// chuong trinh con doc keypad
unsigned char checkpad()
{
	uint8_t i,j,keyin;
	for(i=0;i<4;i++){              
			KEYPAD_PORT=255-(1<<(i+4)); 
			_delay_us(10);
			keyin=KEYPAD_PIN & 15;
			if(keyin!=15)
					for(j=0;j<4;j++)
							if(keyin==scan_code[j]) return ascii_code[j][i];
	}        
	return 0;
}
// chuong trinh con gui 1 byte UART
void uart_char_tx(unsigned char tchr) //Ham gui mot byte
{

      while ((UCSR0A &(1<<UDRE0)) == 0) ; //cho den khi bit UDRE=1 
      UDR0=tchr;
}
// chuong trinh con gui 1 chuoi ky tu
void send(unsigned char *u){//Ham gui mot chuoi ky tu
    while(*u){
        uart_char_tx(*u++);
    }
}

// chuong trinh con nhan du lieu UART
uint8_t uart_char_rx()
{
    // cho khi 1 byte dc nhan
    while((UCSR0A&(1<<RXC0)) == 0)
    {
        ;
    }

    // Return received data
    return UDR0;
}

///////////////////////////////////////////////////////////////
// chuong trinh con khoi tao module thu phat RF CC1101
void cc1101_init()
{ //Ham khoi tao module CC1101
  clr_LCD();
  print_LCD("MODE FU1");//Phuong thuc truyen mac dinh FU1
  SET &=(0<<PE5); 		//Keo chan 5 cua module xuong 0 de dung lenh AT
  send("AT+FU1\r");  	//Cai dat che do FU1 cho module
  _delay_ms(1000);
  clr_LCD();
  print_LCD("CHANNEL 102");
  send("AT+C102\r"); //Cai dat kenh truyen
  _delay_ms(1000);
  clr_LCD();
  print_LCD("ADDRESS 123");
  send("AT+A123\r");  //Cai dat dia chi truyen nhan
  _delay_ms(1000);
  clr_LCD();
  print_LCD("BAUDRATE 57600");
  send("AT+B57600\r");  //Cai dat dia chi truyen nhan
  _delay_ms(1000);
  SET |=(1<<PE5); //Keo chan 5 cua module len 1 de gui DATA
  clr_LCD();
  print_LCD("OK!...");
  _delay_ms(500);
  clr_LCD();
}
// chuong trinh con khoi tao UART
void USART_Init(void)
{
   // Set baud rate
   UBRR0L = (uint8_t) BAUD_PRESCALE;
   UBRR0H = (uint8_t)(BAUD_PRESCALE>>8); 
  
  UCSR0C = ((1<<UCSZ00|1<<UCSZ01)|(0<<USBS0));//8 bit do dai du lieu
  UCSR0B = ((1<<TXEN0)|(1<<RXEN0)|(1<<RXCIE0));// bat interrupt khi nhan ket thuc
}

int main(void)
{

KEYPAD_PORT=15;//Khoi tao cac chan cho keypad
KEYPAD_DDR=240;//   
PORTE=(0<<PE5);//Chan SET cho cc1101 dang o muc cao
DDRE=(1<<DDE5);//Port E la port xuat du lieu
PORTB|=(1<<PB5);//User button o muc cao
DDRB|=(0<<DDB5);//PortB5 la port nhap du lieu

init_LCD();// Khoi tao LCD
print_LCD("Nhom 4");
move_LCD(2,1);
print_LCD("Module CC1101");
_delay_ms(2000);
USART_Init();// Khoi tao UART
cc1101_init();// Khoi tao cc1101
sei();
	
   while(1)
{ 
		if((BUTT&(1<<BUTTPIN)) == 0)
	   { //Kiem tra nut nhan   
	   		int i=0;
			int a=0;
			int l=0;
            clr_LCD(); //Xoa man hinh
            print_LCD("Gui noi dung");       
            move_LCD(2,1);//Dong 2
            print_LCD("---STARTING---");  //Nut # de gui
            _delay_ms(2000);
			clr_LCD();
			print_LCD ("Chon xong nhan");
			move_LCD(2,1);
			print_LCD("nut # de gui di");
			_delay_ms(2000);
			clr_LCD();
			print_LCD("Noi dung gui:");
			move_LCD(2,1);
			key=30;
			while (key!=35)// key khac dau #
			{
				key=checkpad();
				if(key) _delay_ms(100);
				if(key)
				{
				d[i]=key;
				move_LCD(2,i);
				putChar_LCD(key);
				_delay_ms(100);
				i=i+1;
				if (i>dem) 
				{
				i=0;
				l=0;
				a=0;
				break;
				}
				}
			}
			//Ham lay gia tri do dai chuoi gui 
			while (d[a]!='\0')
			{	l++;
				a++;
			}
			
			clr_LCD();
			print_LCD("Ban da nhap xong");
			move_LCD(2,1);
			print_LCD("Dang gui");
			
			for (int q = 0; q<l-1;q++)
			{
			uart_char_tx(d[q]);
			_delay_ms(200);
			}
			send("\r");
			
			clr_LCD();
			print_LCD("Da gui xong");
			_delay_ms(1000);
			clr_LCD();
			clear(l,d);//Ham xoa bo nho dem gui di
		}
      
			else
			if (bit==1)
			{
			xuly();
			_delay_ms(200);
			clear(k,buffer);
			_delay_ms(100);
			k=0;
			bit=0;
			}
			else 
			{	
				print_LCD("Nhan User Button");
				move_LCD(2,1);
				print_LCD("De gui noi dung");
				
			}
	}
        
	    
};
//Ngat khi da nhan du lieu
ISR(SIG_UART0_RECV)
{

	buffer[k]=uart_char_rx();
	k++;
	if (buffer[k]=='\0')
	{
	bit=1;
	}
}
