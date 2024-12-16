/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_pipeline.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qudomino <qudomino@student.42mulhouse.fr>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/16 14:27:25 by qudomino          #+#    #+#             */
/*   Updated: 2024/12/16 14:27:38 by qudomino         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "../include/pipeline.h"
#include "../include/env.h"
#include "../include/execute_pipeline.h"
#include "../include/execute.h"
#include "../include/env.h"

static int count_commands(t_command *cmd)
{
    int count;

	count = 0;
    while (cmd)
    {
        count++;
        cmd = cmd->next;
    }
    return (count);
}

int execute_pipeline(t_pipeline *pipeline, t_env *env)
{
	t_command *cmd;
    int cmd_count;
	int i;
    int idx;
	int j;
	int k;
	int flags;
	int out;
	int status;
    pid_t *pids;
	pid_t pid;
	char **envp_for_exec;
	char *path;

	cmd_count = count_commands(pipeline->commands);
    if (cmd_count == 0)
        return (0);
    // On fait un tableau de pipes
    int (*pipes)[2] = NULL;
    if (cmd_count > 1)
    {
        pipes = malloc(sizeof(int[2]) * (cmd_count - 1));
		i = 0;
        while (i < cmd_count - 1)
        {
            if (pipe(pipes[i]) == -1)
            {
                perror("pipe");
                return (1);
            }
			i++;
        }
    }
	idx = 0;
	cmd = pipeline->commands;
	pids = malloc(sizeof(pid_t) * cmd_count);
    while (cmd)
    {
        pid = fork();
        if (pid == 0)
        {
            // CHilds
            // Redirections de pipe
            if (idx > 0)
            {
                // pas la premiere commande, lire depuis pipe[idx-1][0]
                dup2(pipes[idx-1][0], STDIN_FILENO);
            }
            if (idx < cmd_count - 1)
            {
                // pas la derniere commande, ecrire vers pipe[idx][1]
                dup2(pipes[idx][1], STDOUT_FILENO);
            }
            // Fermer tous les pipes
            if (pipes)
            {
				j = 0;
                while (j < cmd_count - 1)
                {
                    close(pipes[j][0]);
                    close(pipes[j][1]);
					j++;
                }
            }
            // Redirections fichier
            if (cmd->in_fd != -1)
            {
                dup2(cmd->in_fd, STDIN_FILENO);
                close(cmd->in_fd);
            }
            else if (cmd->infile)
            {
                int in = open(cmd->infile, O_RDONLY);
                if (in < 0)
                {
                    perror(cmd->infile);
                    exit(1);
                }
                dup2(in, STDIN_FILENO);
                close(in);
            }
            if (cmd->outfile)
            {
                flags = O_WRONLY | O_CREAT;
                if (cmd->append)
                    flags |= O_APPEND; // Operation bitwise on assigne a gauche la valeur a droite.
                else
                    flags |= O_TRUNC;
                out = open(cmd->outfile, flags, 0644);
                if (out < 0)
                {
                    perror(cmd->outfile);
                    exit(1);
                }
                dup2(out, STDOUT_FILENO);
                close(out);
            }

            // Execution de la commande
            if (!cmd->argv || !cmd->argv[0])
                exit(0); // commande vide
            path = get_command_path(env, cmd->argv[0]);
            if (!path)
            {
                fprintf(stderr, "minishell: %s: command not found\n", cmd->argv[0]);
                exit(127);
            }
            envp_for_exec = env_to_char_array(env);
            execve(path, cmd->argv, envp_for_exec);
            perror("execve");
            // free envp_for_exec
			k = 0;
            while (envp_for_exec[k])
			{
				free(envp_for_exec[k]);
				k++;
			}
            free(envp_for_exec);
            free(path);
            exit(1);
        }
        else if (pid < 0)
        {
            perror("fork");
            return (1);
        }
        else
            pids[idx] = pid;
        cmd = cmd->next;
        idx++;
    }
    // Fermer les pipes dans le parent
    if (pipes)
    {
		j = 0;
        while (j < cmd_count - 1)
        {
            close(pipes[j][0]);
            close(pipes[j][1]);
			j++;
        }
        free(pipes);
    }
    // Attendre tous les childs
	i = 0;
    while (i < cmd_count)
    {
        waitpid(pids[i], &status, 0);
		i++;
        // On pourrait stocker $? ici
    }
    free(pids);
    return (0);
}
