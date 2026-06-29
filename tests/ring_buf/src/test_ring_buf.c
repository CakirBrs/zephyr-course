/*
 * Ring Buffer Module - Homework Test Skeleton
 *
 * test_fresh_state is provided as a worked example. Fill in the remaining
 * 7 ZTEST bodies according to TEST_SPEC.md. Stubs call ztest_test_skip()
 * so the binary builds and runs cleanly before each test is implemented.
 *
 * Run:
 *   west twister -T tests/ring_buf -p native_sim
 */

#include <zephyr/ztest.h>
#include <errno.h>

#include "ring_buf.h"

/*
 * Shared before hook: every suite reinitialises the ring buffer with a
 * capacity of 4 so tests start from a clean, known state. Capacity 4 is
 * enough to exercise FIFO order (push 1, 2, 3) and overflow (full at 4).
 */
static void before(void *f)
{
	ARG_UNUSED(f);
	rb_init(4);
}

/*
 * ============================================================================
 * Test Suite: ring_buf_init
 *
 * Initial state and re-initialization behaviour.
 * ============================================================================
 */
ZTEST_SUITE(ring_buf_init, NULL, NULL, before, NULL, NULL);

/* PROVIDED — study this test before writing the rest. */
ZTEST(ring_buf_init, test_fresh_state)
{
	zassert_true(rb_is_empty(), "Fresh buffer must be empty");
	zassert_equal(rb_count(), 0, "Fresh buffer count must be 0");
}

ZTEST(ring_buf_init, test_reinit_clears_state)
{
	zassert_ok(rb_push(99), "push should succeed");
	zassert_equal(rb_count(), 1, "count should be 1 after push");

	zassert_ok(rb_init(4), "re-init should succeed");

	zassert_true(rb_is_empty(), "buffer must be empty after re-init");
	zassert_equal(rb_count(), 0, "count must be 0 after re-init");
}

/*
 * ============================================================================
 * Test Suite: ring_buf_push_pop
 *
 * Single push/pop round-trip, FIFO order, full error path.
 * ============================================================================
 */
ZTEST_SUITE(ring_buf_push_pop, NULL, NULL, before, NULL, NULL);

ZTEST(ring_buf_push_pop, test_single_push_pop)
{
	int v = 0;

	zassert_ok(rb_push(42), "push should succeed");
	zassert_ok(rb_pop(&v), "pop should succeed");
	zassert_equal(v, 42, "popped value must equal pushed value");
	zassert_true(rb_is_empty(), "buffer must be empty after pop");
}

ZTEST(ring_buf_push_pop, test_fifo_order)
{
	int v = 0;

	zassert_ok(rb_push(1), "push 1 should succeed");
	zassert_ok(rb_push(2), "push 2 should succeed");
	zassert_ok(rb_push(3), "push 3 should succeed");

	zassert_ok(rb_pop(&v), "pop should succeed");
	zassert_equal(v, 1, "first out must be 1");
	zassert_ok(rb_pop(&v), "pop should succeed");
	zassert_equal(v, 2, "second out must be 2");
	zassert_ok(rb_pop(&v), "pop should succeed");
	zassert_equal(v, 3, "third out must be 3");
}

ZTEST(ring_buf_push_pop, test_push_full_returns_enospc)
{
	for (int i = 0; i < 4; i++) {
		zassert_ok(rb_push(i), "push within capacity should succeed");
	}
	zassert_equal(rb_push(99), -ENOSPC, "push on full buffer must return -ENOSPC");
}

/*
 * ============================================================================
 * Test Suite: ring_buf_boundaries
 *
 * Peek semantics and NULL-pointer boundary conditions.
 * ============================================================================
 */
ZTEST_SUITE(ring_buf_boundaries, NULL, NULL, before, NULL, NULL);

ZTEST(ring_buf_boundaries, test_peek_does_not_consume)
{
	int v = 0;

	zassert_ok(rb_push(7), "push should succeed");

	zassert_ok(rb_peek(&v), "peek should succeed");
	zassert_equal(v, 7, "peek must return 7");

	v = 0;
	zassert_ok(rb_peek(&v), "second peek should succeed");
	zassert_equal(v, 7, "peek must still return 7");

	zassert_equal(rb_count(), 1, "peek must not consume; count stays 1");
}

ZTEST(ring_buf_boundaries, test_pop_null_returns_einval)
{
	zassert_equal(rb_pop(NULL), -EINVAL, "pop(NULL) must return -EINVAL");
}

ZTEST(ring_buf_boundaries, test_is_full_after_fill)
{
	for (int i = 0; i < 4; i++) {
		zassert_ok(rb_push(i), "push within capacity should succeed");
	}
	zassert_true(rb_is_full(), "buffer must be full after 4 pushes");
	zassert_equal(rb_count(), 4, "count must be 4 after fill");
}

ZTEST(ring_buf_init, test_init_invalid_capacity)
{
	zassert_equal(rb_init(0), -EINVAL, "init(0) must return -EINVAL");
	zassert_equal(rb_init(RING_BUF_MAX_CAPACITY + 1U), -EINVAL,
		      "init(>MAX) must return -EINVAL");
}

ZTEST(ring_buf_boundaries, test_pop_empty_returns_enodata)
{
	int v = 0;

	zassert_equal(rb_pop(&v), -ENODATA, "pop on empty must return -ENODATA");
}

ZTEST(ring_buf_boundaries, test_peek_empty_returns_enodata)
{
	int v = 0;

	zassert_equal(rb_peek(&v), -ENODATA, "peek on empty must return -ENODATA");
}

ZTEST(ring_buf_boundaries, test_peek_null_returns_einval)
{
	zassert_equal(rb_peek(NULL), -EINVAL, "peek(NULL) must return -EINVAL");
}
