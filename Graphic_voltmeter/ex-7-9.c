//*******************************************************************
// Voltímetro Gráfico
//*******************************************************************
// Autor: Fábio Pereira
// Para o livro Microcontroladores MSP430: Teoria e Prática
//*******************************************************************
// Este programa implementa um voltímetro digital que apresenta gra-
// ficamente a tensão em um display LCD.
// Foi utilizado um LCD gráfico modelo G241281WNHDWB, que utiliza
// um controlador T6963C.
//*******************************************************************

#include <io430x16x.h>
#include <stdio.h>
#include “lcdg.h”
#include “lcdg.c” // caso se utilize a biblioteca compilada esta linha
									// pode ser removida

void main(void)
{
	unsigned char posicao=0;
	unsigned int ultimo,atual,temp2;
	unsigned long int temp;
	unsigned char string[10];
	WDTCTL = WDTPW + WDTHOLD;
	// configura o FLL para 8MHz (a partir do LFXT1 a 32768Hz
	//SCFI0 = FLLD_2 + FN_3;
	//SCFQCTL = 0x79;
	//FLL_CTL0 = DCOPLUS;
	//FLL_CTL1 = 0;
	DCOCTL = 0xff;
	BCSCTL1 = RSEL2+RSEL1+RSEL0;
	ADC12CTL1 = CONSEQ_2 + SHP;
	ADC12MCTL0 = 0;
	ADC12CTL0 = MSC + ADC12ON + ENC + ADC12SC;
  // configura o pino 3.7 para PWM, f= 62,5KHz, ciclo ativo = 34%
  TBCTL = TBSSEL_2 + MC_1;
  TBCCTL6 = OUTMOD_2;
  TBCCR0 = 0xFF;
  TBCCR6 = 0x61;
  P4DIR_bit.P4DIR_6 = 1;
  P4SEL_bit.P4SEL_6 = 1;
  lcdg_ini();
	ultimo = 0;
	lcdg_apaga_tela_texto();
	lcdg_apaga_tela_grafica();
	lcdg_posiciona_texto(10,5);
	lcdg_escreve_string("CPU: MSP430F149");
	lcdg_posiciona_texto(3,6);
	lcdg_escreve_string("DEMO: Mostrador grafico de tensao");
	lcdg_seta_modo(lcdg_modo_XOR);
	while(1)
	{
		//lcdg_apaga_tela_texto();
		//lcdg_escreve_tela(tela1);
		//while (1);
		for (posicao=0;posicao<=230;posicao=posicao+5)
		{
			lcdg_retangulo_preenchido(posicao,0,posicao+5,127,0);
			atual = (long)ADC12MEM0 * 127 / 4095;
			lcdg_linha(posicao,ultimo,posicao+5,127-atual,1);
			//ADC12IFG = 0;
			ultimo = 127-atual;
			lcdg_posiciona_texto(0,0);
			sprintf(string,"%4d",ADC12MEM0);
			lcdg_escreve_string(string);
			lcdg_posiciona_texto(0,1);
			temp = (long)ADC12MEM0 * 3300 / 4095;
			sprintf(string,"%04ld",temp);
			string[5]=0;
			string[4]=string[3];
			string[3]=string[2];
			string[2]=string[1];
			string[1]='.';
			lcdg_escreve_string(string);
			lcdg_escreve_string(" Volts");
		}
	}
}
