/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nahyeon <nahyeon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/24 01:36:19 by hyna              #+#    #+#             */
/*   Updated: 2023/03/27 15:56:13 by nahyeon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

static char	*mv_backup(char	*backup, int	*check)
{
	char	*result;
	int		i;
	int		j;

	i = 0;
	while (backup[i] && (backup[i] != '\n'))
		i++;
	if (backup[i] == 0)
		return (NULL);
	j = i;
	while (backup[i])
		i++;
	result = malloc(i - j);
	if (!result)
	{
		*check = 0;
		return (NULL);
	}
	i = 0;
	j++;
	while (backup[j])
		result[i++] = backup[j++];
	result[i] = 0;
	free(backup);
	return (result);
}

static char	*make_result(char	*backup)
{
	char	*result;
	int		i;
	int		j;

	i = 0;
	j = 0;
	while (backup[i] && (backup[i] != '\n'))
		i++;
	if (!(backup[i]))
		return ((char *)backup);
	result = malloc(i + 2);
	if (!result)
		return (NULL);
	while (j <= i)
	{
		result[j] = backup[j];
		j++;
	}
	result[j] = 0;
	return (result);
}

static char	*make_backup(char	*s1, char	*s2)
{
	char	*result;
	size_t	s1_len;
	size_t	s2_len;
	size_t	i;

	if (s1)
		s1_len = ft_strlen(s1);
	else
		s1_len = 0;
	s2_len = ft_strlen(s2);
	result = malloc(s1_len + s2_len + 1);
	if (!result)
		return (0);
	i = -1;
	while (++i < s1_len)
		result[i] = s1[i];
	i = 0;
	while (i < s2_len)
		result[s1_len++] = s2[i++];
	result[s1_len] = 0;
	if (s1)
		free(s1);
	return (result);
}

static char	*read_files(int fd, char	*backup)
{
	ssize_t	read_len;
	char	buf[BUFFER_SIZE + 1];

	buf[0] = 0;
	buf[BUFFER_SIZE] = 0;
	while (!ft_strchr(buf, '\n'))
	{
		read_len = read(fd, buf, BUFFER_SIZE);
		if (read_len == -1)
			return (NULL);
		else if (read_len == 0)
		{
			if (!backup)
				return (NULL);
			return (backup);
		}
		buf[read_len] = 0;
		backup = make_backup(backup, buf);
	}
	return (backup);
}

char	*get_next_line(int fd)
{
	static char	*backup[OPEN_MAX];
	char		*result;
	char		*tmp;
	int			check;

	check = 1;
	if ((fd < 0) || (BUFFER_SIZE <= 0))
		return (NULL);
	tmp = read_files(fd, backup[fd]);
	if (!tmp)
		return (NULL);
	else if (!tmp[0])
		return (free_str(tmp));
	result = make_result(tmp);
	if (!result)
		return (free_str(backup[fd]));
	backup[fd] = mv_backup(tmp, &check);
	if (!check)
		return (free_str(result));
	return (result);
}
