#ifndef PSE_H
#define PSE_H

#include <stddef.h>

#define PSE_MAX_ACOES 500
#define PSE_CODIGO_MAX 20
#define PSE_TEXTO_MAX 100
#define PSE_DATA_MAX 11
#define PSE_ARQUIVO_DADOS "acoes.dat"

typedef enum {
    ACAO_PLANEJADA = 1,
    ACAO_REALIZADA = 2,
    ACAO_CANCELADA = 3
} SituacaoAcao;

typedef struct {
    char codigo[PSE_CODIGO_MAX];
    char escola[PSE_TEXTO_MAX];
    char tema[PSE_TEXTO_MAX];
    char data[PSE_DATA_MAX];
    char publico_alvo[PSE_TEXTO_MAX];
    char responsavel[PSE_TEXTO_MAX];
    int participantes_previstos;
    int participantes_efetivos;
    SituacaoAcao situacao;
} Acao;

typedef struct {
    Acao itens[PSE_MAX_ACOES];
    size_t quantidade;
} Repositorio;

typedef struct {
    size_t total;
    size_t planejadas;
    size_t realizadas;
    size_t canceladas;
    long long previstos_realizadas;
    long long efetivos_realizadas;
    double percentual_participacao;
} Relatorio;

typedef enum {
    PSE_OK = 0,
    PSE_ERRO_ARGUMENTO,
    PSE_ERRO_LIMITE,
    PSE_ERRO_DUPLICADO,
    PSE_ERRO_NAO_ENCONTRADO,
    PSE_ERRO_ESTADO,
    PSE_ERRO_VALIDACAO,
    PSE_ERRO_IO,
    PSE_ERRO_FORMATO
} PseStatus;

/* Validação e domínio */
int pse_data_valida(const char *data);
int pse_situacao_valida(int situacao);
const char *pse_situacao_texto(SituacaoAcao situacao);
int pse_texto_vazio(const char *texto);

/* Repositório em memória */
void repositorio_inicializar(Repositorio *repo);
PseStatus repositorio_adicionar(Repositorio *repo, const Acao *acao);
Acao *repositorio_por_codigo(Repositorio *repo, const char *codigo);
const Acao *repositorio_por_indice(const Repositorio *repo, size_t indice);
PseStatus repositorio_atualizar_situacao(Repositorio *repo, const char *codigo,
                                         SituacaoAcao situacao, int participantes_efetivos);
PseStatus repositorio_registrar_participantes(Repositorio *repo, const char *codigo,
                                              int participantes_efetivos);
void repositorio_gerar_relatorio(const Repositorio *repo, Relatorio *relatorio);

/* Persistência versionada */
PseStatus armazenamento_carregar(Repositorio *repo, const char *caminho);
PseStatus armazenamento_salvar(const Repositorio *repo, const char *caminho);

/* Interface */
void interface_executar(Repositorio *repo);

#endif
