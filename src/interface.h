#ifndef INTERFACE_H
#define INTERFACE_H
#include "tipos.h"

typedef enum {
    RITMO_NORMAL, RITMO_RAPIDO, RITMO_LENTO, RITMO_SEM_PAUSAS
} RitmoNarrativa;

void configurarRitmo(RitmoNarrativa ritmo);
const char *nomeRitmo(void);
int ajustarRitmo(void);
int tempoLeituraMs(const char *texto);
void aguardarLeitura(const char *texto);
void narrar(const char *texto);

/* Retorna -1 se a entrada terminar; descarta uma linha invalida inteira. */
int lerOpcao(const char *mensagem, int minimo, int maximo);
const char *nomeClasse(Classe classe);
void mostrarPersonagem(const Personagem *p);
void mostrarTitulo(void);
void mostrarMenuPrincipal(void);
void mostrarEscolhaMascaras(void);
void mostrarMonstro(const Monstro *m);
void mostrarCombate(const Personagem *p, const Monstro *m);
void mostrarRegras(void);
void mostrarFase(const Fase *fase);
void mostrarTabuleiro(const Fase *fase);
void mostrarExploracao(const Personagem *p, const Fase *fase);
void mostrarRolagem(int dado, int origem, int destino);
void mostrarEvento(TipoCasa tipo);
void mostrarResumo(const Personagem *p, ResultadoJogo resultado);

#endif
