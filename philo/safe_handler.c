#include "philo.h"

int	safe_thread(pthread_t *thread, int code,
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
	return (ret);
}

int	safe_philo(t_philo *philo, int code,
		void *(*start_routine)(void *), void *data)
{
	int	ret;

	ret = -1;
	if (code == CREATE)
		ret = pthread_create(&philo->thread_id, NULL, start_routine, data);
	else if (code == JOIN)
		ret = pthread_join(philo->thread_id, NULL);
	else if (code == DETACH)
		ret = pthread_detach(philo->thread_id);
	return (ret);
}

int	safe_mutex(t_mtx *mtx, int code)
{
	int	ret;

	ret = 0;
	if (code == INIT)
		ret = pthread_mutex_init(mtx, NULL);
	else if (code == DESTROY)
		ret = pthread_mutex_destroy(mtx);
	else if (code == LOCK)
		ret = pthread_mutex_lock(mtx);
	else if (code == UNLOCK)
		ret = pthread_mutex_unlock(mtx);
	return (ret);
}
