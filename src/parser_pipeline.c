/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_pipeline.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qudomino <qudomino@student.42mulhouse.fr>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/16 14:22:45 by qudomino          #+#    #+#             */
/*   Updated: 2024/12/16 14:23:01 by qudomino         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../include/command.h"
#include "../include/pipeline.h"
#include "../include/parser_pipeline.h"
#include "../include/utils.h"

static t_command	*allocate_command(void)
{
	t_command	*cmd;

	cmd = malloc(sizeof(t_command));
	if (!cmd)
		return (NULL);
	ft_memset(cmd, 0, sizeof(t_command));
	cmd->in_fd = -1;
	cmd->out_fd = -1;
	cmd->append = 0;
	cmd->heredoc_quoted = 0;
	cmd->next = NULL;
	return (cmd);
}

static char	**extend_argv(char **argv, int count)
{
	return (ft_realloc(argv, sizeof(char *) * (count + 2)));
}

static int	extend_arguments(t_command *cmd, char *token)
{
	int		count;
	char	**new_args;

	count = 0;
	while (cmd->argv && cmd->argv[count])
		count++;
	new_args = extend_argv(cmd->argv, count);
	if (!new_args)
		return (0);
	cmd->argv = new_args;
	cmd->argv[count] = ft_strdup(token);
	if (!cmd->argv[count])
		return (0);
	cmd->argv[count + 1] = NULL;
	return (1);
}

static void	append_command_to_pipeline(t_pipeline *pipeline, t_command *cmd)
{
	t_command	*last;

	last = pipeline->commands;
	while (last->next)
		last = last->next;
	last->next = cmd;
}

static t_command	*create_new_command(t_pipeline *pipeline)
{
	t_command	*cmd;

	cmd = allocate_command();
	if (!cmd)
		return (NULL);
	if (!pipeline->commands)
		pipeline->commands = cmd;
	else
		append_command_to_pipeline(pipeline, cmd);
	return (cmd);
}

static int	set_input_file(t_command *cmd, char *token)
{
	cmd->infile = ft_strdup(token);
	return (cmd->infile != NULL);
}

static int	set_output_file(t_command *cmd, char *token, int append)
{
	cmd->outfile = ft_strdup(token);
	if (!cmd->outfile)
		return (0);
	cmd->append = append;
	return (1);
}

static int	set_heredoc(t_command *cmd, char *token)
{
	cmd->heredoc_delim = ft_strdup(token);
	return (cmd->heredoc_delim != NULL);
}

static int	hstok(t_command **cmd, char *token, int *index, char **tokens)
{
	(*index)++;
	if (ft_strcmp(token, "<") == 0)
		return (tokens[*index] && set_input_file(*cmd, tokens[*index]));
	if (ft_strcmp(token, ">") == 0)
		return (tokens[*index] && set_output_file(*cmd, tokens[*index], 0));
	if (ft_strcmp(token, ">>") == 0)
		return (tokens[*index] && set_output_file(*cmd, tokens[*index], 1));
	if (ft_strcmp(token, "<<") == 0)
		return (tokens[*index] && set_heredoc(*cmd, tokens[*index]));
	return (0);
}

static int	pst(t_pipeline *pipe, t_command **cmd, char **tokens, int *index)
{
	char	*token;

	token = tokens[*index];
	if (ft_strcmp(token, "|") == 0)
	{
		*cmd = create_new_command(pipe);
		return (*cmd != NULL);
	}
	if (ft_strcmp(token, "<") == 0 || ft_strcmp(token, ">") == 0
		|| ft_strcmp(token, ">>") == 0 || ft_strcmp(token, "<<") == 0)
		return (hstok(cmd, token, index, tokens));
	return (extend_arguments(*cmd, token));
}

t_pipeline	*parse_pipeline(char **tokens)
{
	t_pipeline	*pipeline;
	t_command	*current_cmd;
	int			i;

	pipeline = malloc(sizeof(t_pipeline));
	if (!pipeline)
		return (NULL);
	pipeline->commands = NULL;
	current_cmd = allocate_command();
	if (!current_cmd)
	{
		free(pipeline);
		return (NULL);
	}
	pipeline->commands = current_cmd;
	i = 0;
	while (tokens && tokens[i])
	{
		if (!pst(pipeline, &current_cmd, tokens, &i))
			return (free_pipeline(pipeline), NULL);
		i++;
	}
	return (pipeline);
}
