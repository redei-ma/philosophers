#include "philo_bonus.h"

static void	cleanup_2(t_table *table, int code)
{
	if (table->forks)
		sem_close(table->forks);
	if (table->print_sem)
		sem_close(table->print_sem);
	if (table->monitor_sem)
		sem_close(table->monitor_sem);
	if (table->limit_sem)
		sem_close(table->limit_sem);
	if (table->die_sem)
		sem_close(table->die_sem);
	if (code == FULL)
	{
		if (table->forks)
			sem_unlink("/forks");
		if (table->print_sem)
			sem_unlink("/print");
		if (table->monitor_sem)
			sem_unlink("/monitor");
		if (table->limit_sem)
			sem_unlink("/limit");
		if (table->die_sem)
			sem_unlink("/die");
	}
}

/**
 * Closes the semaphores and frees the philosopher array.
 *
 * @param table: a table in any state of initialisation
 * @param code: FULL when called by the parent, anything else by a child
 *
 * Unlinking a semaphore only removes its name: processes that already hold it
 * open keep using it normally. That is why a child leaving the table early
 * cannot disturb the philosophers still eating, and why the parent can unlink
 * again at the end without checking whether someone got there first.
 */
void	cleanup(t_table *table, int code)
{
	int		i;
	char	*sem_name;
	char	*number;

	i = 0;
	if (table->philos)
	{
		while (i < table->philo_nbr)
		{
			if (table->philos[i].meal_sem)
			{
				number = ft_itoa(i + 1);
				sem_name = ft_strjoin("/meal_sem_", number);
				sem_close(table->philos[i].meal_sem);
				sem_unlink(sem_name);
				free(number);
				free(sem_name);
			}
			i++;
		}
		free(table->philos);
	}
	cleanup_2(table, code);
}

void	exit_error(char *msg, t_table *table)
{
	cleanup(table, FULL);
	printf("Error: %s\n", msg);
	exit(1);
}

/**
 * Waits for a death to be reported, then terminates every philosopher.
 *
 * @param arg: the table
 * @return always NULL
 *
 * die_sem starts at zero and exactly two events post to it: a philosopher that
 * starved, or finish_dinner once every child has already exited on its own.
 * The two are told apart by the stop flag, which finish_dinner raises before
 * posting, so a dinner that ended peacefully is not followed by a round of
 * kills aimed at processes that are already gone.
 */
static void	*kill_all(void *arg)
{
	int		i;
	t_table	*table;

	table = (t_table *)arg;
	i = 0;
	safe_semaphore(table->die_sem, WAIT, table);
	if (get_bool(table->monitor_sem, &table->end_dinner, table))
	{
		safe_semaphore(table->die_sem, POST, table);
		return (NULL);
	}
	while (i < table->philo_nbr)
	{
		if (table->philos[i].pid > 0)
			kill(table->philos[i].pid, SIGKILL);
		i++;
	}
	return (NULL);
}

/**
 * Reaps every philosopher process and shuts the watcher thread down.
 *
 * @param table: holds the pids and the semaphores
 *
 * The post on die_sem after the wait loop is what releases kill_all when the
 * dinner ended on its own, with no death to report. Without it that thread
 * would stay blocked on a semaphore nobody is going to signal, and the join
 * below would never return.
 */
void	finish_dinner(t_table *table)
{
	int		finished;
	pid_t	pid;

	safe_thread(&table->die_thread, CREATE, kill_all, table);
	finished = 0;
	while (finished < table->philo_nbr)
	{
		pid = waitpid(-1, NULL, 0);
		if (pid < 0)
		{
			if (errno == ECHILD)
				break ;
			else
				exit_error("waitpid failed\n", table);
		}
		finished++;
	}
	set_bool(table->monitor_sem, &table->end_dinner, true, table);
	safe_semaphore(table->die_sem, POST, table);
	safe_thread(&table->die_thread, JOIN, NULL, NULL);
}
