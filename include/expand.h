/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qudomino <qudomino@student.42mulhouse.fr>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/16 17:19:22 by qudomino          #+#    #+#             */
/*   Updated: 2024/12/16 17:19:42 by qudomino         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXPAND_H
# define EXPAND_H

# include "env.h"

typedef struct s_expand_con {
    int last_exit_code;
    int *i;
    t_env *env;
} t_expand_con;

char *expand_token(const char *token, t_env *env, int last_exit_code);

#endif
