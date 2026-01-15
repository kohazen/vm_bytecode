# Garbage Collector Documentation

## Quick Start

### Building
```bash
make gc
make gc-tests
```

### Running All Tests
```bash
bash run_all_gc_tests.sh
```

### Running Individual Tests
```bash
./tests/gc_test_basic
./tests/gc_test_reachability
./tests/gc_test_transitive
./tests/gc_test_sweep
./tests/gc_test_deep
./tests/gc_test_closure_stress
```

## API Reference

### Object Allocation
```c
Object* new_pair(VM *vm, Object *left, Object *right);
Object* new_function(VM *vm);
Object* new_closure(VM *vm, Object *fn, Object *env);
```

### GC Functions
```c
void gc_init(VM *vm);              // Initialize GC
void gc_collect(VM *vm);           // Run full GC cycle
void gc(VM *vm);                   // Manual GC trigger
void gc_cleanup(VM *vm);           // Free all objects
```

### Stack Operations
```c
void push(VM *vm, Value val);      // Push value
Value pop(VM *vm);                 // Pop value
```

### Value Macros
```c
VAL_OBJ(obj)  // Wrap object as Value
VAL_INT(val)  // Wrap integer as Value
```

## Test Coverage

| Test ID | Description | Status |
|---------|-------------|--------|
| 1.6.1 | Basic Reachability | ✓ PASS |
| 1.6.2 | Unreachable Collection | ✓ PASS |
| 1.6.3 | Transitive Reachability | ✓ PASS |
| 1.6.4 | Cyclic References | ✓ PASS |
| 1.6.5 | Deep Object Graph | ✓ PASS |
| 1.6.6 | Closure Capture | ✓ PASS |
| 1.6.7 | Stress Allocation | ✓ PASS |

All mandatory requirements implemented.

## Performance

- **Mark Phase:** O(R) where R = reachable objects
- **Sweep Phase:** O(N) where N = total objects
- **Memory Overhead:** 32 bytes per object
- **GC Trigger:** When num_objects >= max_objects
- **Threshold Update:** max_objects = num_objects * 2 (min 8)

## Implementation Team

- **Student A:** Heap allocator, root discovery, mark phase, sweep phase, closures
- **Student B:** Helpers, recursion, integration, testing, documentation

**Total Commits:** 8 (4 per student over 4 days)
