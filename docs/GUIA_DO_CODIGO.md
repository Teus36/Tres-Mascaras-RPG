# Guia de estudo do código

Este guia acompanha a implementação entregue. Abra os arquivos citados, execute uma partida e acompanhe as mudanças nos atributos. As perguntas ao final ajudam a preparar a explicação oral.

## 1. O caminho de uma partida

```text
main
  -> ler argumentos e inicializar srand
  -> menu principal
  -> escolherPersonagem
  -> jogarCampanha
       -> inicializarFase
       -> mostrarFase e mostrarTabuleiro
       -> lerOpcao
       -> rolarDado e moverJogador
       -> executarEvento
            -> combate, quando necessario
            -> ganharXP e aplicarBonusAleatorio, se vencer
       -> verificar vida e missaoCumprida
       -> proxima fase, se ainda houver
  -> mostrarResumo
```

A `main` não contém a fórmula de dano nem monta os nove monstros. Ela coordena as funções. Isso reduz a quantidade de coisas que precisam ser entendidas de uma vez.

## 2. `struct`: reunir dados de uma entidade

Em `src/tipos.h`, `Personagem` reúne vida, classe, atributos, equipamentos e progressão. Uma variável `Personagem p` guarda todos esses campos na mesma entidade lógica. `Monstro` é separado porque não precisa de poções, máscara ou inteligência.

`Fase` reúne o vetor de 18 casas, três modelos de monstros, posição e os dois estados da missão. A estrutura inteira é reinicializada ao começar uma fase; a estrutura do personagem permanece, conservando sua evolução.

Os nomes são vetores de `char` com espaço para o terminador `\0`. As chamadas de `strcpy` recebem apenas textos constantes internos que cabem nos vetores; nenhum nome digitado pelo usuário é copiado para esses campos.

## 3. `enum`: expressar significado

`CASA_TESOURO` descreve melhor um evento que o número `2`. Da mesma forma, `COMBATE_FUGA` distingue uma saída sem recompensa de uma vitória. As enumerações de resultados permitem que a função chamadora tome a decisão correta sem repetir a lógica de combate.

As opções do menu de combate começam em 1 para corresponder diretamente a `ACAO_ATACAR`, `ACAO_DEFENDER` e às demais ações.

## 4. Ponteiros: alterar a estrutura original

Considere este trecho já usado no projeto:

```c
void receberDano(Personagem *p, int dano)
{
    if (dano <= 0) return;
    p->vida = dano >= p->vida ? 0 : p->vida - dano;
}
```

`Personagem *p` recebe o endereço de um personagem. `p->vida` acessa o campo da estrutura nesse endereço; equivale a `(*p).vida`. Quando uma função chama `receberDano(&heroi, 10)`, o operador `&` fornece o endereço de `heroi`. A vida original muda diretamente.

Nas funções que já recebem `Personagem *p`, basta passar `p` adiante. Não se usa `&p`, pois isso seria o endereço do próprio ponteiro.

Já `mostrarPersonagem(const Personagem *p)` recebe um ponteiro com acesso de leitura: a interface não deve modificar atributos. Os ponteiros do jogo apontam para variáveis que continuam vivas durante as chamadas; não há alocação dinâmica ou `malloc`.

## 5. Vetor e limite do tabuleiro

O jogador vê casas de 1 a 18, mas os índices válidos do vetor vão de 0 a 17. `moverJogador` compara a quantidade de passos com a distância restante antes de somar. O destino nunca passa do índice 17.

A casa 14, índice `CASA_RELIQUIA` igual a 13, possui uma regra adicional: se o movimento passar por ela antes da relíquia ser obtida, a movimentação termina nela. Isso garante que o jogador tenha como concluir a missão mesmo com uma rolagem que pularia essa casa.

`missaoCumprida` usa `reliquiaObtida && chefeDerrotado`: as duas condições precisam ser verdadeiras. Só chegar à última casa não basta.

## 6. Combate e ordem das operações

Em `combate`, o laço continua enquanto personagem e monstro têm vida. A ordem de um turno é:

1. Ler uma ação válida.
2. Validar os recursos e executar a ação; falhas de recurso voltam ao menu com `continue`.
3. Encerrar imediatamente se uma fuga funcionar.
4. Aplicar sangramento, se o monstro ainda estiver vivo.
5. Conceder a recompensa e retornar vitória se o monstro morreu.
6. Executar a reação do monstro e verificar a vida do personagem no laço.

O sangramento da Mortalha é aplicado antes da primeira reação após o Virote e nas duas seguintes. Um monstro que morrer pelo golpe ou pelo sangramento não contra-ataca. A defesa é uma variável local reiniciada a cada turno.

Cada encontro usa uma cópia local de um modelo de monstro da fase. O dano modifica essa cópia via ponteiro; não danifica o modelo que será usado no próximo encontro. Ao fugir e retornar ao chefe, um novo encontro começa com o chefe inteiro.

## 7. Dados e sementes

