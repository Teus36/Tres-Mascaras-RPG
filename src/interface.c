#include <stdio.h>
#include <string.h>
#include <time.h>
#include "interface.h"
#include "personagem.h"

#define LARGURA 72
#define LINHAS_RETRATO 6


static RitmoNarrativa ritmoAtual = RITMO_SEM_PAUSAS;


static int colunasTexto(const char *texto)
{
    int colunas = 0;
    while (*texto) {
        if (((unsigned char)*texto & 0xC0u) != 0x80u) colunas++;
        texto++;
    }
    return colunas;
}

static int bytesAteColunas(const char *texto, int limite)
{
    int bytes = 0, colunas = 0;
    while (texto[bytes] && colunas < limite) {
        bytes++;
        while (((unsigned char)texto[bytes] & 0xC0u) == 0x80u) bytes++;
        colunas++;
    }
    return bytes;
}

static void campo(const char *texto, int largura)
{
    int bytes = bytesAteColunas(texto, largura);
    int i, colunas = 0;
    for (i = 0; i < bytes; i++) {
        if (((unsigned char)texto[i] & 0xC0u) != 0x80u) colunas++;
    }
    printf("%.*s", bytes, texto);
    for (i = colunas; i < largura; i++) putchar(' ');
}

void configurarRitmo(RitmoNarrativa ritmo)
{
    if (ritmo >= RITMO_NORMAL && ritmo <= RITMO_SEM_PAUSAS) ritmoAtual = ritmo;
}

const char *nomeRitmo(void)
{
    static const char *const nomes[] = {"Normal", "Rápido", "Lento", "Sem pausas"};
    return nomes[ritmoAtual];
}

int tempoLeituraMs(const char *texto)
{
    int palavras = 0, emPalavra = 0, pontuacao = 0, tempo;
    const unsigned char *p = (const unsigned char *)texto;
    if (ritmoAtual == RITMO_SEM_PAUSAS || *p == '\0') return 0;

    while (*p) {
        int espaco = *p == ' ' || *p == '\n' || *p == '\r' || *p == '\t';
        if (!espaco && !emPalavra && palavras < 100) palavras++;
        emPalavra = !espaco;
        if ((*p == '.' || *p == '!' || *p == '?') && pontuacao < 20) pontuacao++;
        p++;
    }

    tempo = 600 + palavras * 260 + pontuacao * 100;

    if (tempo < 1400) tempo = 1400;
    if (tempo > 14000) tempo = 14000;
    if (ritmoAtual == RITMO_RAPIDO) tempo = tempo * 2 / 5;
    if (ritmoAtual == RITMO_LENTO) tempo = tempo * 3 / 2;
    return tempo;
}

void aguardarLeitura(const char *texto)
{
    int milissegundos = tempoLeituraMs(texto);

    clock_t inicio;

    fflush(stdout);

    if (milissegundos == 0) return;

    inicio = clock();

    if (inicio == (clock_t)-1) return;

    for (;;) {
        clock_t agora = clock();
        if (agora == (clock_t)-1 ||
            ((double)agora - (double)inicio) * 1000.0 / CLOCKS_PER_SEC >= milissegundos) break;
    }
}

static const char *const mascaras[3][LINHAS_RETRATO] = {
    {"      .------.      ", "     /  .  .  \\     ",
     "     |  >  <  |     ", "     |   /\\   |     ",
     "     |  ====  |     ", "      '------'      "},
    {"       .----.       ", "      / .  . \\      ",
     "      | '  ' |      ", "      |  __  |      ",
     "      | /  \\ |      ", "       '----'       "},
    {"       .----.       ", "      /======\\      ",
     "      | ---- |      ", "      |  /\\  |      ",
     "      | :__: |      ", "       '----'       "}
};

