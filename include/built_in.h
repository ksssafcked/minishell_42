/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   built_in.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsaiti <lsaiti@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/16 16:52:19 by lsaiti            #+#    #+#             */
/*   Updated: 2024/12/16 16:52:20 by lsaiti           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BUILT_IN_H
# define BUILT_IN_H
# include "env.h"

t_env	*cd_do_cmd(t_env *env, char *new_dir);
void	env_cmd(t_env *env);
void	echo_cmd(char **args);
void	export_cmd(t_env *env, char *new_env);
void	unset_cmd(t_env **env, char *delete_var);

#endif
