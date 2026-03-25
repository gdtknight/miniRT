/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/10 13:04:38 by yoshin            #+#    #+#             */
/*   Updated: 2025/01/20 21:23:25 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>

#include "libft.h"

static int	initialize(char ***tokens, const char *s, char c);
static int	count_token(const char *s, const unsigned char c);
static int	tokenize(
				char ***tokens,
				const char *s,
				const char *ss,
				size_t size);
static void	release_tokens(char ***tokens, size_t size);

char	**ft_split(const char *s, char c)
{
	char	**tokens;
	size_t	size;
	size_t	idx;
	size_t	jdx;

	if (!initialize(&tokens, s, c))
		return (NULL);
	size = 0;
	idx = 0;
	while (idx < ft_strlen(s))
	{
		jdx = idx;
		while (s[jdx] != '\0' && s[jdx] != (unsigned char)c)
			jdx++;
		if (idx == jdx && s[jdx] == (unsigned char)c)
		{
			idx++;
			continue ;
		}
		if (!tokenize(&tokens, &s[idx], &s[jdx], size++))
			return (NULL);
		idx = jdx + 1;
	}
	tokens[size] = NULL;
	return (tokens);
}

static int	initialize(char ***tokens, const char *s, char c)
{
	*tokens = (char **) malloc(sizeof(char *) * \
						(count_token(s, c) + 1));
	if (!*tokens)
		return (0);
	return (1);
}

static int	count_token(const char *s, const unsigned char c)
{
	size_t	slen;
	size_t	idx;
	size_t	jdx;
	int		count;

	idx = 0;
	count = 0;
	slen = ft_strlen(s);
	while (idx < slen)
	{
		jdx = idx;
		while (s[jdx] != '\0' && s[jdx] != c)
			jdx++;
		if (idx == jdx && s[jdx] == c)
		{
			idx++;
			continue ;
		}
		count++;
		idx = jdx + 1;
	}
	return (count);
}

static int	tokenize(char ***tokens, const char *s, const char *ss, size_t size)
{
	(*tokens)[size] = ft_substr(s, 0, ss - s);
	if (!(*tokens)[size])
	{
		release_tokens(tokens, size);
		return (0);
	}
	return (1);
}

static void	release_tokens(char ***tokens, size_t size)
{
	while (size--)
		free((*tokens)[size]);
	free(*tokens);
}
