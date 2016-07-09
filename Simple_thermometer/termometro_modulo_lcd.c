//*******************************************************************
// Autor: Fábio Pereira
// Para o livro Microcontroladores MSP430: Teoria e Prática
//*******************************************************************
// Este programa implementa um termometro digital que apresenta a
// temperatura lida em um módulo LCD 16x2. Foi utilizada a placa 
// Microlab X1 com um módulo CPU MSP430F149. A temperatura é lida
// através do sensor interno do módulo ADC12
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
	// configura o DCO para a velocidade máxima
  DCOCTL = 255;
  BCSCTL1 = 7;
  // configura o pino P2.2 como saída e o coloca em nível "0"
  // o pino write do módulo deve ser conectado ao mesmo
	P2DIR_bit.P2DIR_2 = 1;
	P2OUT_bit.P2OUT_2 = 0;
	// configura o ADC12 para medição contínua da memória 0
	ADC12CTL1 = CONSEQ_2 + SHP + ADC12DIV_7;
	// seleciona o diodo de temperatura e a referência Vref+
	// para a memória 0
	ADC12MCTL0 = SREF_1 + INCH_10;
	// seleciona um período de amostragem de 1024 ciclos de clock
	// do ADC12, modo contínuo
	ADC12CTL0 = SHT0_15 + MSC + REFON + ADC12ON + ENC + ADC12SC;
	// inicializa o módulo
  lcdc_ini(lcdc_display_8x5|lcdc_2_linhas,lcdc_display_ligado|lcdc_cursor_desligado|lcdc_cursor_fixo);
  lcdc_escreve_char ('\f');	// apaga a tela
  // posiciona o cursor na primeira coluna da primeira linha
  lcdc_posiciona_texto(1,0);
  lcdc_escreve_string("Temp=");
  offset = 100;
  for (temp=0; temp<8; temp++) buffer[temp]=0;
	while (1)
	{
		// lê o resultado da conversão e calcula a temperatura
		temperatura = (((long)ADC12MEM0*4230)>>12)-2780;
		// faz a média das últimas 8 leituras
		for (temp=0; temp<7; temp++) buffer[temp]=buffer[temp+1];
		buffer[7]= temperatura+offset;
		for (temp=0; temp<7; temp++) temperatura += buffer[temp];
		temperatura >>= 3;	// divide por 8
		// converte o resultado em uma string de 5 dígitos
		sprintf(string,"%5ld",temperatura);
		// desloca os 3 dígitos mais significativos para colocar
		// o ponto decimal
		string[0]=string[1];
		string[1]=string[2];
		string[2]=string[3];
		string[3]='.';
		// escreve o resultado no display
		lcdc_posiciona_texto(6,0);
		lcdc_escreve_string(string);	
		lcdc_escreve_char(0xDF);		// símbolo de grau
		lcdc_escreve_char('C');
		// um atraso utilizando a própria variável temperatura
		for (temperatura=300000;temperatura;temperatura--);	
	}
}
