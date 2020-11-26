/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qfeuilla <qfeuilla@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/21 23:56:42 by qfeuilla          #+#    #+#             */
/*   Updated: 2020/11/26 19:02:32 by qfeuilla         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "mlx.h"
#include "mlx_int.h"
#include <math.h>
#include <pthread.h>

#define TRUE 1
#define FALSE 0

void	*mlx;
void	*win;
void	*g_image_ptr;
int		ready;

unsigned int g_iter;
unsigned int g_min;


char		*g_m;
int			g_bpp;
int			g_size_line;
int			g_endian;

double		offsetX;
double		offsetY;
float		zoom;

#define	WIDTH		500
#define	HEIGTH		500

#define IND			WIDTH * y + x

double			g_u[WIDTH*HEIGTH + WIDTH + 1];
double			g_v[WIDTH*HEIGTH + WIDTH + 1];
double			g_a[WIDTH*HEIGTH + WIDTH + 1];
double			g_b[WIDTH*HEIGTH + WIDTH + 1];
unsigned int	g_n[WIDTH*HEIGTH + WIDTH + 1];
unsigned int	g_nm[WIDTH*HEIGTH + WIDTH + 1];
double			g_k[WIDTH*HEIGTH + WIDTH + 1];

double			g_maxk;


void	mandelbrut(int x, int y) {
	if (ready == TRUE) {
		g_u[IND] = (((x - (WIDTH / 2.0)) / HEIGTH) / zoom) + offsetX;
		g_v[IND] = (((y - (HEIGTH / 2.0)) / HEIGTH) / zoom) + offsetY;
		g_a[IND] = 0;
		g_b[IND] = 0;
		g_n[IND] = 0;
		g_iter = (unsigned int)(g_min);
		g_nm[IND] = g_n[IND] + g_iter;
	} else {
		g_nm[IND] = g_n[IND] + 20;
	}

	// printf("(%d;%d):%d\n", x, y, IND);
	double a0;
	while (g_a[IND] * g_a[IND] + g_b[IND] * g_b[IND] <= 4.0f && g_n[IND] <  g_nm[IND]) {
		a0 = g_a[IND] * g_a[IND] - g_b[IND] * g_b[IND] + (g_u[IND] * 2.0 - 1.0);
		
		g_b[IND] = 2.0 * g_a[IND] * g_b[IND] + (g_v[IND] * 2.0);
		g_a[IND] = a0;

		g_n[IND] += 1;
	}

	g_k[IND] = (double)g_n[IND];
	g_k[IND] = (g_n[IND] == g_nm[IND]) ? 1.0 : (g_k[IND] + 1.0 - log10f(log2f(sqrtf(g_a[IND] * g_a[IND] + g_b[IND] * g_b[IND])))) / g_nm[IND]; 
}

void	zulia(int x, int y, float alpha, float beta) {
	if (ready == TRUE) {
		g_u[IND] = (((x - (WIDTH / 2.0)) / HEIGTH) / zoom) + offsetX;
		g_v[IND] = (((y - (HEIGTH / 2.0)) / HEIGTH) / zoom) + offsetY;
		g_a[IND] = (g_u[IND] * 2.0 - 1.0);
		g_b[IND] = (g_v[IND] * 2.0);
		g_n[IND] = 0;
		g_iter = (unsigned int)(g_min);
		g_nm[IND] = g_n[IND] + g_iter;
	} else {
		g_nm[IND] = g_n[IND] + 20;
	}

	// printf("(%d;%d):%d\n", x, y, IND);
	double a0;
	while (g_a[IND] * g_a[IND] + g_b[IND] * g_b[IND] <= 4.0f && g_n[IND] <  g_nm[IND]) {
		a0 = g_a[IND] * g_a[IND] - g_b[IND] * g_b[IND] + alpha;
		
		g_b[IND] = 2.0 * g_a[IND] * g_b[IND] + beta;
		g_a[IND] = a0;

		g_n[IND] += 1;
	}
			
	g_k[IND] = (double)g_n[IND];
	g_k[IND] = (g_n[IND] == g_nm[IND]) ? 1.0 : (g_k[IND] + 1.0 - log10f(log2f(sqrtf(g_a[IND] * g_a[IND] + g_b[IND] * g_b[IND])))) / g_nm[IND]; 
}

int manage_key(int key) {
	printf("Key in Win : %d\n", key);
	if (key == 65307)
	{
		printf("End of fractol\n");
		mlx_destroy_window(mlx, win);
		free(mlx);
		exit(0);
	} else if (key == 100) {
		ready = TRUE;
		offsetX += 0.2 / (zoom * 2);
	} else if (key == 113) {
		ready = TRUE;
		offsetX -= 0.2 / (zoom * 2);
	} else if (key == 115) {
		ready = TRUE;
		offsetY += 0.2 / (zoom * 2);
	} else if (key == 122) {
		ready = TRUE;
		offsetY -= 0.2 / (zoom * 2);
	}
	return (0);
}

int	end() {
	printf("End of fractol\n");
	free(mlx);
	exit(0);
}

int			render_scene()
{
	int			x;
	int			y;
	mlx_clear_window(mlx, win);
	g_m = mlx_get_data_addr(g_image_ptr, &g_bpp, &g_size_line, &g_endian);
	y = 0;
	g_maxk = 0;
	while (y++ < HEIGTH && (x = 0) == 0)
	{
		while (x++ < WIDTH)
		{
			mandelbrut(x, y);

			g_k[IND] = 1 - g_k[IND];
			if (g_k[IND] > g_maxk) {
				g_maxk = g_k[IND];
			}
		}
	}
	y = 0;
	while (y++ < HEIGTH && (x = 0) == 0)
	{
		while (x++ < WIDTH)
		{
			g_k[IND] = g_k[IND] / g_maxk;
			
			int r1, g1, b1, r2, g2, b2;
			r1 = 225;
			g1 = 225;
			b1 = 225;
			r2 = 88;
			g2 = 0;
			b2 = 0;

			if (g_n[IND] == g_nm[IND]) {
				*g_m++ = 0;
				*g_m++ = 0;
				*g_m++ = 0;
				*g_m++ = 0;
			} else {
				*g_m++ = (int)(g_k[IND] * (b2 - b1) + b1);
				*g_m++ = (int)(g_k[IND] * (g2 - g1) + g1);
				*g_m++ = (int)(g_k[IND] * (r2 - r1) + r1);
				*g_m++ = 0;
			}

		}
	}
	g_iter += 100;
	ready = FALSE;
	mlx_put_image_to_window(mlx, win, g_image_ptr, 0, 0);
	return (1);
}

int		manage_mouse(int key) {
	printf("Mouse_key : %d\n", key);
	int x = 0, y = 0;
	mlx_mouse_get_pos(mlx, win, &x, &y);

	if (key == 5) {
		ready = TRUE;
		g_min -= 5;
		zoom = zoom * 0.9f;
	} else if (key == 4) {
		ready = TRUE;
		g_min += 5;
		zoom = zoom * 1.1f;
	}
	return 0;
}

int main(int ac, char **av) {
	(void)ac;
	(void)av;

	// TODO : check close window
	zoom = 1;
	g_min = 20;
	offsetX = 0;
	offsetY = 0;
	ready = TRUE;
	mlx = mlx_init();
	win = mlx_new_window(mlx, WIDTH, HEIGTH, "fractol");
	g_image_ptr = mlx_new_image(mlx, WIDTH, HEIGTH);
	mlx_key_hook(win, manage_key, NULL);
	mlx_mouse_hook(win, manage_mouse, NULL);
	mlx_loop_hook(mlx, render_scene, NULL);
	mlx_hook(win, 17, 1L << 0, end, NULL);
	mlx_loop(mlx);
	return (1);
}