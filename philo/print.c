#include "philo.h"

static char	*status_msg(int code)
{
	if (code == FORK)
		return (" has taken a fork\n");
	if (code == EAT)
		return (" is eating\n");
	if (code == SLEEP)
		return (" is sleeping\n");
	if (code == THINK)
		return (" is thinking\n");
	return (" died\n");
}

static int	append_str(char *buf, int i, char *s)
{
	int	j;

	j = 0;
	while (s[j])
		buf[i++] = s[j++];
	return (i);
}

static int	append_nbr(char *buf, int i, long n)
{
	char	tmp[24];
	int		len;

	len = 0;
	if (n <= 0)
		tmp[len++] = '0';
	while (n > 0)
	{
		tmp[len++] = (n % 10) + '0';
		n /= 10;
	}
	while (len > 0)
		buf[i++] = tmp[--len];
	return (i);
}

/**
 * Prints one state change, or drops it if the simulation is already over.
 *
 * @param philo: the philosopher the message is about
 * @param code: FORK, EAT, SLEEP, THINK or DIE
 * @param table: holds the print mutex, the stop flag and the start time
 *
 * The line is assembled in a local buffer and handed to a single write(), so
 * it cannot interleave with another thread's line and nothing is left sitting
 * in a stdio buffer if the process is killed. The stop flag is read while
 * holding the print mutex, and that is what guarantees no message can be
 * printed after a death has been announced.
 */
void	print_status(t_philo *philo, int code, t_table *table)
{
	char	buf[64];
	int		i;

	safe_mutex(&table->print_mtx, LOCK);
	if (get_bool(&table->monitor_mtx, &table->end_program) && code != DIE)
	{
		safe_mutex(&table->print_mtx, UNLOCK);
		return ;
	}
	i = 0;
	i = append_nbr(buf, i, get_time() - table->start_time);
	buf[i++] = ' ';
	i = append_nbr(buf, i, philo->id);
	i = append_str(buf, i, status_msg(code));
	write(1, buf, i);
	safe_mutex(&table->print_mtx, UNLOCK);
}
