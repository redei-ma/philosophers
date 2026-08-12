NAME = philo_bonus
CC = cc
CFLAG = -Wall -Wextra -Werror
VFLAG = --leak-check=full --show-leak-kinds=all --track-origins=yes --track-fds=yes --trace-children=yes

HDR =	include/philo_bonus.h

SRC =	main_bonus.c \
		init_bonus.c \
		dinner_bonus.c \
		monitor_bonus.c \
		finish_bonus.c \
		utils_bonus.c \
		print_bonus.c \
		safe_handler_bonus.c \
		set_and_get_bonus.c	\
		ft_strjoin.c \
		ft_itoa.c

all: $(NAME)

$(NAME): $(SRC) $(HDR)
	@echo "Compiling $(NAME)..."
	@$(CC) $(CFLAG) -pthread $(SRC) -Iinclude -o $(NAME)

# ES: make run ARGS="5 800 200 200"
run: $(NAME)
	@echo "Running $(NAME)..."
	@./$(NAME) $(ARGS)

# ES: make val ARGS="5 800 200 200"
val: $(NAME)
	@echo "Using Valgrind..."
	@valgrind $(VFLAG) ./$(NAME) $(ARGS)

# ES: make hel ARGS="5 800 200 200"
hel: $(NAME)
	@echo "Using Helgrind..."
	@valgrind --tool=helgrind ./$(NAME) $(ARGS)

clean:

fclean: clean
	@echo "Removing build output..."
	@rm -rf $(NAME) $(NAME).dSYM

debug: fclean
	@echo "Compiling $(NAME) with debug symbols..."
	@$(CC) $(CFLAG) -g -pthread $(SRC) -Iinclude -o $(NAME)

re: fclean all

.PHONY: all val hel clean fclean re debug