static const char *const criaturas[9][LINHAS_RETRATO] = {
    {"        ___         ", "       /   \\        ", "       |. .|        ",
     "      /| o |\\       ", "     / |___| \\      ", "       /   \\        "},
    {"     /\\      /\\     ", "    /  \\____/  \\    ", "    |  x    x  |    ",
     "    \\    /\\    /    ", "     '._VVVV_.'     ", "      /|/\\/|\\       "},
    {"      .------.  /   ", "      | [] []| /    ", "      |  --  |/     ",
     "    __|______|__    ", "   /  /| ## |\\  \\   ", "      /|____|\\      "},
    {"      .-$$$$-.      ", "     / $    $ \\     ", "     | $o  o$ |     ",
     "     |  $$$$  |     ", "    /|__$$$$__|\\    ", "     /  |  |  \\     "},
    {"       .-~~-.       ", "      / .  . \\      ", "     /  |  |  \\     ",
     "     |   O    |     ", "     |  /|\\   |     ", "     '~~~~~~~~'     "},
    {"         /\\         ", "        /++\\        ", "       /_++_\\       ",
     "       |o  o|       ", "     __|_--_|__     ", "    /___|++|___\\    "},
    {"    \\    ()    /    ", "     \\  .--.  /     ", "   ___\\ |oo| /___   ",
     "  /____\\|__|/____\\  ", "        /++\\        ", "       /____\\       "},
    {"         /\\         ", "        /$$\\        ", "       /_$$_\\       ",
     "       |x  x|       ", "    ___|_==_|___    ", "   /___/||||\\___\\   "},
    {"     .--(())--.     ", "   .'   /++\\   '.   ", "  /    |o  o|    \\  ",
     "  '----| /\\ |----'  ", "     __|_==_|__     ", "    /___/++\\___\\    "}
};

static void borda(void)
{
    int i;
    putchar('+');
    for (i = 0; i < LARGURA + 2; i++) putchar('-');
    puts("+");
}

static void linha(const char *texto)
{
    printf("| ");
    campo(texto, LARGURA);
    puts(" |");
}

static void centro(const char *texto)
{
    int tamanho = colunasTexto(texto);
    int margem = tamanho < LARGURA ? (LARGURA - tamanho) / 2 : 0;
    printf("| %*s", margem, "");
    campo(texto, LARGURA - margem);
    puts(" |");
}


static void imprimirParagrafo(const char *texto, int painel)
{
    while (*texto) {
        int corte = bytesAteColunas(texto, LARGURA);
        const char *quebra = strchr(texto, '\n');
        char trecho[LARGURA * 4 + 1];
        if (quebra != NULL && quebra - texto < corte) corte = (int)(quebra - texto);
        else if (texto[corte] && texto[corte] != '\n') {
            int espaco = corte;
            while (espaco > 0 && texto[espaco] != ' ') espaco--;
            if (espaco > 0) corte = espaco;
        }
        memcpy(trecho, texto, (size_t)corte);
        trecho[corte] = '\0';
        if (painel) linha(trecho);
        else printf("  %s\n", trecho);
        texto += corte;
        while (*texto == ' ') texto++;
        if (*texto == '\n') texto++;
    }
}

static void paragrafo(const char *texto)
{
    imprimirParagrafo(texto, 1);
}

void narrar(const char *texto)
{
    imprimirParagrafo(texto, 0);
    aguardarLeitura(texto);
}

static void abrirPainel(const char *titulo)
{
    putchar('\n');
    borda();
    centro(titulo);
    borda();
}

static void tresColunas(const char *a, const char *b, const char *c)
{
    printf("| ");
    campo(a, 22); printf("  "); campo(b, 22); printf("  "); campo(c, 22);
    puts("   |");
}

static void duasColunas(const char *a, const char *b)
{
    printf("|   ");
    campo(a, 32); printf("    "); campo(b, 32);
    puts("   |");
}

static void barra(char *destino, int atual, int maximo, int largura)
{
    int i;
    int preenchido = maximo > 0 ? (int)((double)atual / maximo * largura) : 0;
    if (preenchido < 0) preenchido = 0;
    if (preenchido > largura) preenchido = largura;
    for (i = 0; i < largura; i++) destino[i] = i < preenchido ? '#' : '.';
    destino[largura] = '\0';
}

static void recursos(const Personagem *p)
{
    char vida[19], foco[19], buffer[160];
    barra(vida, p->vida, p->vidaMax, 18);
    barra(foco, p->foco, p->focoMax, 18);
    snprintf(buffer, sizeof buffer, "PV [%s]   FOCO [%s]   NV %02d",
             vida, foco, p->nivel);
    linha(buffer);
    snprintf(buffer, sizeof buffer, "Vida %d/%d | Foco %d/%d | Poções %d",
             p->vida, p->vidaMax, p->foco, p->focoMax, p->pocoes);
    linha(buffer);
}

