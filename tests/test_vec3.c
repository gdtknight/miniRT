/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_vec3.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miniRT team <miniRT@42.fr>                 +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 00:00:00 by miniRT            #+#    #+#             */
/*   Updated: 2026/03/25 00:00:00 by miniRT           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "vec3.h"
#include <stdio.h>
#include <assert.h>
#include <math.h>

#define TEST_EPS 1e-9

/* ---- vec3_add ---- */

static void	test_vec3_add_basic(void)
{
	t_vec3	r;

	printf("Testing vec3_add (basic)...\n");
	r = vec3_add((t_vec3){1.0, 2.0, 3.0}, (t_vec3){4.0, 5.0, 6.0});
	assert(r.x == 5.0 && r.y == 7.0 && r.z == 9.0);
	printf("  ✓ basic addition correct\n");
}

static void	test_vec3_add_zero(void)
{
	t_vec3	r;

	printf("Testing vec3_add (identity)...\n");
	r = vec3_add((t_vec3){0.0, 0.0, 0.0}, (t_vec3){1.0, -1.0, 0.5});
	assert(r.x == 1.0 && r.y == -1.0 && r.z == 0.5);
	printf("  ✓ zero vector identity correct\n");
}

static void	test_vec3_add_negation(void)
{
	t_vec3	r;

	printf("Testing vec3_add (negation sums to zero)...\n");
	r = vec3_add((t_vec3){-1.0, -2.0, -3.0}, (t_vec3){1.0, 2.0, 3.0});
	assert(r.x == 0.0 && r.y == 0.0 && r.z == 0.0);
	printf("  ✓ negation sum is zero vector\n");
}

/* ---- vec3_subtract ---- */

static void	test_vec3_subtract_basic(void)
{
	t_vec3	r;

	printf("Testing vec3_subtract (basic)...\n");
	r = vec3_subtract((t_vec3){5.0, 7.0, 9.0}, (t_vec3){4.0, 5.0, 6.0});
	assert(r.x == 1.0 && r.y == 2.0 && r.z == 3.0);
	printf("  ✓ basic subtraction correct\n");
}

static void	test_vec3_subtract_self(void)
{
	t_vec3	r;

	printf("Testing vec3_subtract (self)...\n");
	r = vec3_subtract((t_vec3){1.0, 2.0, 3.0}, (t_vec3){1.0, 2.0, 3.0});
	assert(r.x == 0.0 && r.y == 0.0 && r.z == 0.0);
	printf("  ✓ self-subtraction is zero vector\n");
}

static void	test_vec3_subtract_from_zero(void)
{
	t_vec3	r;

	printf("Testing vec3_subtract (from zero)...\n");
	r = vec3_subtract((t_vec3){0.0, 0.0, 0.0}, (t_vec3){1.0, -1.0, 0.5});
	assert(r.x == -1.0 && r.y == 1.0 && r.z == -0.5);
	printf("  ✓ subtract from zero negates correctly\n");
}

/* ---- vec3_multiply ---- */

static void	test_vec3_multiply_scale(void)
{
	t_vec3	r;

	printf("Testing vec3_multiply (scale up)...\n");
	r = vec3_multiply((t_vec3){2.0, -3.0, 1.5}, 2.0);
	assert(r.x == 4.0 && r.y == -6.0 && r.z == 3.0);
	printf("  ✓ scale-up correct\n");
}

static void	test_vec3_multiply_zero(void)
{
	t_vec3	r;

	printf("Testing vec3_multiply (by zero)...\n");
	r = vec3_multiply((t_vec3){1.0, 2.0, 3.0}, 0.0);
	assert(r.x == 0.0 && r.y == 0.0 && r.z == 0.0);
	printf("  ✓ multiply by zero gives zero vector\n");
}

static void	test_vec3_multiply_negate(void)
{
	t_vec3	r;

	printf("Testing vec3_multiply (negate)...\n");
	r = vec3_multiply((t_vec3){1.0, 2.0, 3.0}, -1.0);
	assert(r.x == -1.0 && r.y == -2.0 && r.z == -3.0);
	printf("  ✓ multiply by -1 negates correctly\n");
}

static void	test_vec3_multiply_fraction(void)
{
	t_vec3	r;

	printf("Testing vec3_multiply (fraction)...\n");
	r = vec3_multiply((t_vec3){4.0, 6.0, 8.0}, 0.5);
	assert(r.x == 2.0 && r.y == 3.0 && r.z == 4.0);
	printf("  ✓ multiply by 0.5 halves correctly\n");
}

