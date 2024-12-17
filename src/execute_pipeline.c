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

// Compte le nombre de commandes
static int count_commands(t_command *cmd)
{
    int count = 0;
    while (cmd)
    {
        count++;
        cmd = cmd->next;
    }
    return count;
}

int execute_pipeline(t_pipeline *pipeline, t_env *env)
{
    t_command *cmd;
    int cmd_count;
    int i;
    int idx;
    int j;
    int k;
    int status;
    int in;
    int out;
    int flags;
    pid_t *pids;
    pid_t pid;
    char *path;
    char **envp_for_exec;

    cmd_count = count_commands(pipeline->commands);
    if (cmd_count == 0)
        return 0;

    // Créer un tableau de pipes
    int (*pipes)[2] = NULL;
    if (cmd_count > 1)
    {
        pipes = malloc(sizeof(int[2]) * (cmd_count - 1));
        if (!pipes)
        {
            perror("malloc");
            return 1;
        }
        i = 0;
		j = 0;
        while (i < cmd_count - 1)
        {
            if (pipe(pipes[i]) == -1)
            {
                perror("pipe");
                // Fermer les pipes déjà ouverts
                while (j < i)
                {
                    close(pipes[j][0]);
                    close(pipes[j][1]);
                    j++;
                }
                free(pipes);
                return 1;
            }
            i++;
        }
    }

    idx = 0;
    cmd = pipeline->commands;
    pids = malloc(sizeof(pid_t) * cmd_count);
    if (!pids)
    {
        perror("malloc");
        // Fermer les pipes
        if (pipes)
        {
            for (j = 0; j < cmd_count - 1; j++)
            {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            free(pipes);
        }
        return 1;
    }

    while (cmd)
    {
        pid = fork();
        if (pid == 0) // Processus fils
        {
            // Restaurer les signaux par défaut dans le fils
            signal(SIGINT, SIG_DFL);
            signal(SIGQUIT, SIG_DFL);

            // Redirections de pipe
            if (idx > 0)
            {
                if (dup2(pipes[idx - 1][0], STDIN_FILENO) == -1)
                {
                    perror("dup2");
                    exit(1);
                }
            }
            if (idx < cmd_count - 1)
            {
                if (dup2(pipes[idx][1], STDOUT_FILENO) == -1)
                {
                    perror("dup2");
                    exit(1);
                }
            }

            // Fermer tous les pipes dans le fils
            if (pipes)
            {
                for (j = 0; j < cmd_count - 1; j++)
                {
                    close(pipes[j][0]);
                    close(pipes[j][1]);
                }
            }

            // Redirections fichier (infile, outfile)
            if (cmd->in_fd != -1)
            {
                if (dup2(cmd->in_fd, STDIN_FILENO) == -1)
                {
                    perror("dup2");
                    exit(1);
                }
                close(cmd->in_fd);
            }

            if (cmd->infile)
            {
                in = open(cmd->infile, O_RDONLY);
                if (in < 0)
                {
                    perror(cmd->infile);
                    exit(1); // Erreur d'ouverture du fichier => exit(1) comme bash
                }
                if (dup2(in, STDIN_FILENO) == -1)
                {
                    perror("dup2");
                    close(in);
                    exit(1);
                }
                close(in);
            }

            if (cmd->outfile)
            {
                flags = O_WRONLY | O_CREAT;
                if (cmd->append)
                    flags |= O_APPEND;
                else
                    flags |= O_TRUNC;
                out = open(cmd->outfile, flags, 0644);
                if (out < 0)
                {
                    perror(cmd->outfile);
                    exit(1); // Permission denied ou autre => exit(1)
                }
                if (dup2(out, STDOUT_FILENO) == -1)
                {
                    perror("dup2");
                    close(out);
                    exit(1);
                }
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
            if (!envp_for_exec)
            {
                perror("env_to_char_array");
                free(path);
                exit(1);
            }

            execve(path, cmd->argv, envp_for_exec);
            perror("execve");
            // Libérer les ressources avant de quitter
            for (k = 0; envp_for_exec[k]; k++)
                free(envp_for_exec[k]);
            free(envp_for_exec);
            free(path);
            exit(1);
        }
        else if (pid < 0) // Erreur de fork
        {
            perror("fork");
            // Fermer les pipes et libérer la mémoire
            if (pipes)
            {
                for (j = 0; j < cmd_count - 1; j++)
                {
                    close(pipes[j][0]);
                    close(pipes[j][1]);
                }
                free(pipes);
            }
            free(pids);
            return 1;
        }
        else
        {
            pids[idx] = pid;
        }
        cmd = cmd->next;
        idx++;
    }

    // Fermer les pipes dans le parent
    if (pipes)
    {
        for (j = 0; j < cmd_count - 1; j++)
        {
            close(pipes[j][0]);
            close(pipes[j][1]);
        }
        free(pipes);
    }

    // Attendre tous les fils et récupérer le statut de la dernière commande
    int last_exit_code = 0;
    for (i = 0; i < cmd_count; i++)
    {
        if (waitpid(pids[i], &status, 0) == -1)
        {
            perror("waitpid");
            // Continuer à attendre les autres fils
        }
        if (i == cmd_count - 1)
        {
            if (WIFEXITED(status))
                last_exit_code = WEXITSTATUS(status);
            else if (WIFSIGNALED(status))
                last_exit_code = 128 + WTERMSIG(status);
            else
                last_exit_code = 1; // Autres cas d'erreur
        }
    }
    free(pids);
    return last_exit_code;
}
