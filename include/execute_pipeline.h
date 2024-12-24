/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_pipeline.h                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qudomino <qudomino@student.42mulhouse.fr>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/16 14:27:02 by qudomino          #+#    #+#             */
/*   Updated: 2024/12/16 14:27:07 by qudomino         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXECUTE_PIPELINE_H
# define EXECUTE_PIPELINE_H

# include "pipeline.h"
# include "env.h"
# include "built_in.h"

typedef struct s_exec_context
{
    t_command *cmd;
    int idx;
    int cmd_count;
    int (*pipes)[2];
    t_env *env;
    t_shell *shell;
} t_exec_context;

typedef struct	s_exec_params
{
	t_env		*env;
	t_shell		*shell;
}				t_exec_params;

typedef struct s_exec_context_params
{
    t_command      *cmd;
    int            idx;
    int            cmd_count;
    int            (*pipes)[2];
    t_exec_params  *params;
}   t_exec_context_params;

int execute_pipeline(t_pipeline *pipeline, t_env *env, t_shell *shell);

extern int	g_cmd_error_code;

#endif
