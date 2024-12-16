/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qudomino <qudomino@student.42mulhouse.fr>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/16 14:24:07 by qudomino          #+#    #+#             */
/*   Updated: 2024/12/16 14:24:23 by qudomino         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../include/command.h"
#include "../include/heredoc.h"
#include "../include/utils.h" 

// Lit l'input jusqu'au delim et renvoie un fd (pipe) contenant le contenu oklm
static int create_heredoc_pipe(char *delim)
{
	char *line;
	size_t len;
    int fd[2];
	ssize_t r;
    if (pipe(fd) == -1)
        return (-1);
    line = NULL;
    len = 0;
    while (1)
    {
        write(STDOUT_FILENO, "> ", 2);
        r = getline(&line, &len, stdin); // /!\ IMPLEMENTER GNL ICI QUAND ON PEUT PAR PITIE !!!! (je hais gnl)
        if (r == -1) // Ctrl-D
            break;
        // Enlever le \n
        if (r > 0 && line[r-1] == '\n')
            line[r-1] = '\0';
        if (ft_strcmp(line, delim) == 0)
            break;
        dprintf(fd[1], "%s\n", line);
    }
    free(line);
    close(fd[1]);
    return (fd[0]);
}

int process_heredocs(t_command *cmds)
{
    t_command *c;
	int in_fd;

	c = cmds;
    while (c)
    {
        if (c->heredoc_delim)
        {
            in_fd = create_heredoc_pipe(c->heredoc_delim);
            if (in_fd == -1)
                return (0);
            // On va faire conversion fd en un "fichier" temp, cad on va dupliquer le fd pour l'infile.
            c->infile = NULL; // pas de fichier
            c->append = 0;
            // On utilise in_fd pour indiquer qu'on a un heredoc:
            c->infile = NULL;
            c->in_fd = in_fd;
        }
        c = c->next;
    }
    return (1);
}
