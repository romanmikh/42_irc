/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split_set.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eamsalem <eamsalem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 14:15:37 by eamsalem          #+#    #+#             */
/*   Updated: 2024/11/12 15:22:58by eamsalem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/libft.h"

static void	skip_set_index(char const *str, const char *set, int *i)
{
	while (str[*i] && chrsetcmp(str[*i], set))
		(*i)++;
}

static int	skip_word(char const *str, const char *set, int i)
{
	while (str[i] && !chrsetcmp(str[i], set))
		i++;
	return (i);
}

static char	*get_word(char const *str, const char *set, int *i)
{
	int 	j;
	int		word_len;
	char	*word;

	word_len = skip_word(str, set, *i) - *i;
	word = (char *)malloc(sizeof(char) * (word_len + 1));
	if (!word)
		return (NULL);
	j = 0;
	while (str[*i] && !chrsetcmp(str[*i], set))
		word[j++] = str[(*i)++];
	word[j] = '\0';
	return (word);
}

static int	count_words(char const *str, const char *set)
{
	int	count;
	int	i;

	i = 0;
	count = 0;
	skip_set_index(str, set, &i);
	while (str[i])
	{
		i = skip_word(str, set, i);
		count++;
		skip_set_index(str, set, &i);
	}
	return (count);
}

char	**ft_split_set(char const *str, const char *set)
{
	int		i;
	int		j;
	int		word_count;
	char	**split_strs;

	word_count = count_words(str, set);
	split_strs = (char **)malloc(sizeof(char *) * (word_count + 1));
	if (!split_strs)
		return (NULL);
	i = 0;
	j = 0;
	skip_set_index(str, set, &i);
	while (j < word_count)
	{
		split_strs[j] = get_word(str, set, &i);
		skip_set_index(str, set, &i);
		j++;
	}
	split_strs[j] = 0;
	return (split_strs);
}
/*
#include <stdio.h>

int	main(void)
{
	int	i;
	char	**strs = ft_split_set("  hello	my name is		eden ", " \t");

	i = 0;
	while (strs[i])
	{
		printf("%s\n", strs[i]);
		i++;
	}
}
*/