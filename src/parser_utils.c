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
#include "../include/parser.h"
#include "../include/utils.h"

int is_sep(char c)
{
    return (c == ' ' || c == '\t' || c == '\n');
}

int	is_pipe(char *line)
{
	if ((line[0] == '|' && !is_sep(line[1])) || (line[1] == '|' && !is_sep(line[0])))
		return (1);
	return (0);
}
int	is_double_redirection(char	*line)
{
	if ((line[0] == '<' && line[1] == '<' && !is_sep(line[2])) || (line[1] == '<' && line[2] == '<' && !is_sep(line[0])))
		return (1);
	if ((line[0] == '<' && line[1] == '<' && !is_sep(line[2])) || (line[1] == '<' && line[2] == '<' && !is_sep(line[0])))
		return (1);
	return (0);
}

int	is_single_redirection(char	*line)
{
	if ((line[0] == '<' && !is_sep(line[1]) )|| (line[1] == '<' && !is_sep(line[0])))
		return (1);
	if ((line[0] == '>' && !is_sep(line[1])) || (line[1] == '>' && !is_sep(line[0])))
		return (1);
	return (0);
}


// char    *get_correct_line(char *line)
// {
// 	char    *final;
// 	char    buffer[10] = "|<>";
// 	size_t  bytes;
// 	size_t  i;
// 	bytes = 0;
// 	while (line[i])
// 	{
// 		if (line[i] == '|' && !is_sep(line[i]))
// 			bytes += 1;
// 		else if (line[i] == '<')
// 		{
// 			if (line[i + 1] == '<' && !is_sep(line[i]))
// 			{
// 				i++;
// 				bytes++;
// 			}
// 			else if (!is_sep(line[i]))
// 				bytes++;
// 		}
// 		else if (line[i] == '>')
// 		{
// 			if (line[i + 1] == '>' && !is_sep(line[i]))
// 			{
// 				i++;
// 				bytes++;
// 			}
// 			else if (!is_sep(line[i]))
// 				bytes++;
// 		}
// 		i++;
// 	}
// 	final = malloc(sizeof(char) * (bytes + 2));
// 	if (!final)
// 		return (NULL);
// 	get_correct(&final, line);
// }

// void	get_correct(char **final, char *line)
// {
// 	int	i;

// 	i = 0;
// 	while (line[i])
// 	{
// 		if (!is_sep(line[i]))
// 	}
// }

// char    **parsing(char  *line)
// {
// 	char    **tokens;
// 	size_t  i;

// 	i = 0;
// 	line = get_correct_line(line);
// }


char	*get_clean(char *line, size_t size)
{
	int	i;
	char	*final;
	int	j;
	
	i = 0;
	j = 0;
	final = malloc((size) * sizeof(char));
	if (!final)
		return (NULL);
	while (line[i])
	{
		if (is_pipe(&line[i]))
		{
			final[j++] = line[i++];
			final[j++] = ' ';
		}
		else if (is_single_redirection(&line[i]))
		{
			final[j++] = line[i++];
			final[j++] = ' ';
		}
		else if (is_double_redirection(&line[i]))
		{
			final[j++] = line[i++];
			final[j++] = line[i++];
			final[j++] = ' ';
		}
		else
			(final)[j++] = line[i++];
	}
	final[j] = '\0';
	return (final);
}

char *get_spaces(char *line)
{
    char *final;
    int i;
	int	j;

	i = 0;
	j = 0;
	while (line[i])
	{
		if (is_pipe(&line[i]))
			j++;
		else if (is_single_redirection(&line[i]))
			j++;
		else if (is_double_redirection(&line[i]))
		{
			j += 2;
			i++;
		}
		i++;
	}
	final = get_clean(line, i + j + 1);
    return final;
}
