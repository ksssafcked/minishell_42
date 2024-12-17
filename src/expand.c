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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../include/env.h"
#include "../include/expand.h"
#include "../include/utils.h"

static char *replace_variable(const char *name, t_env *env)
{
	char *val;

    if (!name)
        return (ft_strdup(""));
    val = env_get_value(env, name);
    if (!val)
        return (ft_strdup(""));
    return (ft_strdup(val));
}

char *expand_token(const char *token, t_env *env, int last_exit_code)
{
    char *result;
	char *tmp;
	char *varname;
	char *var_value;
	char buff[12];
	int i;
	int start;
	int len;

	result = ft_strdup(""); 
    if (!result)
        return (NULL);
    i = 0;
    while (token[i])
    {
        if (token[i] == '$')
        {
            i++;
            if (token[i] == '?')
            {
                // Remplacer par last_exit_code
                snprintf(buff, sizeof(buff), "%d", last_exit_code);
                tmp = result;
                result = malloc(ft_strlen(tmp) + ft_strlen(buff) + 1);
                if (!result)
				{ 
					free(tmp); 
					return (NULL); 
				}
                ft_strcpy(result, tmp);
                ft_strcat(result, buff);
                free(tmp);
                i++;
            }
            else if (token[i] && (token[i] == '_' || ft_isalnum((unsigned char)token[i])))
            {
                // On lis un nom de variable
                start = i;
                while (token[i] && (token[i] == '_' || ft_isalnum((unsigned char)token[i])))
                    i++;
                len = i - start;
                varname = malloc(len + 1);
                if (!varname)
				{ 
					free(result); 
					return (NULL); 
				}
                strncpy(varname, token+start, len); // REMPLACER PAR UN VRAI STRNCPY !!!!
                varname[len] = '\0';
                var_value = replace_variable(varname, env);
                free(varname);
                tmp = result;
                result = malloc(ft_strlen(tmp) + ft_strlen(var_value) + 1);
                if (!result)
				{ 
					free(tmp); 
					free(var_value); 
					return (NULL); 
				}
                ft_strcpy(result, tmp);
                ft_strcat(result, var_value);
                free(tmp);
                free(var_value);
            }
            else
            {
                // $ suivi d'un caractere non valide pour une variable => on copie juste '$'
                tmp = result;
                result = malloc(ft_strlen(tmp) + 2);
                if (!result) 
				{ 
					free(tmp); 
					return (NULL); 
				}
                ft_strcpy(result, tmp);
                ft_strcat(result, "$");
                free(tmp);
                // On ne consomme pas token[i] dans ce cas, il sera copie ci-dessous comme un caractere normal
            }
        }
        else
        {
            // caractere normal
            char c[2] = { token[i], '\0' };
            tmp = result;
            result = malloc(ft_strlen(tmp) + 2);
            if (!result) 
			{
				free(tmp); 
				return (NULL); 
			}
            ft_strcpy(result, tmp);
            ft_strcat(result, c);
            free(tmp);
            i++;
        }
    }
    return (result);
}
