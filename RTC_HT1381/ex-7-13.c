//*******************************************************************
// Demonstração do uso do HT1381
//*******************************************************************
// Autor: Fábio Pereira
// Para o livro Microcontroladores MSP430: Teoria e Prática
//*******************************************************************
// Este programa demonstra a comunicação com o chip HT1381 utilizando
// a USART0 no modo SPI. Foi utilizada a placa Microlab X1 e um módu-
// lo de CPU MSP430F149. A placa já integra o HT1381.
//*******************************************************************

#include <io430x16x.h>
#include <lcdc.h>
#include <intrinsics.h>

#define spi_data_pin_in P3IN_bit.P3IN_2
#define spi_data_pin_out P3OUT_bit.P3OUT_2
#define spi_data_pin_dir P3DIR_bit.P3DIR_2
#define spi_clk_pin P3OUT_bit.P3OUT_3
#define spi_clk_pin_dir P3DIR_bit.P3DIR_3
#define rtc_en_pin P3OUT_bit.P3OUT_7
#define rtc_en_pin_dir P3DIR_bit.P3DIR_7

// comandos do HT1381
#define ht138x_escreve_seg 0x80
#define ht138x_escreve_min 0x82
#define ht138x_escreve_hora 0x84
#define ht138x_escreve_data 0x86
#define ht138x_escreve_mes 0x88
#define ht138x_escreve_dia 0x8A
#define ht138x_escreve_ano 0x8C
#define ht138x_escreve_wp 0x8E
#define ht138x_le_seg 0x81
#define ht138x_le_min 0x83
#define ht138x_le_hora 0x85
#define ht138x_le_data 0x87
#define ht138x_le_mes 0x89
#define ht138x_le_dia 0x8B
#define ht138x_le_ano 0x8D
#define ht138x_le_wp 0x8F
#define ht138x_protegido 0x80
#define ht138x_desprotegido 0x00
#define ht138x_wp 0x01	// proteção contra escrita
#define ht138x_ch 0x02	// parada de clock
#define ht138x_24 0x04	// modo 24 horas

struct ehorario
{
	char hora;
	char minuto;
	char segundo;
} horario;

struct edata
{
	char ano;
	char mes;
	char dia;
	char data;
} calendario;

unsigned char spi_le(unsigned char comando)
{
	unsigned int temp;
	unsigned char result;
	spi_clk_pin = 0;
	spi_data_pin_out = 0;
	spi_data_pin_dir = 1;
	spi_clk_pin_dir = 1;
	for (temp=1;temp<=128;temp*=2)
	{
		if (comando & temp) spi_data_pin_out = 1; else spi_data_pin_out = 0;
		spi_clk_pin = 1;
		spi_clk_pin = 0;
	}
	spi_data_pin_dir = 0;
	result = 0;
	for (temp=1;temp<=128;temp*=2)
	{
		if (spi_data_pin_in) result = (result >> 1) + 128; else result = result >> 1;
		spi_clk_pin = 1;
		spi_clk_pin = 0;
	}
	spi_data_pin_dir = 0;
	spi_clk_pin_dir = 0;
	return(result);
}

//unsigned char spi_escreve(unsigned char comando, unsigned char dado)
void spi_escreve(unsigned char comando, unsigned char dado)
{
	unsigned int temp;
	//unsigned char result;
	spi_clk_pin = 0;
	spi_data_pin_out = 0;
	spi_data_pin_dir = 1;
	spi_clk_pin_dir = 1;
	for (temp=1;temp<=128;temp*=2)
	{
		if (comando & temp) spi_data_pin_out = 1; else spi_data_pin_out = 0;
		spi_clk_pin = 1;
		spi_clk_pin = 0;
	}
	for (temp=1;temp<=128;temp*=2)
	{
		if (dado & temp) spi_data_pin_out = 1; else spi_data_pin_out = 0;
		spi_clk_pin = 1;
		spi_clk_pin = 0;
	}

	spi_data_pin_dir = 0;
	spi_clk_pin_dir = 0;
	//return(result);
}

//*******************************************************************
// usart_spi_ini
//*******************************************************************
// Recebe : void	Retorna: void
//*******************************************************************
// Esta função inicializa os registradores e pinos da USART para o
// modo SPI
//*******************************************************************
void usart_spi_ini()
{
	// Configura os pinos da USART para o modo alternativo
	P3SEL_bit.P3SEL_1 = 1;
	P3SEL_bit.P3SEL_2 = 1;
	P3SEL_bit.P3SEL_3 = 1;
	// Configura a USART
	ME1 = URXE0 + UTXE0;	// habilita o módulo
	// Modo = Síncrono, Master, 8 Bits
	U0CTL = SWRST + CHAR + SYNC + MM;
	// Clock = SMCLK, STE desabilitado, polaridade 1
	U0TCTL = SSEL1 + SSEL0 + STC + CKPH;
	U0MCTL = 0;	// modulador = 0
	U0BR0 = 0x2;	// UCLK = SMCLK / 2
	U0BR1 = 0x00;
	U0CTL_bit.SWRST = 0;	// apaga o reset da USART
}

