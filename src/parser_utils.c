/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsaiti <lsaiti@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/16 20:04:00 by lsaiti            #+#    #+#             */
/*   Updated: 2024/12/16 21:08:04 by lsaiti           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../include/utils.h"

int	sep(char c)
{
	return (c == ' ' || c == '\t' || c == '\n');
}

int	is_pipe(char *line)
{
	if (!line)
		return (0);
	if ((line[0] == '|' && !sep(line[1])) || (line[1] == '|' && !sep(line[0])))
		return (1);
	return (0);
}

int	is_double_redirection(char *line)
{
	if (!line)
		return (0);
	if ((line[0] == '<' && line[1] == '<' && !sep(line[2])))
		return (1);
	if ((line[0] == '>' && line[1] == '>' && !sep(line[2])))
		return (1);
	return (0);
}

int	is_single_redirection(char *line)
{
	if (!line)
		return (0);
	if ((line[0] == '<' && !sep(line[1])) || (line[1] == '<' && !sep(line[0])))
		return (1);
	if ((line[0] == '>' && !sep(line[1])) || (line[1] == '>' && !sep(line[0])))
		return (1);
	return (0);
}

char	*get_clean(char *line, size_t size)
{
	(void)size;
	return (ft_strdup(line));
}

char	*get_spaces(char *line)
{
	(void)line;
	return (NULL);
}
