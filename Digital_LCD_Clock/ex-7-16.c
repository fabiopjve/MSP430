//*******************************************************************
// Relógio Digital Simples
//*******************************************************************
// Autor: Fábio Pereira
// Para o livro Microcontroladores MSP430: Teoria e Prática
//*******************************************************************
// Este programa demonstra a construção de um relógio digital simples
// utilizando um MSP430F449. O microcontrolador é mantido no modo
// LPM3 durante a maior parte do tempo, sendo ativado a cada 0,25s
// pelo temporizador básico (BT). Na função de tratamento de inter-
// rupção do mesmo, é medida a passagem de um segundo, contagem dos
// segundos, minutos e horas, além da monitoração das teclas.
//*******************************************************************
#include <io430x44x.h>
#include <intrinsics.h>
#include "lcds.h"
#include "lcds.c"
unsigned char hora,minuto,segundo;

#define BTIP0 1
#define BTIP1 2
#define BTIP2 4
#define tcl_inc_hora P2IN_bit.P2IN_0
#define tcl_inc_minuto P2IN_bit.P2IN_1
//*******************************************************************
// Função: trata_BT
//*******************************************************************
// Esta função trata a interrupção do contador 2 do timer básico. O
// mesmo está configurado para dividir o ACLK por 256 * 32 = 8192,
// resultando em 4 interrupções por segundo (quando utilizando um 
// cristal de 32768Hz no LFXT1). Nesta função também são monitoradas
// as teclas de incremento das horas e dos minutos
//*******************************************************************
#pragma vector = BASICTIMER_VECTOR
__interrupt void trata_BT(void)
{
	static unsigned char contador, contador2, pontos, tempo_tecla;
	if (!tcl_inc_hora)
	{
	// se a tecla de incremento de horas estiver pressionada
		tempo_tecla++;	// incrementa o contador do tempo da tecla
		// se o contador for igual a 1 ou maior que 2
		if (tempo_tecla==1 || tempo_tecla>2)
		{
			hora++;	// incrementa a hora
			// ajusta o horário no formato BCD
			if ((hora & 0x0F)>0x09) hora=(hora & 0xF0)+0x10;
			// se a hora for maior que 11, volta a 0
			if (hora>0x11) hora = 0;
		}
	} else if (!tcl_inc_minuto)
	{
	// se a tecla de incremento de horas estiver pressionada
		tempo_tecla++;	// incrementa o contador do tempo da tecla
		// se o contador for igual a 1 ou maior que 2
		if (tempo_tecla==1 || tempo_tecla>2)
		{
			minuto++;	// incrementa o minuto
			// ajusta o horário no formato BCD
			if ((minuto & 0x0F)>0x09) minuto=(minuto & 0xF0)+0x10;	
			// se minuto for maior que 59, volta a 0
			if (minuto>0x59) minuto = 0;
		}
	// se nenhuma tecla pressionada, volta o contador de tempo das
	// teclas a zero
	} else tempo_tecla = 0;
	if (++contador>=4)
	{
		// se o contador de interrupções for maior ou igual a 4
		// passou-se um segundo
		contador = 0;	// zera o contador
		segundo++;	// incrementa os segundos
		// ajusta a variável no formato BCD
		if ((segundo & 0x0F)>0x09) segundo=(segundo & 0xF0)+0x10;
		if (segundo>0x59)	// se segundos maior que 59
		{
			segundo = 0;	// zera segundos
			minuto++;	// incrementa minutos
			// ajusta a variável no formato BCD
			if ((minuto & 0x0F)>0x09) minuto=(minuto & 0xF0)+0x10;	
			if (minuto>0x59)	// se minutos maior que 59
			{
				minuto = 0;	// zera minutos
				hora++;	// incrementa hora
				// ajusta a variável no formato BCD
				if ((hora & 0x0F)>0x09) hora=(hora & 0xF0)+0x10;
				// se hora maior que 11, zera a hora
				if (hora>0x11) hora = 0;
			}	
		}
		// apresenta o MSD da hora
		lcds_atualiza_digito_4(hora>>4);
		// apresenta o LSD da hora
		lcds_atualiza_digito_3(hora & 0x0F);
		// apresenta o MSD do minuto
		lcds_atualiza_digito_2(minuto>>4);
		// apresenta o LSD do minuto
		lcds_atualiza_digito_1(minuto & 0x0F);
	}
	if (++contador2>=2)	
	{
		// se passou meio segundo
		contador2 = 0;
		pontos = !pontos;	// inverte o estado dos pontos
		// se os pontos estiverem ativos, acende-os no display
		if (pontos) LCDM9^=0x01;
	}
	if (tempo_tecla)
	{
		// se tempo_tecla maior que zero, alguma tecla está
		// pressionada, neste caso, devemos atualizar os
		// dígitos do display
		lcds_atualiza_digito_4(hora>>4);
		lcds_atualiza_digito_3(hora & 0x0F);
		lcds_atualiza_digito_2(minuto>>4);
		lcds_atualiza_digito_1(minuto & 0x0F);
	}
}
void main(void)
{
	unsigned int tempo1, tempo2;
	WDTCTL = WDTPW+WDTHOLD;       // desliga o watchdog
	// configura o FLL para aproximadamente 1MHz 
	// (a partir do LFXT1 a 32768Hz)
	SCFI0 = FLLD_1;
	SCFQCTL = 0x1E;
	FLL_CTL0 = DCOPLUS;
	FLL_CTL1 = SMCLKOFF + XT2OFF;
	while (!IFG1_bit.OFIFG) IFG1_bit.OFIFG = 0;
	// configura o timer básico
	BTCTL = BTDIV + BT_fCLK2_DIV32;	// ACLK / (256 * 32)
	IE2 = BTIE;	// ativa a interrupção do timer básico
	// liga o LCD, seleciona o modo estático
	// habilita todas as saídas do módulo LCD
	LCDCTL = LCDON+LCDSTATIC+LCDP2+LCDP1+LCDP0;
	// configura todos os pinos como saída, para reduzir o consumo
	P1DIR = P3DIR = P4DIR = P5DIR = P6DIR = 0xFF;
	P2DIR = 0xFC;
	// inicia em zero a contagem
	hora = 0x00;
	minuto = 0x00;
	segundo = 0x00;	
	__enable_interrupt();	// habilita as interrupções
	while (1)
	{
		__low_power_mode_3();	// entrada no modo LMP3
	}
}