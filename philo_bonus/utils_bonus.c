#include "philo_bonus.h"

void	precise_usleep(long target, t_table *table)
{
	long	start;
	long	elapsed;
	long	remaining;

	start = get_time(table);
	elapsed = get_time(table) - start;
	while (elapsed < target)
	{
		remaining = target - elapsed;
		if (remaining >= 10)
			usleep(5000);
		else if (remaining >= 3)
			usleep(500);
		else if (remaining >= 1)
			usleep(100);
		else
			usleep(10);
		elapsed = get_time(table) - start;
	}
}

long	get_time(t_table *table)
{
	struct timeval	tv;
	long			ms;

	if (gettimeofday(&tv, NULL) == -1)
		exit_error("gettimeofday failed\n", table);
	ms = (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
	return (ms);
}

int	ft_atoi(const char *str)
{
	int		i;
	long	result;

	i = 0;
	result = 0;
	while (str[i] == ' ' || (str[i] >= 9 && str[i] <= 13))
		i++;
	if (str[i] == '-')
		return (-1);
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
