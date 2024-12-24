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
    char            **argv;          // Command arguments
    char            *infile;         // Input file if '<'
    char            *outfile;        // Output file if '>' or '>>'
    int             append;          // 1 if >>, 0 if >
    char            *heredoc_delim;  // Delimiter for '<<'
    int             in_fd;           // -1 if no heredoc pipe, else the heredoc pipe fd
	int				out_fd;			 // -1 if no outfile, else fd
    int             heredoc_quoted;  // Indicates if heredoc delimiter was quoted
    struct s_command *next;          // Next command in the pipeline
} t_command;

#endif
