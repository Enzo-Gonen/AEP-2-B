# PSE em Ação

Sistema acadêmico em linguagem C para planejamento e acompanhamento de ações coletivas do Programa Saúde na Escola.

## Funcionalidades
- Cadastro de ações com validação de campos e código único.
- Listagem de ações.
- Pesquisa por código, escola ou tema (sem diferenciar maiúsculas/minúsculas).
- Atualização da situação: planejada, realizada ou cancelada.
- Registro de participantes efetivos em ações realizadas.
- Relatório quantitativo e percentual de participação.
- Validação de data, quantidades e opções do menu.

## Compilar e executar

### Windows (MinGW-w64)
Execute `EXECUTAR_WINDOWS.bat` ou rode:
```bat
gcc -std=c99 -Wall -Wextra -pedantic src\main.c -o pse_em_acao.exe
pse_em_acao.exe
```

### Linux, macOS ou WSL
```bash
gcc -std=c99 -Wall -Wextra -pedantic src/main.c -o pse_em_acao
./pse_em_acao
```

## Regra do relatório
O percentual é calculado apenas sobre ações realizadas:
`(participantes efetivos / participantes previstos nas ações realizadas) * 100`.
Se o total previsto for zero, o percentual será 0%.

## Limites
- Até 100 ações por execução.
- Os registros ficam em memória e são perdidos ao encerrar o programa. Persistência em arquivo não foi incluída porque o escopo inicial prevê armazenamento em memória e trata arquivos como melhoria opcional.
- Utilize somente dados fictícios e coletivos. Não insira dados pessoais ou clínicos de estudantes.

## Integrantes
- Enzo Costa Gonçalves
- João Paulo do Amaral
- Pedro Lucas de Melo
- Rhasller

Repositório: https://github.com/Enzo-Gonen/AEP-2-B
