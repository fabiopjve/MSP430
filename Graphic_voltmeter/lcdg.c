//*******************************************************************
// Biblioteca de funções para manipulação de módulos LCD gráficos
//*******************************************************************
// Autor: Fábio Pereira
// Para o livro Microcontroladores MSP430: Teoria e Prática
//*******************************************************************
// Este arquivo contém as funções de acesso e manipulação de módulos 
// de display LCD gráfico que utilizam o controlador T6963C
//*******************************************************************

void lcdg_aguarda_status(unsigned char valor)
{
	unsigned char result = 0;
	while (!result)
	{
		lcdg_dados_dir = 0;	// configura os dados como entradas
   	lcdg_cd = 1;	// linha CD = 1 (modo comando)
   	lcdg_rd = 0;	// seleciona leitura	
   	lcdg_wr = 1;
   	lcdg_ce = 0;	// habilita o controlador
		// lê o status e compara com o valor, se igual, result = 1
		// se diferente, result = 0
		if ((lcdg_dados_in & valor)==valor) result = 1; else result = 0;
		lcdg_ce = 1;	// desabilita o controlador
	}
}

void lcdg_escreve_comando(unsigned char comando)
{
	// aguarda o controlador estar livre (STA1 = STA0 = 1)
	lcdg_aguarda_status(3);
	// coloca o comando na porta de saída
	lcdg_dados_out = comando;
	lcdg_cd = 1;	// modo comando
	lcdg_wr = 0;	// seleciona escrita
	lcdg_rd = 1;
	// configura as linhas de dados como saídas
	lcdg_dados_dir = 0xFF;
	lcdg_ce = 0;	// habilita o controlador
	// o T6963C efetua a leitura do comando nesse momento
	lcdg_ce = 1;	// desabilita o controlador
}

void lcdg_escreve_byte(unsigned char dado)
{
	// aguarda o controlador estar livre (STA1 = STA0 = 1)
	lcdg_aguarda_status(3);
	// carrega a porta de saída com o dado a ser escrito
	lcdg_dados_out = dado;
	lcdg_cd = 0;	// modo de dados
	lcdg_wr = 0;	// seleciona escrita
	lcdg_rd = 1;
	// configura a porta como saída
	lcdg_dados_dir = 0xFF;
	lcdg_ce = 0;	// habilita o controlador
	// o T6963C efetua a leitura do dado nesse momento
	lcdg_ce = 1;	// desabilita o controlador
}

void lcdg_escreve_byte_auto(unsigned char dado)
{
	// aguarda o controlador estar livre (STA3 = 1)
	lcdg_aguarda_status(8);
	// coloca o dado na porta de saída
	lcdg_dados_out = dado;
	lcdg_cd = 0;	// modo de dados
	lcdg_wr = 0;	// seleciona escrita
	lcdg_rd = 1;
	// configura a porta como saída
	lcdg_dados_dir = 0xFF;
	lcdg_ce = 0;	// habilita o controlador
	// o T6963C efetua a leitura do dado nesse momento
	lcdg_ce = 1;	// desabilita o controlador
}

void lcdg_escreve_word(unsigned int dado)
{
	// aguarda o controlador estar livre (STA1 = STA0 = 1)
	lcdg_aguarda_status(3);
	// escreve a parte baixa do dado
	lcdg_escreve_byte(dado);
	// escreve a parte alta do dado
	lcdg_escreve_byte(dado>>8);
}

void lcdg_seta_address_pointer(unsigned int end)
{
	// aguarda o controlador estar livre (STA1 = STA0 = 1)
	lcdg_aguarda_status(3);
	// envia o endereço para o controlador
	lcdg_escreve_word(end);
	// envia o comando
	lcdg_escreve_comando(0x24);
}

void lcdg_apaga_tela_grafica(void)
{
	unsigned int temp;
	// configura o apontador de endereço para o início da área gráfica
	lcdg_seta_address_pointer(lcdg_inicio_area_grafica);
	// seleciona o modo de escrita automática
	lcdg_escreve_comando(lcdg_auto_write_mode);
	// o laço for preenche a área gráfica
	for (temp = (lcdg_bytes_por_linha_grafica * lcdg_pixels_y);temp;temp--)
	{
		lcdg_escreve_byte_auto(0);	// escreve o valor zero
	}
	// desliga o modo auto
	lcdg_escreve_comando(lcdg_desliga_auto_mode);
}

