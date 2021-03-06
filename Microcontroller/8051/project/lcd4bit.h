#ifndef _lcd_h_
#define _lcd_h_

sfr 	lcd = 0x80; //P0=LCD data pins 
sbit	rs = P0^1;
sbit 	rw = P0^2;
sbit 	en = P0^3;

void lcd_cmd(unsigned char cmd)
{
	lcd = cmd&0xf0;   	//put the value on the pins
	rs = 0;
	rw = 0;
	en = 1;          	//strobe the enable pin
	delay(2);       
	en = 0;
	
	lcd = cmd<<4;   	//put the value on the pins
	rs = 0;
	rw = 0;
	en = 1;          	//strobe the enable pin
	delay(2);       
	en = 0;
}
void lcd_data(unsigned char ch)
{ 
	lcd = ch&0xf0;   	//put the value on the pins
	rs = 1;
	rw = 0;
	en = 1;          	//strobe the enable pin
	delay(2);       
	en = 0;
	
	lcd = ch<<4;   	//put the value on the pins
	rs = 1;
	rw = 0;
	en = 1;          	//strobe the enable pin
	delay(2);       
	en = 0;
}
void lcd_init()
{
 lcd_cmd(0x28);
 lcd_cmd(0x0c);
 lcd_cmd(0x06);
 lcd_cmd(0x01);
}
void lcd_string(unsigned char *p)
{
 	while(*p)
	lcd_data(*p++);
}
/*void lcd_integer (int n)
{
	int a[20];
	char i=0,c=0;
	if(n<0)		   								// if negative then show minus sign
	{
	 lcd_data('-');
	 n=-n;											
	}
	 while(n)
	 {
	 	a[i++]=	n%10  ;
	 	n=n/10;
		c++;
	 }
	 for(i=c-1;i>=0;i--)
	 {
	  lcd_data(a[i]+48);
	 }
}	
void lcd_float(float f)
{
	int temp;
	temp=f;
	lcd_integer(temp);
	f=(f-temp)*100;
	temp=f;
	lcd_data('.');
	if(temp<0)
		temp=-temp;
	lcd_integer(temp);	
}

/*void lcd_2digit(unsigned int n)
{
 lcd_data(n/10+48);
 lcd_data(n%10+48);
}
*/
#endif

