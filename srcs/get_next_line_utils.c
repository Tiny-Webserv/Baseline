/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils_bonus.c                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nahyeon <nahyeon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/24 01:36:53 by hyna              #+#    #+#             */
/*   Updated: 2023/03/27 15:57:08 by nahyeon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

char	*ft_strchr(const char	*s, int c)
{
	char	tmp;

	if (!s)
		return (NULL);
	tmp = (char)c;
	while ((*s) && (*s != tmp))
		s++;
	if (*s == tmp)
		return ((char *)s);
	return (NULL);
}

size_t	ft_strlen(const char	*s)
{
	int	len;

	len = 0;
	while (s[len])
		len++;
	return (len);
}

char	*free_str(char	*s)
{
	free(s);
	return (NULL);
}
