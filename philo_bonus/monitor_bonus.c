#include "philo_bonus.h"

/**
 * Watches one philosopher and reports its death to the parent process.
 *
 * @param arg: pointer to the philosopher owned by this process
 * @return always NULL
 *
 * Runs as a thread inside the philosopher's own process, so it keeps checking
 * the clock even while the main thread is blocked waiting for forks. Raising
 * the stop flag here only affects this process, since each child holds its own
 * copy after fork; stopping the rest of the table is the parent's job, and the
 * post on die_sem is what wakes it up to do it.
 */
void	*monitoring(void *arg)
{
	t_philo	*philo;

	philo = (t_philo *)arg;
	while (1)
	{
		if (get_bool(philo->meal_sem, &philo->full, philo->table))
			return (NULL);
		else if ((get_time(philo->table) - get_long(philo->meal_sem,
					&philo->last_meal_time, philo->table))
			> philo->table->time_to_die)
		{
			set_bool(philo->table->monitor_sem, &philo->table->end_dinner,
				true, philo->table);
			print_status(philo, DIE, philo->table);
			safe_semaphore(philo->table->die_sem, POST, philo->table);
			return (NULL);
		}
		usleep(1000);
	}
	return (NULL);
}
