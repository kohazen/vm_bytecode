/*
 * Test 1.6.1: Basic Reachability
 *
 * Purpose: Verify that objects directly reachable from the stack are preserved.
 *
 * Test case from Lab 5 specification:
 *   Obj* a = new_pair(NULL, NULL);
 *   push(vm, VAL_OBJ(a));
 *   gc(vm);
 *   Expected: Object a survives, heap remains unchanged
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "vm.h"  /* Includes gc.h automatically */

void test_basic_reachability() {
    printf("Test 1.6.1: Basic Reachability\n");
    printf("-------------------------------\n");

    /* Create a VM */
    VM *vm = vm_create();
    assert(vm != NULL);

    /* Disable auto GC to control when GC runs */
    gc_set_auto_collect(vm, false);

    /* Create an object */
    Object *a = new_pair(vm, NULL, NULL);
    assert(a != NULL);
    assert(vm->num_objects == 1);
    printf("Created object a: %p\n", (void*)a);

    /* Push object onto stack - making it a root */
    push(vm, VAL_OBJ(a));
    assert(vm->stack_count == 1);
    printf("Pushed object a onto stack\n");

    /* Verify object is not marked initially */
    assert(a->marked == false);
    printf("Object a initially unmarked\n");

    /* Run mark phase */
    gc_mark_roots(vm);

    /* Verify object is now marked (reachable from stack) */
    assert(a->marked == true);
    printf("Object a marked after gc_mark_roots\n");

    /* Verify object still exists */
    assert(vm->num_objects == 1);
    printf("Object count unchanged: %d\n", vm->num_objects);

    /* Cleanup */
    pop(vm);
    gc_cleanup(vm);
    vm_destroy(vm);

    printf("PASS Test 1.6.1\n\n");
}

void test_unmarked_object() {
    printf("Test: Unmarked Object Detection\n");
    printf("--------------------------------\n");

    VM *vm = vm_create();

    /* Disable auto GC */
    gc_set_auto_collect(vm, false);

    /* Create an object but DON'T push it on stack */
    Object *a = new_pair(vm, NULL, NULL);
    assert(a != NULL);
    printf("Created object a: %p (not on stack)\n", (void*)a);

    /* Run mark phase */
    gc_mark_roots(vm);

    /* Verify object is NOT marked (unreachable) */
    assert(a->marked == false);
    printf("Object a remains unmarked\n");
    printf("(This is correct - object is unreachable)\n");

    gc_cleanup(vm);
    vm_destroy(vm);

    printf("PASS Test\n\n");
}

void test_multiple_roots() {
    printf("Test: Multiple Roots\n");
    printf("--------------------\n");

    VM *vm = vm_create();

    /* Disable auto GC */
    gc_set_auto_collect(vm, false);

    /* Create three objects */
    Object *a = new_pair(vm, NULL, NULL);
    Object *b = new_pair(vm, NULL, NULL);
    Object *c = new_pair(vm, NULL, NULL);

    assert(vm->num_objects == 3);
    printf("Created 3 objects\n");

    /* Push two of them onto stack */
    push(vm, VAL_OBJ(a));
    push(vm, VAL_OBJ(c));
    printf("Pushed a and c onto stack (b is not on stack)\n");

    /* Run mark phase */
    gc_mark_roots(vm);

    /* Verify a and c are marked, b is not */
    assert(a->marked == true);
    assert(b->marked == false);
    assert(c->marked == true);

    printf("Object a marked\n");
    printf("Object b unmarked\n");
    printf("Object c marked\n");

    gc_cleanup(vm);
    vm_destroy(vm);

    printf("PASS Test\n\n");
}

void test_stack_with_integers() {
    printf("Test: Stack with Mixed Integer/Object Values\n");
    printf("---------------------------------------------\n");

    VM *vm = vm_create();

    /* Disable auto GC */
    gc_set_auto_collect(vm, false);

    /* Create objects */
    Object *a = new_pair(vm, NULL, NULL);
    Object *b = new_pair(vm, NULL, NULL);

    /* Push mixed values */
    push(vm, VAL_INT(42));      /* Integer */
    push(vm, VAL_OBJ(a));       /* Object */
    push(vm, VAL_INT(100));     /* Integer */
    push(vm, VAL_OBJ(b));       /* Object */
    push(vm, VAL_INT(-5));      /* Integer */

    printf("Stack: [42(int), a(obj), 100(int), b(obj), -5(int)]\n");

    /* Run mark phase */
    gc_mark_roots(vm);

    /* Only objects should be marked */
    assert(a->marked == true);
    assert(b->marked == true);

    printf("Objects a and b correctly marked\n");
    printf("Integers correctly ignored\n");

    gc_cleanup(vm);
    vm_destroy(vm);

    printf("PASS Test\n\n");
}

int main() {
    printf("=================================\n");
    printf("  GC Root Discovery Tests\n");
    printf("=================================\n\n");

    test_basic_reachability();
    test_unmarked_object();
    test_multiple_roots();
    test_stack_with_integers();

    printf("=================================\n");
    printf("  All Tests PASSED\n");
    printf("=================================\n");

    return 0;
}
