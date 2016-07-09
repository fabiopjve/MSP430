#include "io430x16x.h"
#include "intrinsics.h"

#define endereco_escravo   0x50;

int PtrTransmit;
unsigned char I2CBuffer[3];

//*******************************************************************
// Inicializa a USART para o modo I2C
//*******************************************************************
void usart_I2C_init(void)
{
	P3SEL_bit.P3SEL_1 = 1;	// pino P3.1 para função SDA
	P3SEL_bit.P3SEL_3 = 1;	// pino P3.3 para função SCL
	// inicializa a USART para o modo I2C
  	U0CTL |= I2C+SYNC;
  	U0CTL_bit.I2CEN = 0;	// desabilita o módulo I2C
  	// seleciona o tamanho de palavra = byte
  	// clock I2C = SMCLK
  	I2CTCTL = I2CTRX+I2CSSEL_2;
  	I2CSA = endereco_escravo;	// endereço do dispositivo escravo
  	I2COA = 0x01A5;		// endereço do dispositivo mestre
  	I2CPSC = 0x00;		// Clock do I2C = SMCLK / 1
  	I2CSCLH = 0x03;		// Período alto do SCL = 5*I2C clock
  	I2CSCLL = 0x03;		// Período alto do SCL = 5*I2C clock
  	U0CTL_bit.I2CEN = 1;	// habilita o módulo I2C
}

//*******************************************************************
// Escreve um dado no endereço especificado da EEPROM
//*******************************************************************
void EEPROM_bytewrite(unsigned int endereco, unsigned char dado)
{
	unsigned char end_hi,end_lo;
	// aguarda a interface I2C estar pronta
	while (U0RCTL_bit.I2CBUSY); 
	end_hi = endereco >> 8;	// parte alta do endereço
	end_lo = endereco & 0xFF;// parte baixa do endereço
	// armazena o endereço e o dado no buffer I2C
	I2CBuffer[2] = end_hi;
	I2CBuffer[1] = end_lo;
	I2CBuffer[0] = dado;
	PtrTransmit = 2;	// configura o ponteiro de transmissão
	U0CTL_bit.MST = 1;	// modo mestre
	U0TCTL_bit.I2CTRX = 1;	// modo de transmissão
	I2CIFG_bit.TXRDYIFG = 0;// apaga flag de transmissão
	I2CIE_bit.TXRDYIE = 1;	// habilita interrupção de transmissão
	// número de bytes a serem transmitidos =
	// 1 de controle + 2 de endereço e 1 de dado  
	I2CNDAT = 3;
	// inicia a comunicação setando a condição de partida
	// a condição de parada também é setada e inserida ao término
	// da transmissão
	I2CTCTL |= I2CSTT+I2CSTP;
}

//*******************************************************************
// Lê um dado no endereço atual da EEPROM
//*******************************************************************
unsigned char EEPROM_byteread(void)
{
  	// aguarda a interface I2C estar pronta
  	while (U0RCTL_bit.I2CBUSY);
	U0TCTL_bit.I2CTRX = 0;	// modo de recepção
	I2CIE_bit.RXRDYIE = 1;	// habilita interrupção de recepção
	U0CTL_bit.MST = 1;	// modo mestre
	I2CNDAT = 1;		// recepção de 1 byte
	I2CIFG_bit.ARDYIFG = 0;	// apaga flag de acesso
	// inicia a recepção ativando a condição de partida
	// uma condição de re-início e de parada são geradas após
	// a recepção
	I2CTCTL |= I2CSTT+I2CSTP;
	while (!I2CIFG_bit.ARDYIFG);//aguarda a recepção ser completada
	// retorna com o dado recebido
	return I2CBuffer[0];
}

//*******************************************************************
// Lê um dado no endereço especificado da EEPROM
//*******************************************************************

unsigned char EEPROM_byterandomread(unsigned int endereco)
{
	unsigned char end_hi,end_lo;
  	// aguarda a interface I2C estar pronta
  	while (U0RCTL_bit.I2CBUSY);	
	end_hi = endereco >> 8;	// parte alta do endereço
	end_lo = endereco & 0xFF;// parte baixa do endereço
	I2CBuffer[1] = end_hi;
	I2CBuffer[0] = end_lo;
	PtrTransmit = 1;	// configura o ponteiro de transmissão
	U0CTL_bit.MST = 1;	// modo mestre
	U0TCTL_bit.I2CTRX = 1;	// modo de transmissão
	I2CIFG_bit.TXRDYIFG = 0;// apaga flag de transmissão
	I2CIE_bit.TXRDYIE = 1;	// habilita interrupção de transmissão
	// configura o número de bytes a serem transmitidos:
	// 1 de controle + 2 de endereço
	I2CNDAT = 2;
	I2CIFG_bit.ARDYIFG = 0;	// apaga flag de acesso
	U0TCTL_bit.I2CSTT = 1;	// condição de partida, inicia a comunicação
	while (I2CIFG_bit.ARDYIFG);//aguarda a transmissão ser completada
	U0TCTL_bit.I2CTRX = 0;	// modo de recepção
	I2CIE_bit.RXRDYIE = 1;	// habilita interrupção de recepção
	I2CNDAT = 1;		// recepção de 1 byte
	I2CIFG_bit.ARDYIFG = 0;	// apaga flag de acesso
	// inicia a recepção ativando a condição de partida
	// uma condição de re-início e de parada são geradas após
	// a recepção
	I2CTCTL |= I2CSTT+I2CSTP;
	while (!I2CIFG_bit.ARDYIFG);//aguarda a recepção ser completada
	// retorna com o dado recebido
	return I2CBuffer[0];
}

