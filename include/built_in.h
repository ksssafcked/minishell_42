/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   built_in.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsaiti <lsaiti@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/16 16:52:19 by lsaiti            #+#    #+#             */
/*   Updated: 2024/12/24 18:36:50 by lsaiti           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BUILT_IN_H
# define BUILT_IN_H

# include "env.h"

typedef struct s_shell
{
	t_env	*env;
	int last_exit_code; // To store the last exit code
}			t_shell;

int			cd_cmd(t_env *env, char **args);
void		env_cmd(t_env *env);
void		echo_cmd(char **args);
void		export_cmd(t_shell *shell, t_env *env, char **args);
t_env		*unset_cmd(t_env *env, char **args);
int			run_builtin(t_shell *shell, char **args);
int			exit_cmd(char **args, int last_exit_code, t_env *env);

#endif
