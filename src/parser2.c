/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsaiti <lsaiti@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/16 16:51:19 by lsaiti            #+#    #+#             */
/*   Updated: 2024/12/16 17:00:57 by lsaiti           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

static size_t count_words(const char *line)
{
    size_t count;
    size_t i;
    int in_word;

	count = 0;
	i = 0;
	in_word = 0;
    while (line[i])
    {
        if (!is_sep(line[i]) && in_word == 0)
        {
            in_word = 1;
            count++;
        }
        else if (!is_sep(line[i]) && !is_sep(line[i + 1]) && in_word == 1 && line[i] == '|')
            count+= 2;
        else if (is_sep(line[i]))
            in_word = 0;
        i++;
    }
    return (count);
}

char **ft_split_line(const char *line)
{
    
}