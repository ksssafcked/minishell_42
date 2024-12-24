/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsaiti <lsaiti@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/20 16:53:38 by qudomino          #+#    #+#             */
/*   Updated: 2024/12/23 20:08:11 by lsaiti           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include "../include/pipeline.h"

static void	print_file_error(const char *file)
{
	fprintf(stderr, "bash: %s: ", file);
	perror(NULL);
}

static int	open_infile(t_command *cmd)
{
	int	fd;

	if (!cmd->infile)
		return (1);
	fd = open(cmd->infile, O_RDONLY);
	if (fd < 0)
	{
		print_file_error(cmd->infile);
		return (0);
	}
	cmd->in_fd = fd;
	return (1);
}

static int	open_outfile(t_command *cmd)
{
	int	flags;
	int	fd;

	if (cmd->outfile == NULL)
		return (1);
	if (cmd->append == 1)
		flags = O_WRONLY | O_CREAT | O_APPEND;
	else
		flags = O_WRONLY | O_CREAT | O_TRUNC;
	fd = open(cmd->outfile, flags, 0644);
	if (fd < 0)
	{
		print_file_error(cmd->outfile);
		return (0);
	}
	cmd->out_fd = fd;
	return (1);
}

int	handle_input_redirection(t_command *c, int *exit_code)
{
	if (c->infile != NULL)
	{
		if (!open_infile(c))
		{
			*exit_code = 1;
			return (0);
		}
	}
	return (1);
}

int	handle_output_redirection(t_command *c, int *exit_code)
{
	if (c->outfile != NULL)
	{
		if (!open_outfile(c))
		{
			*exit_code = 1;
			return (0);
		}
	}
	return (1);
}

int	open_all_redirections(t_pipeline *pl, int *exit_code)
{
	t_command	*c;

	c = pl->commands;
	while (c != NULL)
	{
		if (c->in_fd == -1)
			c->in_fd = -1;
		else
			c->in_fd = c->in_fd;
		c->out_fd = -1;
		if (!handle_input_redirection(c, exit_code))
		{
			fprintf(stderr, "'%s': No such file or directory\n", c->infile);
			return (0);
		}
		if (!handle_output_redirection(c, exit_code))
		{
			fprintf(stderr, "%s: cd: No such file or directory\n", c->infile);	
			return (0);
		}
		c = c->next;
	}
	return (1);
}
