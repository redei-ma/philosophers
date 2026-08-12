#include "philo_bonus.h"

static int	ft_count_let(int n)
{
	int	count;

	count = 0;
	if (n <= 0)
		count++;
	while (n != 0)
	{
		count++;
		n /= 10;
	}
	return (count);
}

char	*ft_itoa(int n)
{
	char		*dest;
	int			len;
	long long	nb;

	nb = n;
	len = ft_count_let(n);
	dest = (char *)ft_calloc((len + 1), sizeof(char));
	if (!dest)
		return (NULL);
	if (nb < 0)
	{
		dest[0] = '-';
		nb = -nb;
	}
	if (nb == 0)
		dest[0] = '0';
	while (nb != 0)
	{
		dest[--len] = (nb % 10) + '0';
		nb /= 10;
	}
	return (dest);
}