static int retratoMonstro(const Monstro *m)
{
    static const char *const nomes[] = {
        "Devoto Oco", "Fera de Ossos", "Executor de Ferrugem",
        "Avaro de Ferro", "Cantora Enlutada", "Prior das Cinzas",
        "Anjo de Cera", "Bispo do Desespero", "A Providência"
    };
    int i;
    for (i = 0; i < 9; i++) if (strcmp(m->nome, nomes[i]) == 0) return i;
    return 0;
}

int lerOpcao(const char *mensagem, int minimo, int maximo)
{
    char linha[128];
    int i, valor, digitos, invalida, c;
    for (;;) {
        printf("%s", mensagem);
        fflush(stdout);
        if (fgets(linha, sizeof linha, stdin) == NULL) return -1;
        if (strchr(linha, '\n') == NULL && !feof(stdin)) {
            while ((c = getchar()) != '\n' && c != EOF) { }
            puts("Entrada muito longa. Digite apenas uma opção.");
            continue;
        }
        i = 0;
        valor = 0;
        digitos = 0;
        invalida = 0;
        while (linha[i] == ' ' || linha[i] == '\t') i++;
        while (linha[i] >= '0' && linha[i] <= '9') {
            /* Limite anterior a multiplicacao: evita overflow em entradas longas. */
            if (valor > 1000) invalida = 1;
            if (!invalida) valor = valor * 10 + linha[i] - '0';
            digitos++;
            i++;
        }
        while (linha[i] == ' ' || linha[i] == '\t' ||
               linha[i] == '\r' || linha[i] == '\n') i++;
        if (!invalida && digitos && linha[i] == '\0' &&
            valor >= minimo && valor <= maximo) return valor;
        printf("Opção inválida. Escolha um número entre %d e %d.\n", minimo, maximo);
    }
}

const char *nomeClasse(Classe classe)
{
    switch (classe) {
        case MASCARA_ACO: return "Aço / Guerreiro";
        case MASCARA_CINZAS: return "Cinzas / Mago";
        case MASCARA_MORTALHA: return "Mortalha / Arqueiro";
        default: return "Desconhecida";
    }
}

void mostrarPersonagem(const Personagem *p)
{
    char buffer[256], xp[25];
    int i;
    abrirPainel("O ANDARILHO / RELÍQUIA EQUIPADA");
    snprintf(buffer, sizeof buffer, "%s - Máscara de %s", p->nome, nomeClasse(p->classe));
    centro(buffer);
    for (i = 0; i < LINHAS_RETRATO; i++) centro(mascaras[p->classe][i]);
    linha("");
    recursos(p);
    barra(xp, p->xp, 40 * p->nivel, 24);
    snprintf(buffer, sizeof buffer, "XP [%s] %d/%d   |   Total recebido: %d",
             xp, p->xp, 40 * p->nivel, p->xpTotal);
    linha(buffer);
    borda();
    snprintf(buffer, sizeof buffer, "FOR %2d    DEF %2d (+%d)    AGI %2d    INT %2d",
             p->forca, p->defesa, p->bonusArmadura, p->agilidade, p->inteligencia);
    linha(buffer);
    snprintf(buffer, sizeof buffer, "Arma     : %s (+%d)", p->arma, p->bonusArma);
    paragrafo(buffer);
    snprintf(buffer, sizeof buffer, "Armadura : %s", p->armadura);
    paragrafo(buffer);
    snprintf(buffer, sizeof buffer, "Especial : %s", p->habilidade);
    paragrafo(buffer);
    borda();
    snprintf(buffer, sizeof buffer, "%s, máscara de %s. Arma: %s. Armadura: %s.",
             p->nome, nomeClasse(p->classe), p->arma, p->armadura);
    aguardarLeitura(buffer);
}

