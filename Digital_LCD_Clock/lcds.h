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

// Inicializa o controlador de LCDs
void lcds_ini(void);
// Escreve no primeiro d�gito
void lcds_atualiza_digito_1(const unsigned char n);
// Escreve no segundo d�gito
void lcds_atualiza_digito_2(const unsigned char n);
// Escreve no terceiro d�gito
void lcds_atualiza_digito_3(const unsigned char n);
// Escreve no quarto d�gito
void lcds_atualiza_digito_4(const unsigned char n);
// Apaga o display
void lcds_apaga_display(void);



