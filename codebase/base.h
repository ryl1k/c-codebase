#ifndef BASE_H
#define BASE_H

#include <stdint.h>
#include <string.h>

#if !defined(ENABLE_ASSERT)
#define ENABLE_ASSERT
#endif

//////////////////////////////////////////////////////
// NOTE(ryl1k): Helper Macros

#define Stmnt(S) do { S }while(0)

#if !defined(AssertBreak)
#define AssertBreak() (*(int*)0 = 0)
#endif

#if defined(ENABLE_ASSERT)
#define Assert(c) Stmnt(if(!(c)){ AssertBreak(); } )
#else
#define Assert(c)
#endif

#define Stringify_(S) #S
#define Stringify(S) Stringify_(S)
#define Glue_(A,B) A##B
#define Glue (A,B) Glue_(A,B)

#define ArrayCount(a) (sizeof(a)/sizeof(*(a)))

#define IntFromPtr(p) ((uintptr_t)((char*)(p) - (char*)0))
#define PtrFromInt(i) ((void*)((char*)0 + (uintptr_t)(i)))

#define Member(T,m) (((T*)0)->m)
#define OffsetOfMember(T,m)  IntFromPtr(&Member(T,m))

#define Min(a,b) (((a) < (b)) ? (a) : (b))
#define Max(a,b) (((a) > (b)) ? (a) : (b))
#define Clamp(min, val, max) (((val) < (min)) ? (min) :\
                             ((max) < (val)) ? (max) : (val))
#define ClampTop(val,max) Min(a,b)
#define ClampBot(val,max) Max(a,b)

#define global static
#define local static
#define function static

#define c_linkage_begin extern "C" {
#define c_linkage_end   }
#define c_linkage extern "C"

#define MemoryZero(p,z) memset((p),0,(z))
#define MemoryZeroStruct(p) MemoryZero((p),sizeof(*(p)))
#define MemoryZeroArray(p) MemoryZero((p),sizeof(p))
#define MemoryZeroTyped(p,c) MemoryZero((p),sizeof(*(p))*(c))

#define MemoryCompare(a,b,z) ((memcmp((a),(b),(z))) == 0)

#define MemoryCopy(d,s,z) memmove((d),(s),(z))
#define MemoryCopyStruct(d,s) MemoryCopy((d), (s),Min(sizeof(*(d)),(sizeof(*s))))
#define MemoryCopyArray(d,s) MemoryCopy((d),(s),Min(sizeof(s),sizeof(d)))
#define MemoryCopyTyped(d,s,c) MemoryCopy((d),(s),Min(sizeof(*(d)),sizeof(*(s)))*(c))


//////////////////////////////////////////////////////
// NOTE(ryl1k): Memory Macros

typedef struct {
    char* base;       
    size_t size;      
    size_t used;      
} Arena;

typedef struct {
    uint8_t* base;    
    uint64_t size;    
    uint64_t used;    
} Arena64;


function Arena arena_create(size_t size);
function Arena64 arena64_create(uint64_t size);

function void* arena_alloc(Arena* arena, size_t alloc_size);
function void* arena64_alloc(Arena64* arena, uint64_t alloc_size);

function void arena_reset(Arena* arena);
function void arena64_reset(Arena64* arena);

function void arena_destroy(Arena* arena);
function void arena64_destroy(Arena64* arena);

function void initialize_string_arena(size_t size);
function void destroy_string_arena();
function void initialize_utf_string_arena(size_t size);
function void destroy_utf_string_arena();

#define BASE_ALLOC(size) malloc(size)
#define BASE_FREE(ptr)   free(ptr)

#define ALIGN_PTR(ptr, alignment) \
    ((void *)(((uintptr_t)(ptr) + (alignment - 1)) & ~(alignment - 1)))

#define IS_ALIGNED(ptr, alignment) \
    (((uintptr_t)(ptr) & (alignment - 1)) == 0)

Arena g_string_arena;
Arena g_utf_string_arena;

//////////////////////////////////////////////////////
// NOTE(ryl1k): String And UTF Macros

typedef enum {
    Encoding_UTF8,
    Encoding_UTF16
} Encoding;
typedef struct {
    char* data;
    int length;
    Encoding encoding;
} String;



#define string(literal) string_create_literal(literal)

