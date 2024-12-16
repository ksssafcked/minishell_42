/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsaiti <lsaiti@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/14 15:11:08 by qudomino          #+#    #+#             */
/*   Updated: 2024/12/16 21:38:10 by lsaiti           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../include/parser.h"
#include "../include/utils.h"

static int is_sep(char c)
{
    return (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v');
}

static size_t count_words(const char *line)
{
	size_t count;
	size_t i;
	int in_word;

	count = 0;
	i = 0;
	in_word = 0;
	while (line[i])
	{
		if (!is_sep(line[i]) && in_word == 0)
		{
			in_word = 1;
			count++;
		}
		else if (!is_sep(line[i]) && !is_sep(line[i + 1]) && in_word == 1 && line[i] == '|')
			count+= 2;
		else if (is_sep(line[i]))
			in_word = 0;
		i++;
	}
	return (count);
}

// Libere tableau de tokens si une erreur survient
static void free_tokens_array(char **tokens, size_t t)
{
    size_t i;

	i = 0;
    if (!tokens)
        return;
    while (i < t)
	{
        free(tokens[i]);
		i++;
	}
    free(tokens);
}

static char *copy_word(const char *line, size_t start, size_t end)
{
    size_t len;
	len = end - start;
    char *word = malloc(sizeof(char) * (len + 1));
    if (!word)
        return (NULL);
    ft_memcpy(word, line + start, len);
    word[len] = '\0';
    return (word);
}

char **ft_split_line(char *line)
{
	char    **tokens;
	char	*clean_line;
	size_t  word_count;
	size_t  i;
	size_t  start;
	size_t  t; // index pour tokens
	int     in_word;

	i = 0;
	start = 0;
	t = 0;
	in_word = 0;
	if (!line)
		return (NULL);
	clean_line = get_spaces(line);
	word_count = count_words(clean_line);
	tokens = (char **)malloc((word_count + 1) * sizeof(char *));
	if (!tokens)
		return (free(clean_line), NULL);
	while (clean_line[i])
	{
		if (!is_sep(clean_line[i]) && in_word == 0)
		{
			in_word = 1;
			start = i;
		}
		else if ((is_sep(clean_line[i]) || clean_line[i] == '|') && in_word == 1)
		{
			tokens[t] = copy_word(clean_line, start, i);
			if (!tokens[t])
			{
				free_tokens_array(tokens, t);
				return (free(clean_line), NULL);
			}
			t++;
			in_word = 0;
		}
		if (clean_line[i] == '|' && !is_sep(clean_line[i + 1]))
		{
			tokens[t] = copy_word(clean_line, i, i + 1);
			if (!tokens[t])
			{
				free_tokens_array(tokens, t);
				return (free(clean_line), NULL);
			}
			t++;
		}
		i++;
	}
	// Si chaine ne finit pas par un separateur il reste un mot a copier
	if (in_word == 1)
	{
		tokens[t] = copy_word(clean_line, start, i);
		if (!tokens[t])
		{
			free_tokens_array(tokens, t);
			return (free(clean_line), NULL);
		}
		t++;
	}
	tokens[t] = NULL;
	free(clean_line);
	return (tokens);
}

int is_simple_quote(char *line)
{
	int	i;
	
	i = 0;
	while (line[i])
		i++;
	i--;
	if (line[i] == 39 && line[0] == 39)
		return (1);
	return (0);
}

int is_double_quote(char *line)
{
	int	i;
	
	i = 0;
	while (line[i])
		i++;
	i--;
	if (line[i] == 34 && line[0] == 34)
		return (1);
	return (0);
}

char    *delete_quote(char *line)
{
    char   *final;
    size_t  len;
    size_t  i;

    len = ft_strlen(line);
    final = malloc(sizeof(char) * (--len));
    if (!final)
        return (free(line), NULL);
    i = 0;
    while (i < len - 1)
    {
        final[i] = line[i + 1];
        i++;
    }
    final[i] = '\0';
    free(line);
    return (final);
}

// char    **parsing(const char **tokens, t_env *env)
// {
// 	int     i;

// 	i = 0;
// 	while (tokens[i])
// 	{
// 		if (is_simple_quote(tokens[i]))
//             tokens[i] = delete_quote(tokens[i]);
//         else
//         {
//             else if (is_double_quote(tokens[i]))
//                 tokens[i] = delete_quote(tokens[i]);
//             tokens[i] = expand_token(tokens[i], env, 0);
//         }
//         i++;
// 	}
// 	return (tokens);
// }