void mostrarTitulo(void)
{
    int i;
    abrirPainel("C I N D E R Á L I A");
    linha("");
    for (i = 0; i < LINHAS_RETRATO; i++) {
        tresColunas(mascaras[0][i], mascaras[1][i], mascaras[2][i]);
    }
    linha("");
    centro("T R Ê S   M Á S C A R A S");
    centro("A Chaga da Penitência");
    linha("");
    centro("Três pecados. Um andarilho. Nenhuma promessa de salvação.");
    linha("");
    borda();
}

void mostrarMenuPrincipal(void)
{
    char buffer[100];
    abrirPainel("A FOGUEIRA AINDA ESTÁ ACESA");
    linha("  [1] Nova jornada        Vista uma máscara e siga pelas cinzas.");
    linha("  [2] História e regras   Escute o que restou de Cinderália.");
    snprintf(buffer, sizeof buffer, "  [3] Ritmo da narrativa  Atual: %s", nomeRitmo());
    linha(buffer);
    linha("  [0] Sair                Descanse por enquanto.");
    borda();
}

int ajustarRitmo(void)
{
    int opcao;
    abrirPainel("O TEMPO ENTRE AS CINZAS");
    paragrafo("Cada trecho recebe tempo de leitura proporcional ao número de "
              "palavras. As molduras e os desenhos não entram na contagem.");
    linha("");
    linha("[1] Normal       [2] Rápido       [3] Lento       [4] Sem pausas");
    linha("[0] Voltar sem alterar");
    borda();
    opcao = lerOpcao("Ritmo: ", 0, 4);
    if (opcao > 0) configurarRitmo((RitmoNarrativa)(opcao - 1));
    return opcao;
}

void mostrarEscolhaMascaras(void)
{
    Personagem p[3];
    char a[64], b[64], c[64];
    int i;
    for (i = 0; i < 3; i++) criarPersonagem(&p[i], (Classe)i);
    abrirPainel("TRÊS RELÍQUIAS, UM ANDARILHO");
    centro("Escolha a memória que você carregará até o fim da jornada.");
    linha("");
    tresColunas("[1] AÇO", "[2] CINZAS", "[3] MORTALHA");
    tresColunas("O executor", "O eclesiástico", "A caçadora");
    linha("");
    for (i = 0; i < LINHAS_RETRATO; i++) {
        tresColunas(mascaras[0][i], mascaras[1][i], mascaras[2][i]);
    }
    linha("");
    for (i = 0; i < 3; i++) {
        char *campos[3] = {a, b, c};
        int j;
        for (j = 0; j < 3; j++) {
            if (i == 0) snprintf(campos[j], 64, "PV %3d   FOCO %2d", p[j].vidaMax, p[j].focoMax);
            if (i == 1) snprintf(campos[j], 64, "FOR %2d   DEF %2d (+%d)", p[j].forca, p[j].defesa, p[j].bonusArmadura);
            if (i == 2) snprintf(campos[j], 64, "AGI %2d   INT %2d", p[j].agilidade, p[j].inteligencia);
        }
        tresColunas(a, b, c);
    }
    linha("");
    tresColunas("Ira Ferruginosa", "Chama das Lamentações", "Virote da Condenação");
    tresColunas("Poder pelo sacrifício", "Fogo que ignora defesa", "Crítico e sangramento");
    borda();
    linha("Cada especial custa 8 de foco. A Ira também sacrifica 8 de vida.");
    linha("Equipamentos e atributos completos ficam na ficha [2] durante o jogo.");
    linha("[0] Voltar para a fogueira");
    borda();
}

void mostrarMonstro(const Monstro *m)
{
    char buffer[180];
    abrirPainel("ALGO SE MOVE ENTRE AS CINZAS");
    snprintf(buffer, sizeof buffer, "%s (nível %d) - Vida %d/%d | Ataque %d | Defesa %d",
           m->nome, m->nivel, m->vida, m->vidaMax, m->ataque, m->defesa);
    paragrafo(buffer);
    paragrafo(m->descricao);
    borda();
    aguardarLeitura(m->descricao);
}

