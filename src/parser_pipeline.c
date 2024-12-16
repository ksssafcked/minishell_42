/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_pipeline.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qudomino <qudomino@student.42mulhouse.fr>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/16 14:22:45 by qudomino          #+#    #+#             */
/*   Updated: 2024/12/16 14:23:01 by qudomino         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../include/command.h"
#include "../include/pipeline.h"
#include "../include/parser_pipeline.h"
#include "../include/utils.h"

// Fonction utilitaire pour ajouter un argument à un tableau d'arguments.
// Realloue et ajoute l'argument a la fin.
static int add_arg(char ***args, char *token)
{
    int count;
    char **new_args;

    count = 0;
    while (*args && (*args)[count])
        count++;
    new_args = ft_realloc(*args, sizeof(char*) * (count + 2));
    if (!new_args)
        return (0);
    *args = new_args;
    (*args)[count] = ft_strdup(token);
    if (!(*args)[count])
        return (0);
    (*args)[count + 1] = NULL;
    return (1);
}

t_pipeline *parse_pipeline(char **tokens)
{
    t_pipeline *pipeline;
    t_command *current;
    t_command *new_cmd;
    char **args;
    int i;

    pipeline = malloc(sizeof(t_pipeline));
    if (!pipeline)
        return (NULL);
    pipeline->commands = NULL;
    current = malloc(sizeof(t_command));
    if (!current)
    {
        free(pipeline);
        return (NULL);
    }
    ft_memset(current, 0, sizeof(t_command));
    current->in_fd = -1; // Important pour ne pas fermer stdin par erreur
    pipeline->commands = current;
    args = NULL;
    i = 0;
    while (tokens && tokens[i])
    {
        if (ft_strcmp(tokens[i], "|") == 0)
        {
            // Nouvelle commande
            current->argv = args;
            new_cmd = malloc(sizeof(t_command));
            if (!new_cmd)
                return (NULL);
            ft_memset(new_cmd, 0, sizeof(t_command));
            new_cmd->in_fd = -1;
            current->next = new_cmd;
            current = new_cmd;
            args = NULL;
        }
        else if (ft_strcmp(tokens[i], "<") == 0)
        {
            i++;
            if (tokens[i])
                current->infile = ft_strdup(tokens[i]);
        }
        else if (ft_strcmp(tokens[i], ">") == 0)
        {
            i++;
            if (tokens[i])
            {
                current->outfile = ft_strdup(tokens[i]);
                current->append = 0;
            }
        }
        else if (ft_strcmp(tokens[i], ">>") == 0)
        {
            i++;
            if (tokens[i])
            {
                current->outfile = ft_strdup(tokens[i]);
                current->append = 1;
            }
        }
        else if (ft_strcmp(tokens[i], "<<") == 0)
        {
            i++;
            if (tokens[i])
                current->heredoc_delim = ft_strdup(tokens[i]);
        }
        else
        {
            if (!add_arg(&args, tokens[i]))
                return (NULL);
        }
        i++;
    }
    // Dernière commande
    current->argv = args;
    return (pipeline);
}
