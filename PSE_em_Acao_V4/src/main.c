#include "pse.h"

#include <stdio.h>

int main(void) {
    Repositorio repo;
    PseStatus status;

    repositorio_inicializar(&repo);
    status = armazenamento_carregar(&repo, PSE_ARQUIVO_DADOS);

    if (status == PSE_OK) {
        printf("Base carregada: %zu registro(s).\n", repo.quantidade);
    } else if (status == PSE_ERRO_IO) {
        puts("[SANCTUM] Base ainda não criada. Uma nova base será iniciada.");
    } else {
        puts("Aviso: arquivo de dados inválido ou incompatível. A aplicação iniciou vazia.");
    }

    interface_executar(&repo);

    status = armazenamento_salvar(&repo, PSE_ARQUIVO_DADOS);
    if (status != PSE_OK) {
        fprintf(stderr, "ERRO: não foi possível salvar os dados (código %d).\n", status);
        return 1;
    }

    puts("[SANCTUM] Dados salvos com sucesso.");
    return 0;
}