//*******************************************************************
// espelha_byte
//*******************************************************************
// Recebe : unsigned char valor - o valor a ser espelhado	
// Retorna: unsigned char - o valor espelhado
//*******************************************************************
// Esta função faz a inversão da ordem dos bits do dado recebido
//*******************************************************************
unsigned char espelha_byte(unsigned char valor)
{
	unsigned int val,aux,result;
	aux = 128;
	result = 0;
	for (val=1;val<=128;val*=2)
	{
		if (valor & val) result |= aux;
		aux = aux >> 1;
	}
	return (result);
}

//*******************************************************************
// usart_spi_escreve
//*******************************************************************
// Recebe : 	unsigned char comando - o comando a ser enviado
//						unsigned char dado - o dado a ser enviado
// Retorna: 	void
//*******************************************************************
// Transmite um comando seguido de um dado
//*******************************************************************
void usart_spi_escreve(unsigned char comando, unsigned char dado)
{
	volatile unsigned char temp;
	// configura o pino P3.1 para o modo USART
	P3SEL_bit.P3SEL_1 = 1;
	// envia o comando
	U0TXBUF = espelha_byte(comando);
	while (!U0TCTL_bit.TXEPT);	// aguarda a transmissão
	temp = U0RXBUF;	// lê o dado recebido e descarta
	U0TXBUF = espelha_byte(dado);	// envia o dado
	while (!U0TCTL_bit.TXEPT);	// aguarda a transmissão
	temp = U0RXBUF;	// lê o dado recebido e descarta
}

//*******************************************************************
// usart_spi_le
//*******************************************************************
// Recebe : 	unsigned char comando - o comando a ser enviado
// Retorna: 	unsigned char - o dado lido da SPI
//*******************************************************************
// Transmite um comando e lê o dado recebido
//*******************************************************************
unsigned char usart_spi_le(unsigned char comando)
{
	volatile unsigned char temp;
	// configura o pino P3.1 para o modo USART
	P3SEL_bit.P3SEL_1 = 1;
	// envia o comando
	U0TXBUF = espelha_byte(comando);
	while (!U0TCTL_bit.TXEPT);	// aguarda a transmissão
	temp = U0RXBUF;	// lê o dado recebido e descarta
	// configura o pino P3.1 como entrada
	P3SEL_bit.P3SEL_1 = 0;
	P3DIR_bit.P3DIR_1 = 0;
	// envia um comando, apenas para ativar o clock e poder receber
	// o dado
	U0TXBUF = espelha_byte(comando);	
	while (!U0TCTL_bit.TXEPT);	// aguarda a transmissão
	return(espelha_byte(U0RXBUF));	// retorna o dado recebido
}

//*******************************************************************
// ht138x_escreve_horario
//*******************************************************************
// Recebe : 	struct ehorario horario - estrutura com o horário
// Retorna: 	void
//*******************************************************************
// Escreve um horário no HT138x
//*******************************************************************
void ht138x_escreve_horario(struct ehorario horario)
{
	rtc_en_pin = 1;	// coloca o pino RTC_EN em 1
	// envia comando para desproteger o chip contra escrita
	spi_escreve(ht138x_escreve_wp,ht138x_desprotegido);
	rtc_en_pin = 0;	// desativa a linha RTC_EN
	__no_operation();	// aguarda alguns ciclos
	rtc_en_pin = 1;	// coloca o pino RTC_EN em 1
	// configura os minutos
	usart_spi_escreve(ht138x_escreve_min,horario.minuto);
	rtc_en_pin = 0;	// desativa a linha RTC_EN
	__no_operation();	// aguarda alguns ciclos
	rtc_en_pin = 1;	// coloca o pino RTC_EN em 1
	// escreve as horas
	usart_spi_escreve(ht138x_escreve_hora,horario.hora);
	rtc_en_pin = 0;	// desativa a linha RTC_EN
	__no_operation();	// aguarda alguns ciclos
	rtc_en_pin = 1;	// coloca o pino RTC_EN em 1
	// escreve novamente a hora
	usart_spi_escreve(ht138x_escreve_hora,horario.hora);
	rtc_en_pin = 0;	// desativa a linha RTC_EN
}

