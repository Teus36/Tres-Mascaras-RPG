#ifndef TIPOS_H
#define TIPOS_H

#define TOTAL_FASES 3
#define TOTAL_CASAS 18
#define MONSTROS_POR_FASE 3
#define CASA_RELIQUIA 13
#define CUSTO_HABILIDADE 8
#define CUSTO_IRA 8

typedef enum { MASCARA_ACO, MASCARA_CINZAS, MASCARA_MORTALHA } Classe;
typedef enum {
    CASA_VAZIA, CASA_COMBATE, CASA_TESOURO, CASA_CURA,
    CASA_ARMADILHA, CASA_MISSAO, CASA_CHEFE
} TipoCasa;
typedef enum {
    ACAO_ATACAR = 1, ACAO_DEFENDER, ACAO_HABILIDADE, ACAO_FUGIR,
    ACAO_POCAO
} AcaoCombate;
typedef enum {
    COMBATE_DERROTA, COMBATE_VITORIA, COMBATE_FUGA, COMBATE_SAIR
} ResultadoCombate;
typedef enum { JOGO_DERROTA, JOGO_VITORIA, JOGO_SAIR } ResultadoJogo;

typedef struct {
    char nome[40];
    Classe classe;
    char arma[48], armadura[48], habilidade[48];
    int vida, vidaMax, forca, defesa, agilidade, inteligencia;
    int foco, focoMax, pocoes, nivel, xp, xpTotal, vitorias;
    int bonusArma, bonusArmadura;
} Personagem;

typedef struct {
    char nome[48], descricao[140];
    int vida, vidaMax, ataque, defesa, nivel, xpRecompensa;
    int sangramento;
} Monstro;

typedef struct {
    TipoCasa tipo;
    int resolvida;
} Casa;

typedef struct {
    int numero;
    char nome[48], historia[280], missao[180], reliquia[48];
    Monstro monstros[MONSTROS_POR_FASE];
    Casa casas[TOTAL_CASAS];
    int posicao, reliquiaObtida, chefeDerrotado;
} Fase;

#endif
