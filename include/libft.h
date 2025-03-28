#ifndef LIBFT_H
#define LIBFT_H

#include "irc.hpp"

int     chrsetcmp(char c, const char *set);

char    **ft_split_set(char const *str, const char *set);

int     ft_match(const char *s1, const char *s2);

char	**ft_split(char const *str, const char chr);

int	    ft_2darr_len(void **arr);

void	free_2darr(void **arr, int len);

#endif