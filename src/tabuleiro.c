#include <stdio.h>
#include <string.h>
#include "tabuleiro.h"
#include "combate.h"
#include "interface.h"
#include "personagem.h"

static void definirMonstro(Monstro *m, const char *nome, const char *descricao,
                           int vida, int ataque, int defesa, int nivel, int xp)
{
    strcpy(m->nome, nome);
    strcpy(m->descricao, descricao);
    m->vida = m->vidaMax = vida;
    m->ataque = ataque; m->defesa = defesa; m->nivel = nivel;
    m->xpRecompensa = xp;
    m->sangramento = 0;
}

void inicializarFase(Fase *fase, int numero)
{
    static const TipoCasa mapa[TOTAL_CASAS] = {
        CASA_VAZIA, CASA_VAZIA, CASA_COMBATE, CASA_TESOURO,
        CASA_COMBATE, CASA_CURA, CASA_ARMADILHA, CASA_COMBATE,
        CASA_VAZIA, CASA_TESOURO, CASA_COMBATE, CASA_CURA,
        CASA_ARMADILHA, CASA_MISSAO, CASA_COMBATE, CASA_CURA,
        CASA_VAZIA, CASA_CHEFE
    };
    int i;
    memset(fase, 0, sizeof *fase);
    if (numero < 1) numero = 1;
    if (numero > TOTAL_FASES) numero = TOTAL_FASES;
    fase->numero = numero;
    for (i = 0; i < TOTAL_CASAS; i++) fase->casas[i].tipo = mapa[i];
    if (numero == 1) {
        strcpy(fase->nome, "Arrabaldes da Maturação");
        strcpy(fase->historia,
               "As cinzas de Cinderália escondem o primeiro selo do culto. "
               "O Andarilho segue os sinos até o executor que guardava os portões.");
        strcpy(fase->reliquia, "Selo do Arrependimento");
        strcpy(fase->missao, "Obter o Selo do Arrependimento e derrotar o Executor de Ferrugem.");
        definirMonstro(&fase->monstros[0], "Devoto Oco",
                       "Um fiel consumido pela covardia, preso a uma prece sem fim.", 30, 4, 3, 1, 20);
        definirMonstro(&fase->monstros[1], "Fera de Ossos",
                       "A ira amadureceu em ossos entrelacados e passos cegos.", 38, 6, 4, 1, 25);
        definirMonstro(&fase->monstros[2], "Executor de Ferrugem",
                       "Antigo carrasco do culto; sua lâmina cobra uma fé falsa.", 65, 8, 6, 2, 45);
    } else if (numero == 2) {
        strcpy(fase->nome, "Claustro das Lamentações");
        strcpy(fase->historia,
               "O templo queimado ainda exala incenso. Nas galerias, sacerdotes "
               "sem olhos protegem a verdade que recusaram enxergar.");
        strcpy(fase->reliquia, "Brasa da Memória");
        strcpy(fase->missao, "Recuperar a Brasa da Memória e vencer o Prior das Cinzas.");
        definirMonstro(&fase->monstros[0], "Avaro de Ferro",
                       "A cobiça fundiu sua pele em metal incandescente.", 48, 8, 7, 2, 35);
        definirMonstro(&fase->monstros[1], "Cantora Enlutada",
                       "A vaidade veste cera e canta hinos que roubam o fôlego.", 55, 9, 6, 2, 40);
        definirMonstro(&fase->monstros[2], "Prior das Cinzas",
                       "Guardião dos falsos dogmas, alimenta o fogo com lembranças.", 90, 11, 9, 3, 65);
    } else {
        strcpy(fase->nome, "Catedral do Falso Absoluto");
        strcpy(fase->historia,
               "Sob um halo de ouro e cera, a Providência recebe a dor dos devotos. "
               "A última relíquia pode revelar a criatura por trás da luz.");
        strcpy(fase->reliquia, "Verdade sem Véu");
        strcpy(fase->missao, "Revelar a Verdade sem Véu e derrotar A Providência.");
        definirMonstro(&fase->monstros[0], "Anjo de Cera",
                       "Uma promessa de salvação moldada para aprisionar os fiéis.", 65, 11, 10, 3, 50);
        definirMonstro(&fase->monstros[1], "Bispo do Desespero",
                       "Ajoelhado sobre ouro, transforma adoração em correntes.", 75, 12, 11, 3, 55);
        definirMonstro(&fase->monstros[2], "A Providência",
                       "O falso absoluto: luz por fora, a Maturação em seu núcleo.", 125, 14, 13, 4, 100);
    }
}

void moverJogador(Fase *fase, int passos)
{
    int destino;
    if (passos <= 0) return;

    destino = passos >= TOTAL_CASAS - 1 - fase->posicao ?
              TOTAL_CASAS - 1 : fase->posicao + passos;
    if (!fase->reliquiaObtida && fase->posicao < CASA_RELIQUIA && destino >= CASA_RELIQUIA) {
        destino = CASA_RELIQUIA;
    }
    fase->posicao = destino;
}

