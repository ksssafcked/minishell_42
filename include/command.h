/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qudomino <qudomino@student.42mulhouse.fr>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/16 14:21:15 by qudomino          #+#    #+#             */
/*   Updated: 2024/12/16 14:21:25 by qudomino         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMAND_H
# define COMMAND_H

typedef struct s_command {
    char            **argv;          // Arguments de la commande
    char            *infile;         // Fichier d'entree si <
    char            *outfile;        // Fichier de sortie si > ou >>
    int             append;          // 1 si >>, 0 si >
    char            *heredoc_delim;  // Delimiteur pour <<
	int             in_fd;			 // -1 si pas de heredoc pipe, sinon le fd du pipe heredoc
    struct s_command *next;          // Prochaine commande dans le pipeline
} t_command;

#endif
