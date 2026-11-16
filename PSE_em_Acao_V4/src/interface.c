#include "pse.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#ifdef _WIN32
#include <windows.h>
#endif

#define RESET   "\033[0m"
#define RED     "\033[38;5;160m"
#define GOLD    "\033[38;5;178m"
#define WHITE   "\033[97m"
#define GRAY    "\033[38;5;245m"
#define GREEN   "\033[38;5;71m"
#define PURPLE  "\033[38;5;135m"

static void preparar_terminal(void) {
#ifdef _WIN32
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    if (out != INVALID_HANDLE_VALUE && GetConsoleMode(out, &mode))
        SetConsoleMode(out, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
}

static void linha(void) {
    printf(GOLD "======================================================================\n" RESET);
}

static void cabecalho(const char *titulo) {
    linha();
    printf(GOLD "  [ ORDO INTEGRA-PSE ] " WHITE "%s\n" RESET, titulo);
    linha();
}

static void banner(void) {
    printf(RED);
    puts("  ____  ____  _____   _____ __  __    _    ____ _____ ___ ___  _   _");
    puts(" |  _ \\|  ___| ____| | ____|  \\/  |  / \\  / ___|_   _|_ _/ _ \\| \\ | |");
    puts(" | |_) | |_  |  _|   |  _| | |\\/| | / _ \\ \\___ \\ | |  | | | | |  \\| |");
    puts(" |  __/|  _| | |___  | |___| |  | |/ ___ \\ ___) || |  | | |_| | |\\  |");
    puts(" |_|   |_|   |_____| |_____|_|  |_/_/   \\_\\____/ |_| |___\\___/|_| \\_|");
    printf(RESET);
    printf(GOLD "                 PROTOCOLO SANCTUM // VERSÃO III\n" RESET);
    printf(GRAY "       SISTEMA DE REGISTRO E VIGILÂNCIA DAS AÇÕES COLETIVAS\n" RESET);
    linha();
}

static void ler_texto(const char *mensagem, char *destino, size_t tamanho) {
    for (;;) {
        size_t comprimento;
        int caractere;
        printf(WHITE "%s" RESET, mensagem);
        if (fgets(destino, (int)tamanho, stdin) == NULL) {
            destino[0] = '\0';
            return;
        }
        comprimento = strlen(destino);
        if (comprimento > 0 && destino[comprimento - 1] == '\n') {
            destino[--comprimento] = '\0';
        } else {
            while ((caractere = getchar()) != '\n' && caractere != EOF) { }
        }
        while (comprimento > 0 && isspace((unsigned char)destino[comprimento - 1]))
            destino[--comprimento] = '\0';
        char *inicio = destino;
        while (*inicio && isspace((unsigned char)*inicio)) inicio++;
        if (inicio != destino) memmove(destino, inicio, strlen(inicio) + 1);
        if (!pse_texto_vazio(destino)) return;
        printf(RED "  [FALHA DE VALIDAÇÃO] Campo obrigatório. Tente novamente.\n" RESET);
    }
}

static int ler_inteiro(const char *mensagem, int minimo, int maximo) {
    char buffer[80], *fim;
    long valor;
    for (;;) {
        printf(WHITE "%s" RESET, mensagem);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) return minimo;
        valor = strtol(buffer, &fim, 10);
        while (isspace((unsigned char)*fim)) fim++;
        if (fim == buffer || *fim != '\0' || valor < minimo || valor > maximo) {
            printf(RED "  [ENTRADA INVÁLIDA] Informe um valor entre %d e %d.\n" RESET, minimo, maximo);
            continue;
        }
        return (int)valor;
    }
}

static void ler_data(char *data) {
    do {
        ler_texto("Data prevista (DD/MM/AAAA): ", data, PSE_DATA_MAX);
        if (!pse_data_valida(data)) printf(RED "  Data inválida. Verifique dia, mês e ano.\n" RESET);
    } while (!pse_data_valida(data));
}

