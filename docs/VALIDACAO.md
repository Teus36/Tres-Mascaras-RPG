# Validação da implementação

Validação realizada em **15 de novembro de 2026**, em Windows x64, com GCC 16.2.0 (distribuição portátil w64devkit 2.11.0). A distribuição foi obtida da publicação oficial e seu SHA-256 foi conferido antes do uso. O compilador é apenas uma ferramenta de desenvolvimento e não está incluído no pacote final.

## Resultados

| Verificação | Resultado |
|---|---|
| Compilação C99 com `-Wall -Wextra -Wpedantic -Werror -O2` | Sem erros ou avisos |
| Análise estática GCC `-fanalyzer`, também com `-Werror` | Sem diagnósticos |
| Testes das regras em `tests/test_core.c` | 923 verificações passaram |
| Testes do fluxo real de terminal em `tests/test_integration.py` | 70 cenários passaram |
| Campanhas automáticas com sementes 0 a 9, para cada máscara | 30 de 30 chegaram à vitória |
| Interface revisada: 11 telas/estados de apresentação | Português em UTF-8, arte ASCII e molduras de 76 colunas |
| GNU Make: reconstrução do jogo e execução dos testes | Passou |
| Script `compilar.ps1` e opção `--help` | Passaram |

As campanhas usam exclusivamente entradas do terminal, sem alterar vida, dados, atributos ou recompensas. A estratégia usa defesa, habilidades e poções disponíveis no jogo. Os resultados mostram que a campanha é vencível com as três máscaras nessas sementes; não garantem vitória para qualquer estratégia ou semente.

A revisão visual inclui título, seleção das máscaras, as três paisagens do mapa, ficha do personagem, combate, história, regras, as seis faces do dado e encerramentos. As prévias do mapa e do combate foram renderizadas a partir da saída das funções C e inspecionadas. Uma campanha completa também foi conferida sem linhas acima de 76 colunas. Após a alteração da interface, a compilação com avisos como erros, a análise estática e as duas suítes de testes foram executadas novamente.

A revisão de narrativa acrescenta cálculo de tempo por tamanho do texto, ritmos configuráveis e decodificação UTF-8 incremental nos testes de terminal. O cenário de temporização mede a saída de um processo real: no ritmo rápido, o dado antecedeu o evento em aproximadamente 1,45 s e o evento antecedeu o retorno ao mapa em aproximadamente 1,27 s. Os valores são observações dessa execução; o cálculo exato depende do texto e do ritmo. Os demais testes usam Sem pausas para validar as mesmas regras sem aguardar a leitura.

## Cobertura dos testes de regras

- Criação das três classes, equipamentos, atributos e recursos iniciais.
- Dano e cura com limites, vida zero e poções sem efeito em vida cheia.
- Recuperação limitada de foco e restauração entre fases.
- XP excedente e múltiplas subidas de nível na mesma recompensa.
- Bônus aleatório alterando exatamente um atributo por sorteio.
- Rolagens de d6 no intervalo, dano mínimo e fórmula de ataque comum.
- Custo e rejeição da Ira em vida insuficiente ou sem foco.
- Chama ignorando a defesa e Virote com dano dobrado e sangramento limitado.
- Defesa reduzindo o dano, com arredondamento correto.
- Nove modelos de monstros e crescimento dos atributos por fase.
- Missão exigindo relíquia e chefe ao mesmo tempo.
- Movimento em todas as posições, com passos zero, comuns e muito grandes.
- Parada obrigatória na relíquia, eventos de uso único e armadilha fatal.

## Cobertura dos testes de terminal

- Letras, sinal, fração, sufixo inválido, linha vazia, dois números e entrada muito longa.
- Espaços válidos, cancelamento da escolha e EOF no menu, seleção, tabuleiro e combate.
- Argumentos e sementes inválidas, `--help` e sementes aceitas.
- Vitória nas três fases com três relíquias em 30 campanhas distintas.
- Derrota por uma estratégia que apenas defende.
- Fuga comum bem-sucedida.
- Fuga do chefe, recuo à casa 17, retorno à casa 18 e vitória posterior.
- Repetição idêntica da partida com a mesma semente e ações no mesmo executável.

## Comandos para repetir

Na raiz do projeto, com GCC no PATH:

```powershell
gcc -std=c99 -Wall -Wextra -Wpedantic -finput-charset=UTF-8 -fexec-charset=UTF-8 -Werror -O2 -Isrc src/main.c src/interface.c src/personagem.c src/combate.c src/tabuleiro.c -o tres_mascaras.exe
gcc -std=c99 -Wall -Wextra -Wpedantic -finput-charset=UTF-8 -fexec-charset=UTF-8 -Werror -O2 -Isrc tests/test_core.c src/interface.c src/personagem.c src/combate.c src/tabuleiro.c -o testes.exe
.\testes.exe
python tests/test_integration.py .\tres_mascaras.exe
```

Análise estática opcional, em um GCC que forneça `-fanalyzer`:

```powershell
gcc -std=c99 -Wall -Wextra -Wpedantic -finput-charset=UTF-8 -fexec-charset=UTF-8 -Werror -fanalyzer -O0 -Isrc src/main.c src/interface.c src/personagem.c src/combate.c src/tabuleiro.c -o tres_mascaras_analisado.exe
```

## Limites desta validação

A execução foi conferida em Windows de 64 bits. O código usa C99 e bibliotecas padrão, mas não houve execução nativa em Linux ou macOS nesta entrega. O teste de repetição vale para a mesma implementação de `rand()`. Não há vídeo narrado, publicação remota, imagens ou avaliação do professor incluídos nestes resultados.
