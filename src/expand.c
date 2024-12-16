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

// Cette fonction va parcourir 'token' et remplacer $VAR par sa valeur.
// On considere pour le moment que token n'est pas quoted, c'est un cas simple.
// Plus tard, on gerera les quotes.

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
	char *var_value;
	char *tmp;
	int i;
	int start;
	int len;

	result = ft_calloc(1, 1); // chaîne vide
    if (!result)
    {
        return (NULL);
    }

    i = 0;
    while (token[i])
    {
        if (token[i] == '$')
        {
            i++;
            if (token[i] == '?')
            {
                // Remplacer par last_exit_code
                char buff[12];
                snprintf(buff, sizeof(buff), "%d", last_exit_code);

                tmp = result;
                result = malloc(ft_strlen(tmp) + ft_strlen(buff) + 1);
                if (!result)
                {
                    free(tmp);
                    return NULL;
                }
                ft_strcpy(result, tmp);
                ft_strcat(result, buff);
                free(tmp);
            }
            else if (token[i] && (token[i] == '_' || ft_isalnum((unsigned char)token[i])))
            {
                start = i;
                while (token[i] && (token[i] == '_' || ft_isalnum((unsigned char)token[i])))
                    i++;

                len = i - start;
                char varname[len + 1];
                strncpy(varname, token + start, len); // IMPLEMENTER NOTRE PROPRE STRNCPY !!!!
                varname[len] = '\0';
                i--; // Compense le i++ supplementaire de la boucle
                var_value = replace_variable(varname, env);
                tmp = result;
                result = malloc(ft_strlen(tmp) + ft_strlen(var_value) + 1);
                if (!result)
                {
                    free(tmp);
                    free(var_value);
                    return NULL;
                }
                ft_strcpy(result, tmp);
                ft_strcat(result, var_value);
                free(tmp);
                free(var_value);
            }
            else
            {
                // $ suivi d'un caractere non valide : on ajoute juste $
                i--;
                tmp = result;
                result = malloc(strlen(tmp) + 2);
                if (!result)
                {
                    free(tmp);
                    return NULL;
                }
                ft_strcpy(result, tmp);
                ft_strcat(result, "$");
                free(tmp);
            }
        }
        else
        {
            // Ajouter un caractère normal à result
            char tmp_char[2] = {token[i], '\0'};

            tmp = result;
            result = malloc(ft_strlen(tmp) + 2);
            if (!result)
            {
                free(tmp);
                return NULL;
            }
            ft_strcpy(result, tmp);
            ft_strcat(result, tmp_char);
            free(tmp);
        }
        i++;
    }
    return (result);
}

/*
char *expand_token(const char *token, t_env *env, int last_exit_code)
{
    char *result;

	result = ft_calloc(1, 1); // chaîne vide
	if (!result)
	{
        return (NULL);
	}
    for (int i = 0; token[i]; i++)
    {
        if (token[i] == '$')
        {
            // Récupérer le nom de la variable
            i++;
            if (token[i] == '?')
            {
                // Remplacer par last_exit_code
                char buff[12];
                snprintf(buff, sizeof(buff), "%d", last_exit_code);
                char *tmp = result;
                result = malloc(ft_strlen(tmp) + ft_strlen(buff) + 1);
                if (!result) { free(tmp); return NULL; }
                strcpy(result, tmp);
                strcat(result, buff);
                free(tmp);
            }
            else if ((token[i] && (token[i] == '_' || ft_isalnum((unsigned char)token[i]))))
            {
                int start = i;
                while (token[i] && (token[i] == '_' || ft_isalnum((unsigned char)token[i])))
                    i++;
                int len = i - start;
                char varname[len+1];
                strncpy(varname, token+start, len); // RAJOUTER UN STRNCPY FONCTIONNEL
                varname[len] = '\0';
                i--; // on a fait un i++ de trop dans la boucle

                char *var_value = replace_variable(varname, env);
                char *tmp = result;
                result = malloc(ft_strlen(tmp) + ft_strlen(var_value) + 1);
                if (!result) { free(tmp); free(var_value); return NULL; }
                ft_strcpy(result, tmp);
                ft_strcat(result, var_value);
                free(tmp);
                free(var_value);
            }
            else
            {
                // $ suivi d'un char non valide : on met juste $
                i--;
                char *tmp = result;
                result = malloc(strlen(tmp) + 2);
                if (!result) { free(tmp); return NULL; }
                ft_strcpy(result, tmp);
                ft_strcat(result, "$");
                free(tmp);
            }
        }
        else
        {
            // caractere normal
            char c[2] = { token[i], '\0' };
            char *tmp = result;
            result = malloc(ft_strlen(tmp) + 2);
            if (!result) { free(tmp); return NULL; }
            ft_strcpy(result, tmp);
            ft_strcat(result, c);
            free(tmp);
        }
    }
    return (result);
} */
