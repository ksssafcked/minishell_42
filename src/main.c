/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsaiti <lsaiti@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/14 13:32:33 by qudomino          #+#    #+#             */
/*   Updated: 2024/12/16 21:36:58 by lsaiti           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "../include/signals.h"
#include "../include/parser.h"
#include "../include/env.h"
#include "../include/utils.h"
#include "../include/execute.h"
#include "../include/parser_pipeline.h"
#include "../include/execute_pipeline.h"
#include "../include/heredoc.h"
#include "../include/pipeline.h"
#include "../include/command.h"
#include "../include/built_in.h"
#include "../include/expand.h" // Ajout pour l'expansion

// structure globale du shell pour la passer facilement aux fonctions
typedef struct s_shell {
    t_env *env;
    int last_exit_code; // Ajout du champ pour stocker le code de retour
} t_shell;

void free_pipeline(t_pipeline *pl)
{
    t_command *c;
    t_command *next;
    int i;
    if (!pl) 
        return;
    c = pl->commands;
    while (c)
    {
        next = c->next;
        if (c->argv)
        {
            i = 0;
            while (c->argv[i])
                free(c->argv[i++]);
            free(c->argv);
        }
        free(c->infile);
        free(c->outfile);
        free(c->heredoc_delim);
        if (c->in_fd != -1)
            close(c->in_fd);
        free(c);
        c = next;
    }
    free(pl);
}

static void free_tokens(char **tokens)
{
    int i = 0;
    if (!tokens)
        return;
    while (tokens[i])
    {
        free(tokens[i]);
        i++;
    }
    free(tokens);
}

int main(int argc, char **argv, char **envp)
{
    char *line;
    char **tokens;
    char    *expanded;
    t_shell shell;
    t_pipeline *pl;
	int idx;
	int ret;
    (void)argc;
    (void)argv;

    shell.env = env_init(envp); // On recup l environnement utilisateur.
    if (!shell.env)
    {
        fprintf(stderr, "Error: failed to init env\n");
        return (1);
    }
    shell.last_exit_code = 0; // Initialisation du last_exit_code.

    init_signals(); // gestion des signaux.

    while (1)
    {
        line = readline("minishell> ");
        if (line == NULL) // Ctrl-D
        {
            printf("exit\n");
            break;
        }
        if (*line)
            add_history(line);
        tokens = ft_split_line(line);
        if (tokens && tokens[0])
        {
            // Etape d'expansion AVANT parse_pipeline
			idx = 0;
            // while (tokens[idx])
            // {
            //     expanded = expand_token(tokens[idx], shell.env, shell.last_exit_code);
            //     free(tokens[idx]);
            //     tokens[idx] = expanded;
			// 	idx++;
            // }
            while (tokens[idx])
            {
                if (is_simple_quote(tokens[idx]))
                    tokens[idx] = delete_quote(tokens[idx]);
                else
                {
                    if (is_double_quote(tokens[idx]))
                        tokens[idx] = delete_quote(tokens[idx]);
                    expanded = expand_token(tokens[idx], shell.env, shell.last_exit_code);
                    free(tokens[idx]);
                    tokens[idx] = expanded;
                }
                idx++;
            }
            // Verif des built-ins
            if (ft_strcmp(tokens[0], "env") == 0)
                env_print(shell.env);
            else if (ft_strcmp(tokens[0], "cd") == 0)
                shell.env = cd_do_cmd(shell.env, tokens[1]);
            else if (ft_strcmp(tokens[0], "pwd") == 0)
            {
                char *cwd = getcwd(NULL, 0);
                if (cwd)
                {
                    printf("%s\n", cwd);
                    free(cwd);
                }
                else
                    perror("pwd");
            }
            else if (ft_strcmp(tokens[0], "exit") == 0)
            {
                free(line);
                free_tokens(tokens);
                env_free(shell.env);
                return (0);
            }
            else
            {
                // Pipeline
                pl = parse_pipeline(tokens);
                if (!pl)
                {
                    free_tokens(tokens);
                    free(line);
                    continue;
                }
                if (!process_heredocs(pl->commands))
                {
                    free_pipeline(pl);
                    free_tokens(tokens);
                    free(line);
                    continue;
                }
                // Execute pipeline et mettre a jour last_exit_code
                ret = execute_pipeline(pl, shell.env);
                shell.last_exit_code = ret;
                free_pipeline(pl);
            }
        }
        if (tokens)
            free_tokens(tokens);

        printf("You typed in : %s\n", line);
        free(line);
    }
    env_free(shell.env);
    return (0);
}
