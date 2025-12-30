/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   window.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:18:50 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/18 15:18:51 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WINDOW_H
# define WINDOW_H

# include "minirt.h"

/* Window resolution constants - MacBook M1 13-inch native resolution */
# define WINDOW_WIDTH 1440
# define WINDOW_HEIGHT 900
# define ASPECT_RATIO (1440.0 / 900.0)

/* Object type enumeration for selection */
typedef enum e_obj_type
{
	OBJ_NONE = 0,
	OBJ_SPHERE,
	OBJ_PLANE,
	OBJ_CYLINDER
}	t_obj_type;

/* Selected object information */
typedef struct s_selection
{
	t_obj_type	type;
	int			index;
}	t_selection;

/* HUD state structure */
typedef struct s_hud_state
{
	int		visible;
	int		current_page;
	int		objects_per_page;
	int		total_pages;
	int		dirty;
	void	*bg_img;
	char	*bg_data;
	int		bpp;
	int		size_line;
	int		endian;
}	t_hud_state;

/* Key guide state structure */
typedef struct s_keyguide_state
{
	void	*bg_img;
	char	*bg_data;
	int		bpp;
	int		size_line;
	int		endian;
	int		visible;
	int		x;
	int		y;
}	t_keyguide_state;

/* Render context containing MLX pointers and scene data */
typedef struct s_render
{
	void				*mlx;
	void				*win;
	void				*img;
	char				*img_data;
	int					bpp;
	int					size_line;
	int					endian;
	t_scene				*scene;
	t_selection			selection;
	int					dirty;
	int					low_quality;
	int					shift_pressed;
	t_hud_state			hud;
	t_keyguide_state	keyguide;
}	t_render;

/* Initialize MLX window and set up event handlers */
t_render	*init_window(t_scene *scene);
/* Close window and clean up resources */
int			close_window(void *param);
/* Handle keyboard input (ESC to exit) */
int			handle_key(int keycode, void *param);
/* Handle key release events */
int			handle_key_release(int keycode, void *param);
/* Main rendering loop */
int			render_loop(void *param);

/* Initialize MLX connection */
void		*mlx_init(void);
/* Create new window with specified dimensions and title */
void		*mlx_new_window(void *mlx_ptr, int width, int height,
				char *title);
/* Start MLX event loop */
int			mlx_loop(void *mlx_ptr);
/* Register event handler for window events */
int			mlx_hook(void *win_ptr, int event, int mask,
				int (*f)(), void *param);
/* Register keyboard event handler */
int			mlx_key_hook(void *win_ptr, int (*f)(), void *param);
/* Create new image buffer */
void		*mlx_new_image(void *mlx_ptr, int width, int height);
/* Get image data address for direct pixel manipulation */
char		*mlx_get_data_addr(void *img_ptr, int *bpp, int *size_line,
				int *endian);
/* Display image buffer to window */
int			mlx_put_image_to_window(void *mlx_ptr, void *win_ptr,
				void *img_ptr, int x, int y);
/* Destroy image buffer */
int			mlx_destroy_image(void *mlx_ptr, void *img_ptr);
/* Register loop hook for continuous rendering */
int			mlx_loop_hook(void *mlx_ptr, int (*f)(), void *param);

#endif
