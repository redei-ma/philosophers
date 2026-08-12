#include "philo_bonus.h"

int	main(int ac, char **av)
{
	t_table	table;

	if (validate_args(ac, av))
	{
		printf("Usage: ./philo_bonus 5 800 200 200 [7]\n");
		return (1);
	}
	set_dinner(&table, av);
	start_dinner(&table);
	finish_dinner(&table);
	cleanup(&table, FULL);
	return (0);
}
