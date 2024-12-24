/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_pipeline.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsaiti <lsaiti@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/16 14:27:25 by qudomino          #+#    #+#             */
/*   Updated: 2024/12/23 19:50:32 by lsaiti           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// execute_pipeline.c
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include "../include/pipeline.h"
#include "../include/env.h"
#include "../include/execute_pipeline.h"
#include "../include/execute.h"
#include "../include/built_in.h"
#include "../include/utils.h"

int	is_builtin_cmd(char **argv)
{
	if (!argv || !argv[0])
		return (0);
	if (ft_strcmp(argv[0], "echo") == 0)
		return (1);
	if (ft_strcmp(argv[0], "cd") == 0)
		return (1);
	if (ft_strcmp(argv[0], "pwd") == 0)
		return (1);
	if (ft_strcmp(argv[0], "export") == 0)
		return (1);
	if (ft_strcmp(argv[0], "unset") == 0)
		return (1);
	if (ft_strcmp(argv[0], "env") == 0)
		return (1);
	if (ft_strcmp(argv[0], "exit") == 0)
		return (1);
	return (0);
}

static int	count_commands(t_command *cmd)
{
	int	count;

	count = 0;
	while (cmd)
	{
		count++;
		cmd = cmd->next;
	}
	return (count);
}

static void	close_pipes(int cmd_count, int (*pipes)[2])
{
	int	j;

	j = 0;
	while (j < cmd_count - 1)
	{
		close(pipes[j][0]);
		close(pipes[j][1]);
		j++;
	}
	free(pipes);
}

static int	setup_pipes(int cmd_count, int (*pipes)[2])
{
	int	i;
	int	j;

	i = 0;
	while (i < cmd_count - 1)
	{
		if (pipe(pipes[i]) == -1)
		{
			perror("pipe");
			j = 0;
			while (j < i)
			{
				close(pipes[j][0]);
				close(pipes[j][1]);
				j++;
			}
			free(pipes);
			return (-1);
		}
		i++;
	}
	return (0);
}

static int	open_input_file(t_command *cmd)
{
	int	in_fd;

	in_fd = open(cmd->infile, O_RDONLY);
	if (in_fd < 0)
	{
		perror(cmd->infile);
		return (-1);
	}
	return (in_fd);
}

static int	dup_input_fd(int fd)
{
	if (dup2(fd, STDIN_FILENO) == -1)
	{
		perror("dup2");
		close(fd);
		return (-1);
	}
	close(fd);
	return (0);
}

static int	setup_input_redirection(t_command *cmd)
{
	int	in_fd;

	if (cmd->in_fd != -1)
	{
		if (dup2(cmd->in_fd, STDIN_FILENO) == -1)
		{
			perror("dup2");
			return (-1);
		}
		close(cmd->in_fd);
	}
	else if (cmd->infile)
	{
		in_fd = open_input_file(cmd);
		if (in_fd == -1)
			return (-1);
		if (dup_input_fd(in_fd) == -1)
			return (-1);
	}
	return (0);
}

static int	configure_output_flags(t_command *cmd, int *flags)
{
	*flags = O_WRONLY | O_CREAT;
	if (cmd->append)
		*flags |= O_APPEND;
	else
		*flags |= O_TRUNC;
	return (0);
}

static int	open_output_file(t_command *cmd, int flags)
{
	int	out_fd;

	out_fd = open(cmd->outfile, flags, 0644);
	if (out_fd < 0)
	{
		perror(cmd->outfile);
		return (-1);
	}
	return (out_fd);
}

static int	dup_output_fd(int fd)
{
	if (dup2(fd, STDOUT_FILENO) == -1)
	{
		perror("dup2");
		close(fd);
		return (-1);
	}
	close(fd);
	return (0);
}

static int	setup_output_redirection(t_command *cmd)
{
	int	flags;
	int	out_fd;

	if (cmd->out_fd != -1)
	{
		if (dup2(cmd->out_fd, STDOUT_FILENO) == -1)
		{
			perror("dup2");
			return (-1);
		}
		close(cmd->out_fd);
	}
	else if (cmd->outfile)
	{
		configure_output_flags(cmd, &flags);
		out_fd = open_output_file(cmd, flags);
		if (out_fd == -1)
			return (-1);
		if (dup_output_fd(out_fd) == -1)
			return (-1);
	}
	return (0);
}

static int	setup_redirections(t_command *cmd)
{
	if (setup_input_redirection(cmd) == -1)
		return (-1);
	if (setup_output_redirection(cmd) == -1)
		return (-1);
	return (0);
}

static char	*get_command_path_or_exit(t_env *env, char *cmd)
{
	char	*path;

	path = get_command_path(env, cmd);
	if (!path)
	{
		if (g_cmd_error_code != 0)
			exit(g_cmd_error_code);
		else
			exit(127);
	}
	return (path);
}

static char	**prepare_envp(t_env *env)
{
	char	**envp_for_exec;

	envp_for_exec = env_to_char_array(env);
	if (!envp_for_exec)
	{
		perror("env_to_char_array");
		return (NULL);
	}
	return (envp_for_exec);
}

static void	execute_builtin(t_shell *shell, char **argv)
{
	run_builtin(shell, argv);
}

static void	execute_external(t_env *env, char **argv, char *pat, char **e_ex)
{
	int	k;

	(void)env;
	execve(pat, argv, e_ex);
	perror("execve");
	k = 0;
	while (e_ex[k])
	{
		free(e_ex[k]);
		k++;
	}
	free(e_ex);
	free(pat);
	exit(1);
}

static void	handle_signals(void)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
}

static int	duplicate_fds(t_exec_context *ctx)
{
	if (ctx->idx > 0)
	{
		if (dup2(ctx->pipes[ctx->idx - 1][0], STDIN_FILENO) == -1)
			return (-1);
	}
	if (ctx->idx < ctx->cmd_count - 1)
	{
		if (dup2(ctx->pipes[ctx->idx][1], STDOUT_FILENO) == -1)
			return (-1);
	}
	return (0);
}

static void	close_all_pipes(t_exec_context *ctx)
{
	int	j;

	j = 0;
	while (j < ctx->cmd_count -1)
	{
		close(ctx->pipes[j][0]);
		close(ctx->pipes[j][1]);
		j++;
	}
}

static void	execute_command(t_exec_context *ctx)
{
	char	*path;
	char	**envp_for_exec;

	if (is_builtin_cmd(ctx->cmd->argv))
	{
		execute_builtin(ctx->shell, ctx->cmd->argv);
		exit(0);
	}
	else
	{
		path = get_command_path_or_exit(ctx->env, ctx->cmd->argv[0]);
		envp_for_exec = prepare_envp(ctx->env);
		if (!envp_for_exec)
		{
			free(path);
			exit(1);
		}
		execute_external(ctx->env, ctx->cmd->argv, path, envp_for_exec);
	}
}

static void	child_process(t_exec_context *ctx)
{
	handle_signals();
	if (duplicate_fds(ctx) == -1)
	{
		perror("dup2");
		exit(1);
	}
	close_all_pipes(ctx);
	if (setup_redirections(ctx->cmd) == -1)
		exit(1);
	execute_command(ctx);
}

static pid_t	fork_and_execute(t_exec_context *ctx)
{
	pid_t	pid;

	pid = fork();
	if (pid == 0)
		child_process(ctx);
	else if (pid < 0)
		perror("fork");
	return (pid);
}

static void	wait_for_children(int cmd_count, pid_t *pids, int *last_exit_code)
{
	int	status;
	int	i;

	i = 0;
	while (i < cmd_count)
	{
		if (waitpid(pids[i], &status, 0) == -1)
			perror("waitpid");
		if (i == cmd_count - 1)
		{
			if (WIFEXITED(status))
				*last_exit_code = WEXITSTATUS(status);
			else if (WIFSIGNALED(status))
				*last_exit_code = 128 + WTERMSIG(status);
			else
				*last_exit_code = 1;
		}
		i++;
	}
}

static int	initialize_pipes(int cmd_count, int (**pipes)[2])
{
	if (cmd_count > 1)
	{
		*pipes = malloc(sizeof(int [2]) * (cmd_count - 1));
		if (!*pipes)
		{
			perror("malloc");
			return (-1);
		}
		if (setup_pipes(cmd_count, *pipes) == -1)
			return (-1);
	}
	return (0);
}

static t_exec_context	init_exec_context(t_exec_context_params *ctx_params)
{
	t_exec_context	ctx;

	ctx.cmd = ctx_params->cmd;
	ctx.idx = ctx_params->idx;
	ctx.cmd_count = ctx_params->cmd_count;
	ctx.pipes = ctx_params->pipes;
	ctx.env = ctx_params->params->env;
	ctx.shell = ctx_params->params->shell;
	return (ctx);
}

static pid_t	e(t_exec_context_params *ctx_params)
{
	t_exec_context	ctx;

	ctx = init_exec_context(ctx_params);
	return (fork_and_execute(&ctx));
}

static pid_t	*c(int c_c, t_command *cmd, int (*pipes)[2], t_exec_params *par)
{
	int						i;
	pid_t					*pids;
	t_exec_context_params	ctx_params;

	pids = malloc(sizeof(pid_t) * c_c);
	if (!pids)
	{
		perror("malloc");
		return (NULL);
	}
	i = 0;
	while (i < c_c)
	{
		ctx_params.cmd = cmd;
		ctx_params.idx = i;
		ctx_params.cmd_count = c_c;
		ctx_params.pipes = pipes;
		ctx_params.params = par;
		pids[i] = e(&ctx_params);
		if (pids[i] < 0)
			return (free(pids), NULL);
		cmd = cmd->next;
		i++;
	}
	return (pids);
}

static void	init_exec_params(t_exec_params *params, t_env *env, t_shell *shell)
{
	params->env = env;
	params->shell = shell;
}

static int	handle_pids_error(int cmd_count, int (*pipes)[2])
{
	if (pipes)
		close_pipes(cmd_count, pipes);
	return (1);
}

static int	close_pipes_and_wait(int cmd_count, int (*pipes)[2], pid_t *pids)
{
	int	last_exit_code;

	last_exit_code = 0;
	if (pipes)
		close_pipes(cmd_count, pipes);
	wait_for_children(cmd_count, pids, &last_exit_code);
	free(pids);
	return (last_exit_code);
}

int	execute_pipeline(t_pipeline *pipeline, t_env *env, t_shell *shell)
{
	int				cmd_count;
	int				last_exit_code;
	pid_t			*pids;
	t_exec_params	params;
	int				(*pipes)[2];

	pipes = NULL;
	init_exec_params(&params, env, shell);
	cmd_count = count_commands(pipeline->commands);
	if (cmd_count == 0)
		return (0);
	if (initialize_pipes(cmd_count, &pipes) == -1)
		return (1);
	pids = c(cmd_count, pipeline->commands, pipes, &params);
	if (!pids)
		return (handle_pids_error(cmd_count, pipes));
	last_exit_code = close_pipes_and_wait(cmd_count, pipes, pids);
	return (last_exit_code);
}
