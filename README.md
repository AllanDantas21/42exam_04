A função `ft_exec`:

1.  **Separa o comando**: Coloca `NULL` em `argv[i]`, que serve como o fim do array de argumentos para o comando que será executado.
2.  **Redireciona o stdin**: O file descriptor `tmp_fd` é duplicado para o `STDIN_FILENO` (entrada padrão) usando `dup2()`. Util para o pipe.
3.  **Executa o comando**: A função `execve()` substitui o processo atual pelo programa especificado em `argv[0]`, usando os argumentos `argv` e o ambiente `env`.
4.  **Erro na execução**: Se `execve()` falhar, a função imprime uma mensagem de erro usando `ft_error` e encerra o processo com `exit(1)`.

-   **Inicialização**:
    
    -   `tmp_fd = dup(STDIN_FILENO)` faz uma duplicação do `STDIN_FILENO` (entrada padrão) para salvar o file descriptor original.
-   **Loop Principal**: O loop percorre os argumentos passados no `argv` (que representam comandos e seus argumentos).
    
    -   `argv = &argv[i + 1]`:  Move o argv para o primeiro parâmetro.
    -   `while (argv[i] && strcmp(argv[i], ";") && strcmp(argv[i], "|"))`: Esse loop principal percorre os argumentos até encontrar um ponto e vírgula (`;`), um pipe (`|`), ou o fim dos argumentos  (NULL).
-   **Tratamento do Comando `cd`**:
    
    -   Se o comando for `cd`, o código verifica se há exatamente dois argumentos (comando e destino do diretório). Caso contrário, ele exibe um erro.
    -   Usa a função `chdir()` para mudar o diretório. Em caso de falha, exibe um erro.
-   **Exec Sem Pipe**:
    
    -   Caso não haja um pipe (`|`), mas seja um comando regular (ou o separador `;` foi encontrado), o programa faz um `fork()`:
        -   No processo filho (`fork() == 0`), ele executa o  `ft_exec`.
        -   No processo pai, ele fecha o `tmp_fd` e espera pelo término dos processos filhos usando `waitpid()`
    -   Depois disso, `tmp_fd` é duplicado novamente para a entrada padrão (STDIN)
-   **Exec com Pipe**:

    -   Quando um pipe é encontrado, o programa cria um pipe com `pipe(fd)` e, em seguida, faz um `fork()`.
        -   O processo filho redireciona sua saída padrão para o de escrita do pipe (`fd[1]`) e executa o comando com `ft_exec`.
        -   O processo pai fecha a extremidade de escrita do pipe e ajusta `tmp_fd` para o extremo de leitura (`fd[0]`). Isso conecta a saída de um comando à entrada do próximo no pipeline.
-   **Finalização**: Após o loop, `close(tmp_fd)` é chamado para fechar o file descriptor original duplicado.

  ### Porque isso Funciona com Apenas 1 `fd[2]` e `tmp_fd`

-   **`fd[2]`** é reutilizado a cada criação de pipe. Você não precisa de múltiplos pares de descritores de pipe, porque assim que um pipe é usado, os descritores não são mais necessários:
    
    -   O processo atual escreve no `fd[1]`, e depois de usá-lo, o descritor de escrita é fechado.
    -   O próximo processo lê do `fd[0]`, e após isso, o descritor de leitura é fechado.
-   **`tmp_fd`** armazena o lado de leitura anterior (caso tenha havido um pipe), garantindo que o próximo comando possa ler corretamente da saída do processo anterior.
