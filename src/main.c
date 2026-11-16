#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "personagem.h"
#include "tabuleiro.h"
#include "interface.h"

static int lerSemente(const char *texto, unsigned int *semente)
{
    unsigned int valor = 0;
    int i;
    if (texto[0] == '\0') return 0;
    for (i = 0; texto[i] != '\0'; i++) {
        unsigned int digito;
        if (texto[i] < '0' || texto[i] > '9') return 0;
        digito = (unsigned int)(texto[i] - '0');
        if (valor > ((~0u) - digito) / 10u) return 0;
        valor = valor * 10u + digito;
    }
    *semente = valor;
    return 1;
}

static int escolherPersonagem(Personagem *p)
{
    int opcao;
    mostrarEscolhaMascaras();
    opcao = lerOpcao("Máscara [1-3; 0 para voltar]: ", 0, 3);
    if (opcao <= 0) return opcao;
    criarPersonagem(p, (Classe)(opcao - 1));
    return 1;
}

int main(int argc, char **argv)
{
    unsigned int semente = (unsigned int)time(NULL);
    RitmoNarrativa ritmo = RITMO_NORMAL;
    int opcao, i;
    if (argc == 2 && strcmp(argv[1], "--help") == 0) {
        puts("Uso: tres_mascaras [--seed NUMERO] [--ritmo normal|rapido|lento|sem]");
        puts("     tres_mascaras --sem-pausas\nJogo de terminal em C99. Sem arquivos de salvamento.");
        return 0;
    }
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--seed") == 0 && i + 1 < argc) {
            if (!lerSemente(argv[++i], &semente)) break;
        } else if (strcmp(argv[i], "--sem-pausas") == 0) {
            ritmo = RITMO_SEM_PAUSAS;
        } else if (strcmp(argv[i], "--ritmo") == 0 && i + 1 < argc) {
            const char *valor = argv[++i];
            if (strcmp(valor, "normal") == 0) ritmo = RITMO_NORMAL;
            else if (strcmp(valor, "rapido") == 0) ritmo = RITMO_RAPIDO;
            else if (strcmp(valor, "lento") == 0) ritmo = RITMO_LENTO;
            else if (strcmp(valor, "sem") == 0) ritmo = RITMO_SEM_PAUSAS;
            else break;
        } else break;
    }
    if (i != argc) {
        fputs("Argumentos inválidos. Consulte tres_mascaras --help.\n", stderr);
        return 1;
    }
    configurarRitmo(ritmo);
    srand(semente);
    mostrarTitulo();
    printf("  Semente da jornada: %u\n", semente);
    for (;;) {
        Personagem p;
        ResultadoJogo resultado;
        int escolha;
        mostrarMenuPrincipal();
        opcao = lerOpcao("Opção: ", 0, 3);
        if (opcao == 0 || opcao == -1) break;
        if (opcao == 2) { mostrarRegras(); continue; }
        if (opcao == 3) {
            if (ajustarRitmo() == -1) break;
            continue;
        }
        escolha = escolherPersonagem(&p);
        if (escolha == -1) break;
        if (escolha == 0) continue;
        resultado = jogarCampanha(&p);
        mostrarResumo(&p, resultado);
        if (feof(stdin)) break;
    }
    puts("Até a próxima jornada.");
    return 0;
}
