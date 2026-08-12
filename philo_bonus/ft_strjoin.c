#include "philo_bonus.h"

static size_t	ft_strlen(const char *str)
{
	size_t	n;

	n = 0;
	while (str[n] != '\0')
		n++;
	return (n);
}

static size_t	ft_strlcpy(char *dst, const char *src, size_t size)
{
	size_t	i;

	i = 0;
	if (size == 0)
	{
		while (src[i])
			i++;
		return (i);
	}
	while (i < size - 1 && src[i] != '\0')
	{
		dst[i] = src[i];
		i++;
	}
	dst[i] = '\0';
	while (src[i] != '\0')
		i++;
	return (i);
}

static size_t	ft_strlcat(char *dst, const char *src, size_t size)
{
	size_t	i;
	size_t	d_len;
	size_t	s_len;

	d_len = ft_strlen(dst);
	s_len = ft_strlen(src);
	if (size <= d_len)
		return (size + s_len);
	else
	{
		i = 0;
		while (src[i] != '\0' && (d_len + i) < size - 1)
		{
			dst[d_len + i] = src[i];
			i++;
		}
		dst[d_len + i] = '\0';
		return (d_len + s_len);
	}
}

void	*ft_calloc(size_t nmemb, size_t size)
{
	void	*dest;
	size_t	tot;

	if (nmemb == 0 || size == 0)
	{
		return (malloc(0));
	}
	tot = nmemb * size;
	if (tot / size != nmemb)
		return (NULL);
	dest = malloc(tot);
	if (dest == NULL)
		return (NULL);
	while (tot > 0)
	{
		((char *)dest)[tot - 1] = 0;
		tot--;
	}
	return (dest);
}

char	*ft_strjoin(char const *s1, char const *s2)
{
	unsigned int	len_s1;
	unsigned int	len;
	char			*dest;

	len_s1 = ft_strlen(s1);
	len = len_s1 + ft_strlen(s2) + 1;
	dest = ft_calloc(len, 1);
	if (!dest)
		return (NULL);
	ft_strlcpy(dest, s1, len_s1 + 1);
	ft_strlcat(dest, s2, len);
	return (dest);
}