function String string_create_literal(const char* literal);

function char* utf8_to_utf16(const char* utf8);
function char* utf16_to_utf8(const char* utf16);
function int utf8_strlen(const char* utf8);
function int utf16_strlen(const char* utf16);

#define UTF8String(literal) utf8_create_literal(literal)
#define UTF16String(literal) utf16_create_literal(literal)

function String utf8_create_literal(const char* utf8);
function String utf16_create_literal(const char* utf8);

//////////////////////////////////////////////////////
// NOTE(ryl1k): Linked List Macros

#define DLL_NODE(TYPE)\
    struct{           \
        TYPE* prev;   \
        TYPE* next;   \
    }

#define DLL_INIT(node)       \
    do {                     \
        (node)->prev = NULL; \
        (node)->next = NULL; \
    } while (0)

#define DLL_INSERT_AFTER(node, new_node)     \
    do {                                     \
        (new_node)->next = (node)->next;     \
        (new_node)->prev = (node);           \
        if ((node)->next)                    \
            (node)->next->prev = (new_node); \
        (node)->next = (new_node);           \
    } while (0)

#define DLL_INSERT_BEFORE(node, new_node)    \
    do {                                     \
        (new_node)->next = (node)->next;     \
        (new_node)->prev = (node);           \
        if ((node)->next)                    \
            (node)->next->prev = (new_node); \
        (node)->next = (new_node);           \
    } while (0)

#define DLL_REMOVE(node)                        \
    do {                                        \
        if ((node)->prev)                       \
            (node)->prev->next = (node)->next;  \
        if ((node)->next)                       \
            (node)->next->prev = (node)->prev;  \
        (node)->prev = NULL;                    \
        (node)->next = NULL;                    \
    } while (0)

#define SLL_NODE(TYPE) TYPE* next
    
#define SLL_INIT(node)          \
    do{                         \
        (node)->next = NULL;    \
    } while (0)

#define SLL_INSERT(node, new_node)      \
    do{                                 \
        (new_node)->next = node->next;  \
        (node)->next = (new_node)       \
    } while (0)

#define SLL_REMOVE(node)                       \
    do{                                        \
        if ((node)->next != NULL) {            \
            (node)->next = (node)->next->next; \
        }                                      \
    } while (0)

#define QUEUE_INIT(queue)     \
    do {                      \
        (queue).front = NULL; \
        (queue).rear = NULL;  \
    } while (0)

#define QUEUE_ENQUEUE(queue,node)                        \
    do{                                                  \
        if ((queue).rear == NULL){                       \
            (queue).front = (node);                      \
            (queue).rear = (node);                       \
        }else{                                           \
            ((typeof(node))(queue).rear)->next = (node); \
            (queue).rear = (node);                       \
        }                                                \
    } while (0)

#define QUEUE_DEQUEUE(queue, node)                               \
    do {                                                         \
        (node) = (queue).front;                                  \
        if ((queue).front != NULL) {                             \
            (queue).front = ((typeof(node))(queue).front)->next; \
            if ((queue).front == NULL) {                         \
                (queue).rear = NULL;                             \
            }                                                    \
        }                                                        \
    } while (0)

#define QUEUE_PEEK(queue) ((queue).front)



#define STACK_NODE(TYPE)    \
    do{                     \
        struct{             \
            TYPE* next;     \
        };                  \
    } while (0)

#define STACK_INIT(stack) \
    do {                  \
        (stack) = NULL;   \
    } while (0)

#define STACK_PUSH(stack, node) \
    do{                         \
        (node)->next = (stack); \
        (stack) = (node);       \
    } while (0)                  

#define STACK_POP(stack,node)        \
    do{                              \
        (node) = (stack);            \
        if((stack) != NULL)          \
            (stack) = (stack)->next; \
    } while (0)

#define STACK_PEEK(stack) (stack)


//////////////////////////////////////////////////////
// NOTE(ryl1k): Basic Types

typedef int8_t S8;
typedef int16_t S16;
typedef int32_t S32;
typedef int64_t S64;
typedef uint8_t U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;
typedef S8 B8;
typedef S16 B16;
typedef S32 B32;
typedef S64 B64;
typedef float F32;
typedef double F64;

typedef void voidFunc(void);

