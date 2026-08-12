#include "philo.h"

void	set_long(t_mtx *mtx, long *old_value, long value)
{
	safe_mutex(mtx, LOCK);
	*old_value = value;
	safe_mutex(mtx, UNLOCK);
}

long	get_long(t_mtx *mtx, long *value)
{
	long	ret;

	safe_mutex(mtx, LOCK);
	ret = *value;
	safe_mutex(mtx, UNLOCK);
	return (ret);
}

void	set_bool(t_mtx *mtx, bool *old_value, bool value)
{
	safe_mutex(mtx, LOCK);
	*old_value = value;
	safe_mutex(mtx, UNLOCK);
}

bool	get_bool(t_mtx *mtx, bool *value)
{
	bool	ret;

	safe_mutex(mtx, LOCK);
	ret = *value;
	safe_mutex(mtx, UNLOCK);
	return (ret);
}
