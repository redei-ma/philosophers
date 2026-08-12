#include "philo.h"

int	main(int ac, char **av)
{
	t_table	table;

	if (validate_args(ac, av))
	{
		printf("Usage: ./philo 5 800 200 200 [7]\n");
		return (1);
	}
	if (set_dinner(av, &table))
	{
		printf("Error: initialization failed\n");
		cleanup(&table);
		return (1);
	}
	if (start_dinner(&table))
	{
		printf("Error: thread creation failed\n");
		cleanup(&table);
		return (1);
	}
	cleanup(&table);
	return (0);
}
