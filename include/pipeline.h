/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qudomino <qudomino@student.42mulhouse.fr>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/16 14:21:39 by qudomino          #+#    #+#             */
/*   Updated: 2024/12/16 14:21:44 by qudomino         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPELINE_H
# define PIPELINE_H

# include "command.h"

typedef struct s_pipeline {
    t_command   *commands; // Liste chaînee des commandes
} t_pipeline;

void free_pipeline(t_pipeline *pl);

#endif
