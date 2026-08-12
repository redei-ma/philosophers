#include "philo.h"

long	get_time(void)
{
	struct timeval	tv;
	long			ms;

	if (gettimeofday(&tv, NULL) == -1)
		return (-1);
	ms = (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
	return (ms);
}

/**
 * Parses a positive integer and rejects anything else.
 *
 * @param str: the string to parse
 * @return the parsed value, or -1 for a negative number, an overflow past
 *         INT_MAX, or any trailing character
 *
 * Collapsing every failure onto -1 is enough here because the simulation has
 * no use for negative arguments, so validate_args can treat -1 as invalid
 * without needing to tell the failure modes apart.
 */
int	ft_atoi(const char *str)
{
	int		i;
	long	result;

	i = 0;
	result = 0;
	while (str[i] == ' ' || (str[i] >= 9 && str[i] <= 13))
		i++;
	if (str[i] == '-')
		return (-1);	/* a negative count of anything is already invalid */
	else if (str[i] == '+')
		i++;
	while (str[i] >= '0' && str[i] <= '9')
	{
		result = result * 10 + (str[i] - '0');
		i++;
	}
	if (result > 2147483647 || str[i] != '\0')
		return (-1);
	return ((int)result);
}

int	validate_args(int ac, char **av)
{
	int	i;

	if (ac <= 4 || ac >= 7)
	{
		printf("Error: Invalid number of arguments\n");
		return (1);
	}
	if (ft_atoi(av[1]) <= 0)
	{
		printf("Error: Philosophers must be at least 1\n");
		return (1);
	}
	i = 2;
	while (i < ac)
	{
		if (ft_atoi(av[i]) < 0)
		{
			printf("Error: Arguments must be positive integers\n");
			return (1);
		}
		i++;
	}
	return (0);
}
