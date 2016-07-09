//*******************************************************************
// Biblioteca de interface para o controlador de LCD dos MSP430
//*******************************************************************
// Autor: Luiz Eduardo Sutter (Dado)
// Para o livro Microcontroladores MSP430: Teoria e Pr�tica
//*******************************************************************
// Este biblioteca � uma implementa��o alternativa para a interface
// com o controlador de LCDs dos microcontroladores MSP430, operando
// no modo est�tico. As defini��es utilizadas s�o v�lidas para o
// display LCD incluso na placa Microlab X1
//*******************************************************************
#include "lcds.h"

#define LCDMEM ((char*) (0x0091))
char *LCD = LCDMEM;	// acesso � mem�ria do LCD como uma matriz C
// Cada linha corresponde aos valores que ser�o movidos para as posi��es
// de mem�ria do controlador de LCD relativas � posi��o do d�gito em quest�o.
// Cada rotina de atualiza��o de um determinado d�gito sabe onde colocar
// seus dados.
// Pode facilmente ser otimizada pra uma tabela s� e pra uma s� fun��o de
// atualiza��o dos d�gitos, se for necess�rio. Tamb�m o acesso �s posi��es 
// via ponteiros permitiria que v�rios bytes da tabela fossem atualizados 
// de uma s� vez na mem�ria do LCD mas o sistema perderia muito em legibi-
// lidade.
// Decodifica��o do primeiro d�gito (unidades)
const char lcds_1[]= 
{
	0x11, 0x11, 0x10, 0x01, 0x00,	// 0
	0x11, 0x00, 0x00, 0x00, 0x00,	// 1
	0x01, 0x11, 0x10, 0x00, 0x10,	// 2
	0x11, 0x01, 0x10, 0x00, 0x10,	// 3
	0x11, 0x00, 0x00, 0x01, 0x10,	// 4
	0x10, 0x01, 0x10, 0x01, 0x10,	// 5
	0x10, 0x11, 0x10, 0x01, 0x10,	// 6
	0x11, 0x00, 0x10, 0x00, 0x00,	// 7
	0x11, 0x11, 0x10, 0x01, 0x10,	// 8
	0x11, 0x00, 0x10, 0x01, 0x10	// 9
};
// Decodifica��o do segundo d�gito (dezenas)
const char lcds_2[]= 
{
	0x11, 0x10, 0x11, 0x00, 0x01,	// 0
	0x01, 0x10, 0x00, 0x00, 0x00,	// 1
	0x10, 0x10, 0x01, 0x10, 0x01,	// 2
	0x11, 0x10, 0x01, 0x10, 0x00,	// 3
	0x01, 0x10, 0x10, 0x10, 0x00,	// 4
	0x11, 0x00, 0x11, 0x10, 0x00,	// 5
	0x11, 0x00, 0x11, 0x10, 0x01,	// 6
	0x01, 0x10, 0x01, 0x00, 0x00,	// 7
	0x11, 0x10, 0x11, 0x10, 0x01,	// 8
	0x01, 0x10, 0x11, 0x10, 0x00	// 9
};
// Decodifica��o do terceiro d�gito (centenas)
const char lcds_3[]= 
{
	0x01, 0x11, 0x10, 0x10, 0x00, 0x01,	// 0
	0x01, 0x01, 0x00, 0x00, 0x00, 0x00,	// 1
	0x00, 0x11, 0x00, 0x10, 0x10, 0x01,	// 2
	0x01, 0x11, 0x00, 0x00, 0x10, 0x01,	// 3
	0x01, 0x01, 0x10, 0x00, 0x10, 0x00,	// 4
	0x01, 0x10, 0x10, 0x00, 0x10, 0x01,	// 5
	0x01, 0x10, 0x10, 0x10, 0x10, 0x01,	// 6
	0x01, 0x11, 0x00, 0x00, 0x00, 0x00,	// 7
	0x01, 0x11, 0x10, 0x10, 0x10, 0x01,	// 8
	0x01, 0x11, 0x10, 0x00, 0x10, 0x00	// 9
};
// Decodifica��o do terceiro d�gito (milhares)
const char lcds_4[]= 
{
	0x10	// 1
};
// Inicializa��o do controlador e do timer b�sico
void lcds_ini (void) 
{
  LCDCTL = LCDSTATIC + LCDP0 + LCDP1 + LCDP2 + LCDON;
  BTCTL = BT_fCLK2_ACLK;
}
// Esta vers�o s� funciona para o layer 0, LCDs est�ticos....
void lcds_atualiza_digito_1(const unsigned char n) 
{
  LCD[0] = lcds_1[n*5];
  LCD[1] = lcds_1[n*5 + 1];
  LCD[2] = lcds_1[n*5 + 2];
  LCD[3] = lcds_1[n*5 + 3];
  LCD[4] = lcds_1[n*5 + 4];
}
void lcds_atualiza_digito_2(const unsigned char n) 
{
  LCD[5] = lcds_2[n*5];
  LCD[6] = lcds_2[n*5 + 1];
  LCD[7] = lcds_2[n*5 + 2];
  LCD[8] = lcds_2[n*5 + 3];
  LCD[9] = lcds_2[n*5 + 4];
}
void lcds_atualiza_digito_3(const unsigned char n) 
{
  LCD[10] = lcds_3[n*6];
  LCD[11] = lcds_3[n*6 + 1];
  LCD[12] = lcds_3[n*6 + 2];
  LCD[13] = lcds_3[n*6 + 3];
  LCD[14] = lcds_3[n*6 + 4];
  if (lcds_3[n*6 + 5])
    LCD[17] |= 0x01  /* LCD100Data[n*6 + 5] */;
  else
    LCD[17] &= 0xFE  /* ~LCD100Data[n*6 + 5] */;
}
void lcds_atualiza_digito_4(const unsigned char n) 
{
  if (n) LCD[17] |= 0x10; else LCD[17] &= 0xEF;
}
void lcds_apaga_display(void) 
{
  unsigned char temp;
  for (temp=0; temp<20; temp++)	LCD[temp] = 0;
}



