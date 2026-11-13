#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#define MAX_ACOES 100
#define TAM_CODIGO 20
#define TAM_TEXTO 100
#define TAM_DATA 11

typedef enum { PLANEJADA=1, REALIZADA=2, CANCELADA=3 } Situacao;

typedef struct {
    char codigo[TAM_CODIGO], escola[TAM_TEXTO], tema[TAM_TEXTO];
    char data[TAM_DATA], publico_alvo[TAM_TEXTO], responsavel[TAM_TEXTO];
    int previstos, efetivos;
    Situacao situacao;
} Acao;

static Acao acoes[MAX_ACOES];
static int total = 0;

void ler_texto(const char *msg, char *buf, size_t tam) {
    for (;;) {
        printf("%s", msg);
        if (!fgets(buf, (int)tam, stdin)) { buf[0]='\0'; return; }
        size_t n=strlen(buf);
        if (n && buf[n-1]=='\n') buf[n-1]='\0';
        else { int c; while ((c=getchar())!='\n' && c!=EOF) {} }
        char *p=buf;
        while (isspace((unsigned char)*p)) p++;
        if (p!=buf) memmove(buf,p,strlen(p)+1);
        n=strlen(buf);
        while (n && isspace((unsigned char)buf[n-1])) buf[--n]='\0';
        if (n) return;
        puts("Campo obrigatório. Tente novamente.");
    }
}

int ler_numero(const char *msg, int min, int max) {
    char b[80], *fim; long v;
    for (;;) {
        printf("%s",msg);
        if (!fgets(b,sizeof b,stdin)) return min;
        v=strtol(b,&fim,10);
        while (isspace((unsigned char)*fim)) fim++;
        if (fim==b || *fim || v<min || v>max) {
            printf("Informe um número entre %d e %d.\\n",min,max); continue;
        }
        return (int)v;
    }
}

