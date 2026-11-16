#include <stdio.h>
#include <stdlib.h>
#include "combate.h"
#include "interface.h"
#include "personagem.h"

int rolarDado(int lados)
{
    if (lados <= 0) return 0;
    return rand() % lados + 1;
}

int calcularDano(int ataque, int defesa)
{
    return ataque > defesa ? ataque - defesa : 1;
}

void danoNoMonstro(Monstro *m, int dano)
{
    if (dano <= 0) return;
    m->vida = dano >= m->vida ? 0 : m->vida - dano;
}

void atacarMonstro(Personagem *p, Monstro *m)
{
    char mensagem[180];
    int dado = rolarDado(20);
    int critico = dado >= (p->classe == MASCARA_MORTALHA ? 18 : 20);
    int ataque = dado + atributoAtaque(p) + p->bonusArma;
    int dano = calcularDano(ataque, m->defesa);
    if (critico) dano *= 2;
    danoNoMonstro(m, dano);
    snprintf(mensagem, sizeof mensagem, "d20=%d; ataque=%d; dano=%d%s.", dado, ataque, dano,
           critico ? " (CRÍTICO)" : "");
    narrar(mensagem);
}

int usarHabilidade(Personagem *p, Monstro *m)
{
    int dado, dano;
    const char *efeito;
    char mensagem[256];
    if (p->foco < CUSTO_HABILIDADE) {
        narrar("Foco insuficiente. Defenda para recuperar 4 ou escolha outra ação.");
        return 0;
    }
    if (p->classe == MASCARA_ACO && p->vida <= CUSTO_IRA) {
        narrar("Ira exige mais de 8 de vida. Escolha outra ação.");
        return 0;
    }
    p->foco -= CUSTO_HABILIDADE;
    dado = rolarDado(20);
    switch (p->classe) {
        case MASCARA_ACO:
            receberDano(p, CUSTO_IRA);
            dano = calcularDano(dado + 2 * p->forca + p->bonusArma + 8, m->defesa);
            efeito = "Ira Ferruginosa: -8 de vida, -8 de foco; o montante rompe o silêncio.";
            break;
        case MASCARA_CINZAS:
            dano = dado + 2 * p->inteligencia + p->bonusArma;
            efeito = "Chama das Lamentações: -8 de foco; a defesa inimiga é ignorada.";
            break;
        default:
            dano = 2 * calcularDano(dado + p->agilidade + p->bonusArma, m->defesa);
            m->sangramento = 3;
            efeito = "Virote da Condenação: -8 de foco; crítico e sangramento por 3 reações.";
            break;
    }
    danoNoMonstro(m, dano);
    snprintf(mensagem, sizeof mensagem, "%s d20=%d; dano especial=%d.", efeito, dado, dano);
    narrar(mensagem);
    return 1;
}

int aplicarSangramento(Monstro *m)
{
    char mensagem[100];
    if (m->sangramento > 0 && m->vida > 0) {
        danoNoMonstro(m, 4);
        m->sangramento--;
        snprintf(mensagem, sizeof mensagem, "Sangramento: 4 de dano; restam %d reações.", m->sangramento);
        narrar(mensagem);
    }
    return m->vida <= 0;
}

void turnoMonstro(Personagem *p, const Monstro *m, int defendendo)
{
    char mensagem[180];
    int dado = rolarDado(20);
    int dano = calcularDano(dado + m->ataque, p->defesa + p->bonusArmadura);
    if (defendendo) dano = (dano + 1) / 2;
    receberDano(p, dano);
    snprintf(mensagem, sizeof mensagem, "%s: d20=%d; você sofre %d de dano%s.", m->nome, dado,
           dano, defendendo ? " (defesa ativa)" : "");
    narrar(mensagem);
}

void recompensarVitoria(Personagem *p, const Monstro *m)
{
    char mensagem[100];
    p->vitorias++;
    snprintf(mensagem, sizeof mensagem, "Venceu %s! +%d XP.", m->nome, m->xpRecompensa);
    narrar(mensagem);
    ganharXP(p, m->xpRecompensa);
    aplicarBonusAleatorio(p);
}

ResultadoCombate combate(Personagem *p, Monstro *m)
{
    mostrarMonstro(m);
    while (p->vida > 0 && m->vida > 0) {
        int acao, defendendo = 0;
        mostrarCombate(p, m);
        acao = lerOpcao("Ação: ", 1, 5);
        if (acao == -1) return COMBATE_SAIR;
        switch (acao) {
            case ACAO_ATACAR: atacarMonstro(p, m); break;
            case ACAO_DEFENDER:
                defendendo = 1;
                recuperarFoco(p, 4);
                narrar("Você prepara a defesa e recupera até 4 de foco.");
                break;
            case ACAO_HABILIDADE:
                if (!usarHabilidade(p, m)) continue;
                break;
            case ACAO_FUGIR: {
                int dado = rolarDado(20);
                char mensagem[140];
                if (dado + p->agilidade >= 20) {
                    snprintf(mensagem, sizeof mensagem,
                             "Fuga: d20=%d + agilidade %d (alvo 20). Você escapou. Sem XP ou bônus.",
                             dado, p->agilidade);
                    narrar(mensagem);
                    return COMBATE_FUGA;
                }
                snprintf(mensagem, sizeof mensagem,
                         "Fuga: d20=%d + agilidade %d (alvo 20). Fuga falhou!", dado, p->agilidade);
                narrar(mensagem);
                break;
            }
            case ACAO_POCAO:
                if (!usarPocao(p)) {
                    narrar("Sem poções ou vida cheia. Escolha outra ação.");
                    continue;
                }
                break;
        }
        /* Dano continuo ocorre antes da reacao; inimigo morto nunca contra-ataca. */
        if (m->vida > 0) aplicarSangramento(m);
        if (m->vida == 0) {
            recompensarVitoria(p, m);
            return COMBATE_VITORIA;
        }
        turnoMonstro(p, m, defendendo);
    }
    return COMBATE_DERROTA;
}