//*******************************************************************
// Aguarda um ACK da EEPROM
//*******************************************************************
void EEPROM_ackpolling(void)
{ 
  	// aguarda a interface I2C estar pronta
  	while (U0RCTL_bit.I2CBUSY);
  	// modifica a configuração do módulo I2C
	U0CTL_bit.I2CEN = 0;	// desabilita o módulo
	U0TCTL_bit.I2CRM = 1;	// transmissão controlada por software
	U0CTL_bit.I2CEN = 1;	// habilita módulo I2C
	I2CIFG = NACKIFG;	// habilita interrupção de detecção de NACK
	while (I2CIFG_bit.NACKIFG)
	{
		I2CIFG= 0;	// apaga os flags de interrupção
		U0CTL_bit.MST = 1;	// modo mestre
		U0TCTL_bit.I2CTRX = 1;	// modo de transmissão
		U0TCTL_bit.I2CSTT = 1;	// envia START
		while (U0TCTL_bit.I2CSTT); // aguarda o envio do START
		U0TCTL_bit.I2CSTP = 1;	// envia STOP
		// aguarda a interface I2C estar pronta
		while (U0RCTL_bit.I2CBUSY);
	}
	U0CTL_bit.I2CEN = 0;	// desabilita o módulo
	U0TCTL_bit.I2CRM = 0;	// transmissão controlada pelo hardware
	U0CTL_bit.I2CEN = 1;	// habilita módulo I2C
}

//*******************************************************************
// RTI da I2C
//*******************************************************************
#pragma vector=USART0TX_VECTOR
__interrupt void ISR_I2C(void)
{
	switch (I2CIV)
	{ 
		case I2CIV_AL:	// perda de arbitragem
				break;
		case I2CIV_NACK:// NACK
			      	break;
		case I2CIV_OA:	// Próprio endereço (own address)
			      	break;
		case I2CIV_ARDY:// pronto para acesso
			      	break;
		case I2CIV_RXRDY:// recepção
				// armazena o dado no buffer
			      	I2CBuffer[0] = I2CDRB;
			      	break;
		case I2CIV_TXRDY:// transmissão
				// armazena o dado do buffer (apontado pelo
				// PtrTransmit) no registrador de dados da
				// I2C
			      	I2CDRB = I2CBuffer[PtrTransmit];
			      	// se o ponteiro for maior que 0
			      	// decrementa o mesmo, caso seja igual a
			      	// zero, desabilita a interrupção de
			      	// transmissão I2C
			      	if (PtrTransmit) PtrTransmit --; 
			      		else I2CIE_bit.TXRDYIE = 0;
				break;
		case I2CIV_GC:	// chamada geral (general call)
				break;
		case I2CIV_STT:	// condição de partida
				break;
	}
}

//*******************************************************************
// Escreve um valor inteiro de 16 bits no endereço especificado
// da EEPROM
//*******************************************************************
void EEPROM_INTwrite(unsigned int endereco, unsigned int valor)
{
	unsigned char temp;
	temp = valor;
	EEPROM_bytewrite(endereco,temp);
	EEPROM_ackpolling();
	temp = valor>>8;
	EEPROM_bytewrite(endereco+1,temp);
	EEPROM_ackpolling();
}

//*******************************************************************
// Lê um valor inteiro de 16 bits do endereço especificado
// da EEPROM
//*******************************************************************
unsigned int EEPROM_INTrandomread(unsigned int endereco)
{
	unsigned int result;
	result = EEPROM_byterandomread(endereco);
	result+= EEPROM_byteread()<<8;
	return(result);
}

//*******************************************************************
// Escreve um valor inteiro de 32 bits no endereço especificado
// da EEPROM
//*******************************************************************
void EEPROM_LONGwrite(unsigned int endereco, unsigned long valor)
{
	struct svar32
	{
		char byte0;
		char byte1;
		char byte2;
		char byte3;
	};
	union uvar32
	{
		unsigned long int var32;
		struct svar32 var8;
	} temp;
	temp.var32 = valor;
	EEPROM_bytewrite(endereco,temp.var8.byte0);
	EEPROM_ackpolling();
	EEPROM_bytewrite(++endereco,temp.var8.byte1);
	EEPROM_ackpolling();
	EEPROM_bytewrite(++endereco,temp.var8.byte2);
	EEPROM_ackpolling();
	EEPROM_bytewrite(++endereco,temp.var8.byte3);
	EEPROM_ackpolling();		
}

