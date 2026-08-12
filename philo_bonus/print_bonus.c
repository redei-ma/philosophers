#include "philo_bonus.h"

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
 * Prints one state change from a philosopher process.
 *
 * @param philo: the philosopher the message is about
 * @param code: FORK, EAT, SLEEP, THINK or DIE
 * @param table: holds the print and stop semaphores and the start time
 *
 * A death message deliberately keeps print_sem instead of releasing it. The
 * stop flag lives in process memory, so after fork each philosopher holds its
 * own copy and raising it silences nobody else; holding the semaphore does,
 * because every other process blocks on it until the parent kills them. That
 * is what leaves the death as the last line printed, and the only one.
 */
void	print_status(t_philo *philo, int code, t_table *table)
{
	char	buf[64];
	int		i;

	safe_semaphore(table->print_sem, WAIT, table);
	if (get_bool(table->monitor_sem, &table->end_dinner, table)
		&& code != DIE)
	{
		safe_semaphore(table->print_sem, POST, table);
		return ;
	}
	i = 0;
	i = append_nbr(buf, i, get_time(table) - table->start_time);
	buf[i++] = ' ';
	i = append_nbr(buf, i, philo->id);
	i = append_str(buf, i, status_msg(code));
	write(1, buf, i);
	/* left locked on purpose: nothing may be printed after a death */
	if (code == DIE)
		return ;
	safe_semaphore(table->print_sem, POST, table);
}
