#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "personagem.h"
#include "interface.h"

void criarPersonagem(Personagem *p, Classe classe)
{
    memset(p, 0, sizeof *p);
    p->classe = classe;
    strcpy(p->nome, "O Andarilho");
    p->nivel = 1;
    p->pocoes = 2;
    p->bonusArma = 2;
    p->focoMax = 24;
    switch (classe) {
        case MASCARA_ACO:
            p->vidaMax = 140; p->forca = 14; p->defesa = 7;
            p->agilidade = 4; p->inteligencia = 4; p->bonusArmadura = 3;
            strcpy(p->arma, "Montante de Ferro Batido");
            strcpy(p->armadura, "Placas Suplicantes");
            strcpy(p->habilidade, "Ira Ferruginosa");
            break;
        case MASCARA_CINZAS:
            p->vidaMax = 100; p->forca = 4; p->defesa = 4;
            p->agilidade = 7; p->inteligencia = 16; p->bonusArmadura = 2;
            p->focoMax = 32;
            strcpy(p->arma, "Incensário dos Finados");
            strcpy(p->armadura, "Manto Tenebroso");
            strcpy(p->habilidade, "Chama das Lamentações");
            break;
        case MASCARA_MORTALHA:
            p->vidaMax = 115; p->forca = 7; p->defesa = 6;
            p->agilidade = 14; p->inteligencia = 8; p->bonusArmadura = 2;
            strcpy(p->arma, "Besta de Ossos Santos");
            strcpy(p->armadura, "Manto de Sarapilheira");
            strcpy(p->habilidade, "Virote da Condenação");
            break;
    }
    p->vida = p->vidaMax;
    p->foco = p->focoMax;
}

void receberDano(Personagem *p, int dano)
{
    if (dano <= 0) return;
    p->vida = dano >= p->vida ? 0 : p->vida - dano;
}

void curarPersonagem(Personagem *p, int quantidade)
{
    if (quantidade <= 0 || p->vida <= 0) return;
    if (quantidade >= p->vidaMax - p->vida) p->vida = p->vidaMax;
    else p->vida += quantidade;
}

void recuperarFoco(Personagem *p, int quantidade)
{
    if (quantidade <= 0) return;
    if (quantidade >= p->focoMax - p->foco) p->foco = p->focoMax;
    else p->foco += quantidade;
}

int usarPocao(Personagem *p)
{
    int antes = p->vida;
    char mensagem[100];
    if (p->pocoes <= 0 || p->vida == p->vidaMax || p->vida <= 0) return 0;
    p->pocoes--;
    curarPersonagem(p, 35);
    snprintf(mensagem, sizeof mensagem, "Poção: +%d de vida. Restam %d.", p->vida - antes, p->pocoes);
    narrar(mensagem);
    return 1;
}

void ganharXP(Personagem *p, int quantidade)
{
    char mensagem[160];
    if (quantidade <= 0) return;
    p->xp += quantidade;
    p->xpTotal += quantidade;
    while (p->xp >= 40 * p->nivel) {
        p->xp -= 40 * p->nivel;
        p->nivel++;
        p->vidaMax += 12;
        p->forca++; p->defesa++; p->agilidade++; p->inteligencia++;
        p->focoMax += 3;
        curarPersonagem(p, 20);
        recuperarFoco(p, p->focoMax);
        snprintf(mensagem, sizeof mensagem, "NÍVEL %d! Vida máxima +12, atributos +1, foco máximo +3; cura até 20.",
               p->nivel);
        narrar(mensagem);
    }
}

void aplicarBonusAleatorio(Personagem *p)
{
    switch (rand() % 5) {
        case 0: p->forca++; narrar("Recompensa: força +1."); break;
        case 1: p->defesa++; narrar("Recompensa: defesa +1."); break;
        case 2: p->agilidade++; narrar("Recompensa: agilidade +1."); break;
        case 3: p->inteligencia++; narrar("Recompensa: inteligência +1."); break;
        default:
            p->vidaMax += 5;
            curarPersonagem(p, 5);
            narrar("Recompensa: vida máxima +5 e cura até 5.");
            break;
    }
}

void prepararProximaFase(Personagem *p)
{
    curarPersonagem(p, p->vidaMax);
    recuperarFoco(p, p->focoMax);
    if (p->pocoes < 2) p->pocoes = 2;
    narrar("Um abrigo seguro restaura vida e foco e garante ao menos 2 poções.");
}

int atributoAtaque(const Personagem *p)
{
    if (p->classe == MASCARA_CINZAS) return p->inteligencia;
    if (p->classe == MASCARA_MORTALHA) return p->agilidade;
    return p->forca;
}
