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

# include <stddef.h>

char	**ft_split_line(char *line);
char    *delete_quote(char *line);
int is_double_quote(const char *str);
int is_simple_quote(const char *str);
char *remove_all_quotes(const char *str);

typedef struct s_parser_context {
	const char **line_ptr;    // Pointeur vers la ligne en cours d'analyse
	char *token;              // Token en cours de construction
	int in_single_quote;      // Indicateur de quote simple
	int in_double_quote;      // Indicateur de quote double
	size_t capacity;          // Capacite actuelle du token
	size_t length;            // Longueur actuelle du token
}	t_parser_context;

#endif
