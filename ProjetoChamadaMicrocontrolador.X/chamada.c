#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pic18f4520.h>
#include "configura.h"
#include "lcd.h"

#define _XTAL_FREQ 4000000              //define a frequencia que o microcontrolador vai operar
#define botao1 PORTCbits.RC1            //definição da porta usada para o botão 1
#define botao2 PORTCbits.RC2            //definição da porta usada para o botão 2
#define botao3 PORTCbits.RC3            //definição da porta usada para o botão 3
#define botao4 PORTCbits.RC0            //definição da porta usada para o botão 4

#define led_verde PORTBbits.RB0         //definição da porta usada para o led verde
#define led_amarelo PORTBbits.RB5       //definição da porta usada para o led amarelo
#define led_vermelho1 PORTBbits.RB6     //definição da porta usada para o primeiro led vermelho
#define led_vermelho2 PORTBbits.RB7     //definição da porta usada para o segundo led vermelho

//novo tipo de dado, ALUNO
typedef struct Aluno{
    char nome[10];
    int codigo;
    int status;
}Aluno;

//cadastrando aluno1
Aluno aluno1 = {"Jose", 1, 0};

//cadastrando aluno2
Aluno aluno2 = {"Maria", 2, 0};

int aux; //variavel global que armazena o dado a ser enviado via RX/TX

//declaração das funções
void escreve(Aluno aluno);
void envia(void);
void funcao(void);

void main(void){
    config();
    lcd_init();
    
    //zera os ports
    PORTB = 0;
    PORTD = 0;
    PORTE = 0;
    
    while (1){        
        if (!botao3){
            __delay_ms(5);
            if (!botao3){
                while (!botao3);
                LATA5 = !LATA5; //Liga e Desliga o Backlight
            }
        }
        //led verde que demonstra qual aluno foi selecionado
        if(botao4 == 1){
            led_verde = 1;
        }else if(botao4 == 0){
            led_verde = 0;
        }
    }
}

//função de interrupção externa utilizando int1
void __interrupt(high_priority) int1 (void){
    if(INT1IF){    
        __delay_ms(2000);
        led_vermelho1=!led_vermelho1; //led vermelho1 indica quando ocorreu a interrupção
        __delay_ms(500);
        led_vermelho1=!led_vermelho1;
        INT1IF = 0;
        funcao();
    }
}

//função que faz a interpretação de qual dado vai ser enviado via RX/TX
//essa função é chamada toda vez que uma interrupção acontecer
void funcao(){
    if(botao4 == 1){
        __delay_ms(5);
        if (aluno1.status == 0){
            aluno1.status = 1;
            aux = 0x41;
        }
        else if (aluno1.status == 1){
            aluno1.status = 0;
            aux = 0x42;
        }
        escreve(aluno1);
        envia();
    }else if(botao4 == 0){
        __delay_ms(5);
        if (aluno2.status == 0){
            aluno2.status = 1;
            aux = 0x43;
        }
        else if (aluno2.status == 1){
            aluno2.status = 0;
            aux = 0x44;
        }
        escreve(aluno2);
        envia();
    }
}

//função que demonstra a entrada e saida de alunos no LCD
void escreve(Aluno aluno){   
    lcd_clear();
    lcd_goto (0);
    lcd_puts ("Aluno: ");
    lcd_puts (aluno.nome);
    lcd_goto (40);
    lcd_puts ("Status: ");
    if(aluno.status == 1){
        lcd_puts("Entrou");
    }else{
        lcd_puts("Saiu");
    }
    __delay_ms(2000);
    lcd_clear();
}

void envia(void){ //enviar os dados do aluno (código, status "entrou ou saiu") como uma STRING
    while(!TXIF)
           {}
               TXREG = aux;
}