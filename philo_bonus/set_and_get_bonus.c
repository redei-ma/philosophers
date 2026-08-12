#include "philo_bonus.h"

void	set_long(sem_t *sem, long *old_value, long value, t_table *table)
{
	safe_semaphore(sem, WAIT, table);
	*old_value = value;
	safe_semaphore(sem, POST, table);
}

long	get_long(sem_t *sem, long *value, t_table *table)
{
	long	ret;

	safe_semaphore(sem, WAIT, table);
	ret = *value;
	safe_semaphore(sem, POST, table);
	return (ret);
}

void	set_bool(sem_t *sem, bool *old_value, bool value, t_table *table)
{
	safe_semaphore(sem, WAIT, table);
	*old_value = value;
	safe_semaphore(sem, POST, table);
}

bool	get_bool(sem_t *sem, bool *value, t_table *table)
{
	bool	ret;

	safe_semaphore(sem, WAIT, table);
	ret = *value;
	safe_semaphore(sem, POST, table);
	return (ret);
}
