#include "philo.h"

static int	all_philo_full(t_table *table)
{
	int	i;

	i = 0;
	while (i < table->philo_nbr)
	{
		if (!get_bool(&table->philos[i].meal_mtx, &table->philos[i].full))
			return (0);
		i++;
	}
	return (1);
}

/**
 * Declares a philosopher dead if it starved, and stops the simulation.
 *
 * @param philo: the philosopher to check
 * @param table: holds the stop flag and time_to_die
 * @return 1 if this philosopher has just died, 0 otherwise
 *
 * A philosopher that reached its meal quota stops refreshing last_meal_time,
 * so it has to be skipped. Without that check it would be reported dead
 * time_to_die milliseconds after finishing, while the others are still eating.
 */
static int	check_die(t_philo *philo, t_table *table)
{
	if (get_bool(&philo->meal_mtx, &philo->full))
		return (0);
	if ((get_time() - get_long(&philo->meal_mtx, &philo->last_meal_time))
		> table->time_to_die)
	{
		set_bool(&table->monitor_mtx, &table->end_program, true);
		print_status(philo, DIE, table);
		return (1);
	}
	return (0);
}

void	*monitoring(void *arg)
{
	t_table	*table;
	int		i;

	table = (t_table *)arg;
	while (1)
	{
		i = 0;
		if (all_philo_full(table))
		{
			set_bool(&table->monitor_mtx, &table->end_program, true);
			return (NULL);
		}
		while (i < table->philo_nbr)
		{
			if (check_die(&table->philos[i], table))
				return (NULL);
			i++;
		}
		usleep(1000);
	}
	return (NULL);
}