void mostrarCombate(const Personagem *p, const Monstro *m)
{
    char buffer[160], a[80], b[80], vida[19], inimigo[19];
    int i, retrato = retratoMonstro(m);

    abrirPainel("C O N F R O N T O");
    duasColunas("O ANDARILHO", m->nome);
    duasColunas(nomeClasse(p->classe), "A Maturação tem muitos rostos.");
    linha("");

    for (i = 0; i < LINHAS_RETRATO; i++) {
        duasColunas(mascaras[p->classe][i], criaturas[retrato][i]);
    }

    barra(vida, p->vida, p->vidaMax, 18);
    barra(inimigo, m->vida, m->vidaMax, 18);

    snprintf(a, sizeof a, "PV [%s] %d/%d", vida, p->vida, p->vidaMax);
    snprintf(b, sizeof b, "PV [%s] %d/%d", inimigo, m->vida, m->vidaMax);

    duasColunas(a, b);

    snprintf(a, sizeof a, "Foco %d/%d   Poções %d", p->foco, p->focoMax, p->pocoes);

    if (m->sangramento) snprintf(b, sizeof b, "Sangramento: %d reações", m->sangramento);
    else snprintf(b, sizeof b, "Ataque %d   Defesa %d", m->ataque, m->defesa);

    duasColunas(a, b);

    borda();

    linha("[1] Atacar          [2] Defender (+4 foco)       [4] Fugir");
    snprintf(buffer, sizeof buffer, "[3] Especial%s       [5] Poção%s",
             p->foco < CUSTO_HABILIDADE ? " (sem foco)" :
             p->classe == MASCARA_ACO && p->vida <= CUSTO_IRA ? " (vida baixa)" :
             p->classe == MASCARA_ACO ? " (-8 foco / -8 PV)" : " (-8 foco)",
             p->pocoes == 0 ? " (esgotada)" : p->vida == p->vidaMax ? " (vida cheia)" : " (+35 PV)");

    linha(buffer);
    snprintf(buffer, sizeof buffer, "    %s", p->habilidade);
    linha(buffer);
    borda();
}

void mostrarRegras(void)
{
    abrirPainel("AS ÚLTIMAS PÁGINAS DE CINDERÁLIA");
    paragrafo("Em Cinderália, os pecados amadurecem em monstros. A Providência "
              "promete cura, mas se alimenta da adoração e do sofrimento. "
              "Você é o Andarilho: vista uma relíquia e desmonte o falso culto.");
    linha("");
    centro("O CAMINHO");
    paragrafo("Role d6 para andar pelas 18 casas de cada fase. Ao cruzar a casa "
              "14, pare para obter a relíquia obrigatória. Derrote o chefe na "
              "casa 18 para concluir a missão. Vença as três fases para vencer.");
    linha("");
    centro("O CONFRONTO");
    paragrafo("Ataque: d20 + atributo da classe + arma - defesa inimiga, mínimo 1. "
              "Aço usa força; Cinzas usa inteligência; Mortalha usa agilidade.");
    paragrafo("Defender: corta o próximo dano pela metade, para cima, e recupera "
              "4 de foco. O inimigo ainda age.");
    paragrafo("Especial: custa 8 de foco. A Ira do Aço também custa 8 de vida "
              "e exige mais de 8 PV. Cinzas ignora defesa; Mortalha aplica "
              "crítico e sangramento.");
    paragrafo("Fugir: d20 + agilidade >= 20. Falhar permite ataque; escapar não "
              "da recompensa. Fugir do chefe faz você recuar uma casa.");
    paragrafo("Poção: cura até 35 PV e consome um turno se usada no combate.");
    linha("");
    centro("O QUE PERMANECE");
    paragrafo("Vitórias dão XP e um bônus de atributo. Níveis fortalecem o herói "
              "e curam até 20 PV. Entre fases, vida e foco são restaurados. "
              "Eventos resolvidos não dão recompensa novamente.");
    linha("");
    paragrafo("Vida zero significa derrota. EOF encerra com segurança. "
              "Não há salvamento: toda jornada é uma nova penitência.");
    borda();
}

void mostrarFase(const Fase *fase)
{
    char buffer[512];

    abrirPainel("UM NOVO CAPÍTULO");
    snprintf(buffer, sizeof buffer, "FASE %d: %s", fase->numero, fase->nome);
    centro(buffer);

    linha("");
    paragrafo(fase->historia);
    linha("");

    snprintf(buffer, sizeof buffer, "MISSÃO: %s", fase->missao);

    paragrafo(buffer);

    borda();

    snprintf(buffer, sizeof buffer, "%s %s", fase->historia, fase->missao);
    aguardarLeitura(buffer);

}

