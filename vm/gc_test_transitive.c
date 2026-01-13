/*
 * Test 1.6.3: Transitive Reachability
 * Test 1.6.4: Cyclic References
 *
 * Purpose: Verify recursive marking of referenced objects and handling of cycles
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "vm.h"  /* Includes gc.h automatically */

void test_transitive_reachability() {
    printf("Test 1.6.3: Transitive Reachability\n");
    printf("------------------------------------\n");

    VM *vm = vm_create();

    /* Disable auto GC */
    gc_set_auto_collect(vm, false);

    /* Create objects */
    Object *a = new_pair(vm, NULL, NULL);
    Object *b = new_pair(vm, a, NULL);

    assert(a != NULL);
    assert(b != NULL);
    assert(vm->num_objects == 2);

    printf("Created chain: b -> a -> NULL\n");

    /* Only push b onto stack (a is transitively reachable through b) */
    push(vm, VAL_OBJ(b));

    /* Verify initial state */
    assert(a->marked == false);
    assert(b->marked == false);
    printf("Before marking: both unmarked\n");

    /* Run mark phase */
    gc_mark_roots(vm);

    /* Both a and b should be marked */
    assert(b->marked == true);
    assert(a->marked == true);

    printf("After marking:\n");
    printf("  b marked (directly on stack)\n");
    printf("  a marked (transitively reachable through b)\n");

    gc_cleanup(vm);
    vm_destroy(vm);

    printf("PASS Test 1.6.3\n\n");
}

void test_cyclic_references() {
    printf("Test 1.6.4: Cyclic References\n");
    printf("------------------------------\n");

    VM *vm = vm_create();

    /* Disable auto GC */
    gc_set_auto_collect(vm, false);

    /* Create cycle */
    Object *a = new_pair(vm, NULL, NULL);
    Object *b = new_pair(vm, a, NULL);
    a->pair.right = b;  /* Complete the cycle: a <-> b */

    printf("Created cycle: a <-> b\n");

    /* Push a onto stack */
    push(vm, VAL_OBJ(a));

    /* Verify initial state */
    assert(a->marked == false);
    assert(b->marked == false);
    printf("Before marking: both unmarked\n");

    /* Run mark phase - should handle cycle without infinite loop */
    gc_mark_roots(vm);

    /* Both should be marked */
    assert(a->marked == true);
    assert(b->marked == true);

    printf("After marking:\n");
    printf("  a marked\n");
    printf("  b marked\n");
    printf("  No infinite loop\n");

    gc_cleanup(vm);
    vm_destroy(vm);

    printf("PASS Test 1.6.4\n\n");
}

void test_deep_chain() {
    printf("Test: Deep Object Chain\n");
    printf("-----------------------\n");

    VM *vm = vm_create();

    /* Disable auto GC */
    gc_set_auto_collect(vm, false);

    /* Create a chain: a -> b -> c -> d -> e */
    Object *e = new_pair(vm, NULL, NULL);
    Object *d = new_pair(vm, e, NULL);
    Object *c = new_pair(vm, d, NULL);
    Object *b = new_pair(vm, c, NULL);
    Object *a = new_pair(vm, b, NULL);

    assert(vm->num_objects == 5);
    printf("Created chain of 5 objects: a->b->c->d->e\n");

    /* Only push a onto stack */
    push(vm, VAL_OBJ(a));

    /* Run mark phase */
    gc_mark_roots(vm);

    /* All should be marked */
    assert(a->marked == true);
    assert(b->marked == true);
    assert(c->marked == true);
    assert(d->marked == true);
    assert(e->marked == true);

    printf("All 5 objects marked transitively\n");

    gc_cleanup(vm);
    vm_destroy(vm);

    printf("PASS Test\n\n");
}

void test_tree_structure() {
    printf("Test: Tree Structure\n");
    printf("--------------------\n");

    VM *vm = vm_create();

    /* Disable auto GC */
    gc_set_auto_collect(vm, false);

    /* Create a tree */
    Object *c = new_pair(vm, NULL, NULL);
    Object *d = new_pair(vm, NULL, NULL);
    Object *e = new_pair(vm, NULL, NULL);
    Object *f = new_pair(vm, NULL, NULL);
    Object *a = new_pair(vm, c, d);
    Object *b = new_pair(vm, e, f);
    Object *root = new_pair(vm, a, b);

    assert(vm->num_objects == 7);
    printf("Created tree with 7 nodes\n");

    /* Push only root */
    push(vm, VAL_OBJ(root));

    /* Run mark phase */
    gc_mark_roots(vm);

    /* All should be marked */
    assert(root->marked == true);
    assert(a->marked == true);
    assert(b->marked == true);
    assert(c->marked == true);
    assert(d->marked == true);
    assert(e->marked == true);
    assert(f->marked == true);

    printf("All 7 nodes marked transitively\n");

    gc_cleanup(vm);
    vm_destroy(vm);

    printf("PASS Test\n\n");
}

void test_partial_reachability() {
    printf("Test: Partial Reachability\n");
    printf("--------------------------\n");

    VM *vm = vm_create();

    /* Disable auto GC */
    gc_set_auto_collect(vm, false);

    /* Create two chains */
    Object *a1 = new_pair(vm, NULL, NULL);
    Object *a2 = new_pair(vm, a1, NULL);
    Object *a3 = new_pair(vm, a2, NULL);

    Object *b1 = new_pair(vm, NULL, NULL);
    Object *b2 = new_pair(vm, b1, NULL);
    Object *b3 = new_pair(vm, b2, NULL);

    printf("Created two chains:\n");
    printf("  Chain A: a3->a2->a1\n");
    printf("  Chain B: b3->b2->b1\n");

    /* Only push a3 (chain A is reachable, chain B is not) */
    push(vm, VAL_OBJ(a3));

    /* Run mark phase */
    gc_mark_roots(vm);

    /* Chain A should be marked */
    assert(a3->marked == true);
    assert(a2->marked == true);
    assert(a1->marked == true);

    /* Chain B should NOT be marked */
    assert(b3->marked == false);
    assert(b2->marked == false);
    assert(b1->marked == false);

    printf("Chain A marked\n");
    printf("Chain B unmarked\n");

    gc_cleanup(vm);
    vm_destroy(vm);

    printf("PASS Test\n\n");
}

int main() {
    printf("===========================================\n");
    printf("  GC Transitive Reachability Tests\n");
    printf("===========================================\n\n");

    test_transitive_reachability();
    test_cyclic_references();
    test_deep_chain();
    test_tree_structure();
    test_partial_reachability();

    printf("===========================================\n");
    printf("  All Tests PASSED\n");
    printf("===========================================\n");

    return 0;
}
