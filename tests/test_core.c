#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tipos.h"
#include "personagem.h"
#include "combate.h"
#include "tabuleiro.h"
#include "interface.h"

static int verificacoes = 0;

#define VERIFICAR(condicao) do { \
    verificacoes++; \
    if (!(condicao)) { \
        printf("FALHA: %s, linha %d\n", #condicao, __LINE__); \
        exit(EXIT_FAILURE); \
    } \
} while (0)

static Monstro alvo(void)
{
    Monstro m;
    memset(&m, 0, sizeof m);
    strcpy(m.nome, "Alvo de teste");
    m.vida = m.vidaMax = 500;
    m.ataque = 12;
    m.defesa = 10;
    m.xpRecompensa = 20;
    return m;
}

static void testarPersonagens(void)
{
    Personagem p;
    int classe;
    for (classe = 0; classe < 3; classe++) {
        criarPersonagem(&p, (Classe)classe);
        VERIFICAR(p.vida == p.vidaMax && p.vida > 0);
        VERIFICAR(p.foco == p.focoMax && p.pocoes == 2);
        VERIFICAR(p.arma[0] && p.armadura[0] && p.habilidade[0]);
        VERIFICAR(p.nivel == 1 && p.xp == 0 && p.vitorias == 0);
        VERIFICAR(atributoAtaque(&p) == (classe == 0 ? p.forca : classe == 1 ? p.inteligencia : p.agilidade));
    }
    criarPersonagem(&p, MASCARA_ACO);
    receberDano(&p, -10);
    VERIFICAR(p.vida == 140);
    receberDano(&p, 40);
    curarPersonagem(&p, 99999);
    VERIFICAR(p.vida == p.vidaMax);
    VERIFICAR(!usarPocao(&p) && p.pocoes == 2);
    receberDano(&p, 50);
    VERIFICAR(usarPocao(&p) && p.vida == 125 && p.pocoes == 1);
    p.foco = 0;
    recuperarFoco(&p, 99999);
    VERIFICAR(p.foco == p.focoMax);
    receberDano(&p, 99999);
    curarPersonagem(&p, 50);
    VERIFICAR(p.vida == 0 && !usarPocao(&p));
}

static void testarEvolucao(void)
{
    Personagem p;
    int i;
    criarPersonagem(&p, MASCARA_ACO);
    p.vida = 30;
    ganharXP(&p, 125);
    VERIFICAR(p.nivel == 3 && p.xp == 5 && p.xpTotal == 125);
    VERIFICAR(p.vidaMax == 164 && p.vida == 70);
    VERIFICAR(p.forca == 16 && p.defesa == 9 && p.focoMax == 30);
    VERIFICAR(p.foco == p.focoMax);
    for (i = 0; i < 100; i++) {
        Personagem antes = p;
        int ganhos;
        aplicarBonusAleatorio(&p);
        ganhos = (p.forca - antes.forca) + (p.defesa - antes.defesa) +
                 (p.agilidade - antes.agilidade) + (p.inteligencia - antes.inteligencia) +
                 (p.vidaMax - antes.vidaMax) / 5;
        VERIFICAR(ganhos == 1 && p.xp == antes.xp && p.nivel == antes.nivel);
    }
    p.vida = 1; p.foco = 0; p.pocoes = 0;
    prepararProximaFase(&p);
    VERIFICAR(p.vida == p.vidaMax && p.foco == p.focoMax && p.pocoes == 2);
    p.pocoes = 5;
    prepararProximaFase(&p);
    VERIFICAR(p.pocoes == 5);
}

