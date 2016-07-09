//*******************************************************************
// Biblioteca de interface para o controlador de LCD dos MSP430
//*******************************************************************
// Autor: Luiz Eduardo Sutter (Dado)
// Para o livro Microcontroladores MSP430: Teoria e Prática
//*******************************************************************
// Este biblioteca é uma implementação alternativa para a interface
// com o controlador de LCDs dos microcontroladores MSP430, operando
// no modo estático. As definições utilizadas são válidas para o
// display LCD incluso na placa Microlab X1
//*******************************************************************
#include "lcds.h"

#define LCDMEM ((char*) (0x0091))
char *LCD = LCDMEM;	// acesso à memória do LCD como uma matriz C
// Cada linha corresponde aos valores que serão movidos para as posições
// de memória do controlador de LCD relativas à posição do dígito em questão.
// Cada rotina de atualização de um determinado dígito sabe onde colocar
// seus dados.
// Pode facilmente ser otimizada pra uma tabela só e pra uma só função de
// atualização dos dígitos, se for necessário. Também o acesso às posições 
// via ponteiros permitiria que vários bytes da tabela fossem atualizados 
// de uma só vez na memória do LCD mas o sistema perderia muito em legibi-
// lidade.
// Decodificação do primeiro dígito (unidades)
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
// Decodificação do segundo dígito (dezenas)
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
// Decodificação do terceiro dígito (centenas)
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
// Decodificação do terceiro dígito (milhares)
const char lcds_4[]= 
{
	0x10	// 1
};
// Inicialização do controlador e do timer básico
void lcds_ini (void) 
{
  LCDCTL = LCDSTATIC + LCDP0 + LCDP1 + LCDP2 + LCDON;
  BTCTL = BT_fCLK2_ACLK;
}
// Esta versão só funciona para o layer 0, LCDs estáticos....
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



