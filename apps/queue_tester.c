#include <assert.h>
#include <stddef.h>

#include "../libuthread/queue.h"

void test_create_destroy(void) {
    queue_t q = queue_create();
    assert(q != NULL);
    assert(queue_destroy(q) == 0);
}

void test_queue_simple(void)
{
    queue_t q;
    int data = 3, *ptr;

    q = queue_create();
    queue_enqueue(q, &data);
    queue_dequeue(q, (void**)&ptr);
    assert(ptr == &data);
}

void test_fifo_order(void) {
    queue_t q = queue_create();
    int a = 1, b = 2, *ptr;

    queue_enqueue(q, &a);
    queue_enqueue(q, &b);

    queue_dequeue(q, (void**)&ptr);
    assert(*ptr == 1);

    queue_dequeue(q, (void**)&ptr);
    assert(*ptr == 2);
}

static void delete_if_42(queue_t q, void *data) {
    int *val = (int*)data;
    if (*val == 42)
        queue_delete(q, data);
}

void test_iterate_delete(void) {
    queue_t q = queue_create();
    int data[] = {10, 42, 99};

    for (int i = 0; i < 3; i++)
        queue_enqueue(q, &data[i]);

    queue_iterate(q, delete_if_42);

    assert(queue_length(q) == 2);
}

int main() {
    test_create_destroy(); // Call your test functions
    test_queue_simple();
    test_fifo_order();
    test_iterate_delete();

    return 0;
}