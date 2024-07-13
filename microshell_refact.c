#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

void	ft_error(char *err, char *arg)
{
	while(*err)
		write(2, err++, 1);
	if (arg)
		while(*arg)
			write(2, arg++, 1);
	write(2, "\n", 1);
}

char	ft_exec(char **argv, int i, int tmp_fd, char **env)
{
	argv[i] = NULL;
	dup2(tmp_fd, STDIN_FILENO);
	close(tmp_fd);
	execve(argv[0], argv, env);
	ft_error("error: cannot execute ", argv[0]);
	exit(1);
}

void	cd(char **argv, int i)
{
	if (i != 2)
		ft_error("error: cd: bad arguments", NULL);
	else if (chdir(argv[1]) != 0)
		ft_error("error: cd: cannot change directory to", argv[1]);
}

void	simple_exec(char **argv, int i, int tmp_fd, char **env)
{
	if ( fork() == 0 )
			ft_exec(argv, i, tmp_fd, env);
	else
	{
		close(tmp_fd);
		while(waitpid(-1, NULL, WUNTRACED) != -1)
			;
		tmp_fd = dup(STDIN_FILENO);
	}
}

void	pipe_exec(char **argv, int i, int tmp_fd, char **env)
{
	int fd[2];
	pipe(fd);
	if ( fork() == 0 )
	{
		dup2(fd[1], STDOUT_FILENO);
		close(fd[0]);
		close(fd[1]);
		ft_exec(argv, i, tmp_fd, env);
	}
	else
	{
		close(fd[1]);
		close(tmp_fd);
		tmp_fd = fd[0];
	}
}

int	main(int argc, char **argv, char **env)
{
	int tmp_fd;
	int i;
	(void)argc;

	i = 0;
	tmp_fd = dup(STDIN_FILENO);
	while (argv[i] && argv[i + 1])
	{
		argv = &argv[i + 1];
		i = 0;
		while (argv[i] && strcmp(argv[i], ";") && strcmp(argv[i], "|"))
			i++;
		if (strcmp(argv[0], "cd") == 0)
				cd(argv, i);
		else if (i != 0 && (argv[i] == NULL || strcmp(argv[i], ";") == 0))
				simple_exec(argv, i, tmp_fd, env);
		else if (i != 0 && strcmp(argv[i], "|") == 0)
				pipe_exec(argv, i, tmp_fd, env);
	}
	close(tmp_fd);
	return (0);
}
