//*******************************************************************
// Autor: F�bio Pereira
// Para o livro Microcontroladores MSP430: Teoria e Pr�tica
//*******************************************************************
// Este programa implementa um volt�metro digital que apresenta a
// tens�o lida em um m�dulo LCD 16x2. Foi utilizada a placa Microlab
// X1 com um m�dulo CPU MSP430F149. A tens�o foi lida da entrada ANA2
// conectada a um trimpot na pr�pria placa.
//*******************************************************************

#include <io430x16x.h>
#include <lcd.h>

void main(void)
{
	unsigned long int temp;
	unsigned char string[10];
	WDTCTL = WDTPW + WDTHOLD;	// desativa o watchdog
	// configura o DCO para a velocidade m�xima
  DCOCTL = 255;
  BCSCTL1 = 7;
  // configura o pino P2.2 como sa�da e o coloca em n�vel "0"
  // o pino write do m�dulo deve ser conectado ao mesmo
	P2DIR_bit.P2DIR_2 = 1;
	P2OUT_bit.P2OUT_2 = 0;
	// configura o ADC12 para medi��o cont�nua da mem�ria 0 no 
	// primeiro canal de entrada
	ADC12CTL1 = CONSEQ_2 + SHP;
	ADC12MCTL0 = 0;
	ADC12CTL0 = MSC + ADC12ON + ENC + ADC12SC;
	P6DIR_bit.P6DIR_0 = 0;	// pino P6.0 como entrada
	P6SEL_bit.P6SEL_0 = 1;	// pino P6.0 na fun��o ADC
	// inicializa o m�dulo
  lcdc_ini(lcdc_display_8x5|lcdc_2_linhas,lcdc_display_ligado|lcdc_cursor_desligado|lcdc_cursor_fixo);
  lcdc_escreve_char ('\f');	// apaga a tela
  lcdc_posiciona_texto(1,1);	// posiciona o cursor na sexta coluna da primeira linha
  lcdc_escreve_string("Tensao=");	// escreve: MSP430
	while (1)
	{
		temp = (long)ADC12MEM0 * 3300 / 4095;
		sprintf(string,"%04ld",temp);
		string[5]=0;
		string[4]=string[3];
		string[3]=string[2];
		string[2]=string[1];
		string[1]='.';
		lcdc_posiciona_texto(8,1);
		lcdc_escreve_string(string);	// escreve o valor da tens�o
		lcdc_escreve_string("mV");		
	}
}