static void testarCombate(void)
{
    Personagem p, antes;
    Monstro m, n;
    int i, dado, dano, vida;
    VERIFICAR(calcularDano(2, 99) == 1);
    VERIFICAR(calcularDano(26, 3) == 23);
    VERIFICAR(rolarDado(0) == 0);
    for (i = 0; i < 500; i++) {
        dado = rolarDado(6);
        VERIFICAR(dado >= 1 && dado <= 6);
    }
    m = alvo(); danoNoMonstro(&m, 99999);
    VERIFICAR(m.vida == 0);

    criarPersonagem(&p, MASCARA_ACO); m = alvo();
    srand(7); dado = rolarDado(20); srand(7);
    dano = calcularDano(dado + p.forca + p.bonusArma, m.defesa);
    if (dado == 20) dano *= 2;
    atacarMonstro(&p, &m);
    VERIFICAR(m.vida == m.vidaMax - dano);

    antes = p; m = alvo();
    VERIFICAR(usarHabilidade(&p, &m));
    VERIFICAR(p.vida == antes.vida - 8 && p.foco == antes.foco - 8);
    VERIFICAR(m.vida < m.vidaMax);
    p.vida = 8; antes = p; m = alvo();
    VERIFICAR(!usarHabilidade(&p, &m));
    VERIFICAR(p.foco == antes.foco && p.vida == antes.vida && m.vida == m.vidaMax);
    p.vida = 140; p.foco = 7; m = alvo();
    VERIFICAR(!usarHabilidade(&p, &m) && p.foco == 7 && m.vida == m.vidaMax);

    criarPersonagem(&p, MASCARA_CINZAS); m = alvo(); n = m;
    n.defesa = 999;
    srand(12); VERIFICAR(usarHabilidade(&p, &m));
    criarPersonagem(&p, MASCARA_CINZAS);
    srand(12); VERIFICAR(usarHabilidade(&p, &n));
    VERIFICAR(m.vida == n.vida);

    criarPersonagem(&p, MASCARA_MORTALHA); m = alvo();
    srand(17); dado = rolarDado(20); srand(17);
    dano = 2 * calcularDano(dado + p.agilidade + p.bonusArma, m.defesa);
    VERIFICAR(usarHabilidade(&p, &m));
    VERIFICAR(m.sangramento == 3 && m.vida == m.vidaMax - dano);
    vida = m.vida;
    for (i = 0; i < 4; i++) aplicarSangramento(&m);
    VERIFICAR(m.vida == vida - 12 && m.sangramento == 0);
    m.vida = 3; m.sangramento = 1;
    VERIFICAR(aplicarSangramento(&m) && m.vida == 0);

    criarPersonagem(&p, MASCARA_ACO); m = alvo();
    srand(5); turnoMonstro(&p, &m, 0);
    dano = p.vidaMax - p.vida;
    p.vida = p.vidaMax;
    srand(5); turnoMonstro(&p, &m, 1);
    VERIFICAR(p.vidaMax - p.vida == (dano + 1) / 2);
    criarPersonagem(&p, MASCARA_ACO);
    recompensarVitoria(&p, &m);
    VERIFICAR(p.vitorias == 1 && p.xpTotal == 20);
}

