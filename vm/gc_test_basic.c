/*
 * Test 1: Basic Object Allocation
 *
 * Purpose: Verify that objects can be allocated without crashes
 * and that the object count is tracked correctly.
 *
 * Note: Auto GC is disabled for these tests to test pure allocation behavior.
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "vm.h"  /* Includes gc.h automatically */

void test_basic_allocation() {
    printf("Test 1: Basic Allocation\n");
    printf("-------------------------\n");

    /* Create a VM */
    VM *vm = vm_create();
    assert(vm != NULL);

    /* Disable auto GC for basic allocation tests */
    gc_set_auto_collect(vm, false);

    /* Verify initial state */
    assert(vm->num_objects == 0);
    assert(vm->first_object == NULL);
    printf("Initial state: 0 objects\n");

    /* Allocate a single pair */
    Object *a = new_pair(vm, NULL, NULL);
    assert(a != NULL);
    assert(vm->num_objects == 1);
    assert(vm->first_object == a);
    printf("After new_pair(NULL, NULL): 1 object\n");

    /* Allocate another pair */
    Object *b = new_pair(vm, NULL, NULL);
    assert(b != NULL);
    assert(vm->num_objects == 2);
    printf("After second new_pair: 2 objects\n");

    /* Allocate a pair with references */
    Object *c = new_pair(vm, a, b);
    assert(c != NULL);
    assert(c->pair.left == a);
    assert(c->pair.right == b);
    assert(vm->num_objects == 3);
    printf("After new_pair(a, b): 3 objects\n");

    /* Verify the linked list */
    int count = 0;
    Object *obj = vm->first_object;
    while (obj) {
        count++;
        obj = obj->next;
    }
    assert(count == 3);
    printf("Linked list verification: 3 objects found\n");

    /* Cleanup */
    gc_cleanup(vm);
    assert(vm->num_objects == 0);
    assert(vm->first_object == NULL);
    printf("After cleanup: 0 objects\n");

    vm_destroy(vm);

    printf("PASS Test 1\n\n");
}

void test_multiple_allocations() {
    printf("Test 2: Multiple Allocations\n");
    printf("-----------------------------\n");

    VM *vm = vm_create();

    /* Disable auto GC for basic allocation tests */
    gc_set_auto_collect(vm, false);

    /* Allocate many objects */
    const int num_objects = 100;
    for (int i = 0; i < num_objects; i++) {
        Object *obj = new_pair(vm, NULL, NULL);
        assert(obj != NULL);
    }

    assert(vm->num_objects == num_objects);
    printf("Allocated %d objects successfully\n", num_objects);

    /* Verify all objects are in the linked list */
    int count = 0;
    Object *obj = vm->first_object;
    while (obj) {
        count++;
        obj = obj->next;
    }
    assert(count == num_objects);
    printf("Linked list contains all %d objects\n", count);

    gc_cleanup(vm);
    vm_destroy(vm);

    printf("PASS Test 2\n\n");
}

void test_object_references() {
    printf("Test 3: Object References\n");
    printf("--------------------------\n");

    VM *vm = vm_create();

    /* Disable auto GC for basic allocation tests */
    gc_set_auto_collect(vm, false);

    /* Create a chain of pairs */
    Object *a = new_pair(vm, NULL, NULL);
    Object *b = new_pair(vm, a, NULL);
    Object *c = new_pair(vm, b, a);

    /* Verify references */
    assert(b->pair.left == a);
    assert(b->pair.right == NULL);
    assert(c->pair.left == b);
    assert(c->pair.right == a);

    printf("Object reference chain created correctly\n");
    printf("a: %p\n", (void*)a);
    printf("b: %p (left=%p, right=%p)\n", (void*)b, (void*)b->pair.left, (void*)b->pair.right);
    printf("c: %p (left=%p, right=%p)\n", (void*)c, (void*)c->pair.left, (void*)c->pair.right);

    gc_cleanup(vm);
    vm_destroy(vm);

    printf("PASS Test 3\n\n");
}

int main() {
    printf("=================================\n");
    printf("  GC Basic Allocation Tests\n");
    printf("=================================\n\n");

    test_basic_allocation();
    test_multiple_allocations();
    test_object_references();

    printf("=================================\n");
    printf("  All Tests PASSED\n");
    printf("=================================\n");

    return 0;
}
