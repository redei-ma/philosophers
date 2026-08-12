#include "philo.h"

/**
 * Assigns the two forks a philosopher locks, in a deadlock-free order.
 *
 * @param philo: the philosopher to assign forks to
 * @param table: holds the fork array and the philosopher count
 *
 * Odd philosophers reach for their right fork first, even ones for their
 * left. This breaks the circular wait: since every philosopher acquires forks
 * in the same relative order, no cycle of "holds one, waits for the next" can
 * close, and the table cannot deadlock even with an odd number of seats.
 */
static void	assign_fork(t_philo *philo, t_table *table)
{
	int	right;
	int	left;

	right = philo->id - 1;
	left = philo->id % table->philo_nbr;
	if (philo->id % 2 == 0)
	{
		philo->first_fork = &table->forks[left];
		philo->second_fork = &table->forks[right];
	}
	else
	{
		philo->first_fork = &table->forks[right];
		philo->second_fork = &table->forks[left];
	}
}

static int	philo_init(t_table *table)
{
	int	i;

	i = 0;
	while (i < table->philo_nbr)
	{
		table->philos[i].id = i + 1;
		if (table->limit_meals == 0)
			table->philos[i].full = true;
		table->philos[i].table = table;
		assign_fork(&table->philos[i], table);
		if (safe_mutex(&table->philos[i].meal_mtx, INIT))
			return (1);
		table->philos[i].meal_mtx_init = 1;
		i++;
	}
	return (0);
}

static int	set_table(char **av, t_table *table)
{
	table->philos = NULL;
	table->forks = NULL;
	table->print_mtx_init = 0;
	table->monitor_mtx_init = 0;
	table->philo_nbr = ft_atoi(av[1]);
	table->time_to_die = ft_atoi(av[2]);
	table->time_to_eat = ft_atoi(av[3]);
	table->time_to_sleep = ft_atoi(av[4]);
	if (av[5])
		table->limit_meals = ft_atoi(av[5]);
	else
		table->limit_meals = -1;
	table->end_program = false;
	table->philos = malloc(sizeof(t_philo) * table->philo_nbr);
	table->forks = malloc(sizeof(t_fork) * table->philo_nbr);
	if (!table->philos || !table->forks)
		return (1);
	memset(table->philos, 0, sizeof(t_philo) * table->philo_nbr);
	memset(table->forks, 0, sizeof(t_fork) * table->philo_nbr);
	return (0);
}

/**
 * Allocates and initialises the table, the forks and the philosophers.
 *
 * @param av: argv, already checked by validate_args
 * @param table: uninitialised table, filled in on success
 * @return 0 on success, 1 if an allocation or a mutex initialisation failed
 *
 * Every pointer and init flag is zeroed before the first call that can fail,
 * so cleanup() stays safe no matter how far initialisation got before giving
 * up.
 */
int	set_dinner(char **av, t_table *table)
{
	int	i;

	i = 0;
	if (set_table(av, table))
		return (1);
	while (i < table->philo_nbr)
	{
		if (safe_mutex(&table->forks[i].fork, INIT))
			return (1);
		table->forks[i].id = i;
		table->forks[i].mtx_init = 1;
		i++;
	}
	if (safe_mutex(&table->print_mtx, INIT))
		return (1);
	table->print_mtx_init = 1;
	if (safe_mutex(&table->monitor_mtx, INIT))
		return (1);
	table->monitor_mtx_init = 1;
	return (philo_init(table));
}
