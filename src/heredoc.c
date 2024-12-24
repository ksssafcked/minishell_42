/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qudomino <qudomino@student.42mulhouse.fr>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/16 14:24:07 by qudomino          #+#    #+#             */
/*   Updated: 2024/12/16 14:24:23 by qudomino         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "../include/command.h"
#include "../include/heredoc.h"
#include "../include/utils.h"
#include "../include/env.h"
#include "../include/expand.h"

static int	is_heredoc_quoted(const char *delim)
{
	size_t	len;

	if (!delim)
		return (0);
	len = ft_strlen(delim);
	if (len < 2)
		return (0);
	if (delim[0] == '\'' && delim[len - 1] == '\'')
		return (1);
	if (delim[0] == '"' && delim[len - 1] == '"')
		return (1);
	return (0);
}

static char	*remove_quotes(const char *str)
{
	size_t	len;
	char	*new_str;

	if (!str)
		return (NULL);
	len = ft_strlen(str);
	if (len >= 2)
	{
		if ((str[0] == '\'' && str[len - 1] == '\'')
			|| (str[0] == '"' && str[len - 1] == '"'))
		{
			new_str = malloc(len - 1);
			if (!new_str)
				return (NULL);
			ft_strncpy(new_str, str + 1, len - 2);
			new_str[len - 2] = '\0';
			return (new_str);
		}
	}
	return (ft_strdup(str));
}

static int	w_to_p(int fd, const char *line, int h_q, t_expand_context *c)
{
	char	*expanded;

	if (!h_q)
	{
		expanded = expand_token(line, c->env, c->last_exit_code);
		if (!expanded)
			return (-1);
		if (dprintf(fd, "%s\n", expanded) < 0)
		{
			free(expanded);
			return (-1);
		}
		free(expanded);
	}
	else
	{
		if (dprintf(fd, "%s\n", line) < 0)
			return (-1);
	}
	return (0);
}

static int	c_heredoc_pipe(char *delim, int h_q, t_expand_context *context)
{
	int		fd[2];
	char	*line;
	size_t	len;
	ssize_t	r;

	if (pipe(fd) == -1)
		return (-1);
	line = NULL;
	len = 0;
	while (1)
	{
		write(STDOUT_FILENO, "> ", 2);
		r = getline(&line, &len, stdin);
		if (r == -1)
			break ;
		if (r > 0 && line[r - 1] == '\n')
			line[r - 1] = '\0';
		if (ft_strcmp(line, delim) == 0)
			break ;
		if (w_to_p(fd[1], line, h_q, context) == -1)
			return (free(line), close(fd[1]), close(fd[0]), -1);
	}
	free(line);
	close(fd[1]);
	return (fd[0]);
}

static int	handle_single_heredoc(t_command *cmd, t_expand_context *context)
{
	int		heredoc_quoted;
	char	*clean_delim;
	int		in_fd;

	heredoc_quoted = is_heredoc_quoted(cmd->heredoc_delim);
	if (heredoc_quoted)
	{
		clean_delim = remove_quotes(cmd->heredoc_delim);
		if (!clean_delim)
			return (0);
		free(cmd->heredoc_delim);
		cmd->heredoc_delim = clean_delim;
	}
	in_fd = c_heredoc_pipe(cmd->heredoc_delim, heredoc_quoted, context);
	if (in_fd == -1)
		return (0);
	cmd->infile = NULL;
	cmd->append = 0;
	cmd->in_fd = in_fd;
	return (1);
}

int	process_heredocs(t_command *cmds, t_env *env, int last_exit_code)
{
	t_command			*c;
	int					success;
	t_expand_context	context;

	context.env = env;
	context.last_exit_code = last_exit_code;
	c = cmds;
	success = 1;
	while (c)
	{
		if (c->heredoc_delim)
		{
			if (!handle_single_heredoc(c, &context))
			{
				success = 0;
				break ;
			}
		}
		c = c->next;
	}
	return (success);
}
