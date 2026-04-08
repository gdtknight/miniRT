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
CFLAGS		= -Wall -Wextra -Werror -MMD -MP -I$(INC_DIR) -I$(MLX_DIR) -I$(LIBFT_DIR)/includes

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
ifeq ($(UNAME_S),Darwin)
	MLX_DIR		= lib/minilibx-macos
	LDFLAGS		= -L$(MLX_DIR) -lmlx -L$(LIBFT_DIR) -lft -framework OpenGL -framework AppKit -lm
else ifeq ($(UNAME_S),Linux)
	MLX_DIR		= lib/minilibx-linux
	LDFLAGS		= -L$(MLX_DIR) -lmlx -L$(LIBFT_DIR) -lft -lXext -lX11 -lm
else
	$(error Unsupported OS: $(UNAME_S). Supported: Darwin, Linux.)
endif

SRCS		= $(SRC_DIR)/main.c \
			  $(SRC_DIR)/common/vec3.c \
			  $(SRC_DIR)/common/vec3_ops.c \
			  $(SRC_DIR)/common/error.c \
			  $(SRC_DIR)/common/format_helpers.c \
			  $(SRC_DIR)/common/timer.c \
			  $(SRC_DIR)/scene/scene.c \
			  $(SRC_DIR)/scene/object_list.c \
			  $(SRC_DIR)/scene/parser/parser.c \
			  $(SRC_DIR)/scene/parser/parser_dispatch.c \
			  $(SRC_DIR)/scene/parser/parser_utils.c \
			  $(SRC_DIR)/scene/parser/parse_elements.c \
			  $(SRC_DIR)/scene/parser/parse_objects.c \
			  $(SRC_DIR)/scene/parser/parse_cylinder.c \
			  $(SRC_DIR)/scene/parser/parse_cone.c \
			  $(SRC_DIR)/scene/parser/parse_bonus_options.c \
			  $(SRC_DIR)/scene/parser/parse_number.c \
			  $(SRC_DIR)/scene/parser/parse_line_reader.c \
			  $(SRC_DIR)/scene/parser/parse_error.c \
			  $(SRC_DIR)/scene/parser/parse_validation_strict.c \
			  $(SRC_DIR)/scene/parser/parse_vector_validation.c \
			  $(SRC_DIR)/spatial/aabb/aabb.c \
			  $(SRC_DIR)/spatial/aabb/aabb_basic.c \
			  $(SRC_DIR)/spatial/aabb/aabb_bounds.c \
			  $(SRC_DIR)/spatial/bvh/bvh_lifecycle.c \
			  $(SRC_DIR)/spatial/bvh/bvh_build_partition.c \
			  $(SRC_DIR)/spatial/bvh/bvh_build_split.c \
			  $(SRC_DIR)/spatial/bvh/bvh_build_core.c \
			  $(SRC_DIR)/spatial/bvh/bvh_init.c \
			  $(SRC_DIR)/spatial/bvh/bvh_traverse.c \
			  $(SRC_DIR)/spatial/bvh/bvh_any_hit.c \
			  $(SRC_DIR)/spatial/intersect/intersect_object.c \
			  $(SRC_DIR)/spatial/intersect/intersect_cylinder.c \
			  $(SRC_DIR)/spatial/intersect/intersect_cone_body.c \
			  $(SRC_DIR)/spatial/intersect/intersect_cone_cap.c \
			  $(SRC_DIR)/spatial/debug/bvhd_init.c \
			  $(SRC_DIR)/spatial/debug/bvhd_tree.c \
			  $(SRC_DIR)/spatial/debug/bvhd_node.c \
			  $(SRC_DIR)/spatial/debug/bvhd_format.c \
			  $(SRC_DIR)/spatial/debug/bvhd_stats.c \
			  $(SRC_DIR)/spatial/debug/bvhd_prefix.c \
			  $(SRC_DIR)/spatial/debug/bvhd_prefix_push.c \
			  $(SRC_DIR)/spatial/debug/bvhd_print.c \
			  $(SRC_DIR)/render/render.c \
			  $(SRC_DIR)/render/render_init.c \
			  $(SRC_DIR)/render/render_loop.c \
			  $(SRC_DIR)/render/render_camera.c \
			  $(SRC_DIR)/render/render_trace.c \
			  $(SRC_DIR)/render/render_flags_set.c \
			  $(SRC_DIR)/render/render_debounce.c \
			  $(SRC_DIR)/render/render_debounce_timer.c \
			  $(SRC_DIR)/render/window_init.c \
			  $(SRC_DIR)/render/window_destroy.c \
			  $(SRC_DIR)/render/put_pixel.c \
			  $(SRC_DIR)/lighting/shading.c \
			  $(SRC_DIR)/lighting/shading_utils.c \
			  $(SRC_DIR)/lighting/shadow_calc.c \
			  $(SRC_DIR)/lighting/shadow_config.c \
			  $(SRC_DIR)/lighting/shadow_occlusion.c \
			  $(SRC_DIR)/lighting/checkerboard.c \
			  $(SRC_DIR)/lighting/bump_map_load.c \
			  $(SRC_DIR)/lighting/bump_map_perturb.c \
			  $(SRC_DIR)/lighting/texture_utils.c \
			  $(SRC_DIR)/interact/event_dispatch.c \
			  $(SRC_DIR)/interact/input/input_dispatch.c \
			  $(SRC_DIR)/interact/input/input_camera.c \
			  $(SRC_DIR)/interact/input/input_objects.c \
			  $(SRC_DIR)/interact/input/input_resize.c \
			  $(SRC_DIR)/interact/input/input_rotate.c \
			  $(SRC_DIR)/interact/input/input_key_binds.c \
			  $(SRC_DIR)/interact/input/input_key_binds_extra.c \
			  $(SRC_DIR)/interact/hud/hud_format.c \
			  $(SRC_DIR)/interact/hud/hud_format_helpers.c \
			  $(SRC_DIR)/interact/hud/hud_init.c \
			  $(SRC_DIR)/interact/hud/hud_navigation.c \
			  $(SRC_DIR)/interact/hud/hud_performance.c \
			  $(SRC_DIR)/interact/hud/hud_render.c \
			  $(SRC_DIR)/interact/hud/hud_text.c \
			  $(SRC_DIR)/interact/hud/hud_scene.c \
			  $(SRC_DIR)/interact/hud/hud_objects.c \
			  $(SRC_DIR)/interact/hud/hud_obj_render.c \
			  $(SRC_DIR)/interact/hud/hud_toggle.c \
			  $(SRC_DIR)/interact/keyguide/keyguide_init.c \
			  $(SRC_DIR)/interact/keyguide/keyguide_render.c \
			  $(SRC_DIR)/interact/keyguide/keyguide_render_extra.c \
			  $(SRC_DIR)/metrics/metrics_frame.c \
			  $(SRC_DIR)/metrics/metrics_counters.c \
			  $(SRC_DIR)/metrics/metrics_calc.c \
			  $(SRC_DIR)/metrics/metrics_shadow.c