int missaoCumprida(const Fase *fase)
{
    return fase->reliquiaObtida && fase->chefeDerrotado;
}

ResultadoCombate executarEvento(Personagem *p, Fase *fase)
{
    Casa *casa = &fase->casas[fase->posicao];
    ResultadoCombate resultado;

    int quantidade, antes;
    char mensagem[180];
    Monstro inimigo;

    if (casa->resolvida) {
        narrar("O evento desta casa já foi resolvido.");
        return COMBATE_VITORIA;
    }
    mostrarEvento(casa->tipo);
    switch (casa->tipo) {
        case CASA_COMBATE:
        case CASA_CHEFE:
            if (casa->tipo == CASA_CHEFE && !fase->reliquiaObtida) {

                narrar("O selo impede a passagem. Retorne ao santuário da missão.");
                fase->posicao = CASA_RELIQUIA;
                return executarEvento(p, fase);
            }

            inimigo = fase->monstros[casa->tipo == CASA_CHEFE ? 2 : rolarDado(2) - 1];
            resultado = combate(p, &inimigo);
            if (resultado == COMBATE_VITORIA) {
                casa->resolvida = 1;
                if (casa->tipo == CASA_CHEFE) fase->chefeDerrotado = 1;
            } else if (resultado == COMBATE_FUGA && casa->tipo == CASA_CHEFE) {
                fase->posicao = TOTAL_CASAS - 2;
                narrar("Você recua para a casa 17. O chefe continua guardando a saída.");
            }
            return resultado;
        case CASA_TESOURO:
            narrar("Um relicário esquecido concede um bônus de atributo e uma poção.");
            aplicarBonusAleatorio(p);
            p->pocoes++;
            break;
        case CASA_CURA:
            antes = p->vida;
            quantidade = 25 + fase->numero * 5;
            curarPersonagem(p, quantidade);
            recuperarFoco(p, 8);
            snprintf(mensagem, sizeof mensagem, "Altar seguro: +%d de vida e até 8 de foco.", p->vida - antes);
            narrar(mensagem);
            break;
        case CASA_ARMADILHA:
            quantidade = rolarDado(6) + fase->numero * 2;
            receberDano(p, quantidade);
            snprintf(mensagem, sizeof mensagem, "Espinhos sob as cinzas! Você perde %d de vida.", quantidade);
            narrar(mensagem);
            break;
        case CASA_MISSAO:
            fase->reliquiaObtida = 1;
            snprintf(mensagem, sizeof mensagem, "RELÍQUIA OBTIDA: %s. O caminho ao chefe está aberto.", fase->reliquia);
            narrar(mensagem);
            curarPersonagem(p, 15);
            recuperarFoco(p, 8);
            narrar("O santuário cura até 15 de vida e restaura até 8 de foco.");
            break;
        case CASA_VAZIA:
            narrar("Apenas cinzas. O Andarilho segue em frente.");
            break;
    }
    casa->resolvida = 1;
    return p->vida > 0 ? COMBATE_VITORIA : COMBATE_DERROTA;
}

ResultadoJogo jogarCampanha(Personagem *p)
{
    int numero;
    char mensagem[100];
    for (numero = 1; numero <= TOTAL_FASES; numero++) {
        Fase fase;
        inicializarFase(&fase, numero);
        if (numero > 1) prepararProximaFase(p);
        mostrarFase(&fase);
        while (p->vida > 0 && !missaoCumprida(&fase)) {
            int opcao, dado, anterior;
            ResultadoCombate resultado;
            mostrarExploracao(p, &fase);
            opcao = lerOpcao("Opção: ", 0, 4);
            if (opcao == -1 || opcao == 0) return JOGO_SAIR;
            if (opcao == 2) { mostrarPersonagem(p); continue; }
            if (opcao == 3) {
                if (!usarPocao(p)) narrar("Sem poções ou vida cheia.");
                continue;
            }
            if (opcao == 4) { mostrarFase(&fase); continue; }
            dado = rolarDado(6);
            anterior = fase.posicao;
            moverJogador(&fase, dado);
            mostrarRolagem(dado, anterior + 1, fase.posicao + 1);
            resultado = executarEvento(p, &fase);
            if (resultado == COMBATE_SAIR) return JOGO_SAIR;
            if (resultado == COMBATE_DERROTA) return JOGO_DERROTA;
        }
        if (p->vida <= 0) return JOGO_DERROTA;
        snprintf(mensagem, sizeof mensagem, "MISSÃO DA FASE %d CONCLUIDA!", numero);
        narrar(mensagem);
    }
    return JOGO_VITORIA;
}
