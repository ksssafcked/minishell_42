/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsaiti <lsaiti@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/14 16:33:39 by qudomino          #+#    #+#             */
/*   Updated: 2024/12/16 18:44:45 by lsaiti           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ENV_H
# define ENV_H

typedef struct s_env {
    char            *var;
    struct s_env    *next;
} t_env;

t_env *env_init(char **envp);
void  env_print(t_env *env);
void  env_free(t_env *env);
char *env_get_value(t_env *env, const char *name);
char **env_to_char_array(t_env *env);
t_env *env_new_elem(char *var);

#endif