//////////////////////////////////////////////////////
// NOTE(ryl1k): Basic Constants

global S8  min_S8  = (S8) 0x80;
global S16 min_S16 = (S16)0x8000;
global S32 min_S32 = (S32)0x80000000;
global S64 min_S64 = (S64)0x8000000000000000llu;

global S8  max_S8  = (S8) 0x7f;
global S16 max_S16 = (S16)0x7fff;
global S32 max_S32 = (S32)0x7fffffff;
global S64 max_S64 = (S64)0x7fffffffffffffffllu;

global U8  max_U8  = (U8) 0xff;
global U16 max_U16 = (U16)0xffff;
global U32 max_U32 = (U32)0xffffffff;
global U64 max_U64 = (U64)0xffffffffffffffffllu;

global F32 machine_epsilon_F32 = 1.1920929e-7f;
global F32 pi_F32 = 3.14159265359f;
global F32 tau_F32 = 6.28318530718f;
global F32 gold_big_F32 = 1.61803398875f;
global F32 gold_small_F32 = 0.61803398875f;
global F32 e_F32 = 2.71828182846f;


global F64 machine_epsilon_F64 = 2.220446e-16;
global F64 pi_F64 = 3.14159265359;
global F64 tau_F64 = 6.28318530718;
global F64 e_F64 = 2.71828182846;
global F64 gold_big_F64 = 1.61803398875;
global F64 gold_small_F64 = 0.61803398875;

//////////////////////////////////////////////////////
// NOTE(ryl1k): Symbolic Constants

typedef enum {
    Axis_X,
    Axis_Y,
    Axis_Z,
    Axis_W,
} Axis;

typedef enum {
    Side_Min,
    Side_Max,
} Side;

typedef enum {
    OperatingSystem_Null,
    OperatingSystem_Windows,
    OperatingSystem_Linux,
    OperatingSystem_Mac,
    OperatingSystem_COUNT,
} OperatingSystem;

typedef enum {
    Architecture_Null,
    Architecture_x64,
    Architecture_x86,
    Architecture_ARM,
    Architecture_ARM64,
    Architecture_COUNT,
} Architecture;

typedef enum {
    Month_Jan,
    Month_Feb,
    Month_Mar,
    Month_Apr,
    Month_May,
    Month_Jun,
    Month_Jul,
    Month_Aug,
    Month_Sep,
    Month_Oct,
    Month_Nov,
    Month_Dec,
} Month;

typedef enum {
    DayOfWeek_Monday,
    DayOfWeek_Tuesday,
    DayOfWeek_Wednesday,
    DayOfWeek_Thursday,
    DayOfWeek_Friday,
    DayOfWeek_Saturday,
    DayOfWeek_Sunday,
}DayOfWeek;


//////////////////////////////////////////////////////
// NOTE(ryl1k): Compound Types

typedef union {
    struct {
        S32 x;
        S32 y;
    };
    S32 v[2];
} V2S32;


typedef union {
    struct {
        F32 x;
        F32 y;
    };
    F32 v[2];
} V2F32;

typedef union {
    struct {
        F32 x;
        F32 y;
        F32 z;
    };
    F32 v[3];
} V3F32;

typedef union {
    struct {
        F32 x;
        F32 y;
        F32 z;
        F32 w;
    };
    F32 v[4];
} V4F32;


typedef union{
    struct {
        F32 min;
        F32 max;
    };
    F32 v[2];
} I1F32;

typedef union{
    struct {
        U64 min;
        U64 max;
    };
    struct {
        U64 first;
        U64 opl;
    };
    U64 v[2];
} I1U64;

typedef union{
    struct {
        V2S32 min;
        V2S32 max;
    };
    struct {
        V2S32 p0;
        V2S32 p1;
    };
    struct {
        S32 x0;
        S32 y0;
        S32 x1;
        S32 y1;
    };
    V2S32 p[2];
    S32 v[4];
} I2S32;

typedef union{
    struct {
        V2F32 min;
        V2F32 max;
    };
    struct {
        V2F32 p0;
        V2F32 p1;
    };
    struct {
        F32 x0;
        F32 y0;
        F32 x1;
        F32 y1;
    };
    V2F32 p[2];
    F32 v[4];
} I2F32;

//////////////////////////////////////////////////////
// NOTE(ryl1k): Symbolic Constants Functions

