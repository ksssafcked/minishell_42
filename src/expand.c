/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qudomino <qudomino@student.42mulhouse.fr>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/16 17:19:59 by qudomino          #+#    #+#             */
/*   Updated: 2024/12/16 17:20:08 by qudomino         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../include/env.h"
#include "../include/expand.h"
#include "../include/utils.h"

static char	*replace_variable(const char *name, t_env *env)
{
	char	*val;

	if (!name)
		return (ft_strdup(""));
	val = env_get_value(env, name);
	if (!val)
		return (ft_strdup(""));
	return (ft_strdup(val));
}

static char	*str_append(const char *s1, const char *s2)
{
	char	*new_str;
	size_t	len1;
	size_t	len2;

	if (!s1)
		return (ft_strdup(s2));
	if (!s2)
		return (ft_strdup(s1));
	len1 = ft_strlen(s1);
	len2 = ft_strlen(s2);
	new_str = malloc(len1 + len2 + 1);
	if (!new_str)
		return (NULL);
	ft_strcpy(new_str, s1);
	ft_strcat(new_str, s2);
	return (new_str);
}

static char	*append_char(char *result, char c)
{
	char	temp[2];
	char	*new_result;

	temp[0] = c;
	temp[1] = '\0';
	new_result = str_append(result, temp);
	if (!new_result)
	{
		free(result);
		return (NULL);
	}
	free(result);
	return (new_result);
}

static char	*handle_question_mark(char *result, t_expand_con *context)
{
	char	buff[12];
	char	*tmp;
	size_t	tmp_len;
	size_t	buff_len;

	snprintf(buff, sizeof(buff), "%d", context->last_exit_code);
	tmp = result;
	tmp_len = ft_strlen(tmp);
	buff_len = strlen(buff);
	result = malloc(tmp_len + buff_len + 1);
	if (!result)
	{
		free(tmp);
		return (NULL);
	}
	ft_strcpy(result, tmp);
	ft_strcat(result, buff);
	free(tmp);
	(*(context->i))++;
	return (result);
}

static char	*extract_varname(const char *token, int start, int len)
{
	char	*varname;

	varname = malloc(len + 1);
	if (!varname)
		return (NULL);
	ft_strncpy(varname, token + start, len);
	varname[len] = '\0';
	return (varname);
}

static int	is_valid_var_char(char c)
{
	if (c == '_')
		return (1);
	if (ft_isdigit(c))
		return (1);
	if (c >= 'A' && c <= 'Z')
		return (1);
	if (c >= 'a' && c <= 'z')
		return (1);
	return (0);
}

static char	*get_varname(const char *token, int *i, int *len)
{
	int		start;
	char	*varname;

	start = *i;
	while (token[*i] && is_valid_var_char(token[*i]))
		(*i)++;
	*len = *i - start;
	varname = extract_varname(token, start, *len);
	return (varname);
}

static char	*append_var_value(char *result, const char *var_value)
{
	char	*tmp;
	char	*new_result;

	tmp = result;
	new_result = str_append(tmp, var_value);
	if (!new_result)
	{
		free(tmp);
		return (NULL);
	}
	free(tmp);
	return (new_result);
}

static char	*h_v(char *result, const char *token, t_env *env, t_expand_con *con)
{
	int		len;
	char	*varname;
	char	*var_value;

	varname = get_varname(token, con->i, &len);
	if (!varname)
	{
		free(result);
		return (NULL);
	}
	var_value = replace_variable(varname, env);
	free(varname);
	if (!var_value)
	{
		free(result);
		return (NULL);
	}
	result = append_var_value(result, var_value);
	free(var_value);
	if (!result)
		return (NULL);
	return (result);
}

static char	*handle_dollar(char *result)
{
	char	*tmp;

	tmp = result;
	result = append_char(tmp, '$');
	return (result);
}

static char	*process_dollar(char *res, const char *token, t_expand_con *con)
{
	if (token[*(con->i)] == '?')
		res = handle_question_mark(res, con);
	else if (token[*(con->i)] && is_valid_var_char(token[*(con->i)]))
		res = h_v(res, token, con->env, con);
	else
		res = handle_dollar(res);
	return (res);
}

static char	*handle_char(char *result, const char *token, t_expand_con *context)
{
	char	c;

	if (token[*(context->i)] == '$')
	{
		(*(context->i))++;
		return (process_dollar(result, token, context));
	}
	else
	{
		c = token[*(context->i)];
		(*(context->i))++;
		return (append_char(result, c));
	}
}

char	*expand_token(const char *token, t_env *env, int last_exit_code)
{
	t_expand_con	context;
	char			*result;
	int				i;

	result = ft_strdup("");
	if (!result)
		return (NULL);
	i = 0;
	context.last_exit_code = last_exit_code;
	context.i = &i;
	context.env = env;
	while (token && token[i])
	{
		result = handle_char(result, token, &context);
		if (!result)
			return (NULL);
	}
	return (result);
}
