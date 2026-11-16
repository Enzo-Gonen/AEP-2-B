#include "pse.h"

#include <ctype.h>
#include <string.h>

int pse_texto_vazio(const char *texto) {
    if (texto == NULL) return 1;
    while (*texto) {
        if (!isspace((unsigned char)*texto)) return 0;
        texto++;
    }
    return 1;
}

int pse_situacao_valida(int situacao) {
    return situacao >= ACAO_PLANEJADA && situacao <= ACAO_CANCELADA;
}

const char *pse_situacao_texto(SituacaoAcao situacao) {
    switch (situacao) {
        case ACAO_PLANEJADA: return "Planejada";
        case ACAO_REALIZADA: return "Realizada";
        case ACAO_CANCELADA: return "Cancelada";
        default: return "Indefinida";
    }
}

int pse_data_valida(const char *data) {
    int dia, mes, ano;
    static const int dias_mes[] = {31,28,31,30,31,30,31,31,30,31,30,31};

    if (data == NULL || strlen(data) != 10 || data[2] != '/' || data[5] != '/') return 0;

    for (int i = 0; i < 10; i++) {
        if (i != 2 && i != 5 && !isdigit((unsigned char)data[i])) return 0;
    }

    dia = (data[0]-'0')*10 + data[1]-'0';
    mes = (data[3]-'0')*10 + data[4]-'0';
    ano = (data[6]-'0')*1000 + (data[7]-'0')*100 + (data[8]-'0')*10 + data[9]-'0';

    if (ano < 2000 || mes < 1 || mes > 12 || dia < 1) return 0;

    int limite = dias_mes[mes - 1];
    if (mes == 2 && ((ano % 400 == 0) || (ano % 4 == 0 && ano % 100 != 0))) limite = 29;
    return dia <= limite;
}
