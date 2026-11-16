# Conferência de entrega

## Requisitos implementados

| Requisito do enunciado | Local no projeto |
|---|---|
| Três personagens com atributos, equipamentos e especiais próprios | `criarPersonagem`, `usarHabilidade`, `tipos.h` |
| Três fases e pelo menos três monstros por fase | `inicializarFase`, em `tabuleiro.c` |
| Dificuldade progressiva | Tabela de vida, ataque e defesa dos monstros por fase |
| `struct` para personagem e monstro | `tipos.h`, também com `Casa` e `Fase` |
| Enumerações significativas | Classe, casa, ação e resultados em `tipos.h` |
| Alterações de status por ponteiros | `personagem.c`, `combate.c`, `tabuleiro.c` |
| Funções e organização modular | Cinco arquivos `.c` e respectivos cabeçalhos |
| Movimento com dado e limites | `rolarDado`, `moverJogador` |
| Casas de combate, tesouro, cura, armadilha, missão e vazias | `executarEvento` |
| Combate com ataque, defesa, especial e fuga | `combate` |
| XP, bônus aleatório e níveis | `recompensarVitoria`, `ganharXP`, `aplicarBonusAleatorio` |
| Missões que liberam as fases | `missaoCumprida`, `jogarCampanha` |
| Vitória, derrota e resumo | `mostrarResumo` |
| Entrada inválida e EOF | `lerOpcao` e testes de terminal |
| Bibliotecas padrão permitidas e C99 | Includes em `src/`, comandos no README |
| Compilação GCC sem avisos | `docs/VALIDACAO.md` |
| README com história, personagens, monstros, regras e compilação | `README.md` |
| Ilustração da interface | Arte ASCII no código e prévias em `docs/` |

## Etapas pessoais pendentes

- Preencher nome e matrícula no início do README.
- Ler, compreender e personalizar o código para conseguir explicar a implementação.
- Gravar até 10 minutos de tela com sua própria narração; roteiro em `video/roteiro.md`.
- Hospedar o vídeo, verificar o acesso e colocar o link real no README e em `video/link_video.md`.
- Publicar os arquivos em um repositório GitHub/GitLab acessível ao professor.
- Entregar o link conforme o canal e o prazo da disciplina.

Não há conta remota, link de vídeo ou identidade inventada no pacote. A pasta está pronta para ser revisada e enviada a um repositório; nenhuma publicação foi efetuada.

## Publicar após a revisão

Crie um repositório vazio na sua conta e envie a pasta do projeto. Com Git, os comandos abaixo podem ser executados no terminal dentro dessa pasta:

```sh
git init
git add .
git commit -m "Implementa Tres Mascaras em C99"
git branch -M main
git remote add origin URL_REAL_DO_SEU_REPOSITORIO
git push -u origin main
```

Substitua `URL_REAL_DO_SEU_REPOSITORIO` pelo endereço criado na sua conta. Caso o repositório já exista e tenha histórico, siga o fluxo desse repositório. O `.gitignore` exclui executáveis e arquivos temporários; o código-fonte permite recompilar o jogo.

Antes de enviar, abra a página do repositório e o link do vídeo em uma janela sem login para conferir o acesso solicitado na atividade.
