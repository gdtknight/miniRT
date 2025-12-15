/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   window.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miniRT team <miniRT@42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/15 00:00:00 by miniRT           #+#    #+#             */
/*   Updated: 2025/12/15 00:00:00 by miniRT          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WINDOW_H
# define WINDOW_H

# include "minirt.h"

typedef struct s_render
{
	void		*mlx;
	void		*win;
	t_scene		*scene;
}	t_render;

/* Window management */
t_render	*init_window(t_scene *scene);
int			close_window(void *param);
int			handle_key(int keycode, void *param);

/* MLX functions */
void	*mlx_init(void);
void	*mlx_new_window(void *mlx_ptr, int width, int height, char *title);
int		mlx_pixel_put(void *mlx_ptr, void *win_ptr, int x, int y, int color);
int		mlx_loop(void *mlx_ptr);
int		mlx_hook(void *win_ptr, int event, int mask, int (*f)(), void *param);
int		mlx_key_hook(void *win_ptr, int (*f)(), void *param);

#endif
