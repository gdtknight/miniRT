/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_shadow_config.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miniRT team <miniRT@42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/17 00:00:00 by miniRT           #+#    #+#             */
/*   Updated: 2025/12/17 00:00:00 by miniRT          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shadow.h"
#include <stdio.h>
#include <assert.h>

void	test_init_shadow_config(void)
{
	t_shadow_config	config;

	printf("Testing init_shadow_config...\n");
	config = init_shadow_config();
	assert(config.samples == 4);
	assert(config.softness == 0.1);
	assert(config.bias_scale == 2.0);
	assert(config.enable_ao == 0);
	printf("✓ init_shadow_config passed\n");
}

void	test_validate_shadow_config(void)
{
	t_shadow_config	config;

	printf("Testing validate_shadow_config...\n");
	config = init_shadow_config();
	assert(validate_shadow_config(&config) == 1);
	config.samples = 0;
	assert(validate_shadow_config(&config) == 0);
	config.samples = 4;
	config.softness = -0.1;
	assert(validate_shadow_config(&config) == 0);
	config.softness = 1.1;
	assert(validate_shadow_config(&config) == 0);
	config.softness = 0.5;
	config.bias_scale = -1.0;
	assert(validate_shadow_config(&config) == 0);
	assert(validate_shadow_config(NULL) == 0);
	printf("✓ validate_shadow_config passed\n");
}

void	test_set_shadow_samples(void)
{
	t_shadow_config	config;

	printf("Testing set_shadow_samples...\n");
	config = init_shadow_config();
	set_shadow_samples(&config, 16);
	assert(config.samples == 16);
	set_shadow_samples(&config, 0);
	assert(config.samples == 16);
	set_shadow_samples(NULL, 8);
	printf("✓ set_shadow_samples passed\n");
}

void	test_set_shadow_softness(void)
{
	t_shadow_config	config;

	printf("Testing set_shadow_softness...\n");
	config = init_shadow_config();
	set_shadow_softness(&config, 0.5);
	assert(config.softness == 0.5);
	set_shadow_softness(&config, -0.1);
	assert(config.softness == 0.5);
	set_shadow_softness(&config, 1.1);
	assert(config.softness == 0.5);
	set_shadow_softness(NULL, 0.3);
	printf("✓ set_shadow_softness passed\n");
}

int	main(void)
{
	printf("\n=== Shadow Config Unit Tests ===\n\n");
	test_init_shadow_config();
	test_validate_shadow_config();
	test_set_shadow_samples();
	test_set_shadow_softness();
	printf("\n=== All tests passed! ===\n\n");
	return (0);
}
