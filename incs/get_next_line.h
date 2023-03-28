#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H

# include <unistd.h>
# include <stdlib.h>
# include <limits.h>

char	*ft_strchr(const char *s, int c);
char	*get_next_line(int fd);
char	*free_str(char	*s);
size_t	ft_strlen(const char *s);

#endif