/* ---- vec3_dot ---- */

static void	test_vec3_dot_perpendicular(void)
{
	double	d;

	printf("Testing vec3_dot (perpendicular vectors)...\n");
	d = vec3_dot((t_vec3){1.0, 0.0, 0.0}, (t_vec3){0.0, 1.0, 0.0});
	assert(d == 0.0);
	d = vec3_dot((t_vec3){0.0, 1.0, 0.0}, (t_vec3){0.0, 0.0, 1.0});
	assert(d == 0.0);
	printf("  ✓ perpendicular vectors have zero dot product\n");
}

static void	test_vec3_dot_parallel(void)
{
	double	d;

	printf("Testing vec3_dot (parallel vectors)...\n");
	d = vec3_dot((t_vec3){1.0, 2.0, 3.0}, (t_vec3){4.0, 5.0, 6.0});
	assert(d == 32.0);
	d = vec3_dot((t_vec3){3.0, 0.0, 0.0}, (t_vec3){3.0, 0.0, 0.0});
	assert(d == 9.0);
	printf("  ✓ parallel dot product correct\n");
}

static void	test_vec3_dot_antiparallel(void)
{
	double	d;

	printf("Testing vec3_dot (anti-parallel vectors)...\n");
	d = vec3_dot((t_vec3){1.0, 0.0, 0.0}, (t_vec3){-1.0, 0.0, 0.0});
	assert(d == -1.0);
	printf("  ✓ anti-parallel dot product is negative\n");
}

/* ---- vec3_cross ---- */

static void	test_vec3_cross_basis(void)
{
	t_vec3	r;

	printf("Testing vec3_cross (standard basis)...\n");
	r = vec3_cross((t_vec3){1.0, 0.0, 0.0}, (t_vec3){0.0, 1.0, 0.0});
	assert(r.x == 0.0 && r.y == 0.0 && r.z == 1.0);
	r = vec3_cross((t_vec3){0.0, 1.0, 0.0}, (t_vec3){0.0, 0.0, 1.0});
	assert(r.x == 1.0 && r.y == 0.0 && r.z == 0.0);
	r = vec3_cross((t_vec3){0.0, 0.0, 1.0}, (t_vec3){1.0, 0.0, 0.0});
	assert(r.x == 0.0 && r.y == 1.0 && r.z == 0.0);
	printf("  ✓ cross product with standard basis correct\n");
}

static void	test_vec3_cross_anticommutative(void)
{
	t_vec3	r;

	printf("Testing vec3_cross (anti-commutative property)...\n");
	r = vec3_cross((t_vec3){0.0, 1.0, 0.0}, (t_vec3){1.0, 0.0, 0.0});
	assert(r.x == 0.0 && r.y == 0.0 && r.z == -1.0);
	printf("  ✓ cross product is anti-commutative\n");
}

static void	test_vec3_cross_self(void)
{
	t_vec3	r;

	printf("Testing vec3_cross (self cross product)...\n");
	r = vec3_cross((t_vec3){1.0, 2.0, 3.0}, (t_vec3){1.0, 2.0, 3.0});
	assert(r.x == 0.0 && r.y == 0.0 && r.z == 0.0);
	printf("  ✓ self cross product is zero vector\n");
}

/* ---- vec3_magnitude ---- */

static void	test_vec3_magnitude_345(void)
{
	printf("Testing vec3_magnitude (3-4-5 triangle)...\n");
	assert(fabs(vec3_magnitude((t_vec3){3.0, 4.0, 0.0}) - 5.0) < TEST_EPS);
	assert(fabs(vec3_magnitude((t_vec3){0.0, 3.0, 4.0}) - 5.0) < TEST_EPS);
	printf("  ✓ 3-4-5 magnitude correct\n");
}

static void	test_vec3_magnitude_unit(void)
{
	printf("Testing vec3_magnitude (unit axes)...\n");
	assert(fabs(vec3_magnitude((t_vec3){1.0, 0.0, 0.0}) - 1.0) < TEST_EPS);
	assert(fabs(vec3_magnitude((t_vec3){0.0, 1.0, 0.0}) - 1.0) < TEST_EPS);
	assert(fabs(vec3_magnitude((t_vec3){0.0, 0.0, 1.0}) - 1.0) < TEST_EPS);
	printf("  ✓ unit axis magnitudes are 1\n");
}

