#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <util/delay.h>
#include "lcd.h"
#include "uart.h"
#include "touchscreen.h"

#define FREQ 16000000
#define BAUD 9600
#define HIGH 1
#define LOW 0
#define BUFFER 1024
#define BLACK 0x000001

#define c 128

char displayChar = 0;


	uint16_t m0=0;//ADCX
	uint16_t n0=0;//ADCY
	uint16_t m=0;//X
	uint16_t n=0;//Y
	

	uint8_t pongx=63;//Rx
	uint8_t pongy=31;//Ry
	
	uint8_t lpaddle_x = 1, lpaddle_y = 26,rpaddle_x = 125,rpaddle_y = 26;


    int8_t coef=-3;// ball y moving direction
    int8_t mov=-3;//ball x moving direction
	
	uint16_t lplayer=48,rplayer=48;
	
//Buzzer
void buzzer(){
	
	if ((pongx<=3)|(pongx>=124))// when the ball touches the y grid
	{
		OCR1A=500;
		DDRB |= (1<<DDB1);
		TCCR1B |=(1<<CS10);// enable prescaler 
		_delay_ms(1000);
		DDRB &= ~(1<<DDB1);//disable buzzer

	}
	if ((pongy<=3)|(pongy>=60))// when the ball touches the x grid
	{
		OCR1A=2000;
		DDRB |= (1<<DDB1);
		TCCR1B |=(1<<CS10);// enable prescaler 
		_delay_ms(1000);
		DDRB &= ~(1<<DDB1);//disable buzzer

	}
	if ((pongx>=121)&(pongy+2>=rpaddle_y)&(pongy-2<=rpaddle_y+11))// when the ball touches the right paddle
	{
		OCR1A=1000;
		DDRB |= (1<<DDB1);
		TCCR1B |=(1<<CS10);// enable prescaler
		_delay_ms(1000);
		DDRB &= ~(1<<DDB1);//disable buzzer

	}
	if ((pongx<=6)&(pongy+2>=lpaddle_y)&(pongy-2<=lpaddle_y+11))// when the ball touches the left paddle
	{
		OCR1A=1000;
		DDRB |= (1<<DDB1);
		TCCR1B |=(1<<CS10);// enable prescaler
		_delay_ms(1000);
		DDRB &= ~(1<<DDB1);//disable buzzer

	}	
}

//random direction
void randomdirection(){
		if ((0<=TCNT0)&(TCNT0<=63)){mov=3;coef=3;}
		if ((64<=TCNT0)&(TCNT0<=127)){mov=3;coef=-3;}
		if ((128<=TCNT0)&(TCNT0<=191)){mov=-3;coef=3;}
		if ((192<=TCNT0)&(TCNT0<=255)){mov=-3;coef=-3;}	
	
}	

//random speed
void randomspeed(){
	uint8_t i=1;
	if ((0<=TCNT2)&(TCNT2<=84)){i=1;}
	if ((85<=TCNT2)&(TCNT2<=167)){i=2;}
	if ((168<=TCNT2)&(TCNT2<=256)){i=3;}
	mov = i * mov;
	coef = i * coef; 
}

