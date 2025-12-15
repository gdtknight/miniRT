# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: miniRT team <miniRT@42.fr>                +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/12/15 00:00:00 by miniRT           #+#    #+#              #
#    Updated: 2025/12/15 00:00:00 by miniRT          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		= miniRT
CC			= cc
CFLAGS		= -Wall -Wextra -Werror -I$(INC_DIR) -I$(MLX_DIR)
LDFLAGS		= -L$(MLX_DIR) -lmlx -lXext -lX11 -lm

GREEN		= \033[0;32m
RED			= \033[0;31m
RESET		= \033[0m

INC_DIR		= includes
SRC_DIR		= src
OBJ_DIR		= obj
MLX_DIR		= lib/minilibx-linux

SRCS		= $(SRC_DIR)/main.c \
			  $(SRC_DIR)/lighting/lighting.c \
			  $(SRC_DIR)/math/vector.c \
			  $(SRC_DIR)/math/vector_ops.c \
			  $(SRC_DIR)/parser/parse_elements.c \
			  $(SRC_DIR)/parser/parse_objects.c \
			  $(SRC_DIR)/parser/parse_validation.c \
			  $(SRC_DIR)/parser/parser.c \
			  $(SRC_DIR)/ray/intersections.c \
			  $(SRC_DIR)/render/camera.c \
			  $(SRC_DIR)/render/render.c \
			  $(SRC_DIR)/render/trace.c \
			  $(SRC_DIR)/utils/cleanup.c \
			  $(SRC_DIR)/utils/error.c \
			  $(SRC_DIR)/window/window.c

OBJS		= $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

all: $(NAME)

$(NAME): $(OBJS)
	@echo "$(GREEN)Linking $(NAME)...$(RESET)"
	@make -C $(MLX_DIR)
	@$(CC) $(OBJS) $(LDFLAGS) -o $(NAME)
	@echo "$(GREEN)✓ miniRT compiled successfully!$(RESET)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "$(GREEN)Compiling $<...$(RESET)"
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "$(RED)Cleaning object files...$(RESET)"
	@rm -rf $(OBJ_DIR)
	@make -C $(MLX_DIR) clean
	@echo "$(GREEN)✓ Object files cleaned$(RESET)"

fclean: clean
	@echo "$(RED)Removing $(NAME)...$(RESET)"
	@rm -f $(NAME)
	@echo "$(GREEN)✓ Executable removed$(RESET)"

re: fclean all

norm:
	@norminette $(SRC_DIR) $(INC_DIR)

.PHONY: all clean fclean re norm
