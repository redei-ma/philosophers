#include "philo.h"

/**
 * Sleeps for target milliseconds with sub-millisecond accuracy.
 *
 * @param target: how long to sleep, in milliseconds
 *
 * A single usleep(target * 1000) overshoots under load, and that overshoot is
 * charged against time_to_die. Sleeping in shrinking slices and re-reading the
 * clock keeps the error bounded whatever the scheduler decides to do.
 */
static void	precise_usleep(long target)
{
	long	start;
	long	elapsed;
	long	remaining;

	start = get_time();
	elapsed = get_time() - start;
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
		elapsed = get_time() - start;
	}
}

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

	time_since_last_meal = get_time()
		- get_long(&philo->meal_mtx, &philo->last_meal_time);
	think_time = table->time_to_die - time_since_last_meal
		- table->time_to_eat - 10;
	print_status(philo, THINK, table);
	if (think_time > 0)
		usleep(1000);
}

static void	eating(t_philo *philo, t_table *table)
{
	safe_mutex(&philo->first_fork->fork, LOCK);
	print_status(philo, FORK, table);
	safe_mutex(&philo->second_fork->fork, LOCK);
	print_status(philo, FORK, table);
	set_long(&philo->meal_mtx, &philo->last_meal_time, get_time());
	philo->eat_count++;
	if (philo->eat_count == table->limit_meals)
		set_bool(&philo->meal_mtx, &philo->full, true);
	print_status(philo, EAT, table);
	precise_usleep(table->time_to_eat);
}

/**
 * Thread body of one philosopher: eat, sleep, think until full or stopped.
 *
 * @param arg: pointer to the philosopher's t_philo
 * @return always NULL
 *
 * A lone philosopher has a single fork and can never eat, so it takes that
 * fork and returns, leaving the monitor to declare its death. Even-numbered
 * philosophers start one millisecond late, which splits the first round into
 * two waves instead of having every neighbour grab for the same forks at once.
 */
static void	*routine(void *arg)
{
	t_philo	*philo;
	t_table	*table;

	philo = (t_philo *)arg;
	table = philo->table;
	set_long(&philo->meal_mtx, &philo->last_meal_time, get_time());
	if (table->philo_nbr == 1)
		return (print_status(philo, FORK, table), NULL);
	if (philo->id % 2 == 0)
		usleep(1000);
	while (!get_bool(&philo->meal_mtx, &philo->full))
	{
		eating(philo, table);
		safe_mutex(&philo->first_fork->fork, UNLOCK);
		safe_mutex(&philo->second_fork->fork, UNLOCK);
		print_status(philo, SLEEP, table);
		precise_usleep(table->time_to_sleep);
		thinking(philo, table);
		if (get_bool(&table->monitor_mtx, &table->end_program))
			break ;
	}
	return (NULL);
}

/**
 * Starts every philosopher thread plus the monitor, then joins them all.
 *
 * @param table: a fully initialised table
 * @return 0 if the simulation ran to completion, non-zero if a thread failed
 *         to start
 *
 * Only the threads that were actually created get joined. If creation fails
 * partway through, the stop flag is raised first so the philosophers already
 * running leave their loop, instead of being left behind detached from anyone.
 */
int	start_dinner(t_table *table)
{
	int	n;
	int	ret;

	n = 0;
	ret = 0;
	table->start_time = get_time();
	while (n < table->philo_nbr && !ret)
	{
		table->philos[n].last_meal_time = table->start_time;
		ret = safe_philo(&table->philos[n], CREATE,
				routine, &table->philos[n]);
		if (!ret)
			n++;
	}
	if (!ret)
		ret = safe_thread(&table->monitor, CREATE, monitoring, table);
	if (!ret)
		safe_thread(&table->monitor, JOIN, NULL, NULL);
	else
		/* no monitor to stop them, so raise the flag by hand before joining */
		set_bool(&table->monitor_mtx, &table->end_program, true);
	join_philos(table, n);
	return (ret);
}
