#ifndef TABULEIRO_H
#define TABULEIRO_H
#include "tipos.h"

void inicializarFase(Fase *fase, int numero);
void moverJogador(Fase *fase, int passos);
int missaoCumprida(const Fase *fase);
ResultadoCombate executarEvento(Personagem *p, Fase *fase);
ResultadoJogo jogarCampanha(Personagem *p);

#endif
