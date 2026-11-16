"""Testes de terminal sem dependencias externas; Python 3.9+ apenas para testar."""
import argparse
import codecs
import os
from pathlib import Path
import queue
import re
import subprocess
import threading
import time


def executar(exe, entrada="", argumentos=()):
    extras = () if argumentos == ("--help",) else ("--sem-pausas",)
    return subprocess.run([str(exe), *extras, *argumentos], input=entrada,
                          capture_output=True, text=True, encoding="utf-8", timeout=5)


def jogar(exe, classe, seed, estrategia="normal", ritmo="sem", marcos=None):
    """Controla somente as mesmas opcoes de terminal disponiveis a qualquer jogador."""
    processo = subprocess.Popen([str(exe), "--ritmo", ritmo, "--seed", str(seed)], stdin=subprocess.PIPE,
                                stdout=subprocess.PIPE, stderr=subprocess.STDOUT, bufsize=0)
    partes = queue.Queue()

    def ler():
        decoder = codecs.getincrementaldecoder("utf-8")()
        while True:
            trecho = os.read(processo.stdout.fileno(), 4096)
            if not trecho:
                fim = decoder.decode(b"", final=True)
                if fim:
                    partes.put(fim)
                partes.put(None)
                return
            partes.put(decoder.decode(trecho))

    leitor = threading.Thread(target=ler, daemon=True)
    leitor.start()
    historico, pendente = "", ""
    iniciou, fugiu, defendeu, chefe_atual = False, False, False, False
    mapas = 0
    prazo = time.monotonic() + 15
    try:
        while time.monotonic() < prazo:
            try:
                trecho = partes.get(timeout=0.2)
            except queue.Empty:
                continue
            if trecho is None:
                break
            historico += trecho
            pendente += trecho
            if marcos is not None:
                for chave, texto in [("rolagem", "O DESTINO ROLA"),
                                     ("evento", "Espinhos sob as cinzas!")]:
                    if chave not in marcos and texto in historico:
                        marcos[chave] = time.monotonic()
                if "retorno" not in marcos and historico.count("C A M I N H O") >= 2:
                    marcos["retorno"] = time.monotonic()
            encontro = re.search(r"\n([^\n]+) \(nível \d+\) - Vida", pendente)
            if encontro:
                chefe_atual = encontro.group(1).strip("| ") in {
                    "Executor de Ferrugem", "Prior das Cinzas", "A Providência"}
            if pendente.endswith("Máscara [1-3; 0 para voltar]: "):
                escolha = classe
            elif pendente.endswith("Ação: "):
                barras = re.findall(r"PV \[[#.]+\] (\d+)/(\d+).*?PV \[[#.]+\] (\d+)/(\d+)", pendente)
                recursos = re.findall(r"Foco (\d+)/\d+\s+Poções (\d+)", pendente)
                assert barras and recursos, "Estado de combate ausente"
                vida, maxima, inimigo, _ = map(int, barras[-1])
                foco, pocoes = map(int, recursos[-1])
                if estrategia == "derrota":
                    escolha = 2
                elif (estrategia == "fuga" or
                      (estrategia == "fuga-chefe" and chefe_atual)) and not fugiu:
                    escolha = 4
                    # A proxima tentativa continua ate existir fuga bem-sucedida.
                elif vida <= maxima - 35 and pocoes:
                    escolha = 5
                elif not defendeu:
                    escolha, defendeu = 2, True
                elif foco >= 8 and (classe != 1 or vida > 8):
                    escolha = 3
                else:
                    escolha = 1
            elif pendente.endswith("Opção: "):
                if "[1] Nova jornada" in pendente:
                    escolha = 0 if iniciou else 1
                    iniciou = True
                else:
                    status = re.findall(r"Vida (\d+)/(\d+) \| Foco \d+/\d+ \| Poções (\d+)", pendente)
                    assert status, "Estado do tabuleiro ausente"
                    vida, maxima, pocoes = map(int, status[-1])
                    if estrategia != "derrota" and vida <= maxima - 35 and pocoes:
                        escolha = 3
                    else:
                        escolha = 1
                    mapas += 1
                    if estrategia == "um-evento" and mapas > 1:
                        escolha = 0
            else:
                continue
            if "Você escapou." in pendente:
                fugiu = True
            processo.stdin.write(f"{escolha}\n".encode("ascii"))
            processo.stdin.flush()
            pendente = ""
        else:
            raise AssertionError("Partida excedeu 15 segundos; possivel ciclo sem saida")
        processo.stdin.close()
        assert processo.wait(timeout=3) == 0
        assert "Até a próxima jornada." in historico
        return historico
    finally:
        if processo.poll() is None:
            processo.kill()
        processo.wait()
        leitor.join(timeout=1)
        processo.stdout.close()


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("executavel", type=Path)
    parser.add_argument("--salvar-exemplo", type=Path)
    args = parser.parse_args()
    exe = args.executavel.resolve()
    checks = 0
    for inválida in ["abc", "-1", "+1", "1x", "1 2", "1.0", "", "99999999999999999999", "9" * 500]:
        r = executar(exe, inválida + "\n0\n")
        assert r.returncode == 0 and ("inválida" in r.stdout or "muito longa" in r.stdout)
        checks += 1
    assert executar(exe, "  0  \n").returncode == 0
    assert executar(exe).returncode == 0
    assert executar(exe, "1\n").returncode == 0
    assert executar(exe, "1\n1\n").returncode == 0
    assert executar(exe, "1\n0\n0\n").returncode == 0
    checks += 5
    r = executar(exe, "1\n1\n1\n", ("--seed", "42"))
    assert "Ação: " in r.stdout and "Jornada encerrada" in r.stdout
    checks += 1
    for argumentos in [("--seed",), ("--seed", "-1"), ("--seed", "abc"),
                       ("--seed", "999999999999999999999"), ("--outro", "4")]:
        assert executar(exe, argumentos=argumentos).returncode == 1
        checks += 1
    for seed in ["0", "42"]:
        assert executar(exe, "0\n", ("--seed", seed)).returncode == 0
        checks += 1
    assert "Uso:" in executar(exe, argumentos=("--help",)).stdout
    checks += 1
    vitorias = 0
    exemplo = None
    for classe in (1, 2, 3):
        for seed in range(10):
            log = jogar(exe, classe, seed)
            assert "VITÓRIA!" in log or "DERROTA." in log
            if "VITÓRIA!" in log:
                vitorias += 1
                assert log.count("MISSÃO DA FASE") == 3
                assert log.count("RELÍQUIA OBTIDA") == 3
                if exemplo is None:
                    exemplo = log
            checks += 1
        # A campanha deve ser vencivel com todas as mascaras.
        assert any("VITÓRIA!" in jogar(exe, classe, seed) for seed in range(3))
        checks += 1
    derrota = jogar(exe, 2, 42, "derrota")
    assert "DERROTA." in derrota and "VITÓRIA!" not in derrota
    checks += 1
    fuga = jogar(exe, 3, 42, "fuga")
    assert "Você escapou." in fuga
    checks += 1
    fuga_chefe = jogar(exe, 3, 42, "fuga-chefe")
    assert "Você recua para a casa 17" in fuga_chefe
    assert "casa 17 -> casa 18" in fuga_chefe and "VITÓRIA!" in fuga_chefe
    checks += 1
    # Mesmas decisoes e semente no mesmo executavel produzem a mesma partida.
    assert jogar(exe, 2, 42) == jogar(exe, 2, 42)
    checks += 1
    for argumentos in [("--ritmo",), ("--ritmo", "ultra"), ("--ritmo", "")]:
        assert executar(exe, argumentos=argumentos).returncode == 1
        checks += 1
    for ritmo in ["normal", "rapido", "lento", "sem"]:
        assert executar(exe, "0\n", ("--ritmo", ritmo)).returncode == 0
        checks += 1
    menu = executar(exe, "3\n2\n0\n")
    assert "Atual: Rápido" in menu.stdout and menu.returncode == 0
    checks += 1
    texto = jogar(exe, 1, 42)
    for palavra in ["Máscara", "Aço", "Poções", "Missão", "Maturação", "Você"]:
        assert palavra in texto, palavra
    assert "\ufffd" not in texto and "Ã§" not in texto and "Ã£" not in texto
    assert all(len(linha) == 76 for linha in texto.splitlines()
               if linha.startswith(("|", "+---")))
    checks += 1
    # Encontra uma rolagem inicial 6 na implementacao local de rand().
    seed_armadilha = next(seed for seed in range(64)
                          if "d6=6:" in executar(exe, "1\n1\n1\n0\n0\n",
                                                ("--seed", str(seed))).stdout)
    marcos = {}
    jogar(exe, 1, seed_armadilha, "um-evento", ritmo="rapido", marcos=marcos)
    assert set(marcos) == {"rolagem", "evento", "retorno"}, marcos
    assert marcos["evento"] - marcos["rolagem"] >= 1.0, marcos
    assert marcos["retorno"] - marcos["evento"] >= 0.8, marcos
    checks += 1
    if args.salvar_exemplo and exemplo:
        args.salvar_exemplo.write_text(exemplo, encoding="utf-8")
    print(f"OK: {checks} cenarios de integracao. {vitorias}/30 campanhas normais terminaram em vitoria.")
    print("Pausas reais (ritmo rapido): dado -> evento "
          f"{marcos['evento'] - marcos['rolagem']:.2f}s; evento -> mapa "
          f"{marcos['retorno'] - marcos['evento']:.2f}s.")


if __name__ == "__main__":
    main()
