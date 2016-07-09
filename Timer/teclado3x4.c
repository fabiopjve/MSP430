//*******************************************************************
// Autor: F�bio Pereira
// Para o livro Microcontroladores MSP430: Teoria e Pr�tica
//*******************************************************************
// Fun��o de tratamento do teclado 3x4 da placa Microlab X1. As fun-
// ��es neste arquivo possuem o car�ter did�tico e n�o foram otimiza-
// das para aplica��es reais.
//*******************************************************************

// As defini��es a seguir s�o utilizadas para acesso aos pinos do 
// m�dulo LCD gr�fico
// Para redefinir estes pinos, basta declar�-los no programa, antes
// da inclus�o desta biblioteca
#ifndef tcl_col0
	#define tcl_col0 P3OUT_bit.P3OUT_0
	#define tcl_col0_dir P3DIR_bit.P3DIR_0
	#define tcl_col1 P3OUT_bit.P3OUT_1
	#define tcl_col1_dir P3DIR_bit.P3DIR_1
	#define tcl_col2 P3OUT_bit.P3OUT_2
	#define tcl_col2_dir P3DIR_bit.P3DIR_2
	#define tcl_lin0 P3IN_bit.P3IN_3
	#define tcl_lin0_dir P3DIR_bit.P3DIR_3
	#define tcl_lin1 P3IN_bit.P3IN_4
	#define tcl_lin1_dir P3DIR_bit.P3DIR_4
	#define tcl_lin2 P3IN_bit.P3IN_5
	#define tcl_lin2_dir P3DIR_bit.P3DIR_5
	#define tcl_lin3 P3IN_bit.P3IN_6
	#define tcl_lin3_dir P3DIR_bit.P3DIR_6
#endif
#pragma inline=forced
void teclado_coluna0(void)
{
  tcl_col0 = 0;  tcl_col1 = 1;  tcl_col2 = 1;
}
#pragma inline=forced
void teclado_coluna1(void)
{
  tcl_col0 = 1;  tcl_col1 = 0;  tcl_col2 = 1;
}
#pragma inline=forced
void teclado_coluna2(void)
{
  tcl_col0 = 1;  tcl_col1 = 1;  tcl_col2 = 0;
}
// fun��o de atraso muito simples
void tcl_delay(unsigned int tempo)
{
  unsigned int temp;
  for (temp = tempo; temp; temp--);
}
//************************************************************************
// Fun��o varredura do teclado
//************************************************************************
// Argumentos de chamada: void
//************************************************************************
// Retorno : unsigned char - valor da tecla pressionada (255 para nenhuma)
//************************************************************************
unsigned char varre_teclado(void)
{
	static unsigned char ultima;
	unsigned char tecla;
	// configura as portas utilizadas no teclado
	// colunas s�o sa�das
	tcl_col0_dir = 1;
	tcl_col1_dir = 1;
	tcl_col2_dir = 1;
	// linhas s�o entradas
	tcl_lin0_dir = 0;
	tcl_lin1_dir = 0;
	tcl_lin2_dir = 0;
	tcl_lin3_dir = 0;
	tecla=255;
	teclado_coluna0();	// ativa a coluna 0
	tcl_delay(100);	// aguarda um tempo
	// verifica as linhas
	if (!tcl_lin0) tecla=1;
	if (!tcl_lin1) tecla=4;
	if (!tcl_lin2) tecla=7;
	if (!tcl_lin3) tecla=10;
	teclado_coluna1();	// ativa a coluna 1
	tcl_delay(100);	// aguarda um tempo
	// verifica as linhas
	if (!tcl_lin0) tecla=2;
	if (!tcl_lin1) tecla=5;
	if (!tcl_lin2) tecla=8;
	if (!tcl_lin3) tecla=0;
	teclado_coluna2();	// ativa a coluna 2
	tcl_delay(100);	// aguarda um tempo
	// verifica as linhas
	if (!tcl_lin0) tecla=3;
	if (!tcl_lin1) tecla=6;
	if (!tcl_lin2) tecla=9;
	if (!tcl_lin3) tecla=11;
	// se alguma tecla foi ou est� pressionada
	if (tecla<=11)
	{
		// se a tecla atual for diferente da �ltima
		if (tecla!=ultima)
		{
			ultima = tecla;	// memoriza a tecla atual
			return (tecla);	// e retorna o valor da tecla
		} else
		{
			// se for a mesma tecla que ainda est� pressionada
			ultima = tecla;	// memoriza a tecla
			return (254);	// retorna 254
		}
	}
	// se nenhuma tecla est� pressionada
	ultima = 255;	// memoriza
	return (255);	// retorna 255
}
