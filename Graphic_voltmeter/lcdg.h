//*******************************************************************
// Biblioteca de fun��es para manipula��o de m�dulos LCD gr�ficos
//*******************************************************************
// Autor: F�bio Pereira
// Para o livro Microcontroladores MSP430: Teoria e Pr�tica
//*******************************************************************
// Este arquivo cont�m as defini��es e prot�tipos de fun��es de 
// acesso e manipula��o de m�dulos de display LCD gr�fico que utili-
// zam o controlador T6963C
//*******************************************************************

// As defini��es a seguir s�o utilizadas para acesso aos pinos do 
// m�dulo LCD gr�fico
// Para redefinir estes pinos, basta declar�-los no programa, antes
// da inclus�o desta biblioteca
#ifndef lcdg_cd
	#define lcdg_cd P2OUT_bit.P2OUT_1
	#define lcdg_cd_dir P2DIR_bit.P2DIR_1
#endif
#ifndef lcdg_rd
	#define lcdg_rd P2OUT_bit.P2OUT_3
	#define lcdg_rd_dir P2DIR_bit.P2DIR_3
#endif
#ifndef lcdg_wr
	#define lcdg_wr P2OUT_bit.P2OUT_2
	#define lcdg_wr_dir P2DIR_bit.P2DIR_2
#endif
#ifndef lcdg_ce
	#define lcdg_ce P2OUT_bit.P2OUT_0
	#define lcdg_ce_dir P2DIR_bit.P2DIR_0
#endif
#ifndef lcdg_dados_out
	#define lcdg_dados_dir P1DIR
	#define lcdg_dados_out P1OUT
	#define lcdg_dados_in P1IN
#endif

// defini��es do display gr�fico
#define lcdg_largura_caractere 6		// 6 = 6x8  - 8 = 8x8
#define lcdg_pixels_x 240		// largura do display em pixels
#define lcdg_pixels_y 128		// altura do display em pixels
#define lcdg_memoria_ram 8192	// capacidade de mem�ria RAM
#define lcdg_inicio_area_grafica 0
#define lcdg_bytes_por_linha_grafica lcdg_pixels_x/lcdg_largura_caractere
#define lcdg_bytes_por_linha_texto lcdg_bytes_por_linha_grafica
#define lcdg_inicio_area_texto lcdg_memoria_ram-((lcdg_pixels_y/8)*lcdg_bytes_por_linha_texto)
#define lcdg_auto_write_mode 0xB0
#define lcdg_auto_read_mode 0xB1
#define lcdg_desliga_auto_mode 0xB2
#define lcdg_modo_OR 0x80
#define lcdg_modo_XOR 0x81
#define lcdg_modo_AND 0x82


//*******************************************************************
// lcdg_aguarda_status
//*******************************************************************
// Entrada:	unsigned char valor - valor de estado a ser comparado
// Retorna:	void
//*******************************************************************
// Aguarda que os bits de status especificados sejam apagados.
//*******************************************************************
void lcdg_aguarda_status(unsigned char valor);

//*******************************************************************
// lcdg_escreve_comando
//*******************************************************************
// Entrada:	unsigned char comando - comando a ser enviado
// Retorna:	void
//*******************************************************************
// Envia um comando para o display
//*******************************************************************
void lcdg_escreve_comando(unsigned char comando);

//*******************************************************************
// lcdg_escreve_byte
//*******************************************************************
// Entrada:	unsigned char dado - informa��o a ser enviada para o LCD
// Retorna:	void
//*******************************************************************
// Envia um dado para o display
//*******************************************************************
void lcdg_escreve_byte(unsigned char dado);

//*******************************************************************
// lcdg_escreve_byte_auto
//*******************************************************************
// Entrada:	unsigned char dado - informa��o a ser enviada para o LCD
// Retorna:	void
//*******************************************************************
// Envia um dado para o display no modo autom�tico (transfer�ncia de 
// um bloco de dados)
//*******************************************************************
void lcdg_escreve_byte_auto(unsigned char dado);

//*******************************************************************
// lcdg_escreve_word
//*******************************************************************
// Entrada:	unsigned int dado - informa��o a ser enviada para o LCD
// Retorna:	void
//*******************************************************************
// Envia um dado de 16 bits para o display
//*******************************************************************
void lcdg_escreve_word(unsigned int dado);

//*******************************************************************
// lcdg_seta_address_pointer
//*******************************************************************
// Entrada:	unsigned int end - configura o apontador de endere�os do
//														display
// Retorna:	void
//*******************************************************************
// Envia um dado de 16 bits para o display
//*******************************************************************
void lcdg_seta_address_pointer(unsigned int end);

//*******************************************************************
// lcdg_apaga_tela_grafica
//*******************************************************************
// Entrada:	void
// Retorna:	void
//*******************************************************************
// Apaga a informa��o da �rea gr�fica do display, preenchendo toda a
// �rea gr�fico com o valor 0, utilizando o modo autom�tico
//*******************************************************************
void lcdg_apaga_tela_grafica(void);

