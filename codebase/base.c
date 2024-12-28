#include "base.h"
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>
#include <float.h>
#include <locale.h>
#include <wchar.h>
#include <stdbool.h>


//////////////////////////////////////////////////////
// NOTE(ryl1k): Symbolic Constants Functions

function OperatingSystem get_operating_system(void) {
#if defined(_WIN32)
    return OperatingSystem_Windows;
#elif defined(__linux__)
    return OperatingSystem_Linux;
#elif defined(__APPLE__) && defined(__MACH__)
    return OperatingSystem_Mac;
#else
    return OperatingSystem_Null;
#endif
}

function Architecture get_architecture(void) {
#if defined(_M_AMD64) || defined(__amd64__)
    return Architecture_x64;
#elif defined(_M_I86) || defined(__i386__)
    return Architecture_x86;
#elif defined(_M_ARM64) || defined(__aarch64__)
    return Architecture_ARM64;
#elif defined(_M_ARM) || defined(__arm__)
    return Architecture_ARM;
#else
    return Architecture_Null;
#endif
}

function Month get_current_month(void) {
    time_t now = time(NULL);
    struct tm* local_time = localtime(&now);
    switch (local_time->tm_mon) {
    case 0: return Month_Jan;
    case 1: return Month_Feb;
    case 2: return Month_Mar;
    case 3: return Month_Apr;
    case 4: return Month_May;
    case 5: return Month_Jun;
    case 6: return Month_Jul;
    case 7: return Month_Aug;
    case 8: return Month_Sep;
    case 9: return Month_Oct;
    case 10: return Month_Nov;
    case 11: return Month_Dec;
    default: return Month_Jan; 
    }
}

function DayOfWeek get_current_day_of_week(void) {
    time_t now = time(NULL);
    struct tm* local_time = localtime(&now);
    switch (local_time->tm_wday) {
    case 0: return DayOfWeek_Sunday;
    case 1: return DayOfWeek_Monday;
    case 2: return DayOfWeek_Tuesday;
    case 3: return DayOfWeek_Wednesday;
    case 4: return DayOfWeek_Thursday;
    case 5: return DayOfWeek_Friday;
    case 6: return DayOfWeek_Saturday;
    default: return DayOfWeek_Monday; 
    }
}

//////////////////////////////////////////////////////
// NOTE(ryl1k): Memory Functions


function Arena arena_create(size_t size) {
    Arena arena;
    arena.base = malloc(size); 
    if (!arena.base) {
        fprintf(stderr, "Failed to allocate memory for the arena\n");
        exit(EXIT_FAILURE);
    }
    arena.size = size;
    arena.used = 0;
    return arena;
}

function Arena64 arena64_create(uint64_t size) {
    Arena64 arena;
    arena.base = malloc(size); 
    if (!arena.base) {
        fprintf(stderr, "Failed to allocate memory for the 64-bit arena\n");
        exit(EXIT_FAILURE);
    }
    arena.size = size;
    arena.used = 0;
    return arena;
}

function void* arena_alloc(Arena* arena, size_t alloc_size) {
    if (arena->used + alloc_size > arena->size) {
        return NULL; 
    }
    void* result = arena->base + arena->used;
    arena->used += alloc_size;
    return result;
}

function void* arena64_alloc(Arena64* arena, uint64_t alloc_size) {
    if (arena->used + alloc_size > arena->size) {
        return NULL; 
    }
    void* result = arena->base + arena->used;
    arena->used += alloc_size;
    return result;
}

function void arena_reset(Arena* arena) {
    arena->used = 0;
}

function void arena64_reset(Arena64* arena) {
    arena->used = 0;
}

function void arena_destroy(Arena* arena) {
    if (arena->base) {
        free(arena->base);
        arena->base = NULL;
        arena->size = 0;
        arena->used = 0;
    }
}

function void arena64_destroy(Arena64* arena) {
    free(arena->base);
    arena->base = NULL;
    arena->size = 0;
    arena->used = 0;
}

function void initialize_string_arena(size_t size) {
    g_string_arena = arena_create(size);
}

function void initialize_utf_string_arena(size_t size) {
    g_utf_string_arena = arena_create(size);
}

function void destroy_utf_string_arena() {
    arena_destroy(&g_utf_string_arena);
}

function void destroy_string_arena() {
    arena_destroy(&g_string_arena);
}

//////////////////////////////////////////////////////
// NOTE(ryl1k): String And UTF Functions