static void	test_vec3_magnitude_zero(void)
{
	printf("Testing vec3_magnitude (zero vector)...\n");
	assert(vec3_magnitude((t_vec3){0.0, 0.0, 0.0}) == 0.0);
	printf("  ✓ zero vector has magnitude 0\n");
}

static void	test_vec3_magnitude_general(void)
{
	printf("Testing vec3_magnitude (general 3D vector)...\n");
	assert(fabs(vec3_magnitude((t_vec3){1.0, 1.0, 1.0}) - sqrt(3.0)) < TEST_EPS);
	assert(fabs(vec3_magnitude((t_vec3){2.0, 2.0, 1.0}) - 3.0) < TEST_EPS);
	printf("  ✓ general magnitude correct\n");
}

/* ---- vec3_normalize ---- */

static void	test_vec3_normalize_basic(void)
{
	t_vec3	r;

	printf("Testing vec3_normalize (3-4-0 vector)...\n");
	r = vec3_normalize((t_vec3){3.0, 4.0, 0.0});
	assert(fabs(r.x - 0.6) < TEST_EPS);
	assert(fabs(r.y - 0.8) < TEST_EPS);
	assert(fabs(r.z) < TEST_EPS);
	printf("  ✓ normalized 3-4-0 is (0.6, 0.8, 0)\n");
}

static void	test_vec3_normalize_unit_length(void)
{
	t_vec3	r;
	double	len;

	printf("Testing vec3_normalize (result is unit length)...\n");
	r = vec3_normalize((t_vec3){1.0, 2.0, 3.0});
	len = sqrt(r.x * r.x + r.y * r.y + r.z * r.z);
	assert(fabs(len - 1.0) < TEST_EPS);
	r = vec3_normalize((t_vec3){-5.0, 2.0, 8.0});
	len = sqrt(r.x * r.x + r.y * r.y + r.z * r.z);
	assert(fabs(len - 1.0) < TEST_EPS);
	printf("  ✓ normalized vector has unit length\n");
}

static void	test_vec3_normalize_already_unit(void)
{
	t_vec3	r;

	printf("Testing vec3_normalize (already unit vector)...\n");
	r = vec3_normalize((t_vec3){1.0, 0.0, 0.0});
	assert(r.x == 1.0 && r.y == 0.0 && r.z == 0.0);
	printf("  ✓ normalizing a unit vector preserves it\n");
}

static void	test_vec3_normalize_zero_vector(void)
{
	t_vec3	r;

	printf("Testing vec3_normalize (zero vector edge case)...\n");
	r = vec3_normalize((t_vec3){0.0, 0.0, 0.0});
	assert(r.x == 0.0 && r.y == 0.0 && r.z == 0.0);
	printf("  ✓ normalizing zero vector returns zero vector (no crash)\n");
}

/* ---- main ---- */

int	main(void)
{
	printf("\n=== Vec3 Unit Tests ===\n\n");

	printf("--- vec3_add ---\n");
	test_vec3_add_basic();
	test_vec3_add_zero();
	test_vec3_add_negation();

	printf("\n--- vec3_subtract ---\n");
	test_vec3_subtract_basic();
	test_vec3_subtract_self();
	test_vec3_subtract_from_zero();

	printf("\n--- vec3_multiply ---\n");
	test_vec3_multiply_scale();
	test_vec3_multiply_zero();
	test_vec3_multiply_negate();
	test_vec3_multiply_fraction();

	printf("\n--- vec3_dot ---\n");
	test_vec3_dot_perpendicular();
	test_vec3_dot_parallel();
	test_vec3_dot_antiparallel();

	printf("\n--- vec3_cross ---\n");
	test_vec3_cross_basis();
	test_vec3_cross_anticommutative();
	test_vec3_cross_self();

	printf("\n--- vec3_magnitude ---\n");
	test_vec3_magnitude_345();
	test_vec3_magnitude_unit();
	test_vec3_magnitude_zero();
	test_vec3_magnitude_general();

	printf("\n--- vec3_normalize ---\n");
	test_vec3_normalize_basic();
	test_vec3_normalize_unit_length();
	test_vec3_normalize_already_unit();
	test_vec3_normalize_zero_vector();

	printf("\n=== All vec3 tests passed! ===\n\n");
	return (0);
}
