/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsaiti <lsaiti@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/14 16:44:52 by qudomino          #+#    #+#             */
/*   Updated: 2024/12/16 15:36:42 by lsaiti           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/utils.h"
#include <stdlib.h>

int	ft_strcmp(char *s1, char *s2)
{
	while (*s2 != '\0' && *s1 != '\0' && *s1 == *s2)
	{
		s1++;
		s2++;
	}
	return (*s1 - *s2);
}

int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	size_t	i;

	i = 0;
	if (n == 0)
		return (0);
	while (n > i && s1[i] != '\0' && s2[i] != '\0' && s1[i] == s2[i])
		i++;
	if (i == n)
		return (0);
	return ((unsigned char)s1[i] - (unsigned char)s2[i]);
}

size_t	ft_strlen(const char *str)
{
	size_t	i;

	i = 0;
	while (str[i])
		i++;
	return (i);
}

char	*ft_strdup(const char *src)
{
	int		i;
	int		len;
	char	*str;

	len = ft_strlen(src);
	str = malloc((len + 1) * sizeof(char));
	if (str == NULL)
		return (NULL);
	i = 0;
	while (i < len)
	{
		str[i] = src[i];
		i++;
	}
	str[len] = '\0';
	return (str);
}

void	*ft_memcpy(void *dest_str, const void *src_str, size_t n)
{
	const unsigned char	*src;
	unsigned char		*dest;
	size_t				i;

	dest = (unsigned char *)dest_str;
	src = (const unsigned char *)src_str;
	if (dest == src || n == 0)
		return (dest_str);
	i = 0;
	while (n > i)
	{
		dest[i] = src[i];
		i++;
	}
	return (dest_str);
}

char	*ft_strchr(const char *str, int c)
{
	while (*str != (char)c)
	{
		if (*str == '\0')
			return (NULL);
		str++;
	}
	return ((char *)str);
}

void		*ft_realloc(void *ptr, size_t size)
{
	void	*new_ptr;

	if (ptr == NULL)
		return (malloc(size));
	if (!size)
		return (ptr);
	new_ptr = malloc(size);
	if (!new_ptr)
		return (NULL);
	ft_memcpy(new_ptr, ptr, size);
	free(ptr);
	return (new_ptr);
}

void	*ft_memset(void *str, int c, size_t n)
{
	unsigned char	*ptr;
	size_t			i;

	i = 0;
	ptr = (unsigned char *)str;
	while (n > i)
		ptr[i++] = (unsigned char)c;
	return (str);
}

char	*ft_strjoin(char *s1, char *s2, void *ptr)
{
	size_t	total_len;
	char	*str;
	int		i;
	int		j;

	if (!s1 || !s2)
		return (NULL);
	total_len = ft_strlen(s1) + ft_strlen(s2);
	str = malloc((total_len + 1) * sizeof(char));
	if (!str)
		return (NULL);
	i = 0;
	while (s1[i])
	{
		str[i] = s1[i];
		i++;
	}
	j = 0;
	while (s2[j])
	{
		str[i + j] = s2[j];
		j++;
	}
	str[i + j] = '\0';
	if (ptr)
		free(ptr);
	return (str);
}

char	*ft_strnstr(const char *big, const char *little, unsigned int len)
{
	int				i;
	unsigned int	j;

	j = 0;
	if (!*little)
		return ((char *)big);
	while (*big && j < len)
	{
		i = 0;
		if (big[i] == little[i])
		{
			while (big[i] == little[i] && little[i] && big[i] && (j + i < len))
			{
				i++;
			}
			if (little[i] == '\0')
				return ((char *)big);
		}
		j++;
		big++;
	}
	return (0);
}

int	ft_isalnum(int c)
{
	if ((c >= 48 && c <= 57) || (c >= 65 && c <= 90) || (c >= 97 && c <= 122))
		return (1);
	else
		return (0);
}

void	ft_bzero(void *dst, size_t n)
{
	unsigned char	*zero;
	size_t			i;

	i = 0;
	zero = (unsigned char *)dst;
	while (n > i)
	{
		zero[i] = 0;
		i++;
	}
}

void	*ft_calloc(size_t num, size_t size)
{
	unsigned char	*str;
	size_t			total_size;

	total_size = num * size;
	str = (unsigned char *)malloc(total_size);
	if (str == NULL)
		return (NULL);
	ft_bzero(str, total_size);
	return (str);
}

char	*ft_strcat(char *dest, char *src)
{
	int	i;
	int	len;

	i = 0;
	while (dest[i])
		i++;
	len = 0;
	while (src[len])
	{
		dest[i] = src[len];
		i++;
		len++;
	}
	dest[i] = '\0';
	return (dest);
}

char	*ft_strcpy(char *dest, char *src)
{
	int	i;

	i = 0;
	while (src[i])
	{
		dest[i] = src[i];
		i++;
	}
	dest[i] = '\0';
	return (dest);
}

int	ft_isdigit(int d)
{
	if (d >= 48 && d <= 57)
		return (1);
	else
		return (0);
}
