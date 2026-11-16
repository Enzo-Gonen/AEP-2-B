# PSE em Ação — V4 | Protocolo Sanctum

Sistema de planejamento e acompanhamento de ações coletivas do Programa Saúde na Escola, desenvolvido em C para execução em terminal.

## Identidade visual

A V4 mantém a identidade uma apresentação inspirada em interfaces grimdark de ficção científica militar: nomenclatura cerimonial, hierarquia visual, molduras, alertas e paleta textual em vermelho, dourado e cinza. A estética é exclusivamente visual; o propósito acadêmico e as regras funcionais do PSE permanecem inalterados.

## Funcionalidades

- Cadastro de ações com validação de campos e código único.
- Listagem completa dos registros.
- Pesquisa por código, escola ou tema.
- Atualização de situação: planejada, realizada ou cancelada.
- Registro de participantes efetivos em ações realizadas.
- Relatório quantitativo por situação e percentual de participação.
- Persistência local automática ao encerrar a aplicação.
- Validação de integridade e compatibilidade do arquivo de dados.
- Interface de terminal com suporte a cores ANSI e fallback textual.

## Estrutura

```text
PSE_em_Acao_V4/
├── include/pse.h
├── src/
│   ├── main.c
│   ├── dominio.c
│   ├── repositorio.c
│   ├── armazenamento.c
│   └── interface.c
├── tests/teste_dominio.c
├── docs/arquitetura.md
├── Makefile
├── EXECUTAR_WINDOWS.bat
└── README.md
```

## Instalação automática e inicialização — Windows

1. Extraia o ZIP para uma pasta local.
2. Dê duplo clique em `EXECUTAR_WINDOWS.bat`.
3. O inicializador verifica o GCC. Se estiver ausente, instala o MSYS2 pelo `winget` e instala o pacote GCC UCRT64 com o `pacman`.
4. Em seguida, compila o projeto e inicia o executável automaticamente.

**Requisitos do bootstrap:** Windows 10/11 atualizado, Windows PowerShell e Windows Package Manager (`winget`). A instalação do compilador pode solicitar permissões administrativas, conforme a configuração do Windows. O script não altera políticas permanentes do sistema nem instala componentes não relacionados ao projeto.

Se o `winget` não estiver disponível, instale/atualize o aplicativo **App Installer** pela Microsoft Store e execute o BAT novamente.

### Linux, macOS ou WSL
Instale GCC pelo gerenciador de pacotes da sua distribuição e execute `make`.

### Linux, macOS ou WSL
```bash
make
./pse_em_acao
```

Compilação manual:
```bash
gcc -Iinclude -std=c99 -Wall -Wextra -Wpedantic -O2 \
  src/main.c src/dominio.c src/repositorio.c src/armazenamento.c src/interface.c \
  -o pse_em_acao
```

## Testes
```bash
make test
```

## Persistência e compatibilidade

O programa cria `acoes.dat` no diretório de execução. A V4 mantém o formato de dados versionado utilizado pela V2, para que registros existentes possam ser carregados desde que a estrutura e a compilação sejam compatíveis. Faça backup do arquivo antes de atualizar qualquer versão.

## Regras de negócio

- O código da ação é único.
- A data deve estar no formato `DD/MM/AAAA` e representar uma data válida.
- Participantes efetivos só podem ser informados quando a ação estiver como realizada.
- O percentual considera somente ações realizadas: efetivos ÷ previstos × 100.
- Se não houver participantes previstos em ações realizadas, o percentual será 0%.

## Privacidade

Utilize exclusivamente dados fictícios e informações coletivas. Não registre nomes, diagnósticos, prontuários ou quaisquer dados clínicos individuais de estudantes. A aplicação não realiza diagnóstico, triagem ou prescrição.

## Integrantes

- Enzo Costa Gonçalves
- João Paulo do Amaral
- Pedro Lucas de Melo
- Rhasller

## Repositório
https://github.com/Enzo-Gonen/AEP-2-B
