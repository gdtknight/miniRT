# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/09/25 18:16:41 by yoshin            #+#    #+#              #
#    Updated: 2025/07/19 16:08:04 by yoshin           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# ***************** #
#    Color Codes    #
# ***************** #

BLACK			=	'\033[38;5;0m'
MAROON			=	'\033[38;5;1m'
GREEN			=	'\033[38;5;2m'
CYAN_BLUE		=	'\033[38;5;69m'
LIGHT_GREEN		=	'\033[38;5;46m'
CYAN			=	'\033[38;5;81m'
OLIVE			=	'\033[38;5;3m'
NAVY			=	'\033[38;5;4m'
PURPLE			=	'\033[38;5;5m'
TEAL			=	'\033[38;5;6m'
SILVER			=	'\033[38;5;7m'
GREY			=	'\033[38;5;8m'
RED				=	'\033[38;5;9m'
LINE			=	'\033[38;5;10m'
YELLOW			=	'\033[38;5;11m'
LIGHT_YELLOW	=	'\033[38;5;226m'
BLUE			=	'\033[38;5;12m'
FUCHSIA			=	'\033[38;5;13m'
AQUA			=	'\033[38;5;14m'
WHITE			=	'\033[38;5;15m'
ORANGE			=	'\033[38;5;208m'
RESET			=	'\033[0m'

# Makefile v1.0

CC				=	gcc

NAME			=	ft

CFLAGS			=	-Wall -Wextra -Werror
ARFLAGS			=	rcs

PART1_FUNCS		=	ft_isalpha\
					ft_isdigit\
					ft_isalnum\
					ft_isascii\
					ft_isprint\
					ft_strlen\
					ft_memset\
					ft_bzero\
					ft_memcpy\
					ft_memmove\
					ft_strlcpy\
					ft_strlcat\
					ft_toupper\
					ft_tolower\
					ft_strchr\
					ft_strrchr\
					ft_strncmp\
					ft_memchr\
					ft_memcmp\
					ft_strnstr\
					ft_atoi\
					ft_calloc\
					ft_strdup

PART2_FUNCS		=	ft_substr\
					ft_strjoin\
					ft_strtrim\
					ft_split\
					ft_itoa\
					ft_strmapi\
					ft_striteri\
					ft_putchar_fd\
					ft_putstr_fd\
					ft_putendl_fd\
					ft_putnbr_fd

BONUS_FUNCS		=	ft_lstnew\
					ft_lstadd_front\
					ft_lstsize\
					ft_lstlast\
					ft_lstadd_back\
					ft_lstdelone\
					ft_lstclear\
					ft_lstiter\
					ft_lstmap

PART1_SRCS		=	$(addprefix $(SRC_DIR)/, $(addsuffix .c, $(PART1_FUNCS)))
PART2_SRCS		=	$(addprefix $(SRC_DIR)/, $(addsuffix .c, $(PART2_FUNCS)))
BONUS_SRCS		=	$(addprefix $(SRC_DIR)/, $(addsuffix .c, $(BONUS_FUNCS)))

PART1_OBJS		=	$(addprefix $(BUILD_DIR)/, $(addsuffix .o, $(PART1_FUNCS)))
PART2_OBJS		=	$(addprefix $(BUILD_DIR)/, $(addsuffix .o, $(PART2_FUNCS)))
BONUS_OBJS		=	$(addprefix $(BUILD_DIR)/, $(addsuffix .o, $(BONUS_FUNCS)))

OBJS			=	$(PART1_OBJS) $(PART2_OBJS) $(BONUS_OBJS)
HEADER_DIR		=	includes
BUILD_DIR		=	build
SRC_DIR			=	srcs
BIN_DIR			=	bin

ifeq ($(shell uname), Linux)
	PREFIX		:=
else ifeq ($(shell uname), Darwin)
	PREFIX		:=
endif

# **************** #
#    Debug Flag    #
# **************** #

ifeq ($(debug), 1)
	override CFLAGS	+= --debug -g -DDEBUG
endif

.PHONY: all clean fclean re bonus dirs debug
all: lib$(NAME).a

lib$(NAME).a: $(OBJS) | $(BIN_DIR)
	@echo "$(LIGHT_YELLOW)[lib$(NAME)] $(RED)archieve $(BIN_DIR)/$@ ...$(RESET)"
	@$(AR) $(ARFLAGS) $(BIN_DIR)/$@ $(OBJS)
	@echo "$(LIGHT_YELLOW)[lib$(NAME)] $(GREEN)$(BIN_DIR)/$@ archieved!$(RESET)"
	@cp	$(BIN_DIR)/$@ ./$@
	@echo "$(LIGHT_YELLOW)[lib$(NAME)] $(GREEN)copy $@ completed !$(RESET)"

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "$(LIGHT_YELLOW)[lib$(NAME)] $(RED)compile $< ...$(RESET)"
	@$(PREFIX) $(CC) $(CFLAGS) -o $@ -c $< -I$(HEADER_DIR)
	@echo "$(LIGHT_YELLOW)[lib$(NAME)] $(GREEN)$< compiled!$(RESET)"

$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

clean: 
	@rm -rf $(BUILD_DIR)
	@echo "$(LIGHT_YELLOW)[lib$(NAME)]$(RESET) clean completed!"

fclean: clean
	@make clean
	@rm -rf lib$(NAME).a $(BIN_DIR) $(HEADERS).gch
	@echo "$(LIGHT_YELLOW)[lib$(NAME)]$(RESET) fclean completed!"

re: fclean all

dev:
	@make debug=1 all
	@make debug=1 bonus