function OperatingSystem get_operating_system(void);
function Architecture get_architecture(void);
function Month get_current_month(void);
function DayOfWeek get_current_day_of_week(void);

//////////////////////////////////////////////////////
// NOTE(ryl1k): Float Constant Functions

function F32 inf_F32(void);
function F64 inf_F64(void);
function F32 neg_inf_F32(void);
function F64 neg_inf_F64(void);

//////////////////////////////////////////////////////
// NOTE(ryl1k): Math Functions

function F32 abs_F32(F32 x);
function F32 sqrt_F32(F32 x);
function F32 sin_F32(F32 x);
function F32 cos_F32(F32 x);
function F32 tan_F32(F32 x);
function F32 ln_F32(F32 x);


function F64 abs_F64(F64 x);
function F64 sqrt_F64(F64 x);
function F64 sin_F64(F64 x);
function F64 cos_F64(F64 x);
function F64 tan_F64(F64 x);
function F64 ln_F64(F64 x);



function F32 lerp_F32(F32 a, F32 b, F32 t);
function F32 unlerp_F32(F32 a, F32 b, F32 x);

//////////////////////////////////////////////////////
// NOTE(ryl1k): Compound Type Functions

function V2S32 v2s32(S32 x, S32 y);

function V2F32 v2f32(F32 x, F32 y);
function V3F32 v3f32(F32 x, F32 y, F32 z);
function V4F32 v4f32(F32 x, F32 y, F32 z, F32 w);

function I1F32 i1f32(F32 min, F32 max);
function I1U64 i1u64(U64 min, U64 max);

function I2S32 i2s32(S32 x0, S32 y0, S32 x1, S32 y1);
function I2S32 i2s32_vec(V2S32 min ,V2S32 max);

function I2F32 i2f32(F32 x0, F32 y0, F32 x1, F32 y1);
function I2F32 i2f32_vec(V2F32 min, V2F32 max);
function I2F32 i2f32_range(I1F32 x, I1F32 y);

function V2S32 add_V2S32(const V2S32 *a, const V2S32 *b);
function V2F32 add_V2F32(const V2F32 *a, const V2F32 *b);
function V3F32 add_V3F32(const V3F32 *a, const V3F32 *b);
function V4F32 add_V4F32(const V4F32 *a, const V4F32 *b);

function V2S32 subtract_V2S32(const V2S32 *a, const V2S32 *b);
function V2F32 subtract_V2F32(const V2F32 *a, const V2F32 *b);
function V3F32 subtract_V3F32(const V3F32 *a, const V3F32 *b);
function V4F32 subtract_V4F32(const V4F32 *a, const V4F32 *b);

function V2S32 multiply_V2S32(const V2S32 *v, const S32 *s);
function V2F32 multiply_V2F32(const V2F32 *v, const F32 *s);
function V3F32 multiply_V3F32(const V3F32 *v, const F32 *s);
function V4F32 multiply_V4F32(const V4F32 *v, const F32 *s);

function V2F32 V2F32_hadamard(V2F32 a, V2F32 b);
function V3F32 V3F32_hadamard(V3F32 a, V3F32 b);
function V4F32 V4F32_hadamard(V4F32 a, V4F32 b);

function V2F32 V2F32_dot(V2F32 a, V2F32 b);
function V3F32 V3F32_dot(V3F32 a, V3F32 b);
function V4F32 V4F32_dot(V4F32 a, V4F32 b);

function B32 I1F32_overlaps(I1F32 a, I1F32 b);
function B32 I1F32_contains(I1F32 r, F32 x);

function B32 I2F32_overlaps(I2F32 a, I2F32 b);
function B32 I2F32_contains(I2F32 r, V2F32 x);

function B32 I2S32_overlaps(I2S32 a, I2S32 b);
function B32 I2S32_contains(I2S32 r, V2S32 x);

function F32 I1F32_dim(I1F32 r);
function U64 I1U64_dim(I1U64 r);
function V2F32 I2F32_dim(I2F32 r);
function V2S32 I2S32_dim(I2S32 r);

function F32 I1F32_center(I1F32 r);
function V2F32 I2F32_center(I2F32 r);

function I1F32 I1F32_axis(I2F32 r, Axis axis);

#endif // BASE_H


