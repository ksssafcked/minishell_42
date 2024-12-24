/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsaiti <lsaiti@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/14 13:32:33 by qudomino          #+#    #+#             */
/*   Updated: 2024/12/23 20:00:00 by lsaiti           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "../include/signals.h"
#include "../include/parser.h"
#include "../include/env.h"
#include "../include/utils.h"
#include "../include/parser_pipeline.h"
#include "../include/execute_pipeline.h"
#include "../include/heredoc.h"
#include "../include/pipeline.h"
#include "../include/command.h"
#include "../include/built_in.h"
#include "../include/expand.h"

#define TOKEN_UNQUOTED 'U'
#define TOKEN_SINGLE_QUOTE 'S'
#define TOKEN_DOUBLE_QUOTE 'D'

int open_all_redirections(t_pipeline *pl, int *exit_code);

void	free_pipeline(t_pipeline *pl)
{
	t_command *c;
	t_command *next;
	int i;
	if (!pl)
		return ;
	c = pl->commands;
	while (c)
	{
		next = c->next;
		if (c->argv)
		{
			i = 0;
			while (c->argv[i])
				free(c->argv[i++]);
			free(c->argv);
		}
		free(c->infile);
		free(c->outfile);
		free(c->heredoc_delim);
		if (c->in_fd != -1)
			close(c->in_fd);
		if (c->out_fd != -1)
			close(c->out_fd);
		free(c);
		c = next;
	}
	free(pl);
}

static void	free_tokens(char **tokens)
{
	int i;

	i = 0;
	if (!tokens)
		return ;
	while (tokens[i])
	{
		free(tokens[i]);
		i++;
	}
	free(tokens);
}

static int	is_operator_token(char *s)
{
	if (!s)
		return (0);
	if (s[0] != TOKEN_UNQUOTED && s[0] != TOKEN_SINGLE_QUOTE
		&& s[0] != TOKEN_DOUBLE_QUOTE)
		return (0);
	s++;
	if (!ft_strcmp(s, "|") || !ft_strcmp(s, "<") || !ft_strcmp(s, ">")
		|| !ft_strcmp(s, ">>") || !ft_strcmp(s, "<<"))
		return (1);
	return (0);
}

static void remove_empty_tokens(char **tokens)
{
    int i = 0;
    while (tokens[i])
    {
        if (ft_strlen(tokens[i]) == 0)
        {
            free(tokens[i]);
            int j = i;
            while (tokens[j])
            {
                tokens[j] = tokens[j + 1];
                j++;
            }
        }
        else
            i++;
    }
}

static int	is_single_command_no_pipe(t_pipeline *pl)
{
	if (!pl || !pl->commands)
		return (0);
	return (pl->commands->next == NULL);
}

static int	is_builtin_cmd(char **argv)
{
	if (!argv || !argv[0])
		return (0);
	if (!ft_strcmp(argv[0], "echo"))
		return (1);
	if (!ft_strcmp(argv[0], "cd"))
		return (1);
	if (!ft_strcmp(argv[0], "pwd"))
		return (1);
	if (!ft_strcmp(argv[0], "export"))
		return (1);
	if (!ft_strcmp(argv[0], "unset"))
		return (1);
	if (!ft_strcmp(argv[0], "env"))
		return (1);
	if (!ft_strcmp(argv[0], "exit"))
		return (1);
	return (0);
}

static int	validate_syntax(char **tokens, int count)
{
	int i;

	i = 0;
	while (i < count)
	{
		if (is_operator_token(tokens[i]))
		{
			if (i == 0)
				return (i);
			if (i == count - 1)
				return (i);
			if (is_operator_token(tokens[i + 1]))
				return (i + 1);
		}
		i++;
	}
	return (-1);
}

int main(int argc, char **argv, char **envp)
{
    (void)argc;
    (void)argv;
    t_shell shell;
    shell.env = env_init(envp); 
    if (!shell.env)
    {
        fprintf(stderr, "Error: failed to init env\n");
        return (1);
    }
    shell.last_exit_code = 0;
    init_signals();

    while (1)
    {
        char *line = readline("minishell> ");
        if (line == NULL) // Ctrl-D
        {
            printf("exit\n");
            break;
        }
        if (*line)
            add_history(line);

        char **tokens = ft_split_line(line);
        if (!tokens || !tokens[0])
        {
            free_tokens(tokens);
            free(line);
            continue;
        }

        int count_tok = 0;
        while (tokens[count_tok])
            count_tok++;
        char *first = tokens[0];
        char *last = tokens[count_tok - 1];

        int syntax_error = -1;
        if (is_operator_token(first))
        {
            syntax_error = 0; // first token
        }
        else if (is_operator_token(last))
        {
            syntax_error = count_tok -1; // last token
        }
        else
        {
            syntax_error = validate_syntax(tokens, count_tok);
        }

        if (syntax_error != -1)
        {
            fprintf(stderr, "bash: syntax error near unexpected token `%s'\n", tokens[syntax_error] +1);
            free_tokens(tokens);
            free(line);
            shell.last_exit_code = 2;
            continue;
        }

        int idx = 0;
        while (tokens[idx])
        {
            char *expanded;
            if (is_simple_quote(tokens[idx]))
            {
                char *tmp = delete_quote(tokens[idx]); 
                tokens[idx] = tmp;
            }
            else if (is_double_quote(tokens[idx]))
            {
                char *tmp = delete_quote(tokens[idx]);
                expanded = expand_token(tmp, shell.env, shell.last_exit_code);
                free(tmp);
                tokens[idx] = expanded;
            }
            else
            {
                char *tmp = delete_quote(tokens[idx]);
                expanded = expand_token(tmp, shell.env, shell.last_exit_code);
                free(tmp);
                tokens[idx] = expanded;
            }
            idx++;
        }

        remove_empty_tokens(tokens);
        if (!tokens[0])
        {
            free_tokens(tokens);
            free(line);
            continue;
        }

        t_pipeline *pl = parse_pipeline(tokens);
        free_tokens(tokens);
        if (!pl)
        {
            free(line);
            shell.last_exit_code = 2;
            continue;
        }

        if (!process_heredocs(pl->commands, shell.env, shell.last_exit_code))
        {
            free_pipeline(pl);
            free(line);
            continue;
        }

        if (is_single_command_no_pipe(pl) && is_builtin_cmd(pl->commands->argv))
        {
            if (!open_all_redirections(pl, &shell.last_exit_code))
            {
                free_pipeline(pl);
                free(line);
                shell.last_exit_code = 1;
                continue;
            }

            t_command *c = pl->commands;
            int saved_stdin = dup(STDIN_FILENO);
            int saved_stdout = dup(STDOUT_FILENO);
            if (saved_stdin == -1 || saved_stdout == -1)
            {
                perror("dup");
                free_pipeline(pl);
                free(line);
                continue;
            }

            if (c->in_fd != -1)
            {
                if (dup2(c->in_fd, STDIN_FILENO) == -1)
                {
                    perror("dup2");
                    shell.last_exit_code = 1;
                    close(saved_stdin);
                    close(saved_stdout);
                    free_pipeline(pl);
                    free(line);
                    continue;
                }
                close(c->in_fd);
            }
            if (c->out_fd != -1)
            {
                if (dup2(c->out_fd, STDOUT_FILENO) == -1)
                {
                    perror("dup2");
                    shell.last_exit_code = 1;
                    close(saved_stdin);
                    close(saved_stdout);
                    free_pipeline(pl);
                    free(line);
                    continue;
                }
                close(c->out_fd);
            }

            run_builtin(&shell, c->argv);

            if (dup2(saved_stdin, STDIN_FILENO) == -1)
                perror("dup2");
            if (dup2(saved_stdout, STDOUT_FILENO) == -1)
                perror("dup2");
            close(saved_stdin);
            close(saved_stdout);

            free_pipeline(pl);
        }
        else
        {
            int ret = execute_pipeline(pl, shell.env, &shell);
            shell.last_exit_code = ret;
            free_pipeline(pl);
        }

        free(line);
    }

    env_free(shell.env);
    return 0;
}
