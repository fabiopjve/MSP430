  /*********************** ScTec 2006 ******************************
  ********************* www.sctec.com.br ***************************
  ******************************************************************
  ********* AN900 - Temporizador com Minutos e Segundos ************
  ********* Utilizando o Teclado Matricial 3 x 4 Teclas ************
  ******************************************************************
    Esta aplica��o demonstra como utilizar o teclado  matricial 3x4,  
  os displays de LED e a sa�da a rel� da Microlab X1.
    Este programa faz um temporizador digital em que o usu�rio esco-
  lhe o modo (vide nota abaixo) e digita o tempo. Utilizando uma CPU
  MSP430F149 com um cristal oscilador externo  de  32,768KHz,  conta
  regressivamente desde o tempo digitado at�  zero,  ent�o  liga  ou 
  desliga o rel� conforme o modo.
  *******************************************************************
   Modos de temporiza��o oferecidos:
   "000" Reset;
   "--1" em Minutos - liga sa�da no fim da contagem do tempo;
   "--2" em Segundos - liga sa�da no fim da contagem do tempo; 
   "--3" em Minutos - desliga sa�da no fim da contagem do tempo;
   "--4" em Segundos - desliga sa�da no fim da contagem do tempo;
   Teclas:
   # - Enter - confirma modo / confirma valor do tempo;
   * - Reset / Mode - muda o modo / reseta temporiza��o;
   0 a 9 - Tempo entre 000 e 999 (em segundos ou minutos, depende  do 
  modo).
  ******************************************************************* 
  **************** Conex�es na Microlab X1: *************************
    Para que o programa funcione corretamente � necess�rio:
  * Retirar o M�dulo LCD de Caracteres (16x2) da placa;
  * Fechar o jumper DIS_ON;
  * Conectar  os  pinos da porta 1 ao barramento de 8  bits  (P1.0  a 
  DB0, P1.1 a DB1 e assim  sucessivamente  at� o bit 7);
  * Conectar o sinal DIS1_EN ao pino P2.0;
  * Conectar o sinal DIS2_EN ao pino P2.1;
  * Conectar o sinal DIS3_EN ao pino P2.2;
  * Conectar o sinal OUT0 ao pino P4.7;
  * Conectar o sinal TCL_COL0 ao pino P3.0; 
  * Conectar o sinal TCL_COL1 ao pino P3.1;
  * Conectar o sinal TCL_COL2 ao pino P3.2; 
  * Conectar o sinal TCL_LIN0 ao pino P3.3;
  * Conectar o sinal TCL_LIN1 ao pino P3.4;
  * Conectar o sinal TCL_LIN2 ao pino P3.5;
  * Conectar o sinal TCL_LIN3 ao pino P3.6;
    Caso ainda existirem d�vidas, consulte o manual da Microlab X1.
  *******************************************************************/  

#include <io430x14x.h>
#include <intrinsics.h>
#include "teclado3x4.c"    // Biblioteca de manipula��o do teclado matricial

// Constantes que definem os d�gitos (0 1 2 3 4 5 6 7 8 9) 
const char numero[] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x67};

// Define caractere -
#define caractere_ 0x40

// Constantes que definem os displays 
#define disp_1 1
#define disp_2 2
#define disp_3 3

// Outras constantes
#define min 60
#define seg 1
#define saida P4OUT_bit.P4OUT_7
     
// Vari�veis utilizadas em v�rias partes do programa
unsigned char digito_1, digito_2, digito_3;
// enter - tecla #, reset_mode - tecla *, tecla - guarda leitura do teclado, 
// t_on - energiza na temporiza��o, t_off - desenergiza na temporiza��o
unsigned char enter, reset_mode, tecla, t_on, t_off;
unsigned int resol = 1;     // Guarda resolu��o em segundos ou minutos
unsigned int tempo = 0;     // Vari�vel que guarda o valor do tempo em segundos
unsigned char mode = 0;     // Modos --1, --2, --3, --4 e reset "000"

// Prot�tipos de fun��es
void inicializa(void);   
void menu(void);
unsigned int concatena_digitos(void);
void separa_digitos(unsigned int numero);
void mostra_disp(unsigned char dg, unsigned char disp);
void atraso(unsigned int temp);