function char* utf8_to_utf16(const char* utf8) {
    size_t wlen = 0;

    if (mbstowcs_s(&wlen, NULL, 0, utf8, _TRUNCATE) != 0) {
        fprintf(stderr, "Failed to calculate UTF-16 string length\n");
        exit(EXIT_FAILURE);
    }

    wchar_t* utf16 = malloc(wlen * sizeof(wchar_t));
    if (!utf16) {
        fprintf(stderr, "Memory allocation failed for UTF-16 conversion\n");
        exit(EXIT_FAILURE);
    }

    if (mbstowcs_s(NULL, utf16, wlen, utf8, _TRUNCATE) != 0) {
        fprintf(stderr, "UTF-8 to UTF-16 conversion failed\n");
        free(utf16);
        exit(EXIT_FAILURE);
    }

    return (char*)utf16;
}

function char* utf16_to_utf8(const wchar_t* utf16) {

    setlocale(LC_ALL, "en_US.UTF-8");

    size_t slen = 0;
    errno_t err = wcstombs_s(&slen, NULL, 0, utf16, _TRUNCATE);
    if (err != 0) {
        fprintf(stderr, "Failed to calculate UTF-8 string length: %d\n", err);
        exit(EXIT_FAILURE);
    }

    char* utf8 = malloc(slen);
    if (!utf8) {
        fprintf(stderr, "Memory allocation failed for UTF-8 string\n");
        exit(EXIT_FAILURE);
    }

    err = wcstombs_s(NULL, utf8, slen, utf16, _TRUNCATE);
    if (err != 0) {
        fprintf(stderr, "UTF-16 to UTF-8 conversion failed: %d\n", err);
        free(utf8);
        exit(EXIT_FAILURE);
    }

    return utf8;
}

function int utf8_strlen(const char* utf8) {
    int count = 0;
    while (*utf8) {
        count += (*utf8 & 0xC0) != 0x80;
        utf8++;
    }
    return count;
}

function int utf16_strlen(const wchar_t* utf16) {
    return wcslen(utf16); 
}

function String utf8_create_literal(const char* utf8) {
    if (g_utf_string_arena.size == 0) {
        initialize_utf_string_arena(1024 * 1024);
    }
    int length = strlen(utf8);
    char* allocated_memory = arena_alloc(&g_utf_string_arena, length + 1);
    if (!allocated_memory) {
        fprintf(stderr, "Failed to allocate memory for UTF-8 string\n");
        exit(EXIT_FAILURE);
    }

    
    if (strcpy_s(allocated_memory, length + 1, utf8) != 0) {
        fprintf(stderr, "Failed to copy UTF-8 string\n");
        exit(EXIT_FAILURE);
    }

    String result = { allocated_memory, length };
    return result;
}


function String utf16_create_literal(const char* utf8) {
    char* utf16 = utf8_to_utf16(utf8); 
    if (!utf16) {
        fprintf(stderr, "UTF-8 to UTF-16 conversion failed\n");
        exit(EXIT_FAILURE);
    }

    if (g_utf_string_arena.size == 0) {
        initialize_utf_string_arena(1024 * 1024);
    }

    int length = utf16_strlen(utf16);
    char* allocated_memory = arena_alloc(&g_utf_string_arena, (length + 1) * sizeof(wchar_t));
    if (!allocated_memory) {
        fprintf(stderr, "Failed to allocate memory for UTF-16 string\n");
        free(utf16); 
        exit(EXIT_FAILURE);
    }

    memcpy(allocated_memory, utf16, (length + 1) * sizeof(wchar_t));
    free(utf16); 

    String result = { allocated_memory, length };
    return result;
}



function String string_create_literal(const char* literal) {
    int length = strlen(literal);

    
    if (g_string_arena.size == 0) {
        initialize_string_arena(1024 * 1024); 
    }

    char* allocated_memory = arena_alloc(&g_string_arena, length + 1);
    if (!allocated_memory) {
        fprintf(stderr, "Failed to allocate memory for string\n");
        exit(EXIT_FAILURE);
    }

    strcpy_s(allocated_memory, length + 1, literal);

    String result;
    result.data = allocated_memory;
    result.length = length;
    result.encoding = Encoding_UTF8;
    return result;
}

function String string_create(Arena* arena, const char* literal) {
    int length = strlen(literal);
    char* allocated_memory = arena_alloc(arena, length + 1); 

    if (!allocated_memory) {
        fprintf(stderr, "Failed to allocate memory for string\n");
        exit(EXIT_FAILURE);
    }

    strcpy_s(allocated_memory, length+1, literal); 

    String result;
    result.data = allocated_memory;
    result.length = length;
    return result;
}

//////////////////////////////////////////////////////
// NOTE(ryl1k): Hashtable Functions


//////////////////////////////////////////////////////
// NOTE(ryl1k): Float Constant Functions

function F32 inf_F32(void) {
    union { F32 f; U32 u; } r;
    r.u = 0x7f800000;
    return(r.f);
}

function F32 neg_inf_F32(void) {
    union { F32 f; U32 u; } r;
    r.u = 0xff800000;
    return(r.f);
}