void lcdg_apaga_tela_texto(void)
{
	unsigned int temp;
	// configura o apontador de endereço para o início da área de texto
	lcdg_seta_address_pointer(lcdg_inicio_area_texto);
	// seleciona o modo de escrita automática
	lcdg_escreve_comando(lcdg_auto_write_mode);
	// o laço for preenche a área gráfica
	for (temp = (lcdg_bytes_por_linha_texto * (lcdg_pixels_y>>3));temp;temp--)
	{
		lcdg_escreve_byte_auto(0);	// escreve o valor zero
	}
	// desliga o modo auto
	lcdg_escreve_comando(lcdg_desliga_auto_mode);
}

void lcdg_ini(void)
{
	// configura os pinos de controle como saídas
	lcdg_wr_dir = 1;
	lcdg_rd_dir = 1;
	lcdg_ce_dir = 1;
	lcdg_cd_dir = 1;
	// seleciona a CG-ROM
	lcdg_escreve_comando(0x80);
	// configura o início da área gráfica
	lcdg_escreve_word(lcdg_inicio_area_grafica);
	lcdg_escreve_comando(0x42);
	// configura o número de bytes de uma linha gráfica completa
	lcdg_escreve_byte(lcdg_bytes_por_linha_grafica);
	lcdg_escreve_byte(0);
	lcdg_escreve_comando(0x43);
	// configura o início da área de texto
	lcdg_escreve_word(lcdg_inicio_area_texto);
	lcdg_escreve_comando(0x40);
	// configura o número de bytes de uma linha de texto
	lcdg_escreve_byte(lcdg_bytes_por_linha_texto);
	lcdg_escreve_byte(0);
	lcdg_escreve_comando(0x41);
	// seleciona o modo OU
	lcdg_escreve_comando(0x80);
	// seleciona o modo com texto e gráfico ligados
	lcdg_escreve_comando(0x9C);
	lcdg_apaga_tela_grafica();	// apaga a tela gráfica
	lcdg_apaga_tela_texto();		// apaga a tela de texto
}

void lcdg_seta_modo(unsigned char modo)
{
  lcdg_escreve_comando(modo);
}

void lcdg_posiciona_texto(unsigned char col,unsigned char linha)
{
	lcdg_seta_address_pointer(lcdg_inicio_area_texto + ((int)linha * lcdg_bytes_por_linha_texto) + col);
}

void lcdg_escreve_char(unsigned char dado)
{
	// o conjunto de caracteres do display está deslocado 32 posições
	// em relação à tabela ASCII, por isso, subtraímos este valor do
	// código do caractere a ser escrito
	lcdg_escreve_byte(dado-32);
	// escreve o dado e incrementa o apontador de endereço
	lcdg_escreve_comando(0xC0);
}

void lcdg_escreve_string(unsigned char dado[])
{
	unsigned char temp=0;
	// enquanto não encontra um caractere nulo
	while (dado[temp])
	{
		// escreve o caractere da posição atual da matriz
		lcdg_escreve_char(dado[temp]);
		// incrementa o apontador
		temp++;
	}
}

void lcdg_pixel(unsigned char x, unsigned char y, unsigned char cor)
{
	// a função de plotar um pixel na tela funciona de forma diferente
	// nos modos 6x8 ou 8x8
	// no modo 8x8 cada byte da memória guarda 8 pixels
	#if lcdg_largura_caractere==8
		unsigned char temp,end,temp2;
		// primeiro divide o valor x por 8
		end = x>>1 ;	// divide por dois
		end >>= 2;		// divide por quatro
		// o resultado da divisão é multiplicado por 8
		temp2 = end<<1;	// multiplica por dois
		temp2 <<= 2;		// multiplica por quatro
		// dependendo do pixel estar aceso ou apagado
		// é calculado o valor do comando a ser enviado ao display
		// a ordem dos pixels na memória é inversa ao eixo x:
		// pixel 0,0 = bit 7 do endereço 0
		// pixel 1,0 = bit 6 do endereço 0 e assim por diante
		if (cor) temp = 0xF7-(x-temp2); else temp = 0xFF-(x-temp2);
	#else
		// no modo 6x8, cada byte da memória guarda 6 pixels
		// os bits 7 e 6 são mantidos em zero
		unsigned char temp,end;
		end = x / 6;	// divide o valor de x por 6
		// dependendo do pixel estar aceso ou apagado
		// é calculado o valor do comando a ser enviado ao display
		// a ordem dos pixels na memória é inversa ao eixo x:
		// pixel 0,0 = bit 5 do endereço 0
		// pixel 1,0 = bit 4 do endereço 0 e assim por diante
		if (cor) temp = 0xFD-(x-end*6); else temp = 0xF5-(x-end*6);	
	#endif
	// configura o apontador de endereço do display
	lcdg_seta_address_pointer(lcdg_inicio_area_grafica+end+lcdg_bytes_por_linha_grafica*y);
	// escreve o comando de ligar/desligar pixel
	lcdg_escreve_comando(temp);
}

