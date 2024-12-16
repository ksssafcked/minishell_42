/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qudomino <qudomino@student.42mulhouse.fr>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/14 16:32:32 by qudomino          #+#    #+#             */
/*   Updated: 2024/12/14 16:53:43 by qudomino         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../include/env.h"
#include "../include/utils.h"

// *Ici : Built-in env -> affiche toutes les variables d environnements.

t_env *env_new_elem(char *var) // init nouvel elem
{
    t_env *new;

	new = malloc(sizeof(t_env));
    if (!new)
        return (NULL);
    new->var = ft_strdup(var);
    if (!new->var)
    {
        free(new);
		return (NULL);
    }
    new->next = NULL;
    return (new);
}

t_env *env_init(char **envp) // initialise notre liste depuis envp
{
    t_env *head;
    t_env *current;
    int i;
	
	head = NULL;
	current = NULL;
	i = 0;
    while (envp[i])
    {
        t_env *new = env_new_elem(envp[i]);
        if (!new)
        {
            env_free(head); // En cas ou erreur de merde on free tout 
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

void env_print(t_env *env) // affiche toutes les variables utilisees par le builtin env
{
    t_env *cur;

	cur = env;
    while (cur)
    {
        printf("%s\n", cur->var);
        cur = cur->next;
    }
}

void env_free(t_env *env) // free la list
{
    t_env *cur;

	cur = env;
    while (cur)
    {
        t_env *tmp = cur->next;
        free(cur->var);
        free(cur);
        cur = tmp;
    }
}

char *env_get_value(t_env *env, const char *name)
{
    size_t len;

	len = ft_strlen(name);
    while (env)
    {
        // format VAR=VAL
        if (ft_strncmp(env->var, name, len) == 0 && env->var[len] == '=')
            return (env->var + len + 1); // pointeur sur la valeur
        env = env->next;
    }
    return (NULL);
}

int env_size(t_env *env)
{
    int count;

	count = 0;
    while (env)
    {
        count++;
        env = env->next;
    }
    return (count);
}

char **env_to_char_array(t_env *env)
{
    int size;
	int i;
	int j;
	char **arr;
	size = env_size(env);
    arr = malloc(sizeof(char*) * (size + 1));
    if (!arr)
        return (NULL);
    i = 0;
    while (env)
    {
        arr[i] = ft_strdup(env->var); // copie "VAR=VALUE"
        if (!arr[i])
        {
            // liberer ce qui est deja alloc
			j = 0;
            while (j < i)
			{
                free(arr[j]);
				j++;
			}
            free(arr);
            return (NULL);
        }
        i++;
        env = env->next;
    }
    arr[i] = NULL;
    return (arr);
}
