CC ?= gcc
CFLAGS ?= -std=c99 -Wall -Wextra -Wpedantic -O2
CPPFLAGS = -Iinclude
SOURCES = src/main.c src/dominio.c src/repositorio.c src/armazenamento.c src/interface.c
TARGET = pse_em_acao

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(SOURCES) include/pse.h
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SOURCES) -o $(TARGET)

test:
	$(CC) $(CPPFLAGS) $(CFLAGS) tests/teste_dominio.c src/dominio.c src/repositorio.c -o teste_dominio
	./teste_dominio

clean:
	rm -f $(TARGET) $(TARGET).exe teste_dominio teste_dominio.exe acoes.dat acoes.dat.tmp
