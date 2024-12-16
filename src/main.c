/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsaiti <lsaiti@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/14 13:32:33 by qudomino          #+#    #+#             */
/*   Updated: 2024/12/16 16:16:10 by lsaiti           ###   ########.fr       */
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


// structure globale du shell pour la passer facilement aux fonctions
typedef struct s_shell {
    t_env *env;
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
    int j;
    t_shell shell;
	t_pipeline *pl;
    (void)argc;
    (void)argv;

    shell.env = env_init(envp); // On recup l environnement utilisateur et on l'envoie au built-in env.
    if (!shell.env)
    {
        fprintf(stderr, "Error: failed to init env\n");
        return (1);
    }	

    init_signals(); // gestion des signaux (on va les recuperer en fonction de la combinaison de touche qui est rentrée).

    while (1)
    {
        line = readline("minishell> ");
        if (line == NULL) // Si readline retourne NULL c'est que l'on a entre Ctrl-D (EOF).
        {
            printf("exit\n");
            break;
        }
        if (*line) // S'il y a bien un input.
            add_history(line); // On ajoute la ligne rentree à la fin de la liste chaînee 'history'.
        // On tokenise la ligne :
        tokens = ft_split_line(line);
        if (tokens && tokens[0])
        {
            // Avant on affichait les tokens maintenant on va verifier si c'est un builtin
            if (ft_strcmp(tokens[0], "env") == 0)
            {
                // builtin env
                env_print(shell.env);
            }
            else if (ft_strcmp(tokens[0], "export") == 0)
            {
                // builtin export
                export_cmd(shell.env, tokens[1]);
            }
            else if (ft_strcmp(tokens[0], "unset") == 0)
            {
                // builtin unset
                unset_cmd(&shell.env, tokens[1]);
            }
            else if (ft_strcmp(tokens[0], "echo") == 0)
			{
                // builtin echo
            	echo_cmd(&tokens[1]);
            }
            else if (ft_strcmp(tokens[0], "cd") == 0)
			{
                // builtin cd
            	shell.env = cd_do_cmd(shell.env, tokens[1]);
            }
            else if (ft_strcmp(tokens[0], "pwd") == 0)
            {
                // builtin pwd
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
                // builtin exit
                // On clean l'env, etc... 
                free(line);
                j = 0;
                while (tokens[j])
                {
                    free(tokens[j]);
                    j++;
                }
                free(tokens);
                env_free(shell.env);
                return (0);
            }
            else
            {
                // Ici si on est pas sur un env/pwd/exit => on traite la ligne comme un pipeline
                pl = parse_pipeline(tokens);
                if (!pl)
                {
                    // Erreur de parsing, on lib les tokens et on continue
                    free_tokens(tokens);
                    free(line);
                    continue;
                }
                // Process les heredoc avant d'exec
                if (!process_heredocs(pl->commands))
                {
                    // Erreur lors du traitement heredot faut tout free
                    free_pipeline(pl);
                    free_tokens(tokens);
                    free(line);
                    continue;
                }
                // On exec le pipeline (va gerer redirections, pipes, etc.)
                execute_pipeline(pl, shell.env);
                free_pipeline(pl);
            }
        }
        if (tokens)
            free_tokens(tokens);

        printf("You typed in : %s\n", line);
        free(line);
    }
    env_free(shell.env); // Liberation des ressources de l'environnement.
    return (0);
}