OBJS		= $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

all: $(NAME)

$(LIBFT):
	@$(MAKE) -C $(LIBFT_DIR)

$(MLX_DIR)/libmlx.a:
	@if [ -f "$(MLX_DIR)/Makefile" ]; then $(MAKE) -C $(MLX_DIR); fi

$(NAME): $(LIBFT) $(MLX_DIR)/libmlx.a $(OBJS)
	@echo "$(GREEN)Linking $(NAME)...$(RESET)"
	@$(CC) $(OBJS) $(LDFLAGS) -o $(NAME)
	@echo "$(GREEN)✓ miniRT compiled successfully!$(RESET)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "$(GREEN)Compiling $<...$(RESET)"
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "$(RED)Cleaning object files...$(RESET)"
	@rm -rf $(OBJ_DIR)
	@echo "$(GREEN)✓ Object files cleaned$(RESET)"

fclean: clean
	@echo "$(RED)Removing $(NAME)...$(RESET)"
	@rm -f $(NAME)
	@if [ -f "$(MLX_DIR)/Makefile" ]; then $(MAKE) -C $(MLX_DIR) clean; fi
	@$(MAKE) -C $(LIBFT_DIR) fclean
	@echo "$(GREEN)✓ Executable removed$(RESET)"

re: fclean all

# bonus features are compiled into the main binary; this target exists
# for 42 evaluation compatibility (runs the same build as `all`).
bonus: all

norm:
	@norminette $(SRC_DIR) $(INC_DIR)

-include $(OBJS:.o=.d)

.PHONY: all clean fclean re norm bonus
