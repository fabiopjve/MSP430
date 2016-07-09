//*******************************************************************
// Controle de servos R/C por PWM
//*******************************************************************
// Autor: Fábio Pereira
// Para o livro Microcontroladores MSP430: Teoria e Prática
//*******************************************************************
// Este programa demonstra a utilização do timer B dos MSP430 na
// geração de sinais de PWM. No presente caso, os sinais gerados
// pelos canais 4 e 5 são utilizados para o controle de dois servos
// R/C conforme descrito no texto do livro. Foi utilizada a placa
// Microlab X1, um módulo MSP430F149 e dois servos Hobbico modelo
// CS-35MG.
//*******************************************************************
#include <io430x14x.h>
#include “lcdc.h”
#include “lcdc.c” // caso se utilize a biblioteca compilada esta linha
									// pode ser removida
#include “teclado3x4.c” // função de leitura do teclado

void main(void)
{
	unsigned char tecla,direcao,temporizacao = 100;
	unsigned int servo_pos;
	WDTCTL = WDTPW + WDTHOLD; // desativa o watchdog
	// clock interno, aproximadamente 5MHz
	DCOCTL = 255;
	BCSCTL1 = 7;
	// Configura os canais 4 e 5 do timer B para o modo PWM
	// A freqüência do sinal é de aproximadamente 82Hz
	// a variação do ciclo ativo permite alterar a duração dos
	// pulsos na faixa de operação dos servos, que é de 1 a 2ms
	TBCTL = MC_1 + TBSSEL_2;
	TBCCTL4 = OUTMOD_2;
	TBCCTL5 = OUTMOD_2;
	TBCCR0 = 0xf000; // período do sinal
	TBCCR5 = 0xcd00; // ciclo ativo do canal 5
	TBCCR4 = 0xcd00; // ciclo ativo do canal 4
	// configura os pinos P4.4 e P4.5 para a função de saída
	// do timer B
	P4SEL_bit.P4SEL_4=1;
	P4DIR_bit.P4DIR_4=1;
	P4SEL_bit.P4SEL_5=1;
	P4DIR_bit.P4DIR_5=1;
	// configura o pino P2.2 como saída e o coloca em nível "0"
	// o pino write do módulo deve ser conectado ao mesmo
	P2DIR_bit.P2DIR_2 = 1;
	P2OUT_bit.P2OUT_2 = 0;
	// inicializa o módulo LCD
	lcdc_ini(lcdc_display_8x5|lcdc_2_linhas,lcdc_display_ligado|
	lcdc_cursor_desligado|lcdc_cursor_fixo);
	// apaga a tela
	lcdc_escreve_char ('\f');
	lcdc_posiciona_texto(3,0);
	lcdc_escreve_string("MSP430F149");
	lcdc_posiciona_texto(0,1);
	servo_pos = 0xbe00; // posição inicial do servo
	direcao = 1; // direção de deslocamento
	while(1)
	{
		tecla=varre_teclado(); // verifica o teclado
		if (temporizacao) temporizacao--; else
		{
			// movimenta automaticamente o servo conectado ao canal 5
			if (direcao) servo_pos += 10; else servo_pos -= 10;
			if (servo_pos>0xe200) direcao = 0;
			if (servo_pos<0xbf00) direcao = 1;
			TBCCR5 = servo_pos;
			temporizacao = 100;
		}
		if (tecla==10) // se for pressionada a tecla *, apaga a tela
		{
			lcdc_escreve_char('\f');
			lcdc_posiciona_texto(3,0);
			lcdc_escreve_string("MSP430F149");
			lcdc_posiciona_texto(0,1);
		}
		if (tecla<=9) // se a tecla for menor igual a 9
		{
			// escreve o valor da tecla no display
			lcdc_escreve_char(tecla+'0');
			switch (tecla)
			{
				// posiciona o servo ligado ao canal 4, na posição selecionada
				case 0 : TBCCR4 = 0xbe00;
				break;
				case 1 : TBCCR4 = 0xc200;
				break;
				case 2 : TBCCR4 = 0xc600;
				break;
				case 3 : TBCCR4 = 0xca00;
				break;
				case 4 : TBCCR4 = 0xce00;
				break;
				case 5 : TBCCR4 = 0xd200;
				break;
				case 6 : TBCCR4 = 0xd600;
				break;
				case 7 : TBCCR4 = 0xda00;
				break;
				case 8 : TBCCR4 = 0xde00;
				break;
				case 9 : TBCCR4 = 0xe200;
				break;
			}
		}
	}
}