void ballmovement(){   //moving ball
	
	
	
	clear_pong(buff,pongx,pongy);
	if((pongx>=121)&(pongy+2>=rpaddle_y)&(pongy-2<=rpaddle_y+11)&(mov>0))
	{
		mov=-mov;
	}
	if((pongx<=6)&(pongy+2>=lpaddle_y)&(pongy-2<=lpaddle_y+11)&(mov<0))
	{
		mov=-mov;
	}
	
	
	
	if((pongy>=60)|(pongy<=3))
	{
		coef=-coef;
	}
	
	/*
	if((pongx>=124)|(pongx<=3))
	{
		mov=-mov;
	}
	*/
	
	buzzer();
	
	if (pongx<=3)//left player lose
	{
		rplayer++;
		pongx=63;
		pongy=31;
		PORTD |= (1<<PORTD7);
	    OCR1A=666;
		DDRB |= (1<<DDB1);
		TCCR1B |=(1<<CS10);// enable prescaler
		_delay_ms(10000);
		PORTD &= ~(1<<PORTD7);
		DDRB &= ~(1<<DDB1);
		randomdirection();
		randomspeed();
		
	}
	if (pongx>=124)//right player lose
	{
		lplayer++;
		pongx=63;
		pongy=31;
		PORTD |= (1<<PORTD7);
		OCR1A=666;
		DDRB |= (1<<DDB1);
		TCCR1B |=(1<<CS10);// enable prescaler
		_delay_ms(10000);
		PORTD &= ~(1<<PORTD7);
		DDRB &= ~(1<<DDB1);
		randomdirection();
		randomspeed();
	}	

			

	if (rplayer == 50)//right player wins
	{
		drawstring(buff,68,2,c);
		drawchar(buff,65,1,rplayer);
		write_buffer(buff);	
		PORTD |= (1<<PORTD7);
		OCR1A=888;
		DDRB |= (1<<DDB1);
		TCCR1B |=(1<<CS10);// enable prescaler
		_delay_ms(10000);
		DDRB &= ~(1<<DDB1);
		_delay_ms(50000);
		PORTD &= ~(1<<PORTD7);
		clear_buffer(buff);
		write_buffer(buff);	
		pongx=63;
		pongy=31;
		coef=1;                        //
		mov=1;                          //
		rplayer=48;
		lplayer=48;
		
		//restart the paddle
		clearrect(buff,lpaddle_x,lpaddle_y,2,12);
		clearrect(buff,rpaddle_x,rpaddle_y,2,12);
		lpaddle_x = 1;
		lpaddle_y = 26;
		rpaddle_x = 125;
		rpaddle_y = 26;
		fillrect(buff,rpaddle_x,rpaddle_y, 2, 12,BLACK);//right paddle(12x2)
		fillrect(buff,lpaddle_x,lpaddle_y,2, 12,BLACK);//left paddle(12x2)
		write_buffer(buff);
		
		
	}
	if (lplayer == 50)//left player wins
	{
		drawstring(buff,45,2,c);
		drawchar(buff,57,1,lplayer);
		write_buffer(buff);
		PORTD |= (1<<PORTD7);
		OCR1A=888;
		DDRB |= (1<<DDB1);
		TCCR1B |=(1<<CS10);// enable prescaler
		_delay_ms(10000);
		DDRB &= ~(1<<DDB1);
		_delay_ms(50000);
		PORTD &= ~(1<<PORTD7);
		clear_buffer(buff);
		write_buffer(buff);
		pongx=63;
		pongy=31;
		coef=1;                                            //
		mov=1;                                            //          
		rplayer=48;
		lplayer=48;
		
		//restart the paddle
		clearrect(buff,lpaddle_x,lpaddle_y,2,12);
		clearrect(buff,rpaddle_x,rpaddle_y,2,12);
		lpaddle_x = 1;
		lpaddle_y = 26;
		rpaddle_x = 125;
		rpaddle_y = 26;
		fillrect(buff,rpaddle_x,rpaddle_y, 2, 12,BLACK);//right paddle(12x2)
		fillrect(buff,lpaddle_x,lpaddle_y,2, 12,BLACK);//left paddle(12x2)
		write_buffer(buff);
		
		
	}
		drawchar(buff,65,1,rplayer);
		drawchar(buff,57,1,lplayer);
		//write_buffer(buff);//update the score
	    pongx+=mov;
	    pongy+=coef;
	    fillcircle(buff,pongx,pongy,3,BLACK);
	    drawline(buff,63,0, 63,63,BLACK);//middle line
	    write_buffer(buff);
	
	
}

