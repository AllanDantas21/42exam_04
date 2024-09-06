/* pelo o que eu vi, da pra passar no exame com 0 ao inves da flag Wuntraced */

void	ft_error(char *err, char *arg)
{
	while(*err)
		write(2, err++, 1);
	if (arg)
		while(*arg)
			write(2, arg++, 1);
	write(2, "\n", 1);
}

char	ft_exec(char **argv, int i, int tmp_fd, char **env) //função de exec
{
	argv[i] = NULL;                  // -> o argv[i] nesse caso pode ser o "|" , ";". atribuimos o NULL para separar o comando para o execve
	dup2(tmp_fd, STDIN_FILENO);	 // -> no caso de ter tido um pipe anteriomente, fazemos o dup2 do tmp_fd para o stdin (tmp_fd tambem pode estar com o valor do stdin)
	close(tmp_fd);
	execve(argv[0], argv, env);	// se o execve funcionar, esse função vai ser encerrada nesse ponto, pois o execve mataria esse processo.
	ft_error("error: cannot execute ", argv[0]); // se o execve nao funcionar ele desse para essa função de error, e depois para o exit(1), e encerra esse processo.
	exit(1);
}

int	main(int argc, char **argv, char **env)
{
	int tmp_fd;  // fd temporario;
	int fd[2];   // fd para o pipe;
	int i;       // idx do argv
	(void)argc;  // silenciar o argc

	i = 0;
	tmp_fd = dup(STDIN_FILENO);  // duplico o  tmp fd para o stdin / -> tmp_fd vai guardar o fd[0] de um pipe, se houver pipe; 
	while (argv[i] && argv[i + 1])
	{
		argv = &argv[i + 1];  // na proxima iteração do loop, ele começa apartir do "|" ou ";"
		i = 0;                // e o i volta a ser zero
		while (argv[i] && strcmp(argv[i], ";") && strcmp(argv[i], "|")) // idx vai ser incrementado até um ; ou |
			i++;                        
		if (strcmp(argv[0], "cd") == 0)   // esse primeiro if é só para o CD
 		{ 
 			if (i != 2)                                             // se tiver mais que dois argumentos, printa erro;
				ft_error("error: cd: bad arguments", NULL);
			else if (chdir(argv[1]) != 0)                                               // else if chamando o chdir; 
				ft_error("error: cd: cannot change directory to", argv[1]);          //erro do chdir
		}
		else if (i != 0 && (argv[i] == NULL || strcmp(argv[i], ";") == 0))         // tratar o exec
		{                                                                          // puro
			if ( fork() == 0 )  // execa no processo filho 
				ft_exec(argv, i, tmp_fd, env);
			else   			// close no tmp_fd, waitpid e redefinir o tmp_fd para o stdin; -> * o tmp_fd pode estar guardando o fd[0] de um pipe anterior *
			{
 				close(tmp_fd);
				while(waitpid(-1, NULL, WUNTRACED) != -1) // lembrar da flag wuntraced
					;
				tmp_fd = dup(STDIN_FILENO);
			}
		}
		else if (i != 0 && strcmp(argv[i], "|") == 0)  // ultimo if para o pipe
		{  
			pipe(fd); 
			if ( fork() == 0 )
			{   // processo filho
				dup2(fd[1], STDOUT_FILENO); // dup2 do fd[1] pro out
				close(fd[0]);             // close nos fds
				close(fd[1]);
				ft_exec(argv, i, tmp_fd, env); // e exec
			}
			else
			{  // processo pai
				close(fd[1]);     // close no fd[1];
				close(tmp_fd);    // close no temp fd;
				tmp_fd = fd[0];   // atribui o tmp fd pro fd[0]; -> o tmp_fd vai ficar guardando o fd[0] desse pipe. para o caso de multiplos pipes
			}
		}
	}
	close(tmp_fd); // close no tmp_fd
	return (0);
}
