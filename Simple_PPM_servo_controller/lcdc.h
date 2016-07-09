//*******************************************************************
// Biblioteca de fun��es para manipula��o de m�dulos LCD
//*******************************************************************
// Autor: F�bio Pereira
// Para o livro Microcontroladores MSP430: Teoria e Pr�tica
//*******************************************************************
// Este arquivo cont�m as defini��es e prot�tipos de fun��es de 
// acesso e manipula��o de m�dulos de display LCD que utilizam o
// controlador HD44780 ou KS0066.
//*******************************************************************

// As defini��es a seguir s�o utilizadas para acesso aos pinos do 
// m�dulo
// Para redefinir estes pinos, basta declar�-los no programa, antes
// da inclus�o desta biblioteca
#ifndef lcdc_enable
	#define lcdc_enable 	P2OUT_bit.P2OUT_0	// pino enable do LCD
	#define lcdc_enable_dir P2DIR_bit.P2DIR_0	// dire��o do pino enable
#endif
#ifndef lcdc_rs
	#define lcdc_rs	P2OUT_bit.P2OUT_1	// pino rs do LCD
	#define lcdc_rs_dir	P2DIR_bit.P2DIR_1	// dire��o do pino rs
#endif
#ifndef lcdc_d4
	#define lcdc_d4	P1OUT_bit.P1OUT_4	// pino de dados d4 do LCD
	#define lcdc_d4_dir	P1DIR_bit.P1DIR_4	// dire��o do pino d4
	#define lcdc_d5	P1OUT_bit.P1OUT_5	// pino de dados d5 do LCD
	#define lcdc_d5_dir	P1DIR_bit.P1DIR_5	// dire��o do pino d5
	#define lcdc_d6	P1OUT_bit.P1OUT_6	// pino de dados d6 do LCD
	#define lcdc_d6_dir	P1DIR_bit.P1DIR_6	// dire��o do pino d6
	#define lcdc_d7	P1OUT_bit.P1OUT_7	// pino de dados d7 do LCD
	#define lcdc_d7_dir	P1DIR_bit.P1DIR_7	// dire��o do pino d7
#endif

#define lcdc_seg_lin 0x40    // Endere�o da segunda linha na RAM do LCD

// Defini��es utilizadas para configura��o do display
#define lcdc_cursor_ligado 2
#define lcdc_cursor_desligado 0
#define lcdc_cursor_piscante 1
#define lcdc_cursor_fixo 0
#define lcdc_display_ligado 4
#define lcdc_display_desligado 0
#define lcdc_display_8x5 0
#define lcdc_display_10x5 4
#define lcdc_2_linhas 8
#define lcdc_1_linha 0

//************************************************************************
// Fun��o de envio de um nibble para o display
//************************************************************************
// Argumentos de chamada:
// char dado : dado a ser enviado para o display (somente o nibble
// inferior)
//************************************************************************
// Retorno : nada
//************************************************************************
void lcdc_envia_nibble(char dado);

//************************************************************************
// Fun��o de escrita de 1 byte no display
//************************************************************************
// Argumentos de chamada:
// char endereco : 0 se instru��o, 1 se dado
// char dado : dado ou comando a ser escrito
//************************************************************************
// Retorno : nada
//************************************************************************
void lcdc_envia_byte(char endereco, char dado );

//************************************************************************
// Fun��o de inicializa��o do display
//************************************************************************
// Argumentos de chamada:
// char modo1 : modo do display (n�mero de linhas e tamanho dos caracteres
// char modo2 : modo do display (estado do cursor e do display)
//************************************************************************
// Retorno : nada
//************************************************************************
void lcdc_ini(char modo1, char modo2 );

//************************************************************************
// Fun��o de posicionamento do cursor do display
//************************************************************************
// Argumentos de chamada:
// char x : coluna a ser posicionado o cursor (iniciando de 0)
// char y : linha a ser posicionado o cursor (0 ou 1)
//************************************************************************
// Retorno : nada
//************************************************************************
void lcdc_posiciona_texto(unsigned char x, unsigned char y);

//************************************************************************
// Fun��o de escrita de um caractere no display
//************************************************************************
// Argumentos de chamada :
// char c : caractere a ser escrito
//************************************************************************
// Retorno : nada
//************************************************************************
// Observa��es :
//	\f apaga o conte�do do display
// \n e \r retornam o cursor para a primeira coluna da segunda linha
//************************************************************************
void lcdc_escreve_char(unsigned char c);

//************************************************************************
// Fun��o de escrita de uma string no display
//************************************************************************
// Argumentos de chamada:
// char *c : um ponteiro para um caractere
//************************************************************************
// Retorno : nada
//************************************************************************
void lcdc_escreve_string (unsigned char *c);

//************************************************************************
// Fun��o para ativar o display
//************************************************************************
// Argumentos de chamada : nenhum
// Retorno : nada
//************************************************************************
void lcdc_liga_display(void);

//************************************************************************
// Fun��o para desativar o display
//************************************************************************
// Argumentos de chamada : nenhum
// Retorno : nada
//************************************************************************
void lcdc_desliga_display(void);

//************************************************************************
// Fun��o para ativar o cursor
//************************************************************************
// Argumentos de chamada : nenhum
// Retorno : nada
//************************************************************************
void lcdc_liga_cursor(void);

//************************************************************************
// Fun��o para desativar o cursor
//************************************************************************
// Argumentos de chamada : nenhum
// Retorno : nada
//************************************************************************
void lcdc_desliga_cursor(void);

//************************************************************************
// Fun��o para ativar o cursor piscante
//************************************************************************
// Argumentos de chamada : nenhum
// Retorno : nada
//************************************************************************
void lcdc_liga_cursor_piscante(void);

//************************************************************************
// Fun��o para desativar o cursor piscante
//************************************************************************
// Argumentos de chamada : nenhum
// Retorno : nada
//************************************************************************
void lcdc_desliga_cursor_piscante(void);


