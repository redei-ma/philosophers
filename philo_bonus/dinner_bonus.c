#include "philo_bonus.h"

/**
 * Announces the thinking phase and yields briefly to the neighbours.
 *
 * @param philo: the philosopher that has just finished sleeping
 * @param table: holds the timings
 *
 * think_time is the slack left before starvation, and it is deliberately used
 * as a gate rather than as a duration. Sleeping all of it would make every
 * philosopher show up at the forks exactly on its deadline, spending the whole
 * margin on waiting and leaving nothing in reserve for a round where the forks
 * happen to be busy. One millisecond is enough to let a neighbour take its
 * turn, and it keeps the average interval between meals below the limit.
 */
static void	thinking(t_philo *philo, t_table *table)
{
	long	time_since_last_meal;
	long	think_time;

	time_since_last_meal = get_time(table)
		- get_long(philo->meal_sem, &philo->last_meal_time, table);
	think_time = table->time_to_die - time_since_last_meal
		- table->time_to_eat - 10;
	print_status(philo, THINK, philo->table);
	if (think_time > 0)
		usleep(1000);
}

/**
 * Takes two forks from the shared pool, eats, and records the meal.
 *
 * @param philo: the philosopher about to eat
 * @param table: holds the fork, limit and meal semaphores
 *
 * limit_sem admits at most half the philosophers at a time. The forks
 * semaphore alone would not be enough: it counts forks without caring who
 * holds them, so every philosopher could take one and then wait forever for a
 * second. Capping the number of eaters guarantees that whoever is admitted
 * can always complete a pair.
 */
static void	eating(t_philo *philo, t_table *table)
{
	/* the seat is taken before the forks: this is what bounds the eaters */
	safe_semaphore(table->limit_sem, WAIT, table);
	safe_semaphore(table->forks, WAIT, table);
	print_status(philo, FORK, table);
	safe_semaphore(table->forks, WAIT, table);
	print_status(philo, FORK, table);
	set_long(philo->meal_sem, &philo->last_meal_time, get_time(table), table);
	philo->eat_count++;
	if (philo->eat_count == table->limit_meals)
		set_bool(philo->meal_sem, &philo->full, true, table);
	print_status(philo, EAT, table);
	precise_usleep(table->time_to_eat, table);
}

/**
 * Handles the single-philosopher case and terminates the process.
 *
 * @param philo: the only philosopher at the table
 * @param table: holds the timings
 *
 * With one fork on the table the philosopher can never eat, so the outcome is
 * known from the start: take the fork, wait out time_to_die, report the death.
 * Returns immediately when there is more than one philosopher.
 */
static void	solo_philo(t_philo *philo, t_table *table)
{
	if (philo->table->philo_nbr != 1)
		return ;
	print_status(philo, FORK, table);
	usleep(table->time_to_die * 1000);
	print_status(philo, DIE, table);
	cleanup(table, -1);
	exit(0);
}

/**
 * Body of a philosopher process. Never returns: it always ends in exit().
 *
 * @param philo: the philosopher this process stands for
 * @param table: shared timings and semaphores
 *
 * Each process watches itself with a monitor thread, which is what lets a
 * philosopher notice its own starvation while its main thread is blocked on a
 * semaphore waiting for forks. A philosopher that reaches its quota exits on
 * its own; the others are killed by the parent once a death is reported.
 */
static void	routine(t_philo *philo, t_table *table)
{
	solo_philo(philo, table);
	safe_thread(&philo->monitor, CREATE, monitoring, philo);
	if (philo->id % 2 == 0)
		usleep(1000);
	while (!get_bool(philo->meal_sem, &philo->full, table))
	{
		eating(philo, table);
		safe_semaphore(table->forks, POST, table);
		safe_semaphore(table->forks, POST, table);
		safe_semaphore(table->limit_sem, POST, table);
		print_status(philo, SLEEP, table);
		precise_usleep(table->time_to_sleep, table);
		thinking(philo, table);
		if (get_bool(table->monitor_sem, &table->end_dinner, table))
			break ;
	}
	safe_thread(&philo->monitor, JOIN, NULL, NULL);
	cleanup(table, -1);
	exit(0);
}

void	start_dinner(t_table *table)
{
	int	i;

	i = 0;
	table->start_time = get_time(table);
	while (i < table->philo_nbr)
	{
		table->philos[i].last_meal_time = get_time(table);
		table->philos[i].pid = fork();
		/* the child never comes back from here: routine() ends in exit() */
		if (table->philos[i].pid == 0)
			routine(&table->philos[i], table);
		else if (table->philos[i].pid < 0)
			exit_error("Fork failed\n", table);
		i++;
	}
}
