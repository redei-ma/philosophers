#ifndef PHILO_BONUS_H
# define PHILO_BONUS_H

# include <stdio.h>
# include <string.h>
# include <unistd.h>
# include <stdlib.h>
# include <pthread.h>
# include <sys/time.h>
# include <stdbool.h>
# include <sys/wait.h>
# include <signal.h>
# include <semaphore.h>
# include <fcntl.h>
# include <errno.h>

typedef enum e_code
{
	WAIT,
	POST,
	CLOSE,
	CREATE,
	JOIN,
	DETACH,
	FORK,
	EAT,
	SLEEP,
	THINK,
	DIE,
	FULL,
}	t_code;

typedef struct s_table	t_table;

typedef struct s_philo
{
	pid_t		pid;
	int			id;
	long		eat_count;
	bool		full;
	long		last_meal_time;
	pthread_t	monitor;
	sem_t		*meal_sem;
	t_table		*table;
}	t_philo;

struct s_table
{
	int			philo_nbr;
	long		time_to_die;
	long		time_to_eat;
	long		time_to_sleep;
	long		limit_meals;
	long		start_time;
	bool		end_dinner;
	pthread_t	die_thread;
	sem_t		*forks;
	sem_t		*print_sem;
	sem_t		*monitor_sem;
	sem_t		*limit_sem;
	sem_t		*die_sem;
	t_philo		*philos;
};

void	set_dinner(t_table *table, char **av);

void	start_dinner(t_table *table);

void	*monitoring(void *arg);

void	cleanup(t_table *table, int code);
void	exit_error(char *msg, t_table *table);
void	finish_dinner(t_table *table);

void	precise_usleep(long time, t_table *table);
void	print_status(t_philo *philo, int code, t_table *table);
long	get_time(t_table *table);
int		ft_atoi(const char *str);
int		validate_args(int ac, char **av);

void	safe_thread(pthread_t *thread, int code,
			void *(*start_routine)(void *), void *data);
void	safe_semaphore(sem_t *sem, int code, t_table *table);
sem_t	*safe_sem_open(char *name, int value, t_table *table);
void	*safe_malloc(size_t size, t_table *table);

void	set_long(sem_t *sem, long *old_value, long value, t_table *table);
long	get_long(sem_t *sem, long *old_value, t_table *table);
void	set_bool(sem_t *sem, bool *old_value, bool value, t_table *table);
bool	get_bool(sem_t *sem, bool *old_value, t_table *table);

void	*ft_calloc(size_t nmemb, size_t size);
char	*ft_strjoin(const char *s1, const char *s2);
char	*ft_itoa(int n);

#endif
