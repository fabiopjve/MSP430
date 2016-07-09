//*******************************************************************
// Biblioteca de funções para manipulação de módulos LCD
//*******************************************************************
// Autor: Fábio Pereira
// Para o livro Microcontroladores MSP430: Teoria e Prática
//*******************************************************************
// Este arquivo contém as definições e protótipos de funções de 
// acesso e manipulação de módulos de display LCD que utilizam o
// controlador HD44780 ou KS0066.
//*******************************************************************

// As definições a seguir são utilizadas para acesso aos pinos do 
// módulo
// Para redefinir estes pinos, basta declará-los no programa, antes
// da inclusão desta biblioteca
#ifndef lcdc_enable
	#define lcdc_enable 	P2OUT_bit.P2OUT_0	// pino enable do LCD
	#define lcdc_enable_dir P2DIR_bit.P2DIR_0	// direção do pino enable
#endif
#ifndef lcdc_rs
	#define lcdc_rs	P2OUT_bit.P2OUT_1	// pino rs do LCD
	#define lcdc_rs_dir	P2DIR_bit.P2DIR_1	// direção do pino rs
#endif
#ifndef lcdc_d4
	#define lcdc_d4	P1OUT_bit.P1OUT_4	// pino de dados d4 do LCD
	#define lcdc_d4_dir	P1DIR_bit.P1DIR_4	// direção do pino d4
	#define lcdc_d5	P1OUT_bit.P1OUT_5	// pino de dados d5 do LCD
	#define lcdc_d5_dir	P1DIR_bit.P1DIR_5	// direção do pino d5
	#define lcdc_d6	P1OUT_bit.P1OUT_6	// pino de dados d6 do LCD
	#define lcdc_d6_dir	P1DIR_bit.P1DIR_6	// direção do pino d6
	#define lcdc_d7	P1OUT_bit.P1OUT_7	// pino de dados d7 do LCD
	#define lcdc_d7_dir	P1DIR_bit.P1DIR_7	// direção do pino d7
#endif

#define lcdc_seg_lin 0x40    // Endereço da segunda linha na RAM do LCD

// Definições utilizadas para configuração do display
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
// Função de envio de um nibble para o display
//************************************************************************
// Argumentos de chamada:
// char dado : dado a ser enviado para o display (somente o nibble
// inferior)
//************************************************************************
// Retorno : nada
//************************************************************************
void lcdc_envia_nibble(char dado);

//************************************************************************
// Função de escrita de 1 byte no display
//************************************************************************
// Argumentos de chamada:
// char endereco : 0 se instrução, 1 se dado
// char dado : dado ou comando a ser escrito
//************************************************************************
// Retorno : nada
//************************************************************************
void lcdc_envia_byte(char endereco, char dado );

//************************************************************************
// Função de inicialização do display
//************************************************************************
// Argumentos de chamada:
// char modo1 : modo do display (número de linhas e tamanho dos caracteres
// char modo2 : modo do display (estado do cursor e do display)
//************************************************************************
// Retorno : nada
//************************************************************************
void lcdc_ini(char modo1, char modo2 );

//************************************************************************
// Função de posicionamento do cursor do display
//************************************************************************
// Argumentos de chamada:
// char x : coluna a ser posicionado o cursor (iniciando de 0)
// char y : linha a ser posicionado o cursor (0 ou 1)
//************************************************************************
// Retorno : nada
//************************************************************************
void lcdc_posiciona_texto(unsigned char x, unsigned char y);

//************************************************************************
// Função de escrita de um caractere no display
//************************************************************************
// Argumentos de chamada :
// char c : caractere a ser escrito
//************************************************************************
// Retorno : nada
//************************************************************************
// Observações :
//	\f apaga o conteúdo do display
// \n e \r retornam o cursor para a primeira coluna da segunda linha
//************************************************************************
void lcdc_escreve_char(unsigned char c);

//************************************************************************
// Função de escrita de uma string no display
//************************************************************************
// Argumentos de chamada:
// char *c : um ponteiro para um caractere
//************************************************************************
// Retorno : nada
//************************************************************************
void lcdc_escreve_string (unsigned char *c);

//************************************************************************
// Função para ativar o display
//************************************************************************
// Argumentos de chamada : nenhum
// Retorno : nada
//************************************************************************
void lcdc_liga_display(void);

//************************************************************************
// Função para desativar o display
//************************************************************************
// Argumentos de chamada : nenhum
// Retorno : nada
//************************************************************************
void lcdc_desliga_display(void);

//************************************************************************
// Função para ativar o cursor
//************************************************************************
// Argumentos de chamada : nenhum
// Retorno : nada
//************************************************************************
void lcdc_liga_cursor(void);

//************************************************************************
// Função para desativar o cursor
//************************************************************************
// Argumentos de chamada : nenhum
// Retorno : nada
//************************************************************************
void lcdc_desliga_cursor(void);

//************************************************************************
// Função para ativar o cursor piscante
//************************************************************************
// Argumentos de chamada : nenhum
// Retorno : nada
//************************************************************************
void lcdc_liga_cursor_piscante(void);

//************************************************************************
// Função para desativar o cursor piscante
//************************************************************************
// Argumentos de chamada : nenhum
// Retorno : nada
//************************************************************************
void lcdc_desliga_cursor_piscante(void);


