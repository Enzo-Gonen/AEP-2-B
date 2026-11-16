#include "pse.h"

#include <string.h>

void repositorio_inicializar(Repositorio *repo) {
    if (repo != NULL) repo->quantidade = 0;
}

Acao *repositorio_por_codigo(Repositorio *repo, const char *codigo) {
    if (repo == NULL || codigo == NULL) return NULL;
    for (size_t i = 0; i < repo->quantidade; i++) {
        if (strcmp(repo->itens[i].codigo, codigo) == 0) return &repo->itens[i];
    }
    return NULL;
}

const Acao *repositorio_por_indice(const Repositorio *repo, size_t indice) {
    if (repo == NULL || indice >= repo->quantidade) return NULL;
    return &repo->itens[indice];
}

static int acao_valida(const Acao *acao) {
    return acao != NULL &&
           !pse_texto_vazio(acao->codigo) &&
           !pse_texto_vazio(acao->escola) &&
           !pse_texto_vazio(acao->tema) &&
           pse_data_valida(acao->data) &&
           !pse_texto_vazio(acao->publico_alvo) &&
           !pse_texto_vazio(acao->responsavel) &&
           acao->participantes_previstos >= 0 &&
           acao->participantes_efetivos >= 0 &&
           pse_situacao_valida(acao->situacao) &&
           (acao->situacao == ACAO_REALIZADA ||
            acao->participantes_efetivos == 0);
}

PseStatus repositorio_adicionar(Repositorio *repo, const Acao *acao) {
    if (repo == NULL || acao == NULL) return PSE_ERRO_ARGUMENTO;
    if (!acao_valida(acao)) return PSE_ERRO_VALIDACAO;
    if (repo->quantidade >= PSE_MAX_ACOES) return PSE_ERRO_LIMITE;
    if (repositorio_por_codigo(repo, acao->codigo) != NULL) return PSE_ERRO_DUPLICADO;
    repo->itens[repo->quantidade++] = *acao;
    return PSE_OK;
}

PseStatus repositorio_atualizar_situacao(Repositorio *repo, const char *codigo,
                                         SituacaoAcao situacao, int participantes_efetivos) {
    Acao *acao;
    if (repo == NULL || codigo == NULL) return PSE_ERRO_ARGUMENTO;
    if (!pse_situacao_valida(situacao) || participantes_efetivos < 0)
        return PSE_ERRO_VALIDACAO;
    acao = repositorio_por_codigo(repo, codigo);
    if (acao == NULL) return PSE_ERRO_NAO_ENCONTRADO;
    if (situacao != ACAO_REALIZADA && participantes_efetivos != 0)
        return PSE_ERRO_VALIDACAO;

    acao->situacao = situacao;
    acao->participantes_efetivos = situacao == ACAO_REALIZADA ? participantes_efetivos : 0;
    return PSE_OK;
}

PseStatus repositorio_registrar_participantes(Repositorio *repo, const char *codigo,
                                              int participantes_efetivos) {
    Acao *acao;
    if (repo == NULL || codigo == NULL) return PSE_ERRO_ARGUMENTO;
    if (participantes_efetivos < 0) return PSE_ERRO_VALIDACAO;
    acao = repositorio_por_codigo(repo, codigo);
    if (acao == NULL) return PSE_ERRO_NAO_ENCONTRADO;
    if (acao->situacao != ACAO_REALIZADA) return PSE_ERRO_ESTADO;
    acao->participantes_efetivos = participantes_efetivos;
    return PSE_OK;
}

void repositorio_gerar_relatorio(const Repositorio *repo, Relatorio *relatorio) {
    if (relatorio == NULL) return;
    memset(relatorio, 0, sizeof(*relatorio));
    if (repo == NULL) return;

    relatorio->total = repo->quantidade;
    for (size_t i = 0; i < repo->quantidade; i++) {
        const Acao *acao = &repo->itens[i];
        switch (acao->situacao) {
            case ACAO_PLANEJADA:
                relatorio->planejadas++;
                break;
            case ACAO_CANCELADA:
                relatorio->canceladas++;
                break;
            case ACAO_REALIZADA:
                relatorio->realizadas++;
                relatorio->previstos_realizadas += acao->participantes_previstos;
                relatorio->efetivos_realizadas += acao->participantes_efetivos;
                break;
            default:
                break;
        }
    }

    if (relatorio->previstos_realizadas > 0) {
        relatorio->percentual_participacao =
            (double)relatorio->efetivos_realizadas * 100.0 /
            (double)relatorio->previstos_realizadas;
    }
}
