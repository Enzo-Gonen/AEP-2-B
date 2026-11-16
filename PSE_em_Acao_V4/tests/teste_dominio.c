#include "pse.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static void testar_datas(void) {
    assert(pse_data_valida("29/02/2024"));
    assert(!pse_data_valida("29/02/2023"));
    assert(!pse_data_valida("31/04/2026"));
    assert(!pse_data_valida("1/01/2026"));
    assert(pse_data_valida("15/08/2026"));
}

static void testar_repositorio(void) {
    Repositorio repo;
    Acao a = {0};
    Relatorio r;

    repositorio_inicializar(&repo);
    strcpy(a.codigo, "PSE001");
    strcpy(a.escola, "Escola Modelo");
    strcpy(a.tema, "Saúde bucal");
    strcpy(a.data, "15/08/2026");
    strcpy(a.publico_alvo, "Ensino Fundamental");
    strcpy(a.responsavel, "Equipe de Saúde");
    a.participantes_previstos = 40;
    a.situacao = ACAO_PLANEJADA;

    assert(repositorio_adicionar(&repo, &a) == PSE_OK);
    assert(repositorio_adicionar(&repo, &a) == PSE_ERRO_DUPLICADO);
    assert(repositorio_atualizar_situacao(&repo, "PSE001", ACAO_REALIZADA, 32) == PSE_OK);
    repositorio_gerar_relatorio(&repo, &r);
    assert(r.total == 1 && r.realizadas == 1);
    assert(r.previstos_realizadas == 40 && r.efetivos_realizadas == 32);
    assert(r.percentual_participacao > 79.99 && r.percentual_participacao < 80.01);
}

int main(void) {
    testar_datas();
    testar_repositorio();
    puts("Todos os testes passaram.");
    return 0;
}