//*******************************************************************
// Lê um valor inteiro de 32 bits no endereço especificado
// da EEPROM
//*******************************************************************
unsigned long EEPROM_LONGrandomread(unsigned int endereco)
{
	struct svar32
	{
		char byte0;
		char byte1;
		char byte2;
		char byte3;
	};
	union uvar32
	{
		unsigned long int var32;
		struct svar32 var8;
	} temp;
	temp.var8.byte0 = EEPROM_byterandomread(endereco);
	temp.var8.byte1 = EEPROM_byteread();
	temp.var8.byte2 = EEPROM_byteread();
	temp.var8.byte3 = EEPROM_byteread();
	return (temp.var32);
}

//*******************************************************************
// Escreve um valor float no endereço especificado da EEPROM
//*******************************************************************
void EEPROM_FLOATwrite(unsigned int endereco, float valor)
{
	struct svar32
	{
		char byte0;
		char byte1;
		char byte2;
		char byte3;
	};
	union uvarfloat
	{
		float varfloat;
		struct svar32 var8;
	} temp;
	temp.varfloat = valor;
	EEPROM_bytewrite(endereco,temp.var8.byte0);
	EEPROM_ackpolling();
	EEPROM_bytewrite(++endereco,temp.var8.byte1);
	EEPROM_ackpolling();
	EEPROM_bytewrite(++endereco,temp.var8.byte2);
	EEPROM_ackpolling();
	EEPROM_bytewrite(++endereco,temp.var8.byte3);
	EEPROM_ackpolling();	
}

//*******************************************************************
// Lê um valor float do endereço especificado da EEPROM
//*******************************************************************
float EEPROM_FLOATrandomread(unsigned int endereco)
{
	struct svar32
	{
		char byte0;
		char byte1;
		char byte2;
		char byte3;
	};
	union uvarfloat
	{
		float varfloat;
		struct svar32 var8;
	} temp;
	temp.var8.byte0 = EEPROM_byterandomread(endereco);
	temp.var8.byte1 = EEPROM_byteread();
	temp.var8.byte2 = EEPROM_byteread();
	temp.var8.byte3 = EEPROM_byteread();
	return (temp.varfloat);
}

void main(void)
{
	volatile unsigned char Data[6];
	volatile unsigned long teste32;
	volatile float tempflt;	
	WDTCTL = WDTPW+WDTHOLD;	// desliga o Watchdog
	usart_I2C_init();	// Inicializa o módulo I2C
	__enable_interrupt();
	EEPROM_bytewrite(0x0000,0xFA);
	EEPROM_ackpolling();	// Aguarda escrita na EEPROM
	EEPROM_bytewrite(0x0001,0xB1);
	EEPROM_ackpolling();	// Aguarda escrita na EEPROM
	EEPROM_bytewrite(0x0002,0x00);
	EEPROM_ackpolling();	// Aguarda escrita na EEPROM
	EEPROM_bytewrite(0x0003,0x12);
	EEPROM_ackpolling();	// Aguarda escrita na EEPROM
	EEPROM_bytewrite(0x0004,0x34);
	EEPROM_ackpolling();	// Aguarda escrita na EEPROM
	EEPROM_bytewrite(0x0005,0x56);
	EEPROM_ackpolling();	// Aguarda escrita na EEPROM
	// escreve o valor 0xABCD no endereço 0x0010
	EEPROM_INTwrite (0x0010,0xABCD);
	// escreve o valor 0x01234567 no endereço 0x0020
	EEPROM_LONGwrite (0x0020,0x01234567);
	tempflt = 12.3456;
	// escreve o valor 12.3456 no endereço 0x0030
	EEPROM_FLOATwrite(0x0030,tempflt);
	tempflt = 0;
	// Lê um byte no endereço 0x0000
	Data[0] = EEPROM_byterandomread(0x0000);
	// lê um byte no endereço 0x0001
	Data[1] = EEPROM_byteread();
	// lê um byte no endereço 0x0002
	Data[2] = EEPROM_byteread();
	// lê um byte no endereço 0x0003
	Data[3] = EEPROM_byteread();
	// lê um byte no endereço 0x0004
	Data[4] = EEPROM_byteread();
	// lê um byte no endereço 0x0005
	Data[5] = EEPROM_byteread();
	// lê um inteiro no endereço 0x0010
	teste32 = EEPROM_INTrandomread(0x0010);
	teste32++;
	// lê um inteiro longo no endereço 0x0020
	teste32 = EEPROM_LONGrandomread(0x0020);
	teste32++;
	// lê um float no endereço 0x0030
	tempflt = EEPROM_FLOATrandomread(0x0030);
	tempflt += 1;
	while (1);
}