void mostrarTabuleiro(const Fase *fase)
{
    static const char simbolos[] = ".!$+^*B";
    static const char *const eventos[] = {
        "caminho de cinzas", "encontro", "relicário", "altar seguro",
        "armadilha", "santuário da missão", "domínio do chefe"
    };
    static const char *const paisagens[3][4] = {
        {"          /\\         .              _       /\\           .     ",
         "     /\\  /  \\     .     _|_      __| |__   /  \\    _|_        ",
         "    /  \\/    \\  _|_    _| |_    |  _ _  | /    \\    |    _|_  ",
         "   /____\\____/   |      | |     |__|_|__|/______\\  _|_    |   "},
        {"           (  . )           +             ( .  )              ",
         "            )  (        ___/ \\___          )  (               ",
         "       ____/____\\____  |  _   _  |   _____/____\\_____          ",
         "      | []  []  []  []|_| | | | |_|_| []  []  []  [] |         "},
        {"                       .---(())---.                           ",
         "          /\\        .-'     /\\     '-.        /\\              ",
         "         /++\\      /   /\\  /++\\  /\\   \\      /++\\             ",
         "      __|_[]_|____|___|[]||_[]_||[]|___|____|_[]_|__          "}
    };

    char mapa[11][LARGURA + 1], buffer[180];
    int i, fileira, coluna;

    abrirPainel("C A M I N H O   D A   P E N I T Ê N C I A");
    snprintf(buffer, sizeof buffer, "%02d / %02d   %s", fase->numero, TOTAL_FASES, fase->nome);
    centro(buffer);

    for (i = 0; i < 4; i++) linha(paisagens[fase->numero - 1][i]);
    linha("");

    for (i = 0; i < 11; i++) {
        memset(mapa[i], ' ', LARGURA);
        mapa[i][LARGURA] = '\0';
    }
    for (fileira = 0; fileira < 3; fileira++) {
        int y = fileira * 4;
        for (coluna = 0; coluna < 6; coluna++) {
            int casa = fileira * 6 + (fileira % 2 ? 5 - coluna : coluna);
            int x = 7 + coluna * 10;
            int atual = casa == fase->posicao;
            char celula[8];
            char simbolo = fase->casas[casa].resolvida ? '-' : simbolos[fase->casas[casa].tipo];
            snprintf(celula, sizeof celula, "[%02d %c%c]", casa + 1, simbolo, atual ? '@' : ' ');
            memcpy(&mapa[y][x], atual ? "+=====+" : "+-----+", 7);
            memcpy(&mapa[y + 1][x], celula, 7);
            memcpy(&mapa[y + 2][x], atual ? "+=====+" : "+-----+", 7);
            if (coluna < 5) memcpy(&mapa[y + 1][x + 7], fileira % 2 ? "<--" : "-->", 3);
        }
    }
    mapa[3][60] = 'v';
    mapa[7][10] = 'v';
    for (i = 0; i < 11; i++) linha(mapa[i]);
    linha("");
    snprintf(buffer, sizeof buffer, "@ Você: casa %02d/%d - %s%s", fase->posicao + 1,
             TOTAL_CASAS, eventos[fase->casas[fase->posicao].tipo],
             fase->casas[fase->posicao].resolvida ? " (resolvido)" : "");
    linha(buffer);
    linha("! encontro   $ tesouro   + altar   ^ armadilha   . caminho");
    linha("* relíquia   B chefe     - resolvido   @ você   --> direção");
    borda();
    snprintf(buffer, sizeof buffer, "[%c] Relíquia: %s", fase->reliquiaObtida ? 'x' : ' ', fase->reliquia);
    linha(buffer);
    snprintf(buffer, sizeof buffer, "[%c] Chefe: %s", fase->chefeDerrotado ? 'x' : ' ', fase->monstros[2].nome);
    linha(buffer);
    borda();
}