// Fun��o de tratamento de interrup��o do Timer A
#pragma vector = TIMERA1_VECTOR
__interrupt void trata_timer(void)
{
  switch(TAIV)
    case 0x0A :                   // Caso haja estouro de contagem:
    {
    if(tempo > 0)                 // Se temporiza��o iniciada:
    {
      tempo--;                    // Decrementa tempo
      if(tempo == 0)              // Se zerar:
      {
        if(!t_off)t_on = 1;       // Liga t_on ou desliga t_off        
        else {t_off = 0; t_on = 0;} 
        digito_1 = digito_2 = digito_3 = 0;
      }
    }
  }  
}

void main(void)
{
  inicializa();
  digito_1 = digito_2 = digito_3 = 0;
  enter = reset_mode = t_on = t_off = 0;
  while(1)
  {
    menu();                     // Entra nas configura��es
    if(tempo)                   // Se temporiza��o iniciada:
    {                           // Mostra tempo restante nos displays em 
      switch(resol)             // minutos ou segundos
      {
        case min :              
        {
          if((tempo / 60 ) < 1) resol = seg;
          digito_1 = digito_2 = digito_3 = 0;
          separa_digitos(tempo/60); break;
        }
        case seg : 
        {
          digito_1 = digito_2 = digito_3 = 0;
          separa_digitos(tempo); break;
        }
      }
    }
    if(t_on)  saida = 1;                // Coloca sa�da em "1"
    else if(t_off && tempo > 0) saida = 1;
    if(!t_on && !t_off) saida = 0;      // Coloca sa�da em "0"
    mostra_disp(digito_1, disp_1);      // Apresenta primeiro digito e aguarda um tempo
    atraso(500);
    mostra_disp(digito_2, disp_2);      // Apresenta segundo digito e aguarda um tempo
    atraso(500);
    mostra_disp(digito_3, disp_3);      // Apresenta terceiro digito e aguarda um tempo
    atraso(500);
  }
}


void inicializa(void)
{
  WDTCTL = WDTPW + WDTHOLD;          // Desativa o watchdog
  P1DIR = 255;                       // Configura todos os pinos da porta 1 como sa�da
  P2DIR = 255;                       // Configura todos os pinos da porta 2 como sa�da
  P4DIR = 255;                       // Configura todos os pinos da porta 4 como sa�da
  P2OUT = 0;                         // Apaga os bits da porta 2;
  P4OUT = 0;                         // Apaga os bits da porta 4; 
  // Configura DCO para 4.13MHz
  DCOCTL = DCO2 + MOD4 + MOD3 + MOD2; 
  // MCLK = SMCLK = 4.13MHz, ativa LFXT1 em baixa frequ�ncia - clock dividido por 1
  BCSCTL1 = XT2OFF + RSEL2 + RSEL1 + RSEL0 + DIVA_0;
  BCSCTL2 = 0x00;
  // Configura Timer A, TASSEL_1 - clock auxiliar ACLK, ID_0 - clock dividido por 1
  // MC_1 - contagem at� o valor de TACCR0, TAIE - habilita interrup��o por estouro do TA
  TACTL = TASSEL_1 + ID_0 + MC_1 + TAIE; 
  // M�dulo de contagem = frequ�ncia do cristal = 1 interrup��o por segundo
  TACCR0 = 32767;                     
  __enable_interrupt();               // Habilita interrup��es          
}