int main(void)
{
	//setting up the gpio for backlight
	DDRD |= 0x80;
	PORTD &= ~0x80;
	PORTD |= 0x00;
	
	DDRB |= 0x05;
	PORTB &= ~0x05;
	PORTB |= 0x00;
	
	
	//lcd initialisation
	lcd_init();
	lcd_command(CMD_DISPLAY_ON);
	lcd_set_brightness(0x18);
	write_buffer(buff);
	_delay_ms(10000);
	clear_buffer(buff);
	
	setupADC();
	uart_init();
	

	
	drawrect(buff,0, 0, 128,64,BLACK);//gridline
	fillrect(buff,rpaddle_x,rpaddle_y, 2, 12,BLACK);//right paddle(12x2)
	fillrect(buff,lpaddle_x,lpaddle_y,2, 12,BLACK);//left paddle(12x2)
	fillcircle(buff,pongx, pongy, 3,BLACK);//ball at the center
	drawchar(buff,65,1,rplayer);
	drawchar(buff,57,1,lplayer);

	write_buffer(buff);
	
	//Buzzer
	DDRB |= (1<<DDRB);
	PORTB &= ~(1<<PORTB1);
	TCCR1B |=(1<<WGM12);//CTC
	TCCR1A |= (1<<COM1A0);//CTC toggle
	TCCR1B &= ~(1<<CS10);//disable prescaler
	TCCR1B &= ~(1<<CS11);
	TCCR1B &= ~(1<<CS12);
	
	//random direction
	TCCR0B |= (1 << CS00);
	
	//random speed
	TCCR2B |= (1 << CS20);

	
	while (1)
	{
		
		ballmovement();
		
		drawrect(buff,0, 0, 128,64,BLACK);//gridline
		
		//problem: using for will change the ball speed when change the paddle???
		/*
		for (uint8_t i = 6;i <=66;i =i +10)// middle discrete line
		{
			drawline(buff,63, i-5, 63,i,BLACK);
			write_buffer(buff);
		}
		*/
		
		
		// moving paddle
		if (detect())  // if touchscreen
		{
		  
			m0 = touchscreen_readXADC();
		    m = -0.168*m0 +144.662;
			printf("%d\n",m);
		
		    n0 = touchscreen_readYADC();
		    n = 0.124*n0 - 39.529;
		
			
			if ((0<=m)&&(m<=127)&&(1<=n)&&(n<=51))// if touch within the screen size
			{
				if (m <= 63)//if touch at the left side
				{
					if (n>lpaddle_y)//below the original rec
					{
						clearrect(buff,1,lpaddle_y,2,12);
						lpaddle_y = lpaddle_y + 2;
						drawrect(buff,1,lpaddle_y,2,12,BLACK);
						write_buffer(buff);
										
					}
					else//above the original rec
					{
						clearrect(buff,1,lpaddle_y,2,12);
						lpaddle_y = lpaddle_y - 2;
						drawrect(buff,1,lpaddle_y,2,12,BLACK);
						write_buffer(buff);
					}
				}
				
				else//if touch at the right side
				{
					if (n>rpaddle_y)//below the original rec
					{
						clearrect(buff,125,rpaddle_y,2,12);
						rpaddle_y = rpaddle_y + 2;
						drawrect(buff,125,rpaddle_y,2,12,BLACK);
						write_buffer(buff);
					}
					else//above the original rec
					{
						clearrect(buff,125,rpaddle_y,2,12);
						rpaddle_y = rpaddle_y - 2;
						drawrect(buff,125,rpaddle_y,2,12,BLACK);
						write_buffer(buff);
					}
				}
				
				
			}
		
		}
		drawrect(buff,125,rpaddle_y,2,12,BLACK);
		drawrect(buff,1,lpaddle_y,2,12,BLACK);
		write_buffer(buff);
		
		/*
		if (lpaddle_x==3)//left player lose
		{
			rplayer++;
			drawchar(buff,65,0,rplayer);
			write_buffer(buff);
		}
		
		if (rpaddle_x==124)//right player lose
		{
			lplayer++;
			drawchar(buff,57,0,lplayer);
			write_buffer(buff);
		}
		*/
		
	}
	
}