void mostrarExploracao(const Personagem *p, const Fase *fase)
{
    char buffer[100];
    abrirPainel("O ANDARILHO");
    snprintf(buffer, sizeof buffer, "Máscara de %s", nomeClasse(p->classe));
    linha(buffer);
    recursos(p);
    borda();
    mostrarTabuleiro(fase);
    linha("[1] Rolar d6 e caminhar     [2] Personagem     [3] Poção");
    linha("[4] Missão                 [0] Encerrar jornada");
    borda();
}

void mostrarRolagem(int dado, int origem, int destino)
{
    static const char *const faces[6][3] = {
        {"     ", "  o  ", "     "},
        {"o    ", "     ", "    o"},
        {"o    ", "  o  ", "    o"},
        {"o   o", "     ", "o   o"},
        {"o   o", "  o  ", "o   o"},
        {"o   o", "o   o", "o   o"}
    };
    char buffer[100], face[32];
    int i;
    abrirPainel("O DESTINO ROLA");
    duasColunas("       +-------+", "As cinzas guardam seus passos.");
    for (i = 0; i < 3; i++) {
        snprintf(face, sizeof face, "       | %s |", faces[dado - 1][i]);
        if (i == 1) snprintf(buffer, sizeof buffer, "d6=%d: casa %d -> casa %d.", dado, origem, destino);
        else buffer[0] = '\0';
        duasColunas(face, buffer);
    }
    duasColunas("       +-------+", "");
    if (destino == CASA_RELIQUIA + 1 && destino - origem < dado) {
        linha("O caminho exige uma parada no santuário da missão.");
    }
    borda();
    snprintf(buffer, sizeof buffer, "As cinzas guardam seus passos. d6=%d: casa %d para casa %d.",
             dado, origem, destino);
    aguardarLeitura(buffer);
}

void mostrarEvento(TipoCasa tipo)
{
    static const char *const nomes[] = {
        " .  UM INSTANTE DE SILÊNCIO", " !  UM ENCONTRO NO CAMINHO",
        " $  UM RELICÁRIO ESQUECIDO", " +  UM ALTAR ENTRE AS RUÍNAS",
        " ^  ESPINHOS SOB AS CINZAS", " *  O SANTUÁRIO DA RELÍQUIA",
        " B  O GUARDIÃO DA PASSAGEM"
    };
    printf("\n  %s\n  ----------------------------------------------------------------------\n", nomes[tipo]);
}

void mostrarResumo(const Personagem *p, ResultadoJogo resultado)
{
    char buffer[120];
    
    abrirPainel(resultado == JOGO_VITORIA ? "O PRIMEIRO AMANHECER" :
                resultado == JOGO_DERROTA ? "AS CINZAS SE FECHAM" : "DE VOLTA AO SILÊNCIO");

    if (resultado == JOGO_VITORIA) {
        centro("          \\   |   /          ");
        centro("       ---- .---. ----       ");
        centro("  __________|___|__________  ");
        linha("");
        paragrafo("VITÓRIA! As três relíquias rompem o halo da Providência.");
        paragrafo("O falso absoluto cai. Pela primeira vez, Cinderália amanhece "
                  "sem promessas de adoração: o Andarilho pode escolher seu caminho.");

    } else if (resultado == JOGO_DERROTA) {
        centro("             _|_             ");
        centro("              |              ");
        centro("          ____|____          ");
        linha("");
        paragrafo("DERROTA. O Andarilho cai, mas as máscaras aguardam outro despertar.");

    } else {
        paragrafo("Jornada encerrada pelo jogador. O progresso não foi salvo.");
    }

    linha("");

    snprintf(buffer, sizeof buffer, "Combates vencidos: %d | XP total recebido: %d", p->vitorias, p->xpTotal);

    linha(buffer);

    borda();

    aguardarLeitura(resultado == JOGO_VITORIA ?
                    "VITÓRIA! As três relíquias rompem o halo da Providência. "
                    "O falso absoluto cai. Pela primeira vez, Cinderália amanhece "
                    "sem promessas de adoração: o Andarilho pode escolher seu caminho." :
                    resultado == JOGO_DERROTA ?
                    "DERROTA. O Andarilho cai, mas as máscaras aguardam outro despertar." : "");

    mostrarPersonagem(p);
}
