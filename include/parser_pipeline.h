/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_pipeline.h                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qudomino <qudomino@student.42mulhouse.fr>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/16 14:22:01 by qudomino          #+#    #+#             */
/*   Updated: 2024/12/16 14:22:08 by qudomino         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_PIPELINE_H
# define PARSER_PIPELINE_H

# include "pipeline.h"

// Fonction principale qui transforme un tableau de tokens en t_pipeline
t_pipeline *parse_pipeline(char **tokens);

#endif
