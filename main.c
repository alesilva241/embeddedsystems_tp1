#include <msp430g2553.h>

float temp_C=0.0;
int j,i,w =0;
unsigned int temp=0, r_temp=0;
unsigned char b[4], c[5];
unsigned char a=2;

void adcinit()
{
	/* Configure ADC  Channel */
	ADC10CTL1 = INCH_3 + ADC10DIV_3 ;         // Channel 3, ADC10CLK/3
	ADC10CTL0 = SREF_0 + ADC10SHT_3 + ADC10ON + ADC10IE;  // Vcc & Vss as reference, Sample and hold for 64 Clock cycles, ADC on, ADC interrupt enable
	ADC10AE0 |= BIT3;                       //P1.3 ADC option

}



void serialInit()
{
	P1SEL= BIT1 + BIT2; //UTILIZAÇÃO DAS PORTAS P1.1, P1.2 PARA COMUNICAÇÃO BLUETOOTH
	P1SEL2= BIT1 +BIT2; // P1.1=RXD & P1.2=TXD
	UCA0CTL1|= UCSSEL_2; // ATIVAÇÃO REGISTRADOR DE TRANSFERENCIA PARA P1.2
	UCA0BR0=104; // REGISTRADORES DE TAXA DE TRANSMISSÃO PARA APLICACAO CONFIGURADO PARA 9600
	UCA0BR1=0;//1MHz 9600
    UCA0MCTL= UCBRS0; // CONTROLE DO MODULO BLUETOOTH/HABILITA OU NÃO
    UCA0CTL1&=~UCSWRST; //RESET APLICAÇÃO  
    IE2|= UCA0RXIE; // HABILITA INTERRUPCAO 
}

unsigned char serialRead()
{
	while(!(IFG2&UCA0RXIFG));   //verifica dados e recupera enquanto soliticado
	return UCA0RXBUF;

}
void serialWrite(unsigned char c)
{
	while(!(IFG2&UCA0TXIFG));  // verifica buffer de transferencia e envia
	UCA0TXBUF=c; // TX
}
void serialwriteString(const char *str)
{
	while(*str)
		serialWrite(*str++);
}

void serial_write_int(unsigned int temp)
{
	for(i=0;i<4;i++)
	{
		b[i]=temp%10;
		//serialWrite(b[i]);
		temp=temp/10;
	}

	for(j=3;j>=0;j--)
	{
		serialWrite(b[j] + 48);
	}

	serialWrite(' ');
	serialWrite('\n');

}


serial_write_float(float v)
{

	w = v*100;

	for(i=0;i<4;i++)
	{
		c[i]=w%10;
		w=w/10;
	}

	serialWrite(c[3]+48);
	serialWrite(c[2] +48);
	serialWrite('.');
	serialWrite(c[1] + 48);
	serialWrite(c[0] + 48);

	serialWrite(' ');
	serialWrite('\n');

}

void main()
{
	WDTCTL = WDTPW + WDTHOLD;
	BCSCTL1 = CALBC1_1MHZ;
	DCOCTL = CALDCO_1MHZ;
    P1DIR = 0x41;
    P1OUT=0x00;


	adcinit();
	serialInit();
	int value=0;

	while(1)
	{

		ADC10CTL0 |= ENC + ADC10SC;
		while(ADC10CTL1 & ADC10BUSY)
		{}
		value = ADC10MEM;
		temp = value*0.317; 
		
		a=serialRead();

		if (a== '1')
		{

			P1OUT=0x01; // only red led is blinking
			__delay_cycles(5000);
			P1OUT=0x00;
			serialwriteString("Celsius :");
			serial_write_float(temp);

		}
		
	}

}