int data_valida(const char *s) {
    int d,m,a,lim[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    if (strlen(s)!=10 || s[2]!='/' || s[5]!='/') return 0;
    for(int i=0;i<10;i++) if(i!=2 && i!=5 && !isdigit((unsigned char)s[i])) return 0;
    d=(s[0]-'0')*10+s[1]-'0'; m=(s[3]-'0')*10+s[4]-'0';
    a=(s[6]-'0')*1000+(s[7]-'0')*100+(s[8]-'0')*10+s[9]-'0';
    if(a<2000 || m<1 || m>12 || d<1) return 0;
    if(a%400==0 || (a%4==0 && a%100!=0)) lim[1]=29;
    return d<=lim[m-1];
}

void ler_data(char *s) {
    do { ler_texto("Data prevista (DD/MM/AAAA): ",s,TAM_DATA);
         if(!data_valida(s)) puts("Data inválida."); } while(!data_valida(s));
}

int buscar_codigo(const char *s) {
    for(int i=0;i<total;i++) if(strcmp(acoes[i].codigo,s)==0) return i;
    return -1;
}

int contem(const char *s,const char *q) {
    if(!*q) return 1;
    for(;*s;s++) {
        const char *a=s,*b=q;
        while(*a && *b && tolower((unsigned char)*a)==tolower((unsigned char)*b)){a++;b++;}
        if(!*b) return 1;
    }
    return 0;
}

const char *situacao_nome(Situacao s) {
    switch(s){case PLANEJADA:return "Planejada";case REALIZADA:return "Realizada";
    case CANCELADA:return "Cancelada";default:return "Indefinida";}
}

void imprimir(const Acao *a) {
    printf("\\n----------------------------------------\\nCódigo: %s\\nEscola: %s\\nTema: %s\\nData: %s\\nPúblico-alvo: %s\\nResponsável: %s\\nPrevistos: %d\\nEfetivos: %d\\nSituação: %s\\n",
    a->codigo,a->escola,a->tema,a->data,a->publico_alvo,a->responsavel,a->previstos,a->efetivos,situacao_nome(a->situacao));
}

void cadastrar(void) {
    if(total>=MAX_ACOES){printf("Limite de %d ações atingido.\\n",MAX_ACOES);return;}
    Acao a={0}; puts("\\n=== CADASTRO DE AÇÃO ===");
    do {ler_texto("Código único: ",a.codigo,sizeof a.codigo);
        if(buscar_codigo(a.codigo)>=0) puts("Código duplicado. Informe outro.");}
    while(buscar_codigo(a.codigo)>=0);
    ler_texto("Escola: ",a.escola,sizeof a.escola);
    ler_texto("Tema: ",a.tema,sizeof a.tema);
    ler_data(a.data);
    ler_texto("Público-alvo: ",a.publico_alvo,sizeof a.publico_alvo);
    ler_texto("Responsável: ",a.responsavel,sizeof a.responsavel);
    a.previstos=ler_numero("Quantidade prevista: ",0,INT_MAX);
    a.efetivos=0; a.situacao=PLANEJADA;
    acoes[total++]=a; puts("Ação cadastrada com sucesso. Situação inicial: Planejada.");
}

void listar(void) {
    if(!total){puts("Nenhuma ação cadastrada.");return;}
    for(int i=0;i<total;i++) imprimir(&acoes[i]);
    printf("\\nTotal: %d ação(ões).\\n",total);
}

void pesquisar(void) {
    if(!total){puts("Nenhuma ação cadastrada.");return;}
    int op=ler_numero("Pesquisar por: 1-Código  2-Escola  3-Tema: ",1,3), n=0;
    char q[TAM_TEXTO]; ler_texto("Termo: ",q,sizeof q);
    for(int i=0;i<total;i++){
        const char *campo=op==1?acoes[i].codigo:(op==2?acoes[i].escola:acoes[i].tema);
        if(contem(campo,q)){imprimir(&acoes[i]);n++;}
    }
    printf("Encontradas: %d\\n",n);
}

void atualizar(void) {
    char c[TAM_CODIGO]; if(!total){puts("Nenhuma ação cadastrada.");return;}
    ler_texto("Código da ação: ",c,sizeof c); int i=buscar_codigo(c);
    if(i<0){puts("Registro não encontrado.");return;}
    int s=ler_numero("Nova situação: 1-Planejada  2-Realizada  3-Cancelada: ",1,3);
    acoes[i].situacao=(Situacao)s;
    if(s==REALIZADA) acoes[i].efetivos=ler_numero("Participantes efetivos: ",0,INT_MAX);
    else if(s==PLANEJADA) acoes[i].efetivos=0;
    printf("Situação atualizada: %s.\\n",situacao_nome(acoes[i].situacao));
}

void registrar_participantes(void) {
    char c[TAM_CODIGO]; if(!total){puts("Nenhuma ação cadastrada.");return;}
    ler_texto("Código da ação: ",c,sizeof c); int i=buscar_codigo(c);
    if(i<0){puts("Registro não encontrado.");return;}
    if(acoes[i].situacao!=REALIZADA){puts("A ação precisa estar marcada como realizada.");return;}
    acoes[i].efetivos=ler_numero("Quantidade efetiva: ",0,INT_MAX);
    puts("Participantes atualizados.");
}

void relatorio(void) {
    int p=0,r=0,c=0; long long previstos=0,efetivos=0;
    for(int i=0;i<total;i++){
        if(acoes[i].situacao==PLANEJADA)p++;
        else if(acoes[i].situacao==CANCELADA)c++;
        else {r++;previstos+=acoes[i].previstos;efetivos+=acoes[i].efetivos;}
    }
    double pct=previstos>0?(double)efetivos*100.0/(double)previstos:0.0;
    puts("\\n====== RELATÓRIO PSE EM AÇÃO ======");
    printf("Total de ações: %d\\nPlanejadas: %d\\nRealizadas: %d\\nCanceladas: %d\\n",total,p,r,c);
    printf("Previstos nas ações realizadas: %lld\\nEfetivos nas ações realizadas: %lld\\nParticipação: %.2f%%\\n",previstos,efetivos,pct);
    puts("===================================");
}

int main(void) {
    int op;
    do {
        puts("\\n====================================");
        puts("              PSE EM AÇÃO");
        puts("====================================");
        puts("1 - Cadastrar ação");
        puts("2 - Listar ações");
        puts("3 - Pesquisar ação");
        puts("4 - Atualizar situação");
        puts("5 - Registrar participantes");
        puts("6 - Gerar relatório");
        puts("7 - Encerrar");
        op=ler_numero("Escolha: ",1,7);
        switch(op){
            case 1:cadastrar();break; case 2:listar();break; case 3:pesquisar();break;
            case 4:atualizar();break; case 5:registrar_participantes();break;
            case 6:relatorio();break; case 7:puts("Sistema encerrado.");break;
        }
    } while(op!=7);
    return 0;
}
