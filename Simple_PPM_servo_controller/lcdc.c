//*******************************************************************
// Biblioteca de funções para manipulação de módulos LCD
//*******************************************************************
// Autor: Fábio Pereira
// Para o livro Microcontroladores MSP430: Teoria e Prática
//*******************************************************************
// Este arquivo contém as funções de acesso e manipulação de módulos
// de display LCD que utilizam o controlador HD44780 ou KS0066.
//*******************************************************************

// variável global que armazena o configuração do display
static char modo_lcd2;	

union ubyte
{
	char _byte;
	struct
	{
		char b0 : 1;
		char b1 : 1;
		char b2 : 1;
		char b3 : 1;
		char b4 : 1;
		char b5 : 1;
		char b6 : 1;
		char b7 : 1;
	} bit;
};

void lcdc_delay_ms(unsigned int tempo)
{
  volatile unsigned int temp;
  for(;tempo;tempo--) for (temp=1000;temp;temp--);
}

void lcdc_envia_nibble(char dado)
{
	union ubyte teste;
	teste._byte = dado;
	// coloca os quatro bits nas saidas
	lcdc_d4 = teste.bit.b0;
	lcdc_d5 = teste.bit.b1;
	lcdc_d6 = teste.bit.b2;
	lcdc_d7 = teste.bit.b3;
	// pulsa a linha enable
	lcdc_enable = 1;
	lcdc_enable = 0;
}

void lcdc_envia_byte(char endereco, char dado )
{
	// coloca a linha rs em 0
	lcdc_rs = 0;
	// configura a linha rs dependendo do modo selecionado
	lcdc_rs = endereco;
	lcdc_delay_ms(1);
	// desativa linha enable
	lcdc_enable = 0;
	// envia a primeira parte do byte
	lcdc_envia_nibble(dado >> 4);
	// envia a segunda parte do byte
	lcdc_envia_nibble(dado & 0x0f);
}

void lcdc_ini(char modo1, char modo2 )

{
	char conta;
	// configura os pinos como saídas
	lcdc_enable_dir = 1;
	lcdc_rs_dir = 1;
	lcdc_d4_dir = 1;
	lcdc_d5_dir = 1;
	lcdc_d6_dir = 1;
	lcdc_d7_dir = 1;
	// coloca os pinos em nível lógico 0
	lcdc_d4 = 0;
	lcdc_d5 = 0;
	lcdc_d6 = 0;
	lcdc_d7 = 0;
	lcdc_rs = 0;
	lcdc_enable = 0;
	lcdc_delay_ms(15);
	// envia uma seqüência de 3 vezes 0x03
	// e depois 0x02 para configurar o módulo
	// para modo de 4 bits
	for(conta=1;conta<=3;++conta)
	{
		
		lcdc_envia_nibble(3);
		lcdc_delay_ms(5);
	}
	lcdc_envia_nibble(2);
	// envia códigos de inicialização do display
	lcdc_envia_byte(0,0x20 | modo1);
	lcdc_envia_byte(0,0x08 | modo2);
	modo_lcd2 = 0x08 | modo2;
	lcdc_envia_byte(0,1);
	lcdc_envia_byte(0,6);
}

void lcdc_posiciona_texto(unsigned char x, unsigned char y)
{
	char endereco;
	if (y) endereco = lcdc_seg_lin; else endereco = 0;
	endereco += x;
	lcdc_envia_byte(0,0x80|endereco);
}

void lcdc_escreve_char(unsigned char c)
// envia um caractere para o display
{
	switch (c)
	{
		case '\f'	:	lcdc_envia_byte(0,1);
								lcdc_delay_ms(5);
								break;
		case '\n'	:
		case '\r' :	lcdc_posiciona_texto(1,2);
								break;
		default	:		lcdc_envia_byte(1,c);
	}
}

void lcdc_escreve_string (unsigned char *c)
{
	while (*c)
	{
		lcdc_escreve_char(*c);
		c++;
	}
}

void lcdc_liga_display(void)
{
	modo_lcd2 |= 4;
	lcdc_envia_byte (0,modo_lcd2);
}

void lcdc_desliga_display(void)
{
	modo_lcd2 &= 0xFB;
	lcdc_envia_byte (0,modo_lcd2);
}

void lcdc_liga_cursor(void)
{
	modo_lcd2 |= 2;
	lcdc_envia_byte (0,modo_lcd2);
}

void lcdc_desliga_cursor(void)
{
	modo_lcd2 &= 0xFD;
	lcdc_envia_byte (0,modo_lcd2);
}

void lcdc_liga_cursor_piscante(void)
{
	modo_lcd2 |= 1;
	lcdc_envia_byte (0,modo_lcd2);
}

void lcdc_desliga_cursor_piscante(void)
{
	modo_lcd2 &= 0xFE;
	lcdc_envia_byte (0,modo_lcd2);
}