//*******************************************************************
// ht138x_escreve_horario
//*******************************************************************
// Recebe : 	void
// Retorna: 	struct ehorario - estrutura com o horário
//*******************************************************************
// Escreve um horário no HT138x
//*******************************************************************
struct ehorario ht138x_le_horario(void)
{
	struct ehorario horario_temp;
	rtc_en_pin = 1;	// coloca o pino RTC_EN em 1
	// lê os minutos
	horario_temp.minuto = usart_spi_le(ht138x_le_min);
	rtc_en_pin = 0;	// desativa a linha RTC_EN
	__no_operation();
	rtc_en_pin = 1;	// coloca o pino RTC_EN em 1
	// lê os minutos
	horario_temp.minuto = usart_spi_le(ht138x_le_min);
	rtc_en_pin = 0;	// desativa a linha RTC_EN
	__no_operation();
	rtc_en_pin = 1;	// coloca o pino RTC_EN em 1
	// lê as horas
	horario_temp.hora = usart_spi_le(ht138x_le_hora);
	rtc_en_pin = 0;	// desativa a linha RTC_EN
	__no_operation();
	rtc_en_pin = 1;	// coloca o pino RTC_EN em 1
	// lê os segundos
	horario_temp.segundo = usart_spi_le(ht138x_le_seg);
	rtc_en_pin = 0;	// desativa a linha RTC_EN
	return (horario_temp);	// retorna a estrutura
}

//*******************************************************************
// ht138x_ini
//*******************************************************************
// Recebe : 	unsigned char estado - estado a ser configurado
// Retorna: 	void
//*******************************************************************
// Configura o HT138x. Podemos utilizar os seguintes símbolos para
// a configuração:
// ht138x_wp - para ativar a proteção contra escrita
// ht138x_ch - para paralisar o clock e colocar o chip em standby
// ht138x_24 - para selecionar o modo 24 horas
// Após a configuração o horário é apagado
//*******************************************************************
void ht138x_ini(unsigned char estado)
{
	char temp;
	rtc_en_pin_dir = 1;	// configura o pino RTC_EN como saída
	rtc_en_pin = 1;	// coloca o pino RTC_EN em 1
	if (estado && ht138x_wp) temp = 0x80; else temp = 0x00;
	usart_spi_escreve(ht138x_escreve_wp,temp);
	rtc_en_pin = 0;	// desativa a linha RTC_EN
	rtc_en_pin = 1;	// coloca o pino RTC_EN em 1
	if (estado && ht138x_ch) temp = 0x80; else temp = 0x00;
	usart_spi_escreve(ht138x_escreve_seg,temp);
	rtc_en_pin = 0;	// desativa a linha RTC_EN
	rtc_en_pin = 1;	// coloca o pino RTC_EN em 1
	if (estado && ht138x_24) temp = 0x00; else temp = 0x80;
	usart_spi_escreve(ht138x_escreve_hora,temp);
	rtc_en_pin = 0;	// desativa a linha RTC_EN
}

void main( void )
{
	unsigned char digito;
	WDTCTL = WDTPW + WDTHOLD;	// desativa o watchdog
	// configura o horário para 09:59:59
	horario.hora = 0x09;
	horario.minuto = 0x59;
	horario.segundo = 0x59;
	usart_spi_ini();	// inicializa a USART para o modo SPI
	ht138x_ini(0);	// inicializa o HT1381
	ht138x_escreve_horario(horario);
	// configura o pino P2.2 como saída e o coloca em nível "0"
  // o pino write do módulo deve ser conectado ao mesmo
	P2DIR_bit.P2DIR_2 = 1;
	P2OUT_bit.P2OUT_2 = 0;
	lcdc_ini(lcdc_display_8x5|lcdc_2_linhas,lcdc_display_ligado|lcdc_cursor_desligado|lcdc_cursor_fixo);
	lcdc_escreve_char ('\f');	// apaga a tela
	lcdc_posiciona_texto(2,0);
	lcdc_escreve_string("Horario");
	while (1)
	{		
		horario = ht138x_le_horario();	// lê o horário do HT1381
		lcdc_posiciona_texto(2,1);
		digito = (horario.hora>>4)+'0';	// separa o MSD da hora
		lcdc_escreve_char(digito);	// escreve o dígito
		digito = (horario.hora & 0x0F)+'0';	// separa o LSD da hora
		lcdc_escreve_char(digito);	// escreve o dígito
		lcdc_escreve_char(':');
		digito = (horario.minuto>>4)+'0';	// separa o MSD do minuto
		lcdc_escreve_char(digito);	// escreve o dígito
		digito = (horario.minuto & 0x0F)+'0';// separa o LSD do minuto
		lcdc_escreve_char(digito);	// escreve o dígito
		lcdc_escreve_char(':');
		digito = (horario.segundo>>4)+'0';	// separa o MSD do segundo
		lcdc_escreve_char(digito);	// escreve o dígito
		digito = (horario.segundo & 0x0F)+'0';// separa o LSD do segundo
		lcdc_escreve_char(digito);	// escreve o dígito
	}
}
