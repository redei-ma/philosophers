# Philosophers

A simulation of Dijkstra's dining philosophers problem, written twice in C over
two different concurrency models: threads sharing memory under mutexes, and
processes sharing nothing but named semaphores. Philosophers sit around a table
with one fork between each pair, need both forks to eat, and starve if they go
too long between meals. The interesting part is not the simulation but the
constraints around it: no philosopher may deadlock, no two log lines may
interleave, and a death has to be reported within ten milliseconds of happening.

## The two programs

| Directory | Model | Forks | Watcher |
|---|---|---|---|
| `philo/` | one thread per philosopher | one mutex per fork | a single monitor thread scanning every philosopher |
| `philo_bonus/` | one process per philosopher | a counting semaphore shared by all | one monitor thread inside each process |

Both take the same arguments and produce the same log format. The parent process
in the bonus never acts as a philosopher: it only forks, waits, and kills.

## How it works

**Avoiding deadlock, with mutexes.** The textbook failure is every philosopher
picking up their left fork at once and waiting forever for a right one that will
never come. `assign_fork` breaks the circular wait by giving odd and even
philosophers opposite acquisition orders, so no cycle of "holds one, waits for
the next" can close. This holds for an odd number of philosophers too, where the
ordering is not symmetric.

**Avoiding deadlock, with semaphores.** Anonymous forks make the same problem
harder: a counting semaphore knows how many forks are free but not who holds
them, so all philosophers could take one each and stall. The bonus caps the
number of simultaneous eaters at half the table, rounded up, with a second
semaphore taken before the forks. Whoever is admitted is then guaranteed to
complete a pair.

**Detecting death.** A philosopher blocked on a fork cannot check its own clock,
so the check belongs elsewhere. The mandatory part uses one monitor thread that
scans every philosopher; the bonus gives each process its own monitor thread,
since processes cannot see each other's memory. A philosopher that reached its
meal quota stops refreshing its timestamp and is skipped, otherwise it would be
reported dead shortly after finishing.

**Stopping everyone at once.** With threads, a shared flag read under the print
mutex is enough: the check happens inside the critical section, so no message
can slip out after the death has been printed. With processes there is no shared
flag to raise, since each child owns a private copy after `fork`. Instead the
dying philosopher keeps the print semaphore rather than releasing it, which
blocks every other process at its next log attempt, and signals the parent
through a dedicated semaphore so it can terminate them.

**Writing the log.** Each line is assembled in a local buffer and emitted with a
single `write`. Using `printf` would put the output through a stdio buffer,
which is block-buffered when standard output is not a terminal: in the bonus,
where philosophers are killed by a signal, everything still sitting in those
buffers would be lost, and redirecting the output to a file would produce an
empty file.

**Sleeping accurately.** A single `usleep` for the whole duration overshoots
under load, and the overshoot is charged against `time_to_die`. `precise_usleep`
sleeps in shrinking slices and re-reads the clock, which bounds the error
regardless of scheduling.

## Build and run

Requires a C compiler and a POSIX system with pthreads and named semaphores.
Verified on macOS and on Debian 12 with gcc 12 and glibc 2.36, the latter under
Valgrind's memcheck and helgrind and under ThreadSanitizer.

```bash
cd philo && make
./philo number_of_philosophers time_to_die time_to_eat time_to_sleep [meals]

cd philo_bonus && make
./philo_bonus number_of_philosophers time_to_die time_to_eat time_to_sleep [meals]
```

All times are in milliseconds. The optional last argument stops the simulation
once every philosopher has eaten that many times; without it, the simulation
runs until someone starves.

```bash
./philo 5 800 200 200      # nobody should ever die
./philo 4 310 200 100      # someone dies, around 310ms
./philo 5 800 200 200 7    # stops once all five have eaten seven meals
./philo 1 800 200 200      # one fork, so the philosopher starves
```

Both Makefiles provide `all`, `clean`, `fclean` and `re`, plus `val` and `hel`
targets that run the binary under Valgrind and Helgrind with an `ARGS` variable.

## Repository layout

```
philo/                    threads and mutexes
  main.c                  argument check, then hand over to the simulation
  init.c                  allocation, fork assignment, mutex setup
  dinner.c                the philosopher routine and the thread lifecycle
  monitor.c               starvation detection and the stop condition
  print.c                 log line assembly and the print critical section
  finish.c                teardown, safe at any stage of initialisation
  safe_handler.c          thin wrappers returning errors instead of exiting
  set_and_get.c           mutex-guarded accessors for shared fields
  utils.c                 clock, argument parsing and validation

philo_bonus/              processes and semaphores
  main_bonus.c
  init_bonus.c            semaphore creation and sizing
  dinner_bonus.c          the forked philosopher and its lifecycle
  monitor_bonus.c         per-process starvation watcher
  print_bonus.c
  finish_bonus.c          reaping the children and killing them on death
  safe_handler_bonus.c
  set_and_get_bonus.c
  utils_bonus.c
  ft_itoa.c, ft_strjoin.c helpers for building semaphore names
```

## Notes

The two versions do not always agree on borderline parameters, and this is a
property of the models rather than a defect. With `100 200 100 100` the
theoretical cycle of a philosopher equals `time_to_die` exactly. The threaded
version has real contention between neighbours, so someone always overshoots
within a few hundred milliseconds and dies. The semaphore version hands out
anonymous forks to a bounded set of eaters, which packs perfectly and can hold
the table at exactly the deadline for tens of seconds before scheduling jitter
pushes someone over.

There is no upper bound on the number of philosophers. Asking for a few billion
will have the allocation or the thread creation fail, or the process killed by
the system before either. The behaviour has been verified up to 200.

Named semaphores are used rather than unnamed ones because `sem_init` is not
implemented on macOS. Their names are unlinked before being opened, so a
semaphore left behind by an interrupted run cannot be inherited with a stale
count.

## Resources

- E. W. Dijkstra, *Hierarchical ordering of sequential processes* (EWD310), the
  paper the problem comes from.
- `man pthread_create`, `man pthread_mutex_init`, `man sem_open`, `man waitpid`.
- The POSIX specification for named semaphores, for the behaviour of `sem_unlink`
  with respect to already-open handles.