static void testarFases(void)
{
    Fase fase, anterior;
    int n, i, posicao, passo;
    const int passos[] = {0, 1, 6, 1000000};
    inicializarFase(&anterior, 1);
    for (n = 1; n <= TOTAL_FASES; n++) {
        inicializarFase(&fase, n);
        VERIFICAR(fase.numero == n && fase.posicao == 0 && !missaoCumprida(&fase));
        VERIFICAR(fase.casas[CASA_RELIQUIA].tipo == CASA_MISSAO);
        VERIFICAR(fase.casas[TOTAL_CASAS - 1].tipo == CASA_CHEFE);
        for (i = 0; i < MONSTROS_POR_FASE; i++) {
            VERIFICAR(fase.monstros[i].nome[0] && fase.monstros[i].descricao[0]);
            VERIFICAR(fase.monstros[i].vida == fase.monstros[i].vidaMax);
            VERIFICAR(fase.monstros[i].xpRecompensa > 0);
            if (i > 0) VERIFICAR(strcmp(fase.monstros[i].nome, fase.monstros[i-1].nome) != 0);
            if (n > 1) {
                VERIFICAR(fase.monstros[i].vida > anterior.monstros[i].vida);
                VERIFICAR(fase.monstros[i].ataque > anterior.monstros[i].ataque);
                VERIFICAR(fase.monstros[i].defesa > anterior.monstros[i].defesa);
            }
        }
        anterior = fase;
        fase.reliquiaObtida = 1;
        VERIFICAR(!missaoCumprida(&fase));
        fase.chefeDerrotado = 1;
        VERIFICAR(missaoCumprida(&fase));
        fase.reliquiaObtida = 0;
        VERIFICAR(!missaoCumprida(&fase));
    }
    for (posicao = 0; posicao < TOTAL_CASAS; posicao++) {
        for (passo = 0; passo < 4; passo++) {
            inicializarFase(&fase, 1);
            fase.posicao = posicao;
            moverJogador(&fase, passos[passo]);
            VERIFICAR(fase.posicao >= posicao && fase.posicao < TOTAL_CASAS);
            if (posicao < CASA_RELIQUIA) VERIFICAR(fase.posicao <= CASA_RELIQUIA);
            fase.reliquiaObtida = 1;
            moverJogador(&fase, 1000000);
            VERIFICAR(fase.posicao == TOTAL_CASAS - 1);
        }
    }
}

static void testarEventos(void)
{
    Fase fase;
    Personagem p, antes;
    inicializarFase(&fase, 1); criarPersonagem(&p, MASCARA_ACO);
    fase.posicao = 3;
    executarEvento(&p, &fase);
    VERIFICAR(p.pocoes == 3 && fase.casas[3].resolvida);
    antes = p;
    executarEvento(&p, &fase);
    VERIFICAR(p.pocoes == antes.pocoes && p.vidaMax == antes.vidaMax && p.forca == antes.forca);
    fase.posicao = CASA_RELIQUIA;
    executarEvento(&p, &fase);
    VERIFICAR(fase.reliquiaObtida && !missaoCumprida(&fase));
    fase.posicao = 5; p.vida = 30; p.foco = 0;
    executarEvento(&p, &fase);
    VERIFICAR(p.vida == 60 && p.foco == 8);
    fase.posicao = 6; p.vida = 1;
    VERIFICAR(executarEvento(&p, &fase) == COMBATE_DERROTA && p.vida == 0);
}

static void testarTempoDeLeitura(void)
{
    int curto, longo, acentuado;
    const char *texto = "A relíquia repousa no santuário. O Andarilho atravessa as cinzas "
                        "e reconhece as marcas da Maturação nos portões da cidade.";
    configurarRitmo(RITMO_NORMAL);
    curto = tempoLeituraMs("O dado rola.");
    longo = tempoLeituraMs(texto);
    VERIFICAR(curto >= 1400 && longo > curto && longo <= 14000);
    VERIFICAR(tempoLeituraMs("") == 0);
    acentuado = tempoLeituraMs("Ação poção missão máscara força relíquia.");
    VERIFICAR(acentuado == tempoLeituraMs("Acao pocao missao mascara forca reliquia."));
    configurarRitmo(RITMO_RAPIDO);
    VERIFICAR(tempoLeituraMs(texto) == longo * 2 / 5);
    configurarRitmo(RITMO_LENTO);
    VERIFICAR(tempoLeituraMs(texto) == longo * 3 / 2);
    configurarRitmo(RITMO_SEM_PAUSAS);
    VERIFICAR(tempoLeituraMs(texto) == 0);
}

int main(void)
{
    srand(123);
    testarPersonagens();
    testarEvolucao();
    testarCombate();
    testarFases();
    testarEventos();
    testarTempoDeLeitura();
    printf("OK: %d verificacoes passaram.\n", verificacoes);
    return 0;
}
