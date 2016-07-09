//*******************************************************************
// Autor: F�bio Pereira
// Para o livro Microcontroladores MSP430: Teoria e Pr�tica
//*******************************************************************
// Este programa implementa um termometro digital que apresenta a
// temperatura lida em um m�dulo LCD 16x2. Foi utilizada a placa 
// Microlab X1 com um m�dulo CPU MSP430F149. A temperatura � lida
// atrav�s do sensor interno do m�dulo ADC12
//*******************************************************************

#include <io430x16x.h>
#include <stdio.h>
#include "lcdc.h"
#include "lcdc.c"

void main(void)
{
	unsigned long int temperatura;
	unsigned int buffer[8];
	unsigned int offset;
	unsigned char string[10], temp;
	WDTCTL = WDTPW + WDTHOLD;	// desativa o watchdog
	// configura o DCO para a velocidade m�xima
  DCOCTL = 255;
  BCSCTL1 = 7;
  // configura o pino P2.2 como sa�da e o coloca em n�vel "0"
  // o pino write do m�dulo deve ser conectado ao mesmo
	P2DIR_bit.P2DIR_2 = 1;
	P2OUT_bit.P2OUT_2 = 0;
	// configura o ADC12 para medi��o cont�nua da mem�ria 0
	ADC12CTL1 = CONSEQ_2 + SHP + ADC12DIV_7;
	// seleciona o diodo de temperatura e a refer�ncia Vref+
	// para a mem�ria 0
	ADC12MCTL0 = SREF_1 + INCH_10;
	// seleciona um per�odo de amostragem de 1024 ciclos de clock
	// do ADC12, modo cont�nuo
	ADC12CTL0 = SHT0_15 + MSC + REFON + ADC12ON + ENC + ADC12SC;
	// inicializa o m�dulo
  lcdc_ini(lcdc_display_8x5|lcdc_2_linhas,lcdc_display_ligado|lcdc_cursor_desligado|lcdc_cursor_fixo);
  lcdc_escreve_char ('\f');	// apaga a tela
  // posiciona o cursor na primeira coluna da primeira linha
  lcdc_posiciona_texto(1,0);
  lcdc_escreve_string("Temp=");
  offset = 100;
  for (temp=0; temp<8; temp++) buffer[temp]=0;
	while (1)
	{
		// l� o resultado da convers�o e calcula a temperatura
		temperatura = (((long)ADC12MEM0*4230)>>12)-2780;
		// faz a m�dia das �ltimas 8 leituras
		for (temp=0; temp<7; temp++) buffer[temp]=buffer[temp+1];
		buffer[7]= temperatura+offset;
		for (temp=0; temp<7; temp++) temperatura += buffer[temp];
		temperatura >>= 3;	// divide por 8
		// converte o resultado em uma string de 5 d�gitos
		sprintf(string,"%5ld",temperatura);
		// desloca os 3 d�gitos mais significativos para colocar
		// o ponto decimal
		string[0]=string[1];
		string[1]=string[2];
		string[2]=string[3];
		string[3]='.';
		// escreve o resultado no display
		lcdc_posiciona_texto(6,0);
		lcdc_escreve_string(string);	
		lcdc_escreve_char(0xDF);		// s�mbolo de grau
		lcdc_escreve_char('C');
		// um atraso utilizando a pr�pria vari�vel temperatura
		for (temperatura=300000;temperatura;temperatura--);	
	}
}
