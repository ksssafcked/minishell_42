/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsaiti <lsaiti@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/14 15:11:08 by qudomino          #+#    #+#             */
/*   Updated: 2024/12/17 16:58:45 by lsaiti           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../include/parser.h"
#include "../include/utils.h"

#define TOKEN_UNQUOTED 'U'
#define TOKEN_SINGLE_QUOTE 'S'
#define TOKEN_DOUBLE_QUOTE 'D'

static int	is_whitespace(char c)
{
	return (c == ' ' || c == '\t' || c == '\n');
}

static int	is_special_char(char c)
{
	return (c == '|' || c == '<' || c == '>');
}

static void	add_space_if_needed(char *new_line, size_t *j)
{
	if (*j > 0 && new_line[*j - 1] != ' ')
	{
		new_line[*j] = ' ';
		(*j)++;
	}
}

static void	add_special_char(char *new_line, char c, size_t *j)
{
	new_line[*j] = c;
	(*j)++;
}

static int	is_double_operator(const char *line, size_t i)
{
	if ((line[i] == '<' && line[i + 1] == '<')
		|| (line[i] == '>' && line[i + 1] == '>'))
	{
		return (1);
	}
	return (0);
}

static void	hand_t_s(const char *line, char *new_line, size_t i, size_t *j)
{
	if (line[i + 1] && line[i + 1] != ' ')
	{
		new_line[*j] = ' ';
		(*j)++;
	}
}

static size_t	h_spe_char(const char *l, char *n_l, size_t i, size_t *j)
{
	char	c;
	int		double_op;

	c = l[i];
	double_op = is_double_operator(l, i);
	add_space_if_needed(n_l, j);
	add_special_char(n_l, c, j);
	if (double_op)
	{
		i++;
		add_special_char(n_l, c, j);
	}
	hand_t_s(l, n_l, i, j);
	return (i);
}

static void	update_quote_states(char c, int *in_sq, int *in_dq)
{
	if (c == '\'' && *in_dq == 0)
	{
		*in_sq = !*in_sq;
	}
	else if (c == '"' && *in_sq == 0)
	{
		*in_dq = !*in_dq;
	}
}

static int	handle_char(const char *line, char *new_line, size_t i, size_t *j)
{
	if (is_special_char(line[i]))
	{
		return (h_spe_char(line, new_line, i, j));
	}
	new_line[*j] = line[i];
	(*j)++;
	return (i);
}

static char	*allocate_new_line(size_t len)
{
	char	*new_line;

	new_line = malloc(len * 4 + 1);
	if (new_line == NULL)
	{
		return (NULL);
	}
	return (new_line);
}

static void	preprocess_loop(const char *line, char *new_line, size_t len)
{
	int		in_single_quote;
	int		in_double_quote;
	size_t	i;
	size_t	j;

	in_single_quote = 0;
	in_double_quote = 0;
	i = 0;
	j = 0;
	while (i < len)
	{
		update_quote_states(line[i], &in_single_quote, &in_double_quote);
		if (!in_single_quote && !in_double_quote)
			i = handle_char(line, new_line, i, &j);
		else
		{
			new_line[j] = line[i];
			j++;
		}
		i++;
	}
	new_line[j] = '\0';
}

char	*preprocess_line(const char *line)
{
	size_t	len;
	char	*new_line;

	len = ft_strlen(line);
	new_line = allocate_new_line(len);
	if (new_line == NULL)
		return (NULL);
	preprocess_loop(line, new_line, len);
	return (new_line);
}

static void	skip_spaces(const char **line)
{
	while (**line == ' ' || **line == '\t' || **line == '\n'
		|| **line == '\r' || **line == '\v' || **line == '\f')
		(*line)++;
}

static char	*allocate_initial_token(size_t capacity)
{
	char	*token;

	token = malloc(capacity + 1);
	if (token == NULL)
		return (NULL);
	token[0] = TOKEN_UNQUOTED;
	return (token);
}

static char	*resize_token(char *token, size_t *capacity)
{
	char	*new_token;

	*capacity *= 2;
	new_token = ft_realloc(token, *capacity + 1);
	if (new_token == NULL)
	{
		free(token);
		return (NULL);
	}
	return (new_token);
}

static int	handle_quotes(char c, t_parser_context *ctx)
{
	if (c == '\'' && !ctx->in_double_quote)
	{
		if (ctx->in_single_quote)
			ctx->in_single_quote = 0;
		else
		{
			ctx->in_single_quote = 1;
			ctx->token[0] = TOKEN_SINGLE_QUOTE;
		}
		*(ctx->line_ptr) += 1;
		return (1);
	}
	if (c == '"' && !ctx->in_single_quote)
	{
		if (ctx->in_double_quote)
			ctx->in_double_quote = 0;
		else
		{
			ctx->in_double_quote = 1;
			ctx->token[0] = TOKEN_DOUBLE_QUOTE;
		}
		*(ctx->line_ptr) += 1;
		return (1);
	}
	return (0);
}

static char	*pl(t_parser_context *ctx)
{
	char	c;
	char	*resized;

	while (**ctx->line_ptr != '\0')
	{
		c = **ctx->line_ptr;
		if (!ctx->in_single_quote && !ctx->in_double_quote && is_whitespace(c))
			break ;
		if (handle_quotes(c, ctx))
			continue ;
		if (ctx->length + 1 >= ctx->capacity)
		{
			resized = resize_token(ctx->token, &ctx->capacity);
			if (resized == NULL)
				return (NULL);
			ctx->token = resized;
		}
		ctx->token[ctx->length] = c;
		ctx->length++;
		*(ctx->line_ptr) += 1;
	}
	ctx->token[ctx->length] = '\0';
	return (ctx->token);
}

static char	*get_next_token(const char **line)
{
	t_parser_context	ctx;

	ctx.line_ptr = line;
	ctx.in_single_quote = 0;
	ctx.in_double_quote = 0;
	ctx.capacity = 128;
	ctx.length = 1;
	ctx.token = allocate_initial_token(ctx.capacity);
	if (ctx.token == NULL)
		return (NULL);
	if (pl(&ctx) == NULL)
	{
		free(ctx.token);
		return (NULL);
	}
	return (ctx.token);
}

static char	**cleanup_tokens(char **tokens, size_t count)
{
	size_t	i;

	i = 0;
	while (i < count)
	{
		free(tokens[i]);
		i++;
	}
	free(tokens);
	return (NULL);
}

static int	add_token(char ***tokens, size_t *count, char *token)
{
	char	**new_tokens;

	new_tokens = ft_realloc(*tokens, sizeof(char *) * (*count + 2));
	if (!new_tokens)
	{
		free(token);
		return (0);
	}
	*tokens = new_tokens;
	(*tokens)[*count] = token;
	(*count)++;
	(*tokens)[*count] = NULL;
	return (1);
}

static char	**process_tokens(const char *ptr, char **tokens, size_t count)
{
	char	*token;

	while (*ptr)
	{
		token = get_next_token(&ptr);
		if (!token)
			return (cleanup_tokens(tokens, count));
		if (!add_token(&tokens, &count, token))
			return (cleanup_tokens(tokens, count));
		skip_spaces(&ptr);
	}
	return (tokens);
}

char	**ft_split_line(char *line)
{
	char		*preprocessed;
	const char	*ptr;
	char		**tokens;

	if (!line)
		return (NULL);
	preprocessed = preprocess_line(line);
	if (!preprocessed)
		return (NULL);
	ptr = preprocessed;
	tokens = NULL;
	skip_spaces(&ptr);
	tokens = process_tokens(ptr, tokens, 0);
	free(preprocessed);
	return (tokens);
}

int	is_simple_quote(const char *str)
{
	if (!str)
		return (0);
	return (str[0] == TOKEN_SINGLE_QUOTE);
}

int	is_double_quote(const char *str)
{
	if (!str)
		return (0);
	return (str[0] == TOKEN_DOUBLE_QUOTE);
}

char	*delete_quote(char *line)
{
	size_t	len;
	char	*final;
	size_t	i;

	if (!line)
		return (NULL);
	len = ft_strlen(line);
	if (len < 2)
	{
		free(line);
		return (ft_strdup(""));
	}
	final = malloc(sizeof(char) * (len));
	if (!final)
		return (free(line), NULL);
	i = 0;
	while (i < len - 1)
	{
		final[i] = line[i + 1];
		i++;
	}
	final[len - 1] = '\0';
	free(line);
	return (final);
}
