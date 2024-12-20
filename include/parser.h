/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsaiti <lsaiti@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/14 15:11:23 by qudomino          #+#    #+#             */
/*   Updated: 2024/12/16 21:03:43 by lsaiti           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_H
# define PARSER_H

char	**ft_split_line(char *line);
char    *delete_quote(char *line);
int is_double_quote(char *line);
int is_simple_quote(char *line);
char *get_spaces(char *line);

#endif
