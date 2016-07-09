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

// Inicializa o controlador de LCDs
void lcds_ini(void);
// Escreve no primeiro dígito
void lcds_atualiza_digito_1(const unsigned char n);
// Escreve no segundo dígito
void lcds_atualiza_digito_2(const unsigned char n);
// Escreve no terceiro dígito
void lcds_atualiza_digito_3(const unsigned char n);
// Escreve no quarto dígito
void lcds_atualiza_digito_4(const unsigned char n);
// Apaga o display
void lcds_apaga_display(void);



