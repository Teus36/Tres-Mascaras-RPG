#ifndef COMBATE_H
#define COMBATE_H
#include "tipos.h"

int rolarDado(int lados);
int calcularDano(int ataque, int defesa);
void danoNoMonstro(Monstro *m, int dano);
void atacarMonstro(Personagem *p, Monstro *m);
/* Falha por falta de recursos nao consome turno nem sorteia dados. */
int usarHabilidade(Personagem *p, Monstro *m);
int aplicarSangramento(Monstro *m);
void turnoMonstro(Personagem *p, const Monstro *m, int defendendo);
void recompensarVitoria(Personagem *p, const Monstro *m);
ResultadoCombate combate(Personagem *p, Monstro *m);

#endif