void lcdg_linha(unsigned char x1, unsigned char y1, unsigned char x2, 
								unsigned char y2, unsigned char color)
{
	// Esta função utiliza o algoritmo de Bresenham para o cálculo
	// da reta
	signed int  addx, addy, dx, dy, erro;
	unsigned char x,y,i;
	// calcula o módulo das distâncias Dx e Dy
	if (x2>=x1) dx=x2-x1; else dx=x1-x2;
	if (y2>=y1) dy=y2-y1; else dy=y1-y2;
	// ponto inicial da linha
	x = x1;
	y = y1;
	if(x1 > x2) addx = -1; else addx = 1;
	if(y1 > y2) addy = -1; else addy = 1;
	if (!dx)	// linha vertical
		if (addy>0) for( i=y1; i <= y2; i++ ) lcdg_pixel(x, i, color);
			else for( i=y2; i <= y1; i++ ) lcdg_pixel(x, i, color);
	else if (!dy)	// linha horizontal
		if (addx>0)	for( i=x1; i <= x2; i++ ) lcdg_pixel(i, y, color);
			else for( i=x2; i <= x1; i++ ) lcdg_pixel(i, y, color);
	else
	// linha inclinada
	if(dx >= dy)
	{
		// calcula o fator de erro
		erro = (dy<<1) - dx;
		for(i=0; i<=dx; ++i)
		{
			lcdg_pixel(x, y, color);	// plota o ponto
			if(erro < 0)
			{
				erro += dy<<1;
				x += addx;
			} else
			{
				erro += (dy - dx)<<1;
				x += addx;
				y += addy;
			}
		}
	}	else
	{
		// calcula o fator de erro
		erro = (dx<<1) - dy;
		for(i=0; i<=dy; ++i)
		{
			lcdg_pixel(x, y, color);	// plota o ponto
			if(erro < 0)
			{
				erro += dx<<1;
				y += addy;
			} else
			{
				erro += (dx - dy)<<1;
				x += addx;
				y += addy;
			}
		}
	}
}

void lcdg_retangulo(unsigned char x0, unsigned char y0, 
					unsigned char x1, unsigned char y1, unsigned char cor)
{
  lcdg_linha(x0,y0,x1,y0,cor);
  lcdg_linha(x1,y0,x1,y1,cor);
  lcdg_linha(x1,y1,x0,y1,cor);
  lcdg_linha(x0,y1,x0,y0,cor);
}

void lcdg_retangulo_preenchido(unsigned char x0, unsigned char y0, 
						unsigned char x1, unsigned char y1, unsigned char cor)
{
  unsigned char linhas;
  if (y0<y1)
  {
    for (linhas=y0;linhas<=y1;linhas++) lcdg_linha(x0,linhas,x1,linhas,cor);
  } else
  {
    for (linhas=y1;linhas<=y0;linhas++) lcdg_linha(x0,linhas,x1,linhas,cor);
  }
}

void lcdg_escreve_tela (const unsigned char *matriz)
{
  unsigned char indx,indy;
  unsigned int indz,inicio;
  // configura o apontador de endereços do display para o início da
  // área gráfica
  //lcdg_seta_address_pointer(lcdg_inicio_area_grafica);
  inicio = lcdg_inicio_area_grafica;
  indz=0;	// zero o apontador da matriz
  for (indy=0;indy<lcdg_pixels_y;indy++)
  {
  	// configura o apontador de endereços para o início da linha
    lcdg_seta_address_pointer(inicio+indy*lcdg_bytes_por_linha_grafica);
    // seleciona o modo de escrita automática
    lcdg_escreve_comando(lcdg_auto_write_mode);
    // plota uma linha completa da matriz
    for (indx = (lcdg_pixels_x/lcdg_largura_caractere); indx; indx--)
    {
      lcdg_escreve_byte_auto(matriz[indz]);
      indz++;	// incrementa o apontador da matriz
    }
    // desliga o modo automático
    lcdg_escreve_comando(lcdg_desliga_auto_mode);
  }
  // restaura o apontador de endereços do display para o início
  // da área gráfica
  lcdg_seta_address_pointer(lcdg_inicio_area_grafica);
}