static int contem_sem_case(const char *texto, const char *consulta) {
    if (*consulta == '\0') return 1;
    for (; *texto; texto++) {
        const char *a = texto, *b = consulta;
        while (*a && *b && tolower((unsigned char)*a) == tolower((unsigned char)*b)) {
            a++; b++;
        }
        if (*b == '\0') return 1;
    }
    return 0;
}

static void exibir_acao(const Acao *a) {
    printf("\n" GOLD "--------------------------------------------------------------------\n" RESET);
    printf(GOLD "  REGISTRO: " WHITE "%s\n" RESET, a->codigo);
    printf(GRAY "  Unidade: " WHITE "%s\n", a->escola);
    printf(GRAY "  Doutrina/tema: " WHITE "%s\n", a->tema);
    printf(GRAY "  Data: " WHITE "%s\n", a->data);
    printf(GRAY "  Público-alvo: " WHITE "%s\n", a->publico_alvo);
    printf(GRAY "  Responsável: " WHITE "%s\n", a->responsavel);
    printf(GRAY "  Efetivo previsto: " WHITE "%d\n", a->participantes_previstos);
    printf(GRAY "  Efetivo presente: " WHITE "%d\n", a->participantes_efetivos);
    printf(GRAY "  Estado: " GREEN "%s\n" RESET, pse_situacao_texto(a->situacao));
}

static void informar_status(PseStatus status) {
    switch (status) {
        case PSE_OK: printf(GREEN "  [CONFIRMADO] Operação concluída.\n" RESET); break;
        case PSE_ERRO_DUPLICADO: printf(RED "  [CONFLITO] Já existe um registro com esse código.\n" RESET); break;
        case PSE_ERRO_LIMITE: printf(RED "  [LIMITE] Capacidade máxima de registros atingida.\n" RESET); break;
        case PSE_ERRO_NAO_ENCONTRADO: printf(RED "  [AUSENTE] Registro não encontrado.\n" RESET); break;
        case PSE_ERRO_ESTADO: printf(RED "  [ESTADO INVÁLIDO] Ação precisa estar marcada como realizada.\n" RESET); break;
        case PSE_ERRO_VALIDACAO: printf(RED "  [VALIDAÇÃO] Os dados informados não são válidos.\n" RESET); break;
        case PSE_ERRO_IO: printf(RED "  [ARQUIVO] Não foi possível acessar os dados.\n" RESET); break;
        case PSE_ERRO_FORMATO: printf(RED "  [INTEGRIDADE] Arquivo inválido ou incompatível.\n" RESET); break;
        default: printf(RED "  [ERRO] Operação não concluída.\n" RESET); break;
    }
}

static void cadastrar(Repositorio *repo) {
    Acao acao = {0};
    cabecalho("INCORPORAÇÃO DE NOVA AÇÃO");
    ler_texto("Código único: ", acao.codigo, sizeof(acao.codigo));
    ler_texto("Escola/unidade: ", acao.escola, sizeof(acao.escola));
    ler_texto("Tema da ação: ", acao.tema, sizeof(acao.tema));
    ler_data(acao.data);
    ler_texto("Público-alvo: ", acao.publico_alvo, sizeof(acao.publico_alvo));
    ler_texto("Responsável: ", acao.responsavel, sizeof(acao.responsavel));
    acao.participantes_previstos = ler_inteiro("Participantes previstos: ", 0, INT_MAX);
    acao.participantes_efetivos = 0;
    acao.situacao = ACAO_PLANEJADA;
    informar_status(repositorio_adicionar(repo, &acao));
}

static void listar(const Repositorio *repo) {
    cabecalho("REGISTROS SOB CUSTÓDIA");
    if (repo->quantidade == 0) {
        puts(GRAY "  Nenhuma ação registrada no arquivo de campanha." RESET);
        return;
    }
    for (size_t i = 0; i < repo->quantidade; i++)
        exibir_acao(repositorio_por_indice(repo, i));
    printf("\n" GOLD "  Total de registros: " WHITE "%zu\n" RESET, repo->quantidade);
}

