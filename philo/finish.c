#include "philo.h"

/**
 * Destroys every mutex that was created and frees the two arrays.
 *
 * @param table: a table in any state of initialisation, including a failed one
 *
 * Each mutex carries its own init flag and both arrays are tested for NULL, so
 * this can run after a partial initialisation without touching memory that was
 * never set up. That is what lets main() report an error and return instead of
 * calling exit() from wherever the failure happened.
 */
void	cleanup(t_table *table)
{
	int	i;

	if (table->print_mtx_init)
		pthread_mutex_destroy(&table->print_mtx);
	if (table->monitor_mtx_init)
		pthread_mutex_destroy(&table->monitor_mtx);
	i = 0;
	while (table->forks && i < table->philo_nbr)
	{
		if (table->forks[i].mtx_init)
			pthread_mutex_destroy(&table->forks[i].fork);
		i++;
	}
	i = 0;
	while (table->philos && i < table->philo_nbr)
	{
		if (table->philos[i].meal_mtx_init)
			pthread_mutex_destroy(&table->philos[i].meal_mtx);
		i++;
	}
	free(table->philos);
	free(table->forks);
}

void	join_philos(t_table *table, int nbr)
{
	int	i;

	i = 0;
	while (i < nbr)
	{
		safe_philo(&table->philos[i], JOIN, NULL, NULL);
		i++;
	}
}
