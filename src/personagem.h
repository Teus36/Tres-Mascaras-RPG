#ifndef PERSONAGEM_H
#define PERSONAGEM_H
#include "tipos.h"

void criarPersonagem(Personagem *p, Classe classe);
void receberDano(Personagem *p, int dano);
void curarPersonagem(Personagem *p, int quantidade);
void recuperarFoco(Personagem *p, int quantidade);
int usarPocao(Personagem *p);
void ganharXP(Personagem *p, int quantidade);
void aplicarBonusAleatorio(Personagem *p);
void prepararProximaFase(Personagem *p);
int atributoAtaque(const Personagem *p);

#endif
