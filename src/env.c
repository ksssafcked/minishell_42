/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsaiti <lsaiti@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/14 16:32:32 by qudomino          #+#    #+#             */
/*   Updated: 2024/12/24 18:26:28 by lsaiti           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../include/env.h"
#include "../include/built_in.h"
#include "../include/utils.h"

t_env	*env_new_elem(char *var)
{
	t_env	*new;

	new = malloc(sizeof(t_env));
	if (!new)
		return (NULL);
    new->printable = 1;
	new->var = ft_strdup(var);
	if (!new->var)
	{
		free(new);
		return (NULL);
	}
	new->next = NULL;
	return (new);
}

t_env	*env_init(char **envp)
{
	t_env	*head;
	t_env	*current;
	t_env	*new;
	int		i;

	head = NULL;
	current = NULL;
	i = 0;
	while (envp[i])
	{
		new = env_new_elem(envp[i]);
		if (!new)
		{
			env_free(head);
			return (NULL);
		}
		if (!head)
			head = new;
		else
			current->next = new;
		current = new;
		i++;
	}
	return (head);
}

void env_print(t_env *env)
{
    t_env *cur = env;
    while (cur)
    {
        if (cur->printable)
            printf("%s\n", cur->var);
        cur = cur->next;
    }
}

void	env_free(t_env *env)
{
	t_env	*cur;
	t_env	*tmp;

	cur = env;
	while (cur)
	{
		tmp = cur->next;
		free(cur->var);
		free(cur);
		cur = tmp;
	}
}

char	*env_get_value(t_env *env, const char *name)
{
	size_t	len;

	len = ft_strlen(name);
	while (env)
	{
		if (ft_strncmp(env->var, name, len) == 0 && env->var[len] == '=')
			return (env->var + len + 1);
		env = env->next;
	}
	return (NULL);
}

int	env_size(t_env *env)
{
	int	size;

	size = 0;
	while (env)
	{
		size++;
		env = env->next;
	}
	return (size);
}

void	free_array(char **arr, int count)
{
	int	index;

	index = 0;
	while (index < count)
	{
		free(arr[index]);
		index++;
	}
	free(arr);
}

char	**env_to_char_array(t_env *env)
{
	int		size;
	int		i;
	char	**arr;

	size = env_size(env);
	arr = malloc(sizeof(char *) * (size + 1));
	if (!arr)
		return (NULL);
	i = 0;
	while (env)
	{
		arr[i] = strdup(env->var);
		if (!arr[i])
		{
			free_array(arr, i);
			return (NULL);
		}
		i++;
		env = env->next;
	}
	arr[i] = NULL;
	return (arr);
}