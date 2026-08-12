#include "philo_bonus.h"

static void	init_pointers(t_table *table)
{
	table->philos = NULL;
	table->forks = NULL;
	table->print_sem = NULL;
	table->monitor_sem = NULL;
	table->limit_sem = NULL;
	table->die_sem = NULL;
}

/**
 * Opens the private semaphore guarding one philosopher's meal state.
 *
 * @param id: philosopher number, used to build the semaphore name
 * @param table: passed through so a failure can be reported and cleaned up
 * @return the open semaphore
 *
 * The name is unlinked before being opened, so a semaphore left behind by an
 * interrupted run cannot be inherited with a stale count.
 */
static sem_t	*open_meal_sem(int id, t_table *table)
{
	char	*sem_name;
	char	*number;
	sem_t	*sem;

	number = ft_itoa(id);
	if (!number)
		exit_error("malloc failed\n", table);
	sem_name = ft_strjoin("/meal_sem_", number);
	free(number);
	if (!sem_name)
		exit_error("malloc failed\n", table);
	sem_unlink(sem_name);
	sem = safe_sem_open(sem_name, 1, table);
	free(sem_name);
	return (sem);
}

static void	philo_init(t_table *table)
{
	int	i;

	i = 0;
	while (i < table->philo_nbr)
	{
		table->philos[i].id = i + 1;
		if (table->limit_meals == 0)
			table->philos[i].full = true;
		table->philos[i].table = table;
		table->philos[i].meal_sem = open_meal_sem(i + 1, table);
		i++;
	}
}

/**
 * Opens the five semaphores shared by every philosopher process.
 *
 * @param table: table whose semaphore fields are filled in
 *
 * limit_sem is sized at half the philosophers, rounded up, so that a full
 * round of admitted eaters always has two forks each. die_sem starts at zero
 * on purpose: only kill_all ever waits on it, which leaves no room for a race
 * over who picks up the token.
 */
static void	set_semaphores(t_table *table)
{
	int	limit;

	sem_unlink("/forks");
	sem_unlink("/print");
	sem_unlink("/monitor");
	sem_unlink("/limit");
	sem_unlink("/die");
	if (table->philo_nbr % 2 == 0)
		limit = table->philo_nbr / 2;
	else
		limit = (table->philo_nbr / 2) + 1;
	table->forks = safe_sem_open("/forks", table->philo_nbr, table);
	table->print_sem = safe_sem_open("/print", 1, table);
	table->monitor_sem = safe_sem_open("/monitor", 1, table);
	table->limit_sem = safe_sem_open("/limit", limit, table);
	table->die_sem = safe_sem_open("/die", 0, table);
}

void	set_dinner(t_table *table, char **av)
{
	init_pointers(table);
	table->philo_nbr = ft_atoi(av[1]);
	table->time_to_die = ft_atoi(av[2]);
	table->time_to_eat = ft_atoi(av[3]);
	table->time_to_sleep = ft_atoi(av[4]);
	if (av[5])
		table->limit_meals = ft_atoi(av[5]);
	else
		table->limit_meals = -1;
	table->end_dinner = false;
	table->philos = safe_malloc(sizeof(t_philo) * table->philo_nbr, table);
	memset(table->philos, 0, sizeof(t_philo) * table->philo_nbr);
	set_semaphores(table);
	philo_init(table);
}