function F64 inf_F64(void) {
    union { F64 f; U64 u; } r;
    r.u = 0x7ff0000000000000;
    return(r.f);
}

function F64 neg_inf_F64(void) {
    union { F64 f; U64 u; } r;
    r.u = 0xfff0000000000000;
    return(r.f);
}

//////////////////////////////////////////////////////
// NOTE(ryl1k): Math Functions

function F32 abs_F32(F32 x) {
    union { F32 f; U32 u; } r;
    r.f = x;
    r.u &= 0x7fffffff;
    return r.f;
}

function F64 abs_F64(F64 x) {
    union{F64 f; U64 u;} r;
    r.f = x;
    r.u &= 0x7fffffff;
    return r.f;
}

#include <math.h>

function F32 sqrt_F32(F32 x) {
    return(sqrtf(x));
}

function F32 sin_F32(F32 x) {
    return(sinf(x));
}

function F32 cos_F32(F32 x) {
    return(cosf(x));
}

function F32 tan_F32(F32 x) {
    return(tanf(x));
}

function F32 ln_F32(F32 x) {
    return(logf(x));
}

function F32 lerp_F32(F32 a, F32 b, F32 t) {
    return(a + t * (b - a));
}
function F32 unlerp_F32(F32 a, F32 b, F32 x) {
    return((x - a) / (b - a));
}




function F64 sqrt_F64(F64 x) {
    return(sqrtf(x));
}

function F64 sin_F64(F64 x) {
    return(sinf(x));
}

function F64 cos_F64(F64 x) {
    return(cosf(x));
}

function F64 tan_F64(F64 x) {
    return(tanf(x));
}

function F64 ln_F64(F64 x) {
    return(logf(x));
}

//////////////////////////////////////////////////////
// NOTE(ryl1k): Compound Type Functions

function V2S32 v2s32(S32 x, S32 y) {
    V2S32 r = { x,y };
    return (r);
}

function V2F32 v2f32(F32 x, F32 y) {
    V2F32 r = { x,y };
    return (r);
}
function V3F32 v3f32(F32 x, F32 y, F32 z) {
    V3F32 r = { x,y,z };
    return (r);
}
function V4F32 v4f32(F32 x, F32 y, F32 z, F32 w) {
    V4F32 r = { x,y,z,w };
    return (r);
}

function I1F32 i1f32(F32 min, F32 max) {
    I1F32 r = { min, max };
    if (max < min) {
        r.min = max;
        r.max = min;
    }
    return(r);
}
function I1U64 i1u64(U64 min, U64 max) {
    I1U64 r = {min, max};
    if (max < min) {
        r.min = max;
        r.max = min;
    }
    return(r);
}

function I2S32 i2s32(S32 x0, S32 y0, S32 x1, S32 y1) {
    I2S32 r = { x0,y0,x1,y1 };
    if (x1 < x0) {
        r.x0 = x1;
        r.x1 = x0;
    }
    if (y1 < y0) {
        r.y1 = y0;
        r.y0 = y1;
    }
    return(r);
}
function I2S32 i2s32_vec(V2S32 min, V2S32 max) {
    I2S32 r = i2s32(min.x, min.y, max.x, max.y);
    return(r);
}

function I2F32 i2f32(F32 x0, F32 y0, F32 x1, F32 y1) {
    I2F32 r = { x0,y0,x1,y1 };
    if (x1 < x0) {
        r.x0 = x1;
        r.x1 = x0;
    }
    if (y1 < y0) {
        r.y1 = y0;
        r.y0 = y1;
    }
    return(r);
}
function I2F32 i2f32_vec(V2F32 min, V2F32 max) {
    I2F32 r = i2f32(min.x, min.y, max.x, max.y);
    return(r);
}
function I2F32 i2f32_range(I1F32 x, I1F32 y) {
    I2F32 r = i2f32(x.min, y.min, x.max, y.max);
    return(r);
}

function V2S32 add_V2S32(const V2S32 *a, const V2S32 *b) {
    V2S32 r = { a->x + b->x, a->y + b->y };
    return(r);
}
function V2F32 add_V2F32(const V2F32 *a, const V2F32 *b) {
    V2F32 r = { a->x + b->x, a->y + b->y };
    return(r);
}
function V3F32 add_V3F32(const V3F32 *a, const V3F32 *b) {
    V3F32 r = { a->x + b->x, a->y + b->y, a->z + b->z };
    return(r);
}
function V4F32 add_V4F32(const V4F32 *a, const V4F32 *b) {
    V4F32 r = { a->x + b->x, a->y + b->y, a->z + b->z, a->w + b->w };
    return(r);
}

