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
			  $(SRC_DIR)/scene/scene.c \
			  $(SRC_DIR)/scene/scene_flags.c \
			  $(SRC_DIR)/scene/object_list.c \
			  $(SRC_DIR)/bvh_vis/bvh_vis_init.c \
			  $(SRC_DIR)/bvh_vis/bvh_vis_tree.c \
			  $(SRC_DIR)/bvh_vis/bvh_vis_node.c \
			  $(SRC_DIR)/bvh_vis/bvh_vis_format.c \
			  $(SRC_DIR)/bvh_vis/bvh_vis_stats.c \
			  $(SRC_DIR)/bvh_vis/bvh_vis_prefix.c \
			  $(SRC_DIR)/bvh_vis/bvh_vis_prefix_push.c \
			  $(SRC_DIR)/bvh_vis/bvh_vis_print.c \
			  $(SRC_DIR)/hud/hud_format.c \
			  $(SRC_DIR)/hud/hud_format_helpers.c \
			  $(SRC_DIR)/hud/hud_init.c \
			  $(SRC_DIR)/hud/hud_navigation.c \
			  $(SRC_DIR)/hud/hud_performance.c \
			  $(SRC_DIR)/hud/hud_render.c \
			  $(SRC_DIR)/hud/hud_text.c \
			  $(SRC_DIR)/hud/hud_scene.c \
			  $(SRC_DIR)/hud/hud_objects.c \
			  $(SRC_DIR)/hud/hud_obj_render.c \
			  $(SRC_DIR)/hud/hud_toggle.c \
			  $(SRC_DIR)/keyguide/keyguide_init.c \
			  $(SRC_DIR)/keyguide/keyguide_render.c \
			  $(SRC_DIR)/keyguide/keyguide_render_extra.c \
			  $(SRC_DIR)/lighting/lighting.c \
			  $(SRC_DIR)/lighting/lighting_utils.c \
			  $(SRC_DIR)/lighting/shadow_calc.c \
			  $(SRC_DIR)/lighting/shadow_config.c \
			  $(SRC_DIR)/lighting/shadow_test.c \
			  $(SRC_DIR)/math/vec3.c \
			  $(SRC_DIR)/math/vec3_ops.c \
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
			  $(SRC_DIR)/parser/parse_cone.c \
			  $(SRC_DIR)/parser/parse_bonus_options.c \
			  $(SRC_DIR)/parser/parser_utils.c \
			  $(SRC_DIR)/ray/intersect_object.c \
			  $(SRC_DIR)/ray/intersect_cylinder.c \
			  $(SRC_DIR)/ray/intersect_cone_body.c \
			  $(SRC_DIR)/ray/intersect_cone_cap.c \
			  $(SRC_DIR)/render/render_camera.c \
			  $(SRC_DIR)/render/render.c \
			  $(SRC_DIR)/render/render_debounce.c \
			  $(SRC_DIR)/render/render_debounce_timer.c \
			  $(SRC_DIR)/render/render_trace.c \
			  $(SRC_DIR)/metrics/metrics_frame.c \
			  $(SRC_DIR)/metrics/metrics_counters.c \
			  $(SRC_DIR)/metrics/metrics_calc.c \
			  $(SRC_DIR)/metrics/metrics_shadow.c \
			  $(SRC_DIR)/spatial/aabb.c \
			  $(SRC_DIR)/spatial/aabb_basic.c \
			  $(SRC_DIR)/spatial/aabb_bounds.c \
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
			  $(SRC_DIR)/render/render_init.c \
			  $(SRC_DIR)/display/display_events.c \
			  $(SRC_DIR)/display/display_init.c \
			  $(SRC_DIR)/display/display_destroy.c \
			  $(SRC_DIR)/display/display_pixel.c \
			  $(SRC_DIR)/render/render_loop.c \
			  $(SRC_DIR)/input/input_dispatch.c \
			  $(SRC_DIR)/input/input_camera.c \
			  $(SRC_DIR)/input/input_objects.c \
			  $(SRC_DIR)/input/input_resize.c \
			  $(SRC_DIR)/input/input_rotate.c \
			  $(SRC_DIR)/input/input_key_binds.c \
			  $(SRC_DIR)/input/input_key_binds_extra.c \
			  $(SRC_DIR)/render/render_flags_set.c \
			  $(SRC_DIR)/texture/checkerboard.c \
			  $(SRC_DIR)/texture/bump_map_load.c \
			  $(SRC_DIR)/texture/bump_map_perturb.c \
			  $(SRC_DIR)/texture/texture_utils.c

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
