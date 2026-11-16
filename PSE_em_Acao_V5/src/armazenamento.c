#include "pse.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define ARQ_MAGIC "PSEACT02"
#define ARQ_VERSAO 2u

typedef struct {
    char magic[8];
    uint32_t versao;
    uint32_t quantidade;
} CabecalhoArquivo;

static int registro_arquivo_valido(const Acao *a) {
    return memchr(a->codigo, '\0', sizeof(a->codigo)) != NULL &&
           memchr(a->escola, '\0', sizeof(a->escola)) != NULL &&
           memchr(a->tema, '\0', sizeof(a->tema)) != NULL &&
           memchr(a->data, '\0', sizeof(a->data)) != NULL &&
           memchr(a->publico_alvo, '\0', sizeof(a->publico_alvo)) != NULL &&
           memchr(a->responsavel, '\0', sizeof(a->responsavel)) != NULL &&
           !pse_texto_vazio(a->codigo) &&
           !pse_texto_vazio(a->escola) &&
           !pse_texto_vazio(a->tema) &&
           pse_data_valida(a->data) &&
           !pse_texto_vazio(a->publico_alvo) &&
           !pse_texto_vazio(a->responsavel) &&
           a->participantes_previstos >= 0 &&
           a->participantes_efetivos >= 0 &&
           pse_situacao_valida(a->situacao) &&
           (a->situacao == ACAO_REALIZADA || a->participantes_efetivos == 0);
}

PseStatus armazenamento_carregar(Repositorio *repo, const char *caminho) {
    FILE *arquivo;
    CabecalhoArquivo cabecalho;
    Repositorio temporario;

    if (repo == NULL || caminho == NULL) return PSE_ERRO_ARGUMENTO;
    arquivo = fopen(caminho, "rb");
    if (arquivo == NULL) return PSE_ERRO_IO;

    if (fread(&cabecalho, sizeof(cabecalho), 1, arquivo) != 1) {
        fclose(arquivo);
        return PSE_ERRO_FORMATO;
    }

    if (memcmp(cabecalho.magic, ARQ_MAGIC, 8) != 0 ||
        cabecalho.versao != ARQ_VERSAO ||
        cabecalho.quantidade > PSE_MAX_ACOES) {
        fclose(arquivo);
        return PSE_ERRO_FORMATO;
    }

    repositorio_inicializar(&temporario);
    for (uint32_t i = 0; i < cabecalho.quantidade; i++) {
        Acao acao;
        if (fread(&acao, sizeof(acao), 1, arquivo) != 1 ||
            !registro_arquivo_valido(&acao) ||
            repositorio_por_codigo(&temporario, acao.codigo) != NULL) {
            fclose(arquivo);
            return PSE_ERRO_FORMATO;
        }
        temporario.itens[temporario.quantidade++] = acao;
    }

    if (fgetc(arquivo) != EOF) {
        fclose(arquivo);
        return PSE_ERRO_FORMATO;
    }

    fclose(arquivo);
    *repo = temporario;
    return PSE_OK;
}

PseStatus armazenamento_salvar(const Repositorio *repo, const char *caminho) {
    FILE *arquivo;
    char temporario[512];
    CabecalhoArquivo cabecalho;

    if (repo == NULL || caminho == NULL) return PSE_ERRO_ARGUMENTO;
    if (repo->quantidade > PSE_MAX_ACOES) return PSE_ERRO_LIMITE;
    if (snprintf(temporario, sizeof(temporario), "%s.tmp", caminho) >= (int)sizeof(temporario))
        return PSE_ERRO_ARGUMENTO;

    arquivo = fopen(temporario, "wb");
    if (arquivo == NULL) return PSE_ERRO_IO;

    memcpy(cabecalho.magic, ARQ_MAGIC, 8);
    cabecalho.versao = ARQ_VERSAO;
    cabecalho.quantidade = (uint32_t)repo->quantidade;

    if (fwrite(&cabecalho, sizeof(cabecalho), 1, arquivo) != 1) {
        fclose(arquivo);
        remove(temporario);
        return PSE_ERRO_IO;
    }

    for (size_t i = 0; i < repo->quantidade; i++) {
        if (!registro_arquivo_valido(&repo->itens[i]) ||
            fwrite(&repo->itens[i], sizeof(Acao), 1, arquivo) != 1) {
            fclose(arquivo);
            remove(temporario);
            return PSE_ERRO_IO;
        }
    }

    if (fflush(arquivo) != 0 || fclose(arquivo) != 0) {
        remove(temporario);
        return PSE_ERRO_IO;
    }

    /* A gravação temporária evita deixar o arquivo principal parcialmente escrito. */
    if (remove(caminho) != 0) {
        /* Arquivo inexistente também é uma condição normal na primeira execução. */
    }
    if (rename(temporario, caminho) != 0) {
        remove(temporario);
        return PSE_ERRO_IO;
    }
    return PSE_OK;
}
