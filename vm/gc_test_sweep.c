/*
 * Test 1.6.2: Unreachable Object Collection
 * Purpose: Verify that unreachable objects are reclaimed by sweep phase
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "vm.h"  /* Includes gc.h automatically */

void test_unreachable_collection() {
    printf("Test 1.6.2: Unreachable Object Collection\n");
    printf("------------------------------------------\n");

    /*
     * From spec:
     *   Obj* a = new_pair(NULL, NULL);
     *   gc(vm);
     *   Expected: Object a is freed, heap is empty
     */

    VM *vm = vm_create();

    /* Disable auto GC to control when GC runs */
    gc_set_auto_collect(vm, false);

    /* Create an object but don't push it onto stack */
    Object *a = new_pair(vm, NULL, NULL);
    (void)a;  /* Intentionally unreachable, suppress warning */
    assert(vm->num_objects == 1);
    printf("Created unreachable object\n");

    /* Run full GC cycle */
    gc_collect(vm);

    /* Object should be freed, heap should be empty */
    assert(vm->num_objects == 0);
    assert(vm->first_object == NULL);
    printf("After GC: 0 objects (object freed)\n");

    vm_destroy(vm);
    printf("PASS Test 1.6.2\n\n");
}

void test_full_gc_cycle() {
    printf("Test: Full GC Cycle\n");
    printf("-------------------\n");

    VM *vm = vm_create();

    /* Disable auto GC to control when GC runs */
    gc_set_auto_collect(vm, false);

    /* Create 5 objects, only 2 reachable */
    Object *a = new_pair(vm, NULL, NULL);  /* unreachable */
    Object *b = new_pair(vm, NULL, NULL);  /* reachable via d */
    Object *c = new_pair(vm, NULL, NULL);  /* unreachable */
    Object *d = new_pair(vm, b, NULL);     /* reachable (references b) */
    Object *e = new_pair(vm, NULL, NULL);  /* unreachable */

    /* Suppress warnings for intentionally unreachable objects */
    (void)a;
    (void)c;
    (void)e;

    assert(vm->num_objects == 5);
    printf("Created 5 objects\n");

    /* Push only d onto stack (d and b are reachable) */
    push(vm, VAL_OBJ(d));
    printf("Pushed d onto stack (d->b transitively reachable)\n");

    /* Run GC */
    gc_collect(vm);

    /* Should have 2 objects remaining (d and b) */
    assert(vm->num_objects == 2);
    printf("After GC: 2 objects remain (d and b)\n");
    printf("Freed 3 unreachable objects (a, c, e)\n");

    gc_cleanup(vm);
    vm_destroy(vm);
    printf("PASS Test\n\n");
}

void test_mark_reset() {
    printf("Test: Mark Reset After Sweep\n");
    printf("-----------------------------\n");

    VM *vm = vm_create();

    /* Disable auto GC to control when GC runs */
    gc_set_auto_collect(vm, false);

    /* Create object and push onto stack */
    Object *a = new_pair(vm, NULL, NULL);
    push(vm, VAL_OBJ(a));

    /* Run GC */
    gc_collect(vm);

    /* Object should survive but marked should be reset to false */
    assert(vm->num_objects == 1);
    assert(a->marked == false);
    printf("Object survived\n");
    printf("Mark bit reset\n");

    gc_cleanup(vm);
    vm_destroy(vm);
    printf("PASS Test\n\n");
}

void test_gc_threshold_update() {
    printf("Test: GC Threshold Update\n");
    printf("-------------------------\n");

    VM *vm = vm_create();

    /* Disable auto GC to control when GC runs */
    gc_set_auto_collect(vm, false);

    assert(vm->max_objects == 8);
    printf("Initial threshold: %d\n", vm->max_objects);

    /* Create 5 reachable objects (so threshold = 10, above min of 8) */
    Object *a = new_pair(vm, NULL, NULL);
    Object *b = new_pair(vm, a, NULL);
    Object *c = new_pair(vm, b, NULL);
    Object *d = new_pair(vm, c, NULL);
    Object *e = new_pair(vm, d, NULL);

    push(vm, VAL_OBJ(e));

    /* Run GC */
    gc_collect(vm);

    /* Threshold should be num_objects * 2 = 5 * 2 = 10 */
    assert(vm->num_objects == 5);
    assert(vm->max_objects == 10);
    printf("After GC: %d objects, threshold = %d (5 * 2 = 10)\n",
           vm->num_objects, vm->max_objects);

    gc_cleanup(vm);
    vm_destroy(vm);
    printf("PASS Test\n\n");
}

int main() {
    printf("=======================================\n");
    printf("  GC Sweep Phase Tests\n");
    printf("=======================================\n\n");

    test_unreachable_collection();
    test_full_gc_cycle();
    test_mark_reset();
    test_gc_threshold_update();

    printf("=======================================\n");
    printf("  All Tests PASSED\n");
    printf("=======================================\n");

    return 0;
}
