#include "philo_bonus.h"

void	safe_thread(pthread_t *thread, int code,
	void *(*start_routine)(void *), void *data)
{
	int	ret;

	ret = -1;
	if (code == CREATE)
		ret = pthread_create(thread, NULL, start_routine, data);
	else if (code == JOIN)
		ret = pthread_join(*thread, NULL);
	else if (code == DETACH)
		ret = pthread_detach(*thread);
	if (ret != 0)
		exit_error("Thread operation failed\n", (t_table *)data);
}

void	safe_semaphore(sem_t *sem, int code, t_table *table)
{
	int	ret;

	ret = 0;
	if (code == WAIT)
		ret = sem_wait(sem);
	else if (code == POST)
		ret = sem_post(sem);
	else if (code == CLOSE)
		ret = sem_close(sem);
	if (ret == -1)
		exit_error("Semaphore operation failed\n", table);
}

sem_t	*safe_sem_open(char *name, int value, t_table *table)
{
	sem_t	*sem;

	sem = sem_open(name, O_CREAT, 0644, value);
	if (sem == SEM_FAILED)
		exit_error("Semaphore open failed\n", table);
	return (sem);
}

void	*safe_malloc(size_t size, t_table *table)
{
	void	*ptr;

	ptr = malloc(size);
	if (!ptr)
		exit_error("malloc failed\n", table);
	return (ptr);
}
