/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsaiti <lsaiti@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/14 13:32:33 by qudomino          #+#    #+#             */
/*   Updated: 2024/12/17 17:21:01 by lsaiti           ###   ########.fr       */
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

		// Fonctions utilitaires locales
		// Définissons une fonction inline ici pour vérifier si un token est un opérateur
		// comme |, <, >, << ou >>
static int is_operator_token(char *s)
{
	if (!s)
		return 0;
	// On considère |, <, >, <<, >> comme des opérateurs
	// Attention, si tu as déjà défini une telle fonction, réutilise-la
	if (!ft_strcmp(s, "|") || !ft_strcmp(s, "<") || !ft_strcmp(s, ">") || !ft_strcmp(s, ">>") || !ft_strcmp(s, "<<"))
		return 1;
	return 0;
}

int main(int argc, char **argv, char **envp)
{
    char *line;
    char **tokens;
	char *tmp;
	char *first;
	char *last;
	int j;
    char    *expanded;
    t_shell shell;
    t_pipeline *pl;
	int idx;
	int ret;
	int count_tok;
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
        tokens = ft_split_line(line); // Lexer
		if (!tokens || !tokens[0])
		{
		// Ligne vide ou seulement espaces
			if (tokens)
				free_tokens(tokens);
			free(line);
			continue; // On attend un nouvel input
		}
		// Vérification de syntaxe de base : pipe ou redirection en debut / fin de ligne
		first = tokens[0];
		// Trouver le dernier token
		count_tok = 0;
		while (tokens[count_tok])
			count_tok++;
		last = tokens[count_tok - 1];
		// Si premier token est un operateur == erreur de syntaxe
		if (is_operator_token(first))
		{
			fprintf(stderr, "minishell: syntax error near unexpected token `%s`\n", first);
			free_tokens(tokens);
			free(line);
			continue;
		}
		// Si le dernier token est un opérateur, c'est aussi une erreur de syntaxe
		if (is_operator_token(last))
		{
			fprintf(stderr, "minishell: syntax error near unexpected token `%s`\n", last);
			free_tokens(tokens);
			free(line);
			continue;
		}	
        if (tokens && tokens[0])
        {
            // Etape d'expansion AVANT parse_pipeline
			idx = 0;
			while (tokens[idx])
			{
				if (is_simple_quote(tokens[idx]))
				{
				// Detail des etapes :
				// Le token est entre quotes simples, ex: 'hello $USER'
				// On retire les quotes simples, pas d'expansion.
					tmp = tokens[idx];
					tokens[idx] = delete_quote(tmp); // delete_quote enleve les quotes de debut et de fin
				// PAS d'expansion ici, on laisse tel quel.
				}
				else if (is_double_quote(tokens[idx]))
				{
					// Le token est entre quotes doubles, ex: "hello $USER"
					// On retire les quotes doubles, puis on fait l'expansion
					tmp = tokens[idx];
					tokens[idx] = delete_quote(tmp);
					expanded = expand_token(tokens[idx], shell.env, shell.last_exit_code);
					free(tokens[idx]);
					tokens[idx] = expanded;
				}
				else
				{
				// Le token n'a pas de quotes englobantes. ex: hello $USER
				// On fait l'expansion directement
					expanded = expand_token(tokens[idx], shell.env, shell.last_exit_code);
					free(tokens[idx]);
					tokens[idx] = expanded;
				}
				idx++;
			}	
            // Verif des built-ins
            if (ft_strcmp(tokens[0], "env") == 0)
                env_print(shell.env);
			else if (ft_strcmp(tokens[0], "export") == 0)
                export_cmd(shell.env, tokens[1]);
            else if (ft_strcmp(tokens[0], "unset") == 0)
                unset_cmd(shell.env, tokens[1]);
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
			// DEBUT
			else if (ft_strcmp(tokens[0], "exit") == 0)
			{
			// Gestion du builtin exit avec codes de retour
			// Format: exit [n]
			// Si n est un nombre, exit avec ce code
			// Si pas d'argument, exit avec shell->last_exit_code
			// Si argument invalide, afficher erreur et ne pas quitter

				fprintf(stderr, "exit\n"); // Bash affiche "exit" quand on quitte

				int exit_code;
				if (!tokens[1])
				{
				// Pas d’argument -> on quitte avec shell->last_exit_code
					exit_code = shell.last_exit_code;
				}
				else
				{
				// On a un argument
				// Verif s’il est numerique
					int is_numeric = 1;
					int x = 0;
					while (tokens[1][x])
					{
						if ((x == 0 && (tokens[1][x] == '+' || tokens[1][x] == '-')) 
							|| ft_isdigit(tokens[1][x]))
							continue;
						else
						{
							is_numeric = 0;
							break;
						}
						x++;
					}

					if (!is_numeric)
					{
						// Argument non numerique
						fprintf(stderr, "minishell: exit: %s: numeric argument required\n", tokens[1]);
						// Dans bash, si argument non numerique, on quitte avec 2
						// bash quitte quand même avec code 2
						exit_code = 2;
					}
					else
					{
						// Argument numerique
						long val = strtol(tokens[1], NULL, 10); // REmplacer la fonction
						// On prend val % 256 pour avoir un comportement similaire a bash
						exit_code = (unsigned char)val;
					}
				}
				free(line);
				j = 0;
				while (tokens[j])
				{
					free(tokens[j]);
					j++;
				}
				free(tokens);
				env_free(shell.env);
				exit(exit_code);
			}
			// fin
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
        free(line);
    }
    env_free(shell.env);
    return (0);
}