// Fun��o que mostra menu, escolhe um dos 4 modos de temporiza��o e define valor do tempo
void menu(void)
{
  tecla = varre_teclado();          // Faz varredura de teclado
   if(tecla == 254) return;         // A mesma tecla est� pressionada
   if(tecla == 255) return;         // Nenhuma tecla pressionada
   if(tecla == 10)                  // Se foi pressionada a tecla "*" (reset_mode):
    {
      switch(mode)
      {
        case 0 :                    
        {
          digito_2 = digito_3 = 10; // Apresenta "--1" modo: minutos
          digito_1 = 1;
          mode = 1;
        } break;
        case 1 :                    
        {
          digito_2 = digito_3 = 10; // Apresenta "--2" modo: segundos
          digito_1 = 2;               
          mode = 2;
        } break;
        case 2 :                    
        {
          digito_2 = digito_3 = 10; // Apresenta "--3" modo: minutos
          digito_1 = 3;
          mode = 3;
        } break;
        case 3 :                    
        {
          digito_2 = digito_3 = 10; // Apresenta "--4" modo: segundos
          digito_1 = 4;               
          mode = 4;
        } break;
        case 4 : 
        {
          digito_1 = digito_2 = digito_3 = 0; // Apresenta "000" 
          mode = 0;
        } break;          
        case 10 :                    // Caso esteja em temporiza��o, reseta tudo
        {
          digito_1 = digito_2 = digito_3 = 0;
          mode = t_on = tempo = 0;
          saida = 0;
        } break;
      }
    }else
    if(tecla == 11)                 // Se for pressionado a tecla "#" enter: 
    {
      switch(mode)
      {
        case 1 :                    // Em modo 1 (minutos), tempo x 60s : 
        {                           // liga no fim da contagem do tempo
          resol = min;
          digito_1 = digito_2 = digito_3 = 0;
          t_on = 0;
          t_off = 0;
        } break;
        case 2 :                    // Em modo 2 tempo em segundos:
        {                           // liga no fim da contagem do tempo  
          resol = seg;
          digito_1 = digito_2 = digito_3 = 0;
          t_on = 0;
          t_off = 0;
        } break;
        case 3 :                    // Em modo 3 (minutos), tempo x 60s :
        {                           // desliga no fim da contagem do tempo
          resol = min;
          digito_1 = digito_2 = digito_3 = 0;
          t_on = 0;
          t_off = 1;
        } break;
        case 4 :                    // Em modo 4 tempo em segundos:
        {                           // desliga no fim da contagem do tempo
          resol = seg;
          digito_1 = digito_2 = digito_3 = 0;
          t_on = 0;
          t_off = 1;
        } break;
        case 10 :                   // Inicia contagem do tempo
        {
          tempo = concatena_digitos();
          tempo = tempo * resol;
        }
      }
    }else
    if(mode)                      // Se j� estiver em algum dos modos e 
    {                             // for pressionado uma tecla entre 0 e 9:
      if(digito_3 > 0) return;    // Retorna se j� foram digitados os 3 d�gitos
      digito_3 = digito_2;        // Faz o cursor mudar para pr�ximo d�gito
      digito_2 = digito_1;      
      digito_1 = tecla;           // Atualiza �ltimo d�gito teclado
      mode = 10;                  // Autoriza mas n�o inicia contagem do tempo
    }
}  

// Fun��o que concatena os 3 d�gitos formando um �nico n�mero
unsigned int concatena_digitos(void)
{
    unsigned int numero = 0;
    numero = digito_1;
    numero = (numero + (digito_2 * 10));
    numero = (numero + (digito_3 * 100));
    return (numero);
}

// Fun��o que separa os d�gitos do n�mero de contagem
// Argumento de chamada: numero entre 0 e 999
void separa_digitos(unsigned int numero)
{
    unsigned int b = 0; 
    if(numero < 1000) 
    {      
      for(b=99; b < numero; )           // Separa d�gito da centena
      {
        numero = numero - 100;
        digito_3++;
      } 
      for(b=9; b < numero; )            // Separa d�gito da dezena
      {
        numero = numero - 10;
        digito_2++;
      } 
      if(numero < 10) digito_1 = numero;// Separa d�gito da unidade
    }
}

     
// Fun��o que apresenta nos displays os d�gitos
// Argumentos de chamada: valor do d�gito, qual display
void mostra_disp(unsigned char dg, unsigned char disp)
{
    P1OUT = 0;
    switch(disp)                      // Define qual display ligar
    {
      case disp_1 : P2OUT = 4; break;
      case disp_2 : P2OUT = 2; break;
      case disp_3 : P2OUT = 1; break; 
    }  
    switch(dg)                        // Define qual d�gito mostrar 
    {      
      case 0 :  P1OUT = numero[0]; break;
      case 1 :  P1OUT = numero[1]; break;
      case 2 :  P1OUT = numero[2]; break; 
      case 3 :  P1OUT = numero[3]; break; 
      case 4 :  P1OUT = numero[4]; break; 
      case 5 :  P1OUT = numero[5]; break; 
      case 6 :  P1OUT = numero[6]; break; 
      case 7 :  P1OUT = numero[7]; break; 
      case 8 :  P1OUT = numero[8]; break; 
      case 9 :  P1OUT = numero[9]; break;
      case 10 : P1OUT = caractere_; break;
   }     
}

void atraso(unsigned int temp)       // Fun��o para delay
{
  unsigned int t = 0;
  for(t = 0; t < temp; t ++);
  for(t = 0; t < temp; t ++);
}

