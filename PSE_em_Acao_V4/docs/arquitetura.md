# Arquitetura da aplicação — V4 Protocolo Sanctum

## Camadas

- **Interface (`interface.c`)**: menus, leitura de entradas e apresentação dos resultados. A identidade visual grimdark é implementada nesta camada e não altera as regras do domínio.
- **Domínio (`dominio.c`)**: validação de datas, estados e textos.
- **Repositório (`repositorio.c`)**: operações sobre a coleção em memória e regras de consistência.
- **Persistência (`armazenamento.c`)**: leitura e gravação versionada dos registros.
- **Inicialização (`main.c`)**: carregamento da base, ciclo da aplicação e salvamento no encerramento.

## Fluxo de execução

1. Inicializa a estrutura do repositório.
2. Tenta carregar `acoes.dat`.
3. Executa o menu e encaminha as operações às funções de domínio/repositório.
4. Ao encerrar, grava os dados em arquivo temporário e promove o arquivo para o nome definitivo.

## Diretrizes visuais da V4

- Paleta ANSI: vermelho, dourado, branco e cinza.
- Molduras e separadores para organizar os painéis.
- Terminologia temática aplicada apenas aos rótulos da interface.
- Mensagens de validação e erro preservam significado claro para o usuário.
- Nenhuma dependência externa foi adicionada.

## Decisões técnicas

- C99, sem dependências externas.
- Limite explícito de 500 ações.
- Estruturas e funções separadas por responsabilidade.
- Status de retorno para distinguir sucesso, validação, duplicidade, ausência de registro e falhas de I/O.
- Arquivo de dados com assinatura e versão para detectar formatos incompatíveis.
- Testes automatizados para validação de datas, duplicidade e cálculo do relatório.

## Limitações conhecidas

- A persistência é local, sem criptografia e sem controle de concorrência entre múltiplas instâncias.
- O arquivo binário depende da representação das estruturas da compilação utilizada; mantenha backup antes de trocar a versão.
- A pesquisa textual ignora diferença entre maiúsculas e minúsculas para caracteres ASCII; acentuação é comparada conforme os bytes da codificação.
- O salvamento ocorre ao encerrar normalmente a aplicação.


## Bootstrap Windows — V4

O `EXECUTAR_WINDOWS.bat` chama `scripts/bootstrap.ps1`, que procura um GCC existente. Caso não encontre, utiliza `winget` para instalar MSYS2 e `pacman` para instalar `mingw-w64-ucrt-x86_64-gcc`. Depois do bootstrap, o BAT adiciona o caminho conhecido do compilador ao PATH do processo, compila o projeto e inicia o executável. O bootstrap é idempotente: se GCC já estiver disponível, não reinstala o toolchain.