static void pesquisar(const Repositorio *repo) {
    int criterio, encontrados = 0;
    char termo[PSE_TEXTO_MAX];
    cabecalho("BUSCA NOS REGISTROS");
    if (repo->quantidade == 0) {
        puts(GRAY "  Não existem registros para pesquisar." RESET);
        return;
    }
    criterio = ler_inteiro("  1 - Código | 2 - Escola | 3 - Tema: ", 1, 3);
    ler_texto("Termo de pesquisa: ", termo, sizeof(termo));
    for (size_t i = 0; i < repo->quantidade; i++) {
        const Acao *a = &repo->itens[i];
        const char *campo = criterio == 1 ? a->codigo : criterio == 2 ? a->escola : a->tema;
        if (contem_sem_case(campo, termo)) {
            exibir_acao(a);
            encontrados++;
        }
    }
    printf("\n" GOLD "  Registros localizados: " WHITE "%d\n" RESET, encontrados);
}

static void atualizar(Repositorio *repo) {
    char codigo[PSE_CODIGO_MAX];
    int situacao, efetivos = 0;
    cabecalho("ALTERAÇÃO DE ESTADO OPERACIONAL");
    ler_texto("Código da ação: ", codigo, sizeof(codigo));
    situacao = ler_inteiro("1 - Planejada | 2 - Realizada | 3 - Cancelada: ", 1, 3);
    if (situacao == ACAO_REALIZADA)
        efetivos = ler_inteiro("Participantes efetivos: ", 0, INT_MAX);
    informar_status(repositorio_atualizar_situacao(repo, codigo, (SituacaoAcao)situacao, efetivos));
}

static void registrar_participantes(Repositorio *repo) {
    char codigo[PSE_CODIGO_MAX];
    int efetivos;
    cabecalho("REGISTRO DO EFETIVO PARTICIPANTE");
    ler_texto("Código da ação: ", codigo, sizeof(codigo));
    efetivos = ler_inteiro("Quantidade efetiva: ", 0, INT_MAX);
    informar_status(repositorio_registrar_participantes(repo, codigo, efetivos));
}

static void relatorio(const Repositorio *repo) {
    Relatorio r;
    repositorio_gerar_relatorio(repo, &r);
    cabecalho("BALANÇO GERAL DA OPERAÇÃO");
    printf("  Ações cadastradas ............ %zu\n", r.total);
    printf("  Planejadas ................... %zu\n", r.planejadas);
    printf("  Realizadas ................... %zu\n", r.realizadas);
    printf("  Canceladas ................... %zu\n", r.canceladas);
    printf("  Participantes previstos* ..... %lld\n", r.previstos_realizadas);
    printf("  Participantes efetivos ....... %lld\n", r.efetivos_realizadas);
    printf(GOLD "  Índice de participação ....... %.2f%%\n" RESET, r.percentual_participacao);
    printf(GRAY "  * Considera somente ações realizadas.\n" RESET);
    linha();
}

void interface_executar(Repositorio *repo) {
    int opcao;
    preparar_terminal();
    do {
        banner();
        printf(WHITE "  [1] " GOLD "Registrar ação\n");
        printf(WHITE "  [2] " GOLD "Consultar todos os registros\n");
        printf(WHITE "  [3] " GOLD "Pesquisar por código, escola ou tema\n");
        printf(WHITE "  [4] " GOLD "Atualizar estado da ação\n");
        printf(WHITE "  [5] " GOLD "Registrar participantes efetivos\n");
        printf(WHITE "  [6] " GOLD "Emitir relatório consolidado\n");
        printf(WHITE "  [7] " RED  "Encerrar protocolo\n" RESET);
        linha();
        opcao = ler_inteiro(GOLD "  Selecione uma operação: " RESET, 1, 7);
        switch (opcao) {
            case 1: cadastrar(repo); break;
            case 2: listar(repo); break;
            case 3: pesquisar(repo); break;
            case 4: atualizar(repo); break;
            case 5: registrar_participantes(repo); break;
            case 6: relatorio(repo); break;
            case 7: printf(RED "\n  Protocolo encerrado. Salvaguardando registros...\n" RESET); break;
        }
        if (opcao != 7) {
            printf(GRAY "\n  Pressione ENTER para retornar ao painel..." RESET);
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) { }
        }
    } while (opcao != 7);
}
