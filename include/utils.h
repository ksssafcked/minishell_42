/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsaiti <lsaiti@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/14 16:45:09 by qudomino          #+#    #+#             */
/*   Updated: 2024/12/16 15:36:57 by lsaiti           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_H
# define UTILS_H

#include <stddef.h>

int		ft_strcmp(char *s1, char *s2);
int     ft_strncmp(const char *s1, const char *s2, size_t n);
size_t	ft_strlen(const char *str);
char	*ft_strdup(const char *src);
void	*ft_memcpy(void *dest_str, const void *src_str, size_t n);
char	*ft_strchr(const char *str, int c);
void	*ft_realloc(void *ptr, size_t size);
void	*ft_memset(void *str, int c, size_t n);
char	*ft_strjoin(char *s1, char *s2, void *ptr);
char	*ft_strnstr(const char *big, const char *little, unsigned int len);
int		ft_isalnum(int c);
void	*ft_calloc(size_t num, size_t size);
char	*ft_strcat(char *dest, char *src);
char	*ft_strcpy(char *dest, char *src);
int		ft_isdigit(int d);


#endif
