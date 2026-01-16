/*
 * Test 1.6.6: Closure Capture
 * Test 1.6.7: Stress Allocation
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "vm.h"  /* Includes gc.h automatically */

void test_closure_capture() {
    printf("Test 1.6.6: Closure Capture\n");
    printf("----------------------------\n");

    VM *vm = vm_create();

    /* Disable auto GC */
    gc_set_auto_collect(vm, false);

    /* Create environment */
    Object *env = new_pair(vm, NULL, NULL);

    /* Create function */
    Object *fn = new_function(vm);

    /* Create closure that captures environment */
    Object *cl = new_closure(vm, fn, env);

    assert(vm->num_objects == 3);
    printf("Created closure, function, and environment\n");

    /* Push closure onto stack */
    push(vm, VAL_OBJ(cl));

    /* Run GC */
    gc(vm);

    /* All three should survive */
    assert(vm->num_objects == 3);
    printf("Closure, function, and environment survived\n");

    gc_cleanup(vm);
    vm_destroy(vm);

    printf("PASS Test 1.6.6\n\n");
}

void test_stress_allocation() {
    printf("Test 1.6.7: Stress Allocation\n");
    printf("------------------------------\n");

    VM *vm = vm_create();

    /* Enable auto GC - let it handle the load */
    gc_set_auto_collect(vm, true);

    /* Allocate 100000 unreachable objects */
    for (int i = 0; i < 100000; i++) {
        new_pair(vm, NULL, NULL);
    }

    printf("Allocated 100000 objects (with auto GC)\n");

    /* Run final GC to clean up any remaining */
    gc(vm);

    /* Heap should be empty after GC */
    assert(vm->num_objects == 0);
    printf("Heap empty after GC\n");
    printf("No memory leaks or crashes\n");

    gc_cleanup(vm);
    vm_destroy(vm);

    printf("PASS Test 1.6.7\n\n");
}

int main() {
    printf("=======================================\n");
    printf("  GC Closure & Stress Tests\n");
    printf("=======================================\n\n");

    test_closure_capture();
    test_stress_allocation();

    printf("=======================================\n");
    printf("  All Tests PASSED\n");
    printf("=======================================\n");

    return 0;
}