function V2S32 subtract_V2S32(const V2S32 *a, const V2S32 *b) {
    V2S32 r = { a->x - b->x, a->y - b->y };
    return(r);
}
function V2F32 subtract_V2F32(const V2F32 *a, const V2F32 *b) {
    V2F32 r = { a->x - b->x, a->y - b->y };
    return(r);
}
function V3F32 subtract_V3F32(const V3F32 *a, const V3F32 *b) {
    V3F32 r = { a->x - b->x, a->y - b->y, a->z - b->z };
    return(r);
}
function V4F32 subtract_V4F32(const V4F32 *a, const V4F32 *b) {
    V4F32 r = { a->x - b->x, a->y - b->y, a->z - b->z, a->w - b->w };
    return(r);
}


function V2S32 multiply_V2S32(const V2S32 *v, const S32 *s) {
    V2S32 r = { v->x * (*s), v->y * (*s) };
    return(r);
}
function V2F32 multiply_V2F32(const V2F32 *v, const F32 *s) {
    V2F32 r = { v->x * (*s), v->y * (*s) };
    return(r);
}
function V3F32 multiply_V3F32(const V3F32 *v, const F32 *s) {
    V3F32 r = { v->x * (*s), v->y * (*s), v->z * (*s) };
    return(r);
}
function V4F32 multiply_V4F32(const V4F32 *v, const F32 *s) {
    V4F32 r = { v->x * (*s), v->y * (*s), v->z * (*s), v->w * (*s) };
    return(r);
}


function V2F32 V2F32_hadamard(V2F32 a, V2F32 b) {
    V2F32 r = { a.x * b.x, a.y * b.y };
    return (r);
}
function V3F32 V3F32_hadamard(V3F32 a, V3F32 b) {
    V3F32 r = { a.x * b.x, a.y * b.y, a.z * b.z };
    return (r);
}
function V4F32 V4F32_hadamard(V4F32 a, V4F32 b){
    V4F32 r = { a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w };
    return (r);
}

function V2F32 V2F32_dot(V2F32 a, V2F32 b) {
    V2F32 r = { a.x * b.x, a.y * b.y };
    return (r);
}
function V3F32 V3F32_dot(V3F32 a, V3F32 b) {
    V3F32 r = { a.x * b.x, a.y * b.y , a.z * b.z };
    return (r);
}
function V4F32 V4F32_dot(V4F32 a, V4F32 b) {
    V4F32 r = { a.x * b.x, a.y * b.y , a.z * b.z, a.w * b.w };
    return (r);
}

function B32 I1F32_overlaps(I1F32 a, I1F32 b) {
    B32 result = ( b.min < a.max && a.min < b.max );
    return (result);
}
function B32 I1F32_contains(I1F32 r, F32 x) {
    B32 result = (r.min <= x && x < r.max);
    return result;
}
function B32 I2F32_contains(I2F32 r, V2F32 x) {
    return (r.min.x <= x.x && x.x <= r.max.x &&
        r.min.y <= x.y && x.y <= r.max.y); 
}
function B32 I2F32_overlaps(I2F32 a, I2F32 b) {
    return (b.min.x <= a.max.x && a.min.x <= b.max.x &&
        b.min.y <= a.max.y && a.min.y <= b.max.y); 
}
function B32 I2S32_overlaps(I2S32 a, I2S32 b) {
    B32 result = (b.min.x < a.max.x && a.min.x < b.max.x &&
                  b.min.y < a.max.y && a.min.y < b.max.y);
    return result;
}
function B32 I2S32_contains(I2S32 r, V2S32 x) {
    B32 result = (r.min.x <= x.x && x.x < r.max.x &&
                  r.min.y <= x.y && x.y < r.max.y);
    return result;
}

function F32 I1F32_dim(I1F32 r) {
    if (r.max == FLT_MAX && r.min == -FLT_MAX) {
        return FLT_MAX; 
    }
    return r.max - r.min;
}

function U64 I1U64_dim(I1U64 r) {
    U64 result = r.max - r.min;
    return(result);
}
function V2F32 I2F32_dim(I2F32 r) {
    V2F32 result = { r.x1 - r.x0, r.y1 - r.y0 };
    return(result);
}
function V2S32 I2S32_dim(I2S32 r) {
    V2S32 result = {r.x1 - r.x0, r.y1 - r.y0};
    return(result);
}

function F32 I1F32_center(I1F32 r) {
    F32 result = (r.min + r.max) * 0.5f;
    return(result);
}
function V2F32 I2F32_center(I2F32 r) {
    V2F32 result = {
        .x = (r.min.x + r.max.x) * 0.5f,
        .y = (r.min.y + r.max.y) * 0.5f
    };
    return result;
}

function I1F32 I1F32_axis(I2F32 r, Axis axis) {
    I1F32 result = {
        .min = r.min.v[axis],
        .max = r.max.v[axis]
    };
    return result;
}