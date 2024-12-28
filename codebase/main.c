#include "base.h"
#include "base.c"
#include <float.h>
#include <stdio.h>
#include <string.h>

void test_math_operations_edge_cases() {
    // Test add_V2F32
    V2F32 a = v2f32(0.0f, 0.0f);
    V2F32 b = v2f32(-FLT_MAX, FLT_MAX);
    V2F32 result = add_V2F32(&a, &b);
    printf("add_V2F32 (edge case): (%.2f, %.2f)\n", result.x, result.y);

    a = v2f32(-FLT_MAX, -FLT_MAX);
    b = v2f32(FLT_MAX, FLT_MAX);
    result = add_V2F32(&a, &b);
    printf("add_V2F32 (extremes): (%.2f, %.2f)\n", result.x, result.y);

    // Test subtract_V2F32
    a = v2f32(FLT_MAX, FLT_MIN);
    b = v2f32(-FLT_MAX, FLT_MAX);
    result = subtract_V2F32(&a, &b);
    printf("subtract_V2F32 (edge case): (%.2f, %.2f)\n", result.x, result.y);

    // Test multiply_V2F32
    F32 scalar = FLT_MAX;
    a = v2f32(1.0f, -1.0f);
    result = multiply_V2F32(&a, &scalar);
    printf("multiply_V2F32 (large scalar): (%.2f, %.2f)\n", result.x, result.y);

    scalar = 0.0f;
    result = multiply_V2F32(&a, &scalar);
    printf("multiply_V2F32 (zero scalar): (%.2f, %.2f)\n", result.x, result.y);

    scalar = -1.0f;
    result = multiply_V2F32(&a, &scalar);
    printf("multiply_V2F32 (negative scalar): (%.2f, %.2f)\n", result.x, result.y);
}

void test_compound_types_edge_cases() {
    // Test I1F32_contains
    I1F32 interval = i1f32(0.0f, 10.0f);
    F32 test_value = 10.0f; // Boundary case
    B32 contains = I1F32_contains(interval, test_value);
    printf("I1F32_contains (boundary): %d\n", contains);

    interval = i1f32(-FLT_MAX, FLT_MAX);
    test_value = 0.0f;
    contains = I1F32_contains(interval, test_value);
    printf("I1F32_contains (full range): %d\n", contains);

    // Test I2F32_contains
    I2F32 box = i2f32(0.0f, 0.0f, 10.0f, 10.0f);
    V2F32 point = v2f32(10.0f, 10.0f); // Boundary case
    contains = I2F32_contains(box, point);
    printf("I2F32_contains (boundary): %d\n", contains);

    box = i2f32(-FLT_MAX, -FLT_MAX, FLT_MAX, FLT_MAX);
    point = v2f32(0.0f, 0.0f);
    contains = I2F32_contains(box, point);
    printf("I2F32_contains (full range): %d\n", contains);

    // Test I2F32_overlaps
    I2F32 other_box = i2f32(5.0f, 5.0f, 15.0f, 15.0f);
    B32 overlaps = I2F32_overlaps(box, other_box);
    printf("I2F32_overlaps (overlapping): %d\n", overlaps);

    other_box = i2f32(20.0f, 20.0f, 30.0f, 30.0f);
    overlaps = I2F32_overlaps(box, other_box);
    printf("I2F32_overlaps (non-overlapping): %d\n", overlaps);
}

void test_utility_functions_edge_cases() {
    // Test I1F32_dim and I1F32_center
    I1F32 interval = i1f32(0.0f, 0.0f); // Zero-size interval
    F32 dim = I1F32_dim(interval);
    printf("I1F32_dim (zero-size): %.2f\n", dim);

    F32 center = I1F32_center(interval);
    printf("I1F32_center (zero-size): %.2f\n", center);

    interval = i1f32(-FLT_MAX, FLT_MAX);
    dim = I1F32_dim(interval);
    printf("I1F32_dim (full range): %.2f\n", dim);

    center = I1F32_center(interval);
    printf("I1F32_center (full range): %.2f\n", center);

    // Test I2F32_center
    I2F32 box = i2f32(0.0f, 0.0f, 0.0f, 0.0f); // Zero-size box
    V2F32 center_box = I2F32_center(box);
    printf("I2F32_center (zero-size): (%.2f, %.2f)\n", center_box.x, center_box.y);

    box = i2f32(-FLT_MAX, -FLT_MAX, FLT_MAX, FLT_MAX);
    center_box = I2F32_center(box);
    printf("I2F32_center (full range): (%.2f, %.2f)\n", center_box.x, center_box.y);
}

void test_memory_operations_edge_cases() {
    struct TestStruct {
        int id;
        char name[20];
        double value;
    };

    struct TestStruct test1 = { 42, "TestName", 3.14 };
    struct TestStruct test2;

    // Test MemoryZeroStruct with uninitialized memory
    MemoryZeroStruct(&test2);
    printf("After MemoryZero, id: %d, name: '%s', value: %.2f\n", test2.id, test2.name, test2.value);

    // Test MemoryCopyStruct with null pointers
    MemoryCopyStruct(&test2, &test1);
    printf("After MemoryCopy, id: %d, name: '%s', value: %.2f\n", test2.id, test2.name, test2.value);
}

void test_macros_edge_cases() {
    // Test Clamp, Min, Max
    F32 value = 5.0f;
    F32 clamped = Clamp(0.0f, value, 10.0f);
    printf("Clamp (within range): %.2f\n", clamped);

    clamped = Clamp(0.0f, -5.0f, 10.0f);
    printf("Clamp (below range): %.2f\n", clamped);

    clamped = Clamp(0.0f, 15.0f, 10.0f);
    printf("Clamp (above range): %.2f\n", clamped);

    F32 minimum = Min(FLT_MAX, FLT_MIN);
    printf("Min (extremes): %.2f\n", minimum);

    F32 maximum = Max(FLT_MAX, FLT_MIN);
    printf("Max (extremes): %.2f\n", maximum);
}

void test_string_functions_edge_cases() {
    // UTF-8 String
    String utf8_str = string("Hello, ії!");
    printf("UTF-8 String: %s, Length: %d\n", utf8_str.data, utf8_str.length);

    // UTF-16 String
    wchar_t utf16_str[] = L"Hello, ії!";
    char* converted_utf8 = utf16_to_utf8(utf16_str);
    printf("UTF-16 String as UTF-16: %ls\n", utf16_str);
    printf("UTF-16 String Length: %d\n", utf16_strlen(utf16_str));

    free(converted_utf8); // Free the converted string
}



void test_linked_list_macros_edge_cases() {
    typedef struct Node {
        DLL_NODE(struct Node);
        int value;
    } Node;

    Node node1, node2, node3;
    DLL_INIT(&node1);
    DLL_INIT(&node2);
    DLL_INIT(&node3);

    node1.value = 1;
    node2.value = 2;
    node3.value = 3;

    DLL_INSERT_AFTER(&node1, &node2);
    DLL_INSERT_AFTER(&node2, &node3);

    printf("DLL_INSERT_AFTER: node1.next->value: %d\n", node1.next->value);
    printf("DLL_INSERT_AFTER: node2.next->value: %d\n", node2.next->value);

    DLL_REMOVE(&node2);
    printf("DLL_REMOVE: node1.next->value: %d\n", node1.next->value);
}

void test_stack_and_queue_macros_edge_cases() {
    typedef struct Node {
        SLL_NODE(struct Node);
        int value;
    } Node;

    Node stack_node1, stack_node2;
    stack_node1.value = 1;
    stack_node2.value = 2;

    Node* stack = NULL;
    STACK_PUSH(stack, &stack_node1);
    STACK_PUSH(stack, &stack_node2);

    if (stack) {
        printf("STACK_PUSH: stack->value: %d\n", stack->value);
    }

    Node* popped = NULL;
    STACK_POP(stack, popped);
    if (popped) {
        printf("STACK_POP: popped->value: %d\n", popped->value);
    }
}


void test_arena_operations_edge_cases() {
    Arena arena = arena_create(128);
    void* ptr1 = arena_alloc(&arena, 64);
    printf("arena_alloc (64 bytes): %p\n", ptr1);

    void* ptr2 = arena_alloc(&arena, 64);
    printf("arena_alloc (another 64 bytes): %p\n", ptr2);

    void* ptr3 = arena_alloc(&arena, 1);
    printf("arena_alloc (exceeding size): %p\n", ptr3);

    arena_reset(&arena);
    printf("arena_reset: used: %zu\n", arena.used);

    arena_destroy(&arena);
    printf("arena_destroy: base: %p\n", arena.base);
}

void test_symbolic_constants_functions_edge_cases() {
    OperatingSystem os = get_operating_system();
    printf("get_operating_system: %d\n", os);

    Architecture arch = get_architecture();
    printf("get_architecture: %d\n", arch);

    Month month = get_current_month();
    printf("get_current_month: %d\n", month);

    DayOfWeek day = get_current_day_of_week();
    printf("get_current_day_of_week: %d\n", day);
}

void test_miscellaneous_edge_cases() {
    F32 aligned_value = 1.0f;
    void* aligned_ptr = ALIGN_PTR(&aligned_value, 16);
    printf("ALIGN_PTR: %p\n", aligned_ptr);

    B32 is_aligned = IS_ALIGNED(aligned_ptr, 16);
    printf("IS_ALIGNED: %d\n", is_aligned);

    F32 inf = inf_F32();
    F32 neg_inf = neg_inf_F32();
    printf("inf_F32: %f, neg_inf_F32: %f\n", inf, neg_inf);
}

int main() {

    printf("--- Testing Math Operations Edge Cases ---\n");
    test_math_operations_edge_cases();

    printf("\n--- Testing Compound Types Edge Cases ---\n");
    test_compound_types_edge_cases();

    printf("\n--- Testing Utility Functions Edge Cases ---\n");
    test_utility_functions_edge_cases();

    printf("\n--- Testing Memory Operations Edge Cases ---\n");
    test_memory_operations_edge_cases();

    printf("\n--- Testing Macros Edge Cases ---\n");
    test_macros_edge_cases();

    printf("\n--- Testing String Functions Edge Cases ---\n");
    test_string_functions_edge_cases();

    printf("\n--- Testing Linked List Macros Edge Cases ---\n");
    test_linked_list_macros_edge_cases();

    printf("\n--- Testing Stack and Queue Macros Edge Cases ---\n");
    test_stack_and_queue_macros_edge_cases();

    printf("\n--- Testing Arena Operations Edge Cases ---\n");
    test_arena_operations_edge_cases();

    printf("\n--- Testing Symbolic Constants Functions Edge Cases ---\n");
    test_symbolic_constants_functions_edge_cases();

    printf("\n--- Testing Miscellaneous Edge Cases ---\n");
    test_miscellaneous_edge_cases();

    printf("\nAll edge case tests completed!\n");
    return 0;
}
