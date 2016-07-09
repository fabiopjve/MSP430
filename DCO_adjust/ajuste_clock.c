//*******************************************************************
// Ajuste do DCO
//*******************************************************************
// Autor: Fábio Pereira
// Para o livro Microcontroladores MSP430: Teoria e Prática
//*******************************************************************
// Este programa demonstra o ajuste do DCO a partir do oscilador
// LFXT1 operando em baixa freqüência (32768Hz)
// Isto permite construir aplicações que necessitem da medição
// permanente e precisa de tempo (utilizando o cristal de 32768Hz)
// ao mesmo tempo em que, quando necessário, a CPU e periféricos
// podem utilizar um clock de alta freqüência, originado pelo DCO
// O ajuste efetuado por este tipo de programa, garante uma grande
// precisão na seleção da freqüência do DCO
// O funcionamento é simples: utilizamos o watchdog operando como um
// timer, a fonte de clock do mesmo é o ACLK, que neste caso é igual
// a 32768Hz. Programando-se o watchdog para gerar uma interrupção a
// cada 64 pulsos do ACLK, teremos uma base de tempo de referência
// para a medição do sinal gerado pelo DCO.
// Utilizamos também o timer A, que será o responsável pela contagem
// dos pulsos de clock do DCO, para isso, configuramos a fonte de
// clock do timer como sendo o SMCLK e configuramos este clock para
// ser originado do DCO.
// Feito isso, a cada interrupção do watchdog, verificamos a contagem
// do TAR.
// Se quisermos trabalhar com uma frequência de 1MHz, a cada inter-
// rupção do watchdog, o TAR deverá ter feito 1953 contagens. Se o
// número de contagens for maior, quer dizer que a freqüência do DCO
// é maior que 1MHz, se for menor, também menor é a freqüência do
// DCO.
//*******************************************************************

#include <io430x14x.h>
#include <intrinsics.h>

#define FCLOCK 4000000	// freqüência desejada em Hz

#define clock_ok 1

// estrutura utilizada para o controle da função de ajuste de clock
struct sclkadj
{
	char ajustando : 1;
	char err : 1;
} clock_result;	// variável global

// variáveis de ajuste do DCO
unsigned char ajusteMODx, ajusteDCOx, ajusteRSELx;

//*******************************************************************
// aumenta_DCO
//*******************************************************************
// Entrada: void	Saída: unsigned char
//*******************************************************************
// A cada chamada desta função a freqüência do DCO é incrementada em
// um passo. Isto é feito incrementando-se o MODx até atingir 31, ao
// ultrapassar este valor, o DCOx é aumentado até ser maior que 224,
// em seguida, o RSELx é incrementado até 7. Caso a função seja cha-
// mada e necessite incrementar RSELx acima de 7, é retornado o valor
// 0, indicando o erro. Caso o aumento seja bem sucedido, retorna 1
//*******************************************************************
unsigned char aumenta_DCO(void)
{
	unsigned char result = 1;
	// se MODx é menor que 31, incrementa
	if (ajusteMODx<31) ajusteMODx++; else
	{
		// se MODx é maior que 31
		ajusteMODx = 0;	// MODx = 0
		// se DCOx é menor quye 224, aumenta 32
		if (ajusteDCOx<224) ajusteDCOx+=32; else
		{
			// se DCOx é maior que 224
			// apaga DCOx
			ajusteDCOx = 0;
			// se RSELx é menor que 7, incrementa
			if (ajusteRSELx<7) ajusteRSELx++; 
				// senão, o resultado é falso
				else result = 0;
		}
	}
	// configura o DCOCTL de acordo com as variáveis de ajuste
	DCOCTL = ajusteDCOx + ajusteMODx;
	// configura o BCSCTL1 de acordo com a variável de ajuste dos  RSELx
	BCSCTL1 = ajusteRSELx;
	// retorna o resultado
	return (result);
}

void ajusta_clock(void)
{
	unsigned int IE1_temp, TACTL_temp;
	// configura o WDT para o modo temporizador, com clock = ACLK
	// fator de divisão de 64
	WDTCTL = WDTPW + WDTTMSEL + WDTCNTCL + WDTSSEL + WDTIS1 + WDTIS0;
	IFG1_bit.WDTIFG = 0;	// apaga o sinalizador de interrupção do watchdog
	IE1_temp = IE1;		// salva o registrador IE1
	IE1_bit.WDTIE = 1;	// habilita a interrupção do watchdog
	// configura o DCO de acordo com a faixa de freqüência selecionada
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
	// salva a configuração atual do timer A
	TACTL_temp = TACTL;
	// configura o Timer A:
	// modo 2, fonte de clock = SMCLK
	TACTL = TASSEL_2 + MC_2 + TACLR;
	__enable_interrupt();	// habilita interrupções
	// apaga indicador de erro
	clock_result.err = 0;
	// ativa indicador de ajuste em andamento;
	clock_result.ajustando = 1;
	while (clock_result.ajustando);	// aguarda o término do ajuste
	// desliga o watchdog
	WDTCTL = WDTPW + WDTHOLD;
	// restaura a configuração do timer A e apaga a contagem
	TACTL = TACTL_temp + TACLR;
	// restaura a configuração de interrupções anterior
	IE1 = IE1_temp;
}

//*******************************************************************
// trata_wdt
//*******************************************************************
// Entrada: void	Saída: void
//*******************************************************************
// Função de tratamento de interrupção. A cada 64 pulsos de clock do
// ACLK, o watchdog gera esta interrupção. Ela verifica o número de
// contagens feitas no timer A e caso menor que o valor calculado,
// aumenta a freqüência do DCO em um passo.
// Quando a quantidade de pulsos contadosé igual ou maior ao valor
// programado, a variável indicadora de ajuste é apagada
//*******************************************************************
#pragma vector=WDT_VECTOR
__interrupt void trata_wdt (void)
{
	unsigned int temp;
	temp = TAR;	// armazena temporáriamente a contagem do TAR
	TACTL_bit.TACLR = 1;	// apaga a contagem do TAR
	// se a contagem for menor que o valor programado
	if (temp < FCLOCK/512)
	{
		// se a função de aumento da freqüência do DCO retornar
		// falso, termina o ajuste
		if (!aumenta_DCO()) clock_result.ajustando = 0;
	// se a contagem for maior ou igual ao o valor programado
	// termina o ajuste
	} else clock_result.ajustando = 0;
}

void main( void )
{
	// o loop principal demonstra o funcionamento ...
	// a freqüência do DCO pode ser visualizada através do pino P5.4
	// (nos chips F13x, F14x, F15x e F16x)
	P5SEL_bit.P5SEL_4 = 1;	// configura o pino P5.4 para a função alternativa
	P5DIR_bit.P5DIR_4 = 1;	// configura o pino P5.4 como saída
	// o pino P5.6 pode ser utilizado para medir o tempo de ajuste da freqüência
	P5DIR_bit.P5DIR_6 = 1;	// configura o pino P5.4 como saída
	P5OUT_bit.P5OUT_6 = 1;	// seta o pino
	ajusta_clock();		// chama a função de ajuste do DCO
	P5OUT_bit.P5OUT_6 = 0;	// desliga o pino
	while(1);
}