//*******************************************************************
// lcdg_apaga_tela_texto
//*******************************************************************
// Entrada:	void
// Retorna:	void
//*******************************************************************
// Apaga a informa��o da �rea de texto do display, preenchendo toda a
// �rea de texto com zero, utilizando o modo autom�tico
//*******************************************************************
void lcdg_apaga_tela_texto(void);

//*******************************************************************
// lcdg_ini
//*******************************************************************
// Entrada:	void
// Retorna:	void
//*******************************************************************
// Inicializa o display
//*******************************************************************
void lcdg_ini(void);

//*******************************************************************
// lcdg_seta_modo
//*******************************************************************
// Entrada:	unsigned char modo - modo do display
// Retorna:	void
//*******************************************************************
// Seleciona um dos modos de intera��o entre a �rea gr�fica e a �rea
// de texto: 
// OR 	- opera��o OR entre os pontos gr�ficos e de texto
// XOR 	- opera��o XOR entre os pontos gr�ficos e de texto
// AND 	- opera��o AND entre os pontos gr�ficos e de texto
//*******************************************************************
void lcdg_seta_modo(unsigned char modo);

//*******************************************************************
// lcdg_posiciona_texto
//*******************************************************************
// Entrada:	unsigned char col - coluna da tela de texto
//					unsigned char linha - linha da tela de texto
// Retorna:	void
//*******************************************************************
// Posiciona o cursor para a escrita de caracteres no display
//*******************************************************************
void lcdg_posiciona_texto(unsigned char col,unsigned char linha);

//*******************************************************************
// lcdg_escreve_char
//*******************************************************************
// Entrada:	unsigned char dado - caractere a ser escrito
// Retorna:	void
//*******************************************************************
// Escreve um caractere na �rea indicada pelo cursor
//*******************************************************************
void lcdg_escreve_char(unsigned char dado);

//*******************************************************************
// lcdg_escreve_string
//*******************************************************************
// Entrada:	unsigned char dado[] - ponteiro para uma string de carac-
//															teres terminada por nulo
// Retorna:	void
//*******************************************************************
// Escreve uma string de caracteres no local apontado pelo cursor
//*******************************************************************
void lcdg_escreve_string(unsigned char dado[]);

//*******************************************************************
// lcdg_pixel
//*******************************************************************
// Entrada:	unsigned char x - coluna da tela gr�fica
//					unsigned char y - linha da tela gr�fica
//					unsigned char cor - 0 - apagado, 1 - aceso
// Retorna:	void
//*******************************************************************
// Liga ou desliga um pixel na tela gr�fica
//*******************************************************************
void lcdg_pixel(unsigned char x, unsigned char y, unsigned char cor);

//*******************************************************************
// lcdg_linha
//*******************************************************************
// Entrada:	unsigned char x1 - coluna inicial
//					unsigned char y1 - linha inicial
//					unsigned char x2 - coluna final
//					unsigned char y2 - linha final
//					unsigned char cor - 0 - apagado, 1 - aceso
// Retorna:	void
//*******************************************************************
// Liga ou desliga um pixel na tela gr�fica
//*******************************************************************
void lcdg_linha(unsigned char x1, unsigned char y1, unsigned char x2, 
								unsigned char y2, unsigned char color);

//*******************************************************************
// lcdg_retangulo
//*******************************************************************
// Entrada:	unsigned char x1 - coluna inicial
//					unsigned char y1 - linha inicial
//					unsigned char x2 - coluna final
//					unsigned char y2 - linha final
//					unsigned char cor - 0 - apagado, 1 - aceso
// Retorna:	void
//*******************************************************************
// Desenha um quadrado/ret�ngulo na tela
//*******************************************************************
void lcdg_retangulo(unsigned char x0, unsigned char y0, 
							unsigned char x1, unsigned char y1, unsigned char cor);

//*******************************************************************
// lcdg_retangulo_preenchido
//*******************************************************************
// Entrada:	unsigned char x1 - coluna inicial
//					unsigned char y1 - linha inicial
//					unsigned char x2 - coluna final
//					unsigned char y2 - linha final
//					unsigned char cor - 0 - apagado, 1 - aceso
// Retorna:	void
//*******************************************************************
// Desenha um quadrado/ret�ngulo totalmente preenchido na tela
//*******************************************************************
void lcdg_retangulo_preenchido(unsigned char x0, unsigned char y0, 
						unsigned char x1, unsigned char y1, unsigned char cor);

//*******************************************************************
// lcdg_escreve_tela
//*******************************************************************
// Entrada:	unsigned char *matriz - ponteiro para a matriz com os
//																	elementos da tela
// Retorna:	void
//*******************************************************************
// Copia uma matriz da mem�ria para a tela do display
//*******************************************************************
void lcdg_escreve_tela (const unsigned char *matriz);

#include <lcdg.c>
