#ifndef PHILO_H
# define PHILO_H

# include <stdio.h>
# include <string.h>
# include <unistd.h>
# include <stdlib.h>
# include <pthread.h>
# include <sys/time.h>
# include <stdbool.h>

typedef enum e_code
{
	INIT,
	DESTROY,
	LOCK,
	UNLOCK,
	CREATE,
	JOIN,
	DETACH,
	FORK,
	EAT,
	SLEEP,
	THINK,
	DIE,
}	t_code;

typedef pthread_mutex_t	t_mtx;

typedef struct s_table	t_table;

typedef struct s_fork
{
	t_mtx	fork;
	int		mtx_init;
	int		id;
}	t_fork;

typedef struct s_philo
{
	pthread_t	thread_id;
	int			id;
	long		eat_count;
	bool		full;
	long		last_meal_time;
	t_fork		*first_fork;
	t_fork		*second_fork;
	t_mtx		meal_mtx;
	int			meal_mtx_init;
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
	bool		end_program;
	t_mtx		print_mtx;
	int			print_mtx_init;
	pthread_t	monitor;
	t_mtx		monitor_mtx;
	int			monitor_mtx_init;
	t_philo		*philos;
	t_fork		*forks;
};

int		set_dinner(char **av, t_table *table);

int		start_dinner(t_table *table);

void	*monitoring(void *arg);

void	cleanup(t_table *table);
void	join_philos(t_table *table, int nbr);

void	print_status(t_philo *philo, int code, t_table *table);
long	get_time(void);
int		ft_atoi(const char *str);
int		validate_args(int ac, char **av);

int		safe_thread(pthread_t *thread, int code,
			void *(*start_routine)(void *), void *data);
int		safe_philo(t_philo *philo, int code,
			void *(*start_routine)(void *), void *data);
int		safe_mutex(t_mtx *mtx, int code);

void	set_long(t_mtx *mtx, long *old_value, long value);
long	get_long(t_mtx *mtx, long *value);
void	set_bool(t_mtx *mtx, bool *old_value, bool value);
bool	get_bool(t_mtx *mtx, bool *value);

#endif
