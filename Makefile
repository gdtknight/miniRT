# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/12/18 15:20:43 by yoshin            #+#    #+#              #
#    Updated: 2025/12/18 15:20:44 by yoshin           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		= miniRT
CC			= cc
CFLAGS		= -Wall -Wextra -Werror -I$(INC_DIR) -I$(MLX_DIR)

GREEN		= \033[0;32m
RED			= \033[0;31m
RESET		= \033[0m

INC_DIR		= includes
SRC_DIR		= src
OBJ_DIR		= build

# OS-specific MLX configuration
UNAME_S		:= $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	MLX_DIR		= lib/minilibx-linux
	LDFLAGS		= -L$(MLX_DIR) -lmlx -lXext -lX11 -lm
endif
ifeq ($(UNAME_S),Darwin)
	MLX_DIR		= lib/minilibx-macos
	LDFLAGS		= -L$(MLX_DIR) -lmlx -framework OpenGL -framework AppKit -lm
endif

SRCS		= $(SRC_DIR)/main.c \
			  $(SRC_DIR)/hud/hud_format.c \
			  $(SRC_DIR)/hud/hud_init.c \
			  $(SRC_DIR)/hud/hud_navigation.c \
			  $(SRC_DIR)/hud/hud_render.c \
			  $(SRC_DIR)/hud/hud_toggle.c \
			  $(SRC_DIR)/hud/hud_transparency.c \
			  $(SRC_DIR)/keyguide/keyguide_init.c \
			  $(SRC_DIR)/keyguide/keyguide_render.c \
			  $(SRC_DIR)/keyguide/keyguide_cleanup.c \
			  $(SRC_DIR)/lighting/lighting.c \
			  $(SRC_DIR)/lighting/shadow_calc.c \
			  $(SRC_DIR)/lighting/shadow_attenuation.c \
			  $(SRC_DIR)/lighting/shadow_config.c \
			  $(SRC_DIR)/lighting/shadow_test.c \
			  $(SRC_DIR)/math/vector.c \
			  $(SRC_DIR)/math/vector_ops.c \
			  $(SRC_DIR)/parser/parse_elements.c \
			  $(SRC_DIR)/parser/parse_objects.c \
			  $(SRC_DIR)/parser/parse_validation.c \
			  $(SRC_DIR)/parser/parser.c \
			  $(SRC_DIR)/ray/intersections.c \
			  $(SRC_DIR)/ray/intersect_cylinder.c \
			  $(SRC_DIR)/render/camera.c \
			  $(SRC_DIR)/render/render.c \
			  $(SRC_DIR)/render/render_state.c \
			  $(SRC_DIR)/render/trace.c \
			  $(SRC_DIR)/spatial/aabb.c \
			  $(SRC_DIR)/spatial/bounds.c \
			  $(SRC_DIR)/spatial/bvh_build.c \
			  $(SRC_DIR)/spatial/bvh_init.c \
			  $(SRC_DIR)/spatial/bvh_traverse.c \
			  $(SRC_DIR)/utils/cleanup.c \
			  $(SRC_DIR)/utils/error.c \
			  $(SRC_DIR)/utils/ft_atoi.c \
			  $(SRC_DIR)/utils/ft_atof.c \
			  $(SRC_DIR)/utils/timer.c \
			  $(SRC_DIR)/window/window.c

OBJS		= $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

all: $(NAME)

$(NAME): $(OBJS)
	@echo "$(GREEN)Linking $(NAME)...$(RESET)"
	@if [ -f "$(MLX_DIR)/Makefile" ]; then make -C $(MLX_DIR); fi
	@$(CC) $(OBJS) $(LDFLAGS) -o $(NAME)
	@echo "$(GREEN)✓ miniRT compiled successfully!$(RESET)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "$(GREEN)Compiling $<...$(RESET)"
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "$(RED)Cleaning object files...$(RESET)"
	@rm -rf $(OBJ_DIR)
	@if [ -f "$(MLX_DIR)/Makefile" ]; then make -C $(MLX_DIR) clean; fi
	@echo "$(GREEN)✓ Object files cleaned$(RESET)"

fclean: clean
	@echo "$(RED)Removing $(NAME)...$(RESET)"
	@rm -f $(NAME)
	@echo "$(GREEN)✓ Executable removed$(RESET)"

re: fclean all

norm:
	@norminette $(SRC_DIR) $(INC_DIR)

.PHONY: all clean fclean re norm
