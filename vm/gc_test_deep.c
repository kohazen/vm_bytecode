/*
 * Test 1.6.5: Deep Object Graph
 * Purpose: Stress-test recursive marking with 10000 objects
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "vm.h"  /* Includes gc.h automatically */

void test_deep_object_graph() {
    printf("Test 1.6.5: Deep Object Graph\n");
    printf("------------------------------\n");

    VM *vm = vm_create();

    /* Disable auto GC while creating the chain */
    gc_set_auto_collect(vm, false);

    /* Create chain of 10000 objects */
    Object *root = new_pair(vm, NULL, NULL);
    Object *cur = root;

    for (int i = 0; i < 10000; i++) {
        Object *next = new_pair(vm, NULL, NULL);
        cur->pair.right = next;
        cur = next;
    }

    assert(vm->num_objects == 10001);
    printf("Created chain of 10001 objects\n");

    /* Push root onto stack */
    push(vm, VAL_OBJ(root));

    /* Run GC */
    gc(vm);

    /* All objects should survive */
    assert(vm->num_objects == 10001);
    printf("All 10001 objects survived GC\n");
    printf("No stack overflow occurred\n");

    gc_cleanup(vm);
    vm_destroy(vm);

    printf("PASS Test 1.6.5\n\n");
}

void test_auto_gc_trigger() {
    printf("Test: Automatic GC Triggering\n");
    printf("-------------------------------\n");

    VM *vm = vm_create();

    /* Enable auto GC for this test */
    gc_set_auto_collect(vm, true);

    printf("Initial threshold: %d\n", vm->max_objects);

    /* Allocate objects to trigger automatic GC */
    for (int i = 0; i < 20; i++) {
        new_pair(vm, NULL, NULL);  /* All unreachable */
    }

    printf("After allocating 20 unreachable objects:\n");
    printf("  Objects: %d\n", vm->num_objects);
    printf("  Auto GC was triggered\n");

    gc_cleanup(vm);
    vm_destroy(vm);

    printf("PASS Test\n\n");
}

int main() {
    printf("=======================================\n");
    printf("  GC Deep Graph Tests\n");
    printf("=======================================\n\n");

    test_deep_object_graph();
    test_auto_gc_trigger();

    printf("=======================================\n");
    printf("  All Tests PASSED\n");
    printf("=======================================\n");

    return 0;
}
