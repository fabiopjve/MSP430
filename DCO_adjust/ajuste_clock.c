//*******************************************************************
// Ajuste do DCO
//*******************************************************************
// Autor: F�bio Pereira
// Para o livro Microcontroladores MSP430: Teoria e Pr�tica
//*******************************************************************
// Este programa demonstra o ajuste do DCO a partir do oscilador
// LFXT1 operando em baixa freq��ncia (32768Hz)
// Isto permite construir aplica��es que necessitem da medi��o
// permanente e precisa de tempo (utilizando o cristal de 32768Hz)
// ao mesmo tempo em que, quando necess�rio, a CPU e perif�ricos
// podem utilizar um clock de alta freq��ncia, originado pelo DCO
// O ajuste efetuado por este tipo de programa, garante uma grande
// precis�o na sele��o da freq��ncia do DCO
// O funcionamento � simples: utilizamos o watchdog operando como um
// timer, a fonte de clock do mesmo � o ACLK, que neste caso � igual
// a 32768Hz. Programando-se o watchdog para gerar uma interrup��o a
// cada 64 pulsos do ACLK, teremos uma base de tempo de refer�ncia
// para a medi��o do sinal gerado pelo DCO.
// Utilizamos tamb�m o timer A, que ser� o respons�vel pela contagem
// dos pulsos de clock do DCO, para isso, configuramos a fonte de
// clock do timer como sendo o SMCLK e configuramos este clock para
// ser originado do DCO.
// Feito isso, a cada interrup��o do watchdog, verificamos a contagem
// do TAR.
// Se quisermos trabalhar com uma frequ�ncia de 1MHz, a cada inter-
// rup��o do watchdog, o TAR dever� ter feito 1953 contagens. Se o
// n�mero de contagens for maior, quer dizer que a freq��ncia do DCO
// � maior que 1MHz, se for menor, tamb�m menor � a freq��ncia do
// DCO.
//*******************************************************************

#include <io430x14x.h>
#include <intrinsics.h>

#define FCLOCK 4000000	// freq��ncia desejada em Hz

#define clock_ok 1

// estrutura utilizada para o controle da fun��o de ajuste de clock
struct sclkadj
{
	char ajustando : 1;
	char err : 1;
} clock_result;	// vari�vel global

// vari�veis de ajuste do DCO
unsigned char ajusteMODx, ajusteDCOx, ajusteRSELx;

//*******************************************************************
// aumenta_DCO
//*******************************************************************
// Entrada: void	Sa�da: unsigned char
//*******************************************************************
// A cada chamada desta fun��o a freq��ncia do DCO � incrementada em
// um passo. Isto � feito incrementando-se o MODx at� atingir 31, ao
// ultrapassar este valor, o DCOx � aumentado at� ser maior que 224,
// em seguida, o RSELx � incrementado at� 7. Caso a fun��o seja cha-
// mada e necessite incrementar RSELx acima de 7, � retornado o valor
// 0, indicando o erro. Caso o aumento seja bem sucedido, retorna 1
//*******************************************************************
unsigned char aumenta_DCO(void)
{
	unsigned char result = 1;
	// se MODx � menor que 31, incrementa
	if (ajusteMODx<31) ajusteMODx++; else
	{
		// se MODx � maior que 31
		ajusteMODx = 0;	// MODx = 0
		// se DCOx � menor quye 224, aumenta 32
		if (ajusteDCOx<224) ajusteDCOx+=32; else
		{
			// se DCOx � maior que 224
			// apaga DCOx
			ajusteDCOx = 0;
			// se RSELx � menor que 7, incrementa
			if (ajusteRSELx<7) ajusteRSELx++; 
				// sen�o, o resultado � falso
				else result = 0;
		}
	}
	// configura o DCOCTL de acordo com as vari�veis de ajuste
	DCOCTL = ajusteDCOx + ajusteMODx;
	// configura o BCSCTL1 de acordo com a vari�vel de ajuste dos  RSELx
	BCSCTL1 = ajusteRSELx;
	// retorna o resultado
	return (result);
}

