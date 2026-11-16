CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -Wpedantic -finput-charset=UTF-8 -fexec-charset=UTF-8 -O2
CPPFLAGS = -Isrc
SOURCES = src/main.c src/interface.c src/personagem.c src/combate.c src/tabuleiro.c
CORE = src/interface.c src/personagem.c src/combate.c src/tabuleiro.c
HEADERS = $(wildcard src/*.h)

ifeq ($(OS),Windows_NT)
EXE = .exe
endif

.PHONY: all test clean
all: tres_mascaras$(EXE)

tres_mascaras$(EXE): $(SOURCES) $(HEADERS)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SOURCES) -o $@

testes$(EXE): tests/test_core.c $(CORE) $(HEADERS)
	$(CC) $(CPPFLAGS) $(CFLAGS) tests/test_core.c $(CORE) -o $@

test: testes$(EXE)
	./testes$(EXE)

clean:
	$(RM) tres_mascaras$(EXE) testes$(EXE)
