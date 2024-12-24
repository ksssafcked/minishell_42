/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qudomino <qudomino@student.42mulhouse.fr>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/16 14:23:46 by qudomino          #+#    #+#             */
/*   Updated: 2024/12/16 14:23:53 by qudomino         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HEREDOC_H
# define HEREDOC_H

# include "command.h"
# include "env.h"

typedef struct s_expand_context {
    t_env *env;
    int last_exit_code;
} t_expand_context;

// Traite tous les heredocs du pipeline et remplace heredoc_delim par un infile pipe
int process_heredocs(t_command *cmds, t_env *env, int last_exit_code);

#endif
