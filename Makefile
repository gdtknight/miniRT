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
CFLAGS		= -Wall -Wextra -Werror -I$(INC_DIR) -I$(MLX_DIR) -I$(LIBFT_DIR)/includes

GREEN		= \033[0;32m
RED			= \033[0;31m
RESET		= \033[0m

INC_DIR		= includes
SRC_DIR		= src
OBJ_DIR		= build
LIBFT_DIR	= lib/libft
LIBFT		= $(LIBFT_DIR)/libft.a

# OS-specific MLX configuration
UNAME_S		:= $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	MLX_DIR		= lib/minilibx-linux
	LDFLAGS		= -L$(MLX_DIR) -lmlx -L$(LIBFT_DIR) -lft -lXext -lX11 -lm
endif
ifeq ($(UNAME_S),Darwin)
	MLX_DIR		= lib/minilibx-macos
	LDFLAGS		= -L$(MLX_DIR) -lmlx -L$(LIBFT_DIR) -lft -framework OpenGL -framework AppKit -lm
endif

SRCS		= $(SRC_DIR)/main.c \
			  $(SRC_DIR)/scene/scene.c \
			  $(SRC_DIR)/scene/scene_flags.c \
			  $(SRC_DIR)/scene/object_list.c \
			  $(SRC_DIR)/bvh_vis/bvh_vis_init.c \
			  $(SRC_DIR)/bvh_vis/bvh_vis_tree.c \
			  $(SRC_DIR)/bvh_vis/bvh_vis_node.c \
			  $(SRC_DIR)/bvh_vis/bvh_vis_format.c \
			  $(SRC_DIR)/bvh_vis/bvh_vis_stats.c \
			  $(SRC_DIR)/bvh_vis/bvh_vis_prefix.c \
			  $(SRC_DIR)/bvh_vis/bvh_vis_print.c \
			  $(SRC_DIR)/hud/hud_color.c \
			  $(SRC_DIR)/hud/hud_color_extract.c \
			  $(SRC_DIR)/hud/hud_format.c \
			  $(SRC_DIR)/hud/hud_format_helpers.c \
			  $(SRC_DIR)/hud/hud_format_simple.c \
			  $(SRC_DIR)/hud/hud_init.c \
			  $(SRC_DIR)/hud/hud_navigation.c \
			  $(SRC_DIR)/hud/hud_performance.c \
			  $(SRC_DIR)/hud/hud_render.c \
			  $(SRC_DIR)/hud/hud_text.c \
			  $(SRC_DIR)/hud/hud_scene.c \
			  $(SRC_DIR)/hud/hud_objects.c \
			  $(SRC_DIR)/hud/hud_obj_render.c \
			  $(SRC_DIR)/hud/hud_toggle.c \
			  $(SRC_DIR)/hud/hud_transparency.c \
			  $(SRC_DIR)/keyguide/keyguide_init.c \
			  $(SRC_DIR)/keyguide/keyguide_render.c \
			  $(SRC_DIR)/keyguide/keyguide_render_extra.c \
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
			  $(SRC_DIR)/parser/parse_validation_strict.c \
			  $(SRC_DIR)/parser/parse_vector_validation.c \
			  $(SRC_DIR)/parser/parse_cylinder.c \
			  $(SRC_DIR)/parser/parse_error.c \
			  $(SRC_DIR)/parser/parse_error_msg.c \
			  $(SRC_DIR)/parser/parse_token.c \
			  $(SRC_DIR)/parser/parse_number.c \
			  $(SRC_DIR)/parser/parse_number_utils.c \
			  $(SRC_DIR)/parser/parse_line_reader.c \
			  $(SRC_DIR)/parser/parser.c \
			  $(SRC_DIR)/parser/parser_dispatch.c \
			  $(SRC_DIR)/parser/parser_utils.c \
			  $(SRC_DIR)/ray/intersect_object.c \
			  $(SRC_DIR)/ray/intersect_cyl_new.c \
			  $(SRC_DIR)/render/camera.c \
			  $(SRC_DIR)/render/metrics_frame.c \
			  $(SRC_DIR)/render/metrics_counters.c \
			  $(SRC_DIR)/render/metrics_calc.c \
			  $(SRC_DIR)/render/metrics_shadow.c \
			  $(SRC_DIR)/render/pixel_timing.c \
			  $(SRC_DIR)/render/pixel_timing_print.c \
			  $(SRC_DIR)/render/render.c \
			  $(SRC_DIR)/render/render_debounce.c \
			  $(SRC_DIR)/render/render_debounce_timer.c \
			  $(SRC_DIR)/render/trace.c \
			  $(SRC_DIR)/spatial/aabb.c \
			  $(SRC_DIR)/spatial/aabb_basic.c \
			  $(SRC_DIR)/spatial/bounds.c \
			  $(SRC_DIR)/spatial/bvh_lifecycle.c \
			  $(SRC_DIR)/spatial/bvh_build_partition.c \
			  $(SRC_DIR)/spatial/bvh_build_split.c \
			  $(SRC_DIR)/spatial/bvh_build_core.c \
			  $(SRC_DIR)/spatial/bvh_init.c \
			  $(SRC_DIR)/spatial/bvh_traverse.c \
			  $(SRC_DIR)/spatial/bvh_any_hit.c \
			  $(SRC_DIR)/utils/error.c \
			  $(SRC_DIR)/utils/format_helpers.c \
			  $(SRC_DIR)/utils/timer.c \
			  $(SRC_DIR)/window/window_init.c \
			  $(SRC_DIR)/window/window_lifecycle.c \
			  $(SRC_DIR)/window/window_events.c \
			  $(SRC_DIR)/window/window_key_handlers.c \
			  $(SRC_DIR)/window/window_loop.c \
			  $(SRC_DIR)/window/window_camera.c \
			  $(SRC_DIR)/window/window_selection.c \
			  $(SRC_DIR)/window/window_objects.c \
			  $(SRC_DIR)/window/window_resize.c \
			  $(SRC_DIR)/window/window_rotate.c \
			  $(SRC_DIR)/window/mlx_context.c \
			  $(SRC_DIR)/window/mlx_pixel_codec.c \
			  $(SRC_DIR)/window/mlx_pixel.c \
			  $(SRC_DIR)/window/render_flags_set.c

OBJS		= $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

all: $(NAME)

$(LIBFT):
	@$(MAKE) -C $(LIBFT_DIR)

$(NAME): $(LIBFT) $(OBJS)
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
	@$(MAKE) -C $(LIBFT_DIR) clean
	@echo "$(GREEN)✓ Object files cleaned$(RESET)"

fclean: clean
	@echo "$(RED)Removing $(NAME)...$(RESET)"
	@rm -f $(NAME)
	@$(MAKE) -C $(LIBFT_DIR) fclean
	@echo "$(GREEN)✓ Executable removed$(RESET)"

re: fclean all

norm:
	@norminette $(SRC_DIR) $(INC_DIR)

.PHONY: all clean fclean re norm