void ajusta_clock(void)
{
	unsigned int IE1_temp, TACTL_temp;
	// configura o WDT para o modo temporizador, com clock = ACLK
	// fator de divis�o de 64
	WDTCTL = WDTPW + WDTTMSEL + WDTCNTCL + WDTSSEL + WDTIS1 + WDTIS0;
	IFG1_bit.WDTIFG = 0;	// apaga o sinalizador de interrup��o do watchdog
	IE1_temp = IE1;		// salva o registrador IE1
	IE1_bit.WDTIE = 1;	// habilita a interrup��o do watchdog
	// configura o DCO de acordo com a faixa de freq��ncia selecionada
	BCSCTL2 = 0;
	ajusteMODx = 0;
	#if FCLOCK<200000
		BCSCTL1 = 0;
		DCOCTL = 0;
		ajusteDCOx = 0;
		ajusteRSELx = 0;
	#elif FCLOCK<500000
		BCSCTL1 = 1;
		DCOCTL = 64;
		ajusteDCOx = 64;
		ajusteRSELx = 1;
	#elif FCLOCK<1000000
		BCSCTL1 = 3;
		DCOCTL = 64;
		ajusteDCOx = 64;
		ajusteRSELx = 3;
	#elif FCLOCK<2000000
		BCSCTL1 = 4;
		DCOCTL = 96;
		ajusteDCOx = 96;
		ajusteRSELx = 4;
	#elif FCLOCK<3000000
		BCSCTL1 = 6;
		DCOCTL = 32;
		ajusteDCOx = 32;
		ajusteRSELx = 6;
	#else
		BCSCTL1 = 7;
		DCOCTL = 32;
		ajusteDCOx = 32;
		ajusteRSELx = 7;
	#endif
	// salva a configura��o atual do timer A
	TACTL_temp = TACTL;
	// configura o Timer A:
	// modo 2, fonte de clock = SMCLK
	TACTL = TASSEL_2 + MC_2 + TACLR;
	__enable_interrupt();	// habilita interrup��es
	// apaga indicador de erro
	clock_result.err = 0;
	// ativa indicador de ajuste em andamento;
	clock_result.ajustando = 1;
	while (clock_result.ajustando);	// aguarda o t�rmino do ajuste
	// desliga o watchdog
	WDTCTL = WDTPW + WDTHOLD;
	// restaura a configura��o do timer A e apaga a contagem
	TACTL = TACTL_temp + TACLR;
	// restaura a configura��o de interrup��es anterior
	IE1 = IE1_temp;
}

//*******************************************************************
// trata_wdt
//*******************************************************************
// Entrada: void	Sa�da: void
//*******************************************************************
// Fun��o de tratamento de interrup��o. A cada 64 pulsos de clock do
// ACLK, o watchdog gera esta interrup��o. Ela verifica o n�mero de
// contagens feitas no timer A e caso menor que o valor calculado,
// aumenta a freq��ncia do DCO em um passo.
// Quando a quantidade de pulsos contados� igual ou maior ao valor
// programado, a vari�vel indicadora de ajuste � apagada
//*******************************************************************
#pragma vector=WDT_VECTOR
__interrupt void trata_wdt (void)
{
	unsigned int temp;
	temp = TAR;	// armazena tempor�riamente a contagem do TAR
	TACTL_bit.TACLR = 1;	// apaga a contagem do TAR
	// se a contagem for menor que o valor programado
	if (temp < FCLOCK/512)
	{
		// se a fun��o de aumento da freq��ncia do DCO retornar
		// falso, termina o ajuste
		if (!aumenta_DCO()) clock_result.ajustando = 0;
	// se a contagem for maior ou igual ao o valor programado
	// termina o ajuste
	} else clock_result.ajustando = 0;
}

void main( void )
{
	// o loop principal demonstra o funcionamento ...
	// a freq��ncia do DCO pode ser visualizada atrav�s do pino P5.4
	// (nos chips F13x, F14x, F15x e F16x)
	P5SEL_bit.P5SEL_4 = 1;	// configura o pino P5.4 para a fun��o alternativa
	P5DIR_bit.P5DIR_4 = 1;	// configura o pino P5.4 como sa�da
	// o pino P5.6 pode ser utilizado para medir o tempo de ajuste da freq��ncia
	P5DIR_bit.P5DIR_6 = 1;	// configura o pino P5.4 como sa�da
	P5OUT_bit.P5OUT_6 = 1;	// seta o pino
	ajusta_clock();		// chama a fun��o de ajuste do DCO
	P5OUT_bit.P5OUT_6 = 0;	// desliga o pino
	while(1);
}