`rand() % lados + 1` produz valores no intervalo desejado. `srand` inicializa o gerador uma única vez na `main`, evitando reinicialização a cada jogada. `time(NULL)` varia a semente de partidas comuns. `--seed 42` permite investigar uma partida repetível no mesmo ambiente.

O projeto usa a fórmula simples de dados sugerida no enunciado. O operador módulo pode introduzir um pequeno viés estatístico dependendo de `RAND_MAX`; é adequado ao protótipo didático, sem pretensão de aleatoriedade criptográfica.

## 8. XP e bônus

`ganharXP` atualiza o XP disponível e o total recebido. O `while` permite subir mais de um nível com uma única recompensa grande. Exemplo: receber 125 XP no nível 1 gasta 40 para chegar ao nível 2, depois 80 para chegar ao nível 3, deixando 5 XP.

`aplicarBonusAleatorio` tem cinco resultados possíveis e altera o personagem via ponteiro. A recompensa só é chamada após uma vitória. Casas resolvidas têm uma marca para impedir uma segunda recompensa do mesmo evento.

## 9. Entrada inválida e EOF

`lerOpcao` lê uma linha com `fgets` e verifica cada caractere. Isso evita o problema de um `scanf` falhar e deixar a mesma letra presa no fluxo de entrada. Linhas grandes demais são descartadas até a quebra de linha ou EOF. O acúmulo numérico possui limite antes da multiplicação, impedindo overflow.

Uma linha contendo `1x` não vira opção 1. Entradas vazias ou com dois números são rejeitadas. Espaços nas extremidades são aceitos. EOF retorna `-1`; as funções propagam esse resultado até encerrar a partida com segurança.

### Apresentação em ASCII

Em `interface.c`, `linha`, `centro`, `paragrafo` e `borda` montam painéis com largura fixa. `paragrafo` quebra textos em palavras; `barra` transforma a proporção entre valor atual e máximo em uma sequência de `#` e `.`. `campo` e `bytesAteColunas` medem as letras em UTF-8 sem cortar uma sequência de bytes no meio. Assim, “poção” usa cinco colunas, embora ocupe sete bytes. O jogo continua usando apenas a biblioteca padrão.

`mostrarTabuleiro` desenha um vetor de linhas de texto. Na primeira e na terceira fileiras, as casas aparecem da esquerda para a direita; na fileira central, a ordem visual é invertida. Essa transformação muda somente o desenho: `fase->posicao` continua sendo um índice do vetor original, e `moverJogador` continua aplicando as mesmas regras. As setas mostram essa correspondência.

As paisagens, máscaras e criaturas são vetores constantes de strings. `mostrarCombate` consulta os atributos atuais em cada turno para redesenhar as barras e os recursos disponíveis, sem modificar o estado do personagem. Não há cores ANSI nem dependência de comandos específicos de um sistema operacional.

### Tempo de leitura

`narrar` imprime um bloco e chama `aguardarLeitura`. Esta executa `fflush(stdout)` antes de esperar: isso garante que o trecho apareça antes da pausa, mesmo quando a saída é redirecionada. `tempoLeituraMs` usa o número de palavras e a pontuação para escolher a duração, com limites e multiplicadores por ritmo.

`mostrarRolagem` aguarda sua leitura antes de retornar para `executarEvento`. O evento narra o resultado e também aguarda antes que a campanha redesenhe o mapa. As regras e as rolagens aleatórias continuam as mesmas.

O jogo usa `clock()` para respeitar a lista de bibliotecas do enunciado. Essa é uma espera ativa: não é uma função de suspensão do sistema operacional. A opção Sem pausas desativa esse custo e é usada nos testes de lógica. Um teste de integração separado mede as pausas reais com o ritmo rápido.

## 10. Como conferir e personalizar

Compile com avisos ativados, execute os testes e jogue com cada máscara. Para uma personalização pequena, altere a descrição de um monstro em `inicializarFase` e confirme onde a mensagem aparece. Para alterar um atributo inicial, procure `criarPersonagem`; ajuste também as tabelas do README e os testes que verificam aquele valor.

Não acrescente uma mecânica apenas no texto: uma regra documentada precisa existir no código. Ao mudar o custo de uma habilidade, confira a constante, a validação, o efeito, a interface e a documentação correspondente.

## Perguntas para ensaiar a apresentação

- Qual a diferença entre passar `p` e passar `&p` quando `p` já é um ponteiro?
- Por que `mostrarPersonagem` recebe `const Personagem *`?
- Onde o jogo garante que vida não fica negativa?
- Como a casa obrigatória evita uma missão impossível de concluir?
- O que acontece ao fugir do chefe e por que a próxima rolagem funciona?
- Por que o monstro do encontro é uma cópia do modelo da fase?
- Por que `continue` é usado quando falta foco?
- Qual a diferença entre XP disponível e XP total recebido?
- Onde se impede um contra-ataque de um monstro morto?
- Quais atributos da segunda e da terceira fase tornam os encontros mais difíceis?

Responda apontando as funções correspondentes e mostrando uma execução. Use o roteiro de vídeo como apoio à explicação com suas próprias palavras.
