	/* Por que a flag wuntraced?
		 -O objetivo de usar WUNTRACED neste contexto é garantir que o processo pai aguarde a conclusão do processo filho ou entre em um estado interrompido
		 antes de continuar a execução.
		Isso é necessário para evitar race conditions e garantir que o processo filho tenha concluído sua execução antes que o processo pai tente ler ou
		 gravar no canal ou modificar quaisquer recursos compartilhados.*/


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
		argv = &argv[i + 1];
		i = 0;
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
			else   			// close no tmp_fd, waitpid e redefinir o tmp_fd para o stdin;
			{
				close(tmp_fd);
				while(waitpid(-1, NULL, WUNTRACED) != -1) // lembrar da flag wuntraced
					;
				tmp_fd = dup(STDIN_FILENO);
			}
		}
		else if (i != 0 && strcmp(argv[i], "|") == 0) 
		{            // e por ultimo, tratar o pipe.
			pipe(fd);  // pipa o fd;
			if ( fork() == 0 ) // faz o fork pro exec 
			{   // processo filho
				dup2(fd[1], STDOUT_FILENO); // dup2 do fd[1] pro Out
				close(fd[0]);             // close nos fds
				close(fd[1]);
				ft_exec(argv, i, tmp_fd, env); // e exec
			}
			else
			{  // processo pai
				close(fd[1]);     // close no fd[1];
				close(tmp_fd);    // close no temp fd;
				tmp_fd = fd[0];   // reseta o tmp fd pro fd[0];
			}
		}
	}
	close(tmp_fd); // close no tmp_fd
	return (0);  // return
}
