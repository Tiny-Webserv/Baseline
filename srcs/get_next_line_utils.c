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
