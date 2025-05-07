#ifndef __JULIE_H__
#define __JULIE_H__

#define _JULIE_STATUS                                                                                                             \
    _JULIE_STATUS_X(JULIE_SUCCESS,                             "No error.")                                                       \
    _JULIE_STATUS_X(JULIE_ERR_UNEXPECTED_EOS,                  "Unexpected end of input.")                                        \
    _JULIE_STATUS_X(JULIE_ERR_UNEXPECTED_TOK,                  "Unexpected token.")                                               \
    _JULIE_STATUS_X(JULIE_ERR_EXTRA_RPAREN,                    "Extraneous closing parenthesis.")                                 \
    _JULIE_STATUS_X(JULIE_ERR_MISSING_RPAREN,                  "End of line while parentheses left open.")                        \
    _JULIE_STATUS_X(JULIE_ERR_LOOKUP,                          "Failed to find symbol.")                                          \
    _JULIE_STATUS_X(JULIE_ERR_BAD_APPLY,                       "Value is not something that can be applied in this way.")         \
    _JULIE_STATUS_X(JULIE_ERR_ARITY,                           "Incorrect number of arguments.")                                  \
    _JULIE_STATUS_X(JULIE_ERR_TYPE,                            "Incorrect argument type.")                                        \
    _JULIE_STATUS_X(JULIE_ERR_OBJECT_KEY_TYPE,                 "Expression is not a valid key type.")                             \
    _JULIE_STATUS_X(JULIE_ERR_MISSING_VAL,                     "Missing value expression.")                                       \
    _JULIE_STATUS_X(JULIE_ERR_BAD_INDEX,                       "Field or element not found.")                                     \
    _JULIE_STATUS_X(JULIE_ERR_EVAL_CANCELLED,                  "Evaluation was cancelled.")                                       \
    _JULIE_STATUS_X(JULIE_ERR_FILE_NOT_FOUND,                  "File not found.")                                                 \
    _JULIE_STATUS_X(JULIE_ERR_FILE_IS_DIR,                     "File is a directory.")                                            \
    _JULIE_STATUS_X(JULIE_ERR_MMAP_FAILED,                     "mmap() failed.")                                                  \
    _JULIE_STATUS_X(JULIE_ERR_RELEASE_WHILE_BORROWED,          "Value released while a borrowed reference remains outstanding.")  \
    _JULIE_STATUS_X(JULIE_ERR_REF_OF_TRANSIENT,                "References may only be taken to non-transient values.")           \
    _JULIE_STATUS_X(JULIE_ERR_NOT_LVAL,                        "Result of expression is not assignable.")                         \
    _JULIE_STATUS_X(JULIE_ERR_MODIFY_WHILE_ITER,               "Value modified while being iterated.")                            \
    _JULIE_STATUS_X(JULIE_ERR_LOAD_PACKAGE_FAILURE,            "Failed to load package.")                                         \
    _JULIE_STATUS_X(JULIE_ERR_USE_PACKAGE_FORBIDDEN,           "use-package has been disabled.")                                  \
    _JULIE_STATUS_X(JULIE_ERR_ADD_PACKAGE_DIRECTORY_FORBIDDEN, "add-package-directory has been disabled.")                        \
    _JULIE_STATUS_X(JULIE_ERR_INFIX,                           "infix function must be the middle expression of three.")          \
    _JULIE_STATUS_X(JULIE_ERR_MUST_FOLLOW_IF,                  "This special-form function must follow `if` or `elif`.")

#define _JULIE_STATUS_X(e, s) e,
typedef enum { _JULIE_STATUS } Julie_Status;
#undef _JULIE_STATUS_X

#define _JULIE_TYPE                                                                    \
    _JULIE_TYPE_X(JULIE_UNKNOWN,           "<unknown type>")                           \
    _JULIE_TYPE_X(JULIE_NIL,               "nil")                                      \
    _JULIE_TYPE_X(JULIE_SINT,              "signed integer")                           \
    _JULIE_TYPE_X(JULIE_UINT,              "unsigned integer")                         \
    _JULIE_TYPE_X(JULIE_FLOAT,             "float")                                    \
    _JULIE_TYPE_X(JULIE_STRING,            "string")                                   \
    _JULIE_TYPE_X(JULIE_SYMBOL,            "symbol")                                   \
    _JULIE_TYPE_X(JULIE_LIST,              "list")                                     \
    _JULIE_TYPE_X(JULIE_OBJECT,            "object")                                   \
    _JULIE_TYPE_X(JULIE_FN,                "function")                                 \
    _JULIE_TYPE_X(_JULIE_INTEGER,          "integer")                                  \
    _JULIE_TYPE_X(_JULIE_NUMBER,           "number")                                   \
    _JULIE_TYPE_X(_JULIE_LIST_OR_OBJECT,   "list or object")                           \
    _JULIE_TYPE_X(_JULIE_KEYLIKE,          "keylike (string, symbol, number, or nil)")

#define _JULIE_TYPE_X(e, s) e,
typedef enum { _JULIE_TYPE } Julie_Type;
#undef _JULIE_TYPE_X

#define JULIE_TYPE_IS_KEYLIKE(_t) \
    (  (_t) == JULIE_STRING       \
    || (_t) == JULIE_SYMBOL       \
    || (_t) == JULIE_SINT         \
    || (_t) == JULIE_UINT         \
    || (_t) == JULIE_FLOAT        \
    || (_t) == JULIE_NIL)

#define JULIE_TYPE_IS_INTEGER(_t) \
    (  (_t) == JULIE_SINT         \
    || (_t) == JULIE_UINT)

#define JULIE_TYPE_IS_NUMBER(_t)  \
    (  (_t) == JULIE_SINT         \
    || (_t) == JULIE_UINT         \
    || (_t) == JULIE_FLOAT)

enum {
    JULIE_NO_QUOTE  = 1u << 0u,
    JULIE_MULTILINE = 1u << 1u,
};

typedef struct Julie_Error_Info_Struct Julie_Error_Info;
typedef void (*Julie_Error_Callback)(Julie_Error_Info *info);
typedef struct Julie_Interp_Struct  Julie_Interp;
typedef struct Julie_Array_Struct   Julie_Array;
typedef struct Julie_Value_Struct   Julie_Value;
typedef struct Julie_String_Struct  Julie_String;
typedef        Julie_String        *Julie_String_ID;
typedef Julie_Status (*Julie_Fn)(Julie_Interp*, Julie_Value*, unsigned, Julie_Value**, Julie_Value**);

#ifdef JULIE_IMPL

#include <assert.h>

#ifndef JULIE_ASSERTIONS
#define JULIE_ASSERTIONS (1)
#endif

#if JULIE_ASSERTIONS
#define JULIE_ASSERT(...) assert(__VA_ARGS__)
#else
#define JULIE_ASSERT(...)
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h> /* strlen, memcpy, memset, memcmp */
#include <stdarg.h>
#include <alloca.h>
#include <math.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <dlfcn.h>

#define ALIGN_UP(x, align)   ((__typeof(x))((((unsigned long long)(x)) + ((unsigned long long)align)) & ~(((unsigned long long)align) - 1ull)))
#define ALIGN_DOWN(x, align) ((__typeof(x))(((unsigned long long)(x)) & ~(((unsigned long long)align) - 1ull)))
#define IS_ALIGNED(x, align) (!(((unsigned long long)(x)) & (((unsigned long long)align) - 1ull)))
#define IS_POWER_OF_TWO(x)   ((x) != 0 && IS_ALIGNED((x), (x)))
#define NEXT_POT_2(x)        (           (x) | (           (x) >>  1ull))
#define NEXT_POT_4(x)        ( NEXT_POT_2(x) | ( NEXT_POT_2(x) >>  2ull))
#define NEXT_POT_8(x)        ( NEXT_POT_4(x) | ( NEXT_POT_4(x) >>  4ull))
#define NEXT_POT_16(x)       ( NEXT_POT_8(x) | ( NEXT_POT_8(x) >>  8ull))
#define NEXT_POT_32(x)       (NEXT_POT_16(x) | (NEXT_POT_16(x) >> 16ull))
#define NEXT_POT_64(x)       (NEXT_POT_32(x) | (NEXT_POT_32(x) >> 32ull))
#define NEXT_POT(x)          (NEXT_POT_64((x) - 1ull) + 1ull)
#define CLZ(_val)            (__builtin_clzll((_val) | 1ull))
#define BITFIELD_FULL        (0xFFFFFFFFFFFFFFFF)

#ifndef JULIE_MALLOC
#define JULIE_MALLOC (malloc)
#endif
#ifndef JULIE_REALLOC
#define JULIE_REALLOC (realloc)
#endif
#ifndef JULIE_ALIGNED_MEM
#define JULIE_ALIGNED_MEM (posix_memalign)
#endif
#ifndef JULIE_FREE
#define JULIE_FREE (free)
#endif


/*********************************************************
 *                    Data Structures                    *
 *********************************************************/

#define hash_table_make(K_T, V_T, HASH) (CAT2(hash_table(K_T, V_T), _make)((HASH), NULL))
#define hash_table_make_e(K_T, V_T, HASH, EQU) (CAT2(hash_table(K_T, V_T), _make)((HASH), (EQU)))
#define hash_table_len(t) ((t)->len)
#define hash_table_free(t) ((t)->_free((t)))
#define hash_table_get_key(t, k) ((t)->_get_key((t), (k)))
#define hash_table_get_val(t, k) ((t)->_get_val((t), (k)))
#define hash_table_insert(t, k, v) ((t)->_insert((t), (k), (v)))
#define hash_table_delete(t, k) ((t)->_delete((t), (k)))
#define hash_table_traverse(t, key, val_ptr)                         \
    for (/* vars */                                                  \
         uint64_t __i    = 0,                                        \
                  __size = (t)->prime_sizes[(t)->_size_idx];         \
         /* conditions */                                            \
         __i < __size;                                               \
         /* increment */                                             \
         __i += 1)                                                   \
        for (/* vars */                                              \
             __typeof__(*(t)->_data) *__slot_ptr = (t)->_data + __i, \
                                    __slot     = *__slot_ptr;        \
                                                                     \
             /* conditions */                                        \
             __slot != NULL                 &&                       \
             ((key)     = __slot->_key   , 1) &&                     \
             ((val_ptr) = &(__slot->_val), 1);                       \
                                                                     \
             /* increment */                                         \
             __slot_ptr = &(__slot->_next),                          \
             __slot = *__slot_ptr)                                   \
            /* LOOP BODY HERE */                                     \


#define STR(x) _STR(x)
#define _STR(x) #x

#define CAT2(x, y) _CAT2(x, y)
#define _CAT2(x, y) x##y

#define CAT3(x, y, z) _CAT3(x, y, z)
#define _CAT3(x, y, z) x##y##z

#define CAT4(a, b, c, d) _CAT4(a, b, c, d)
#define _CAT4(a, b, c, d) a##b##c##d

#define _hash_table_slot(K_T, V_T) CAT4(_hash_table_slot_, K_T, _, V_T)
#define hash_table_slot(K_T, V_T) CAT4(hash_table_slot_, K_T, _, V_T)
#define _hash_table(K_T, V_T) CAT4(_hash_table_, K_T, _, V_T)
#define hash_table(K_T, V_T) CAT4(hash_table_, K_T, _, V_T)
#define hash_table_pretty_name(K_T, V_T) ("hash_table(" CAT3(K_T, ", ", V_T) ")")

#define _HASH_TABLE_EQU(t_ptr, l, r) \
    ((t_ptr)->_equ ? (t_ptr)->_equ((l), (r)) : (memcmp(&(l), &(r), sizeof((l))) == 0))

#define DEFAULT_START_SIZE_IDX (3)

#define use_hash_table(K_T, V_T)                                                             \
    static uint64_t CAT2(hash_table(K_T, V_T), _prime_sizes)[] = {                           \
        5ULL,        11ULL,        23ULL,        47ULL,        97ULL,                        \
        199ULL,        409ULL,        823ULL,        1741ULL,        3469ULL,                \
        6949ULL,        14033ULL,        28411ULL,        57557ULL,                          \
        116731ULL,        236897ULL,        480881ULL,        976369ULL,                     \
        1982627ULL,        4026031ULL,        8175383ULL,        16601593ULL,                \
        33712729ULL,        68460391ULL,        139022417ULL,                                \
        282312799ULL,        573292817ULL,        1164186217ULL,                             \
        2364114217ULL,        4294967291ULL,        8589934583ULL,                           \
        17179869143ULL,        34359738337ULL,        68719476731ULL,                        \
        137438953447ULL,        274877906899ULL,        549755813881ULL,                     \
        1099511627689ULL,        2199023255531ULL,        4398046511093ULL,                  \
        8796093022151ULL,        17592186044399ULL,        35184372088777ULL,                \
        70368744177643ULL,        140737488355213ULL,                                        \
        281474976710597ULL,        562949953421231ULL,                                       \
        1125899906842597ULL,        2251799813685119ULL,                                     \
        4503599627370449ULL,        9007199254740881ULL,                                     \
        18014398509481951ULL,        36028797018963913ULL,                                   \
        72057594037927931ULL,        144115188075855859ULL,                                  \
        288230376151711717ULL,        576460752303423433ULL,                                 \
        1152921504606846883ULL,        2305843009213693951ULL,                               \
        4611686018427387847ULL,        9223372036854775783ULL,                               \
        18446744073709551557ULL                                                              \
    };                                                                                       \
                                                                                             \
    struct _hash_table(K_T, V_T);                                                            \
                                                                                             \
    typedef struct _hash_table_slot(K_T, V_T) {                                              \
        K_T _key;                                                                            \
        V_T _val;                                                                            \
        uint64_t _hash;                                                                      \
        struct _hash_table_slot(K_T, V_T) *_next;                                            \
    }                                                                                        \
    *hash_table_slot(K_T, V_T);                                                              \
                                                                                             \
    typedef void (*CAT2(hash_table(K_T, V_T), _free_t))                                      \
        (struct _hash_table(K_T, V_T) *);                                                    \
    typedef K_T* (*CAT2(hash_table(K_T, V_T), _get_key_t))                                   \
        (struct _hash_table(K_T, V_T) *, K_T);                                               \
    typedef V_T* (*CAT2(hash_table(K_T, V_T), _get_val_t))                                   \
        (struct _hash_table(K_T, V_T) *, K_T);                                               \
    typedef void (*CAT2(hash_table(K_T, V_T), _insert_t))                                    \
        (struct _hash_table(K_T, V_T) *, K_T, V_T);                                          \
    typedef int (*CAT2(hash_table(K_T, V_T), _delete_t))                                     \
        (struct _hash_table(K_T, V_T) *, K_T);                                               \
    typedef unsigned long long (*CAT2(hash_table(K_T, V_T), _hash_t))(K_T);                  \
    typedef int (*CAT2(hash_table(K_T, V_T), _equ_t))(K_T, K_T);                             \
                                                                                             \
    typedef struct _hash_table(K_T, V_T) {                                                   \
        hash_table_slot(K_T, V_T) *_data;                                                    \
        uint64_t len, _size_idx, _load_thresh;                                               \
        uint64_t *prime_sizes;                                                               \
                                                                                             \
        CAT2(hash_table(K_T, V_T), _free_t)    const _free;                                  \
        CAT2(hash_table(K_T, V_T), _get_key_t) const _get_key;                               \
        CAT2(hash_table(K_T, V_T), _get_val_t) const _get_val;                               \
        CAT2(hash_table(K_T, V_T), _insert_t)  const _insert;                                \
        CAT2(hash_table(K_T, V_T), _delete_t)  const _delete;                                \
        CAT2(hash_table(K_T, V_T), _hash_t)    const _hash;                                  \
        CAT2(hash_table(K_T, V_T), _equ_t)     const _equ;                                   \
    }                                                                                        \
    *hash_table(K_T, V_T);                                                                   \
                                                                                             \
    /* hash_table slot */                                                                    \
    static inline hash_table_slot(K_T, V_T)                                                  \
        CAT2(hash_table_slot(K_T, V_T), _make)(K_T key, V_T val, uint64_t hash) {            \
        hash_table_slot(K_T, V_T) slot = JULIE_MALLOC(sizeof(*slot));                        \
                                                                                             \
        slot->_key  = key;                                                                   \
        slot->_val  = val;                                                                   \
        slot->_hash = hash;                                                                  \
        slot->_next = NULL;                                                                  \
                                                                                             \
        return slot;                                                                         \
    }                                                                                        \
                                                                                             \
    /* hash_table */                                                                         \
    static inline void CAT2(hash_table(K_T, V_T), _rehash_insert)                            \
        (hash_table(K_T, V_T) t, hash_table_slot(K_T, V_T) insert_slot) {                    \
                                                                                             \
        uint64_t h, data_size, idx;                                                          \
        hash_table_slot(K_T, V_T) slot, *slot_ptr;                                           \
                                                                                             \
        h         = insert_slot->_hash;                                                      \
        data_size = t->prime_sizes[t->_size_idx];                                            \
        idx       = h % data_size;                                                           \
        slot_ptr  = t->_data + idx;                                                          \
                                                                                             \
        while ((slot = *slot_ptr))    { slot_ptr = &(slot->_next); }                         \
                                                                                             \
        *slot_ptr = insert_slot;                                                             \
    }                                                                                        \
                                                                                             \
    static inline void                                                                       \
        CAT2(hash_table(K_T, V_T), _update_load_thresh)(hash_table(K_T, V_T) t) {            \
                                                                                             \
        uint64_t cur_size;                                                                   \
                                                                                             \
        cur_size        = t->prime_sizes[t->_size_idx];                                      \
        t->_load_thresh = ((double)((cur_size << 1ULL))                                      \
                            / ((double)(cur_size * 3)))                                      \
                            * cur_size;                                                      \
    }                                                                                        \
                                                                                             \
    static inline void CAT2(hash_table(K_T, V_T), _rehash)(hash_table(K_T, V_T) t) {         \
        uint64_t                   old_size,                                                 \
                                   new_data_size;                                            \
        hash_table_slot(K_T, V_T) *old_data,                                                 \
                                   slot,                                                     \
                                  *slot_ptr,                                                 \
                                   next;                                                     \
                                                                                             \
        old_size      = t->prime_sizes[t->_size_idx];                                        \
        old_data      = t->_data;                                                            \
        t->_size_idx += 1;                                                                   \
        new_data_size = sizeof(hash_table_slot(K_T, V_T)) * t->prime_sizes[t->_size_idx];    \
        t->_data      = JULIE_MALLOC(new_data_size);                                         \
        memset(t->_data, 0, new_data_size);                                                  \
                                                                                             \
        for (uint64_t i = 0; i < old_size; i += 1) {                                         \
            slot_ptr = old_data + i;                                                         \
            next = *slot_ptr;                                                                \
            while ((slot = next)) {                                                          \
                next        = slot->_next;                                                   \
                slot->_next = NULL;                                                          \
                CAT2(hash_table(K_T, V_T), _rehash_insert)(t, slot);                         \
            }                                                                                \
        }                                                                                    \
                                                                                             \
        JULIE_FREE(old_data);                                                                \
                                                                                             \
        CAT2(hash_table(K_T, V_T), _update_load_thresh)(t);                                  \
    }                                                                                        \
                                                                                             \
    static inline void                                                                       \
        CAT2(hash_table(K_T, V_T), _insert)(hash_table(K_T, V_T) t, K_T key, V_T val) {      \
        uint64_t h, data_size, idx;                                                          \
        hash_table_slot(K_T, V_T) slot, *slot_ptr;                                           \
                                                                                             \
        h         = t->_hash(key);                                                           \
        data_size = t->prime_sizes[t->_size_idx];                                            \
        idx       = h % data_size;                                                           \
        slot_ptr  = t->_data + idx;                                                          \
                                                                                             \
        while ((slot = *slot_ptr)) {                                                         \
            if (_HASH_TABLE_EQU(t, slot->_key, key)) {                                       \
                slot->_val = val;                                                            \
                return;                                                                      \
            }                                                                                \
            slot_ptr = &(slot->_next);                                                       \
        }                                                                                    \
                                                                                             \
        *slot_ptr = CAT2(hash_table_slot(K_T, V_T), _make)(key, val, h);                     \
        t->len   += 1;                                                                       \
                                                                                             \
        if (t->len == t->_load_thresh) {                                                     \
            CAT2(hash_table(K_T, V_T), _rehash)(t);                                          \
        }                                                                                    \
    }                                                                                        \
                                                                                             \
    static inline int CAT2(hash_table(K_T, V_T), _delete)                                    \
        (hash_table(K_T, V_T) t, K_T key) {                                                  \
                                                                                             \
        uint64_t h, data_size, idx;                                                          \
        hash_table_slot(K_T, V_T) slot, prev, *slot_ptr;                                     \
                                                                                             \
        h = t->_hash(key);                                                                   \
        data_size = t->prime_sizes[t->_size_idx];                                            \
        idx = h % data_size;                                                                 \
        slot_ptr = t->_data + idx;                                                           \
        prev = NULL;                                                                         \
                                                                                             \
        while ((slot = *slot_ptr)) {                                                         \
            if (_HASH_TABLE_EQU(t, slot->_key, key)) {                                       \
                break;                                                                       \
            }                                                                                \
            prev     = slot;                                                                 \
            slot_ptr = &(slot->_next);                                                       \
        }                                                                                    \
                                                                                             \
        if ((slot = *slot_ptr)) {                                                            \
            if (prev) {                                                                      \
                prev->_next = slot->_next;                                                   \
            } else {                                                                         \
                *slot_ptr = slot->_next;                                                     \
            }                                                                                \
            JULIE_FREE(slot);                                                                \
            t->len -= 1;                                                                     \
            return 1;                                                                        \
        }                                                                                    \
        return 0;                                                                            \
    }                                                                                        \
                                                                                             \
    static inline K_T*                                                                       \
        CAT2(hash_table(K_T, V_T), _get_key)(hash_table(K_T, V_T) t, K_T key) {              \
                                                                                             \
        uint64_t h, data_size, idx;                                                          \
        hash_table_slot(K_T, V_T) slot, *slot_ptr;                                           \
                                                                                             \
        h         = t->_hash(key);                                                           \
        data_size = t->prime_sizes[t->_size_idx];                                            \
        idx       = h % data_size;                                                           \
        slot_ptr  = t->_data + idx;                                                          \
                                                                                             \
        while ((slot = *slot_ptr)) {                                                         \
            if (_HASH_TABLE_EQU(t, slot->_key, key)) {                                       \
                return &slot->_key;                                                          \
            }                                                                                \
            slot_ptr = &(slot->_next);                                                       \
        }                                                                                    \
                                                                                             \
        return NULL;                                                                         \
    }                                                                                        \
                                                                                             \
    static inline V_T*                                                                       \
        CAT2(hash_table(K_T, V_T), _get_val)(hash_table(K_T, V_T) t, K_T key) {              \
                                                                                             \
        uint64_t h, data_size, idx;                                                          \
        hash_table_slot(K_T, V_T) slot, *slot_ptr;                                           \
                                                                                             \
        h         = t->_hash(key);                                                           \
        data_size = t->prime_sizes[t->_size_idx];                                            \
        idx       = h % data_size;                                                           \
        slot_ptr  = t->_data + idx;                                                          \
                                                                                             \
        while ((slot = *slot_ptr)) {                                                         \
            if (_HASH_TABLE_EQU(t, slot->_key, key)) {                                       \
                return &slot->_val;                                                          \
            }                                                                                \
            slot_ptr = &(slot->_next);                                                       \
        }                                                                                    \
                                                                                             \
        return NULL;                                                                         \
    }                                                                                        \
                                                                                             \
    static inline void CAT2(hash_table(K_T, V_T), _free)(hash_table(K_T, V_T) t) {           \
        for (uint64_t i = 0; i < t->prime_sizes[t->_size_idx]; i += 1) {                     \
            hash_table_slot(K_T, V_T) next, slot = t->_data[i];                              \
            while (slot != NULL) {                                                           \
                next = slot->_next;                                                          \
                JULIE_FREE(slot);                                                            \
                slot = next;                                                                 \
            }                                                                                \
        }                                                                                    \
        JULIE_FREE(t->_data);                                                                \
        JULIE_FREE(t);                                                                       \
    }                                                                                        \
                                                                                             \
    static inline hash_table(K_T, V_T)                                                       \
    CAT2(hash_table(K_T, V_T), _make)(CAT2(hash_table(K_T, V_T), _hash_t) hash,              \
                                      CAT2(hash_table(K_T, V_T), _equ_t)equ) {               \
        hash_table(K_T, V_T) t = JULIE_MALLOC(sizeof(*t));                                   \
                                                                                             \
        uint64_t data_size                                                                   \
            =   CAT2(hash_table(K_T, V_T), _prime_sizes)[DEFAULT_START_SIZE_IDX]             \
              * sizeof(hash_table_slot(K_T, V_T));                                           \
        hash_table_slot(K_T, V_T) *the_data = JULIE_MALLOC(data_size);                       \
                                                                                             \
        memset(the_data, 0, data_size);                                                      \
                                                                                             \
        struct _hash_table(K_T, V_T)                                                         \
            init                 = {._size_idx = DEFAULT_START_SIZE_IDX,                     \
                    ._data       = the_data,                                                 \
                    .len         = 0,                                                        \
                    .prime_sizes = CAT2(hash_table(K_T, V_T), _prime_sizes),                 \
                    ._free       = CAT2(hash_table(K_T, V_T), _free),                        \
                    ._get_key    = CAT2(hash_table(K_T, V_T), _get_key),                     \
                    ._get_val    = CAT2(hash_table(K_T, V_T), _get_val),                     \
                    ._insert     = CAT2(hash_table(K_T, V_T), _insert),                      \
                    ._delete     = CAT2(hash_table(K_T, V_T), _delete),                      \
                    ._equ        = (CAT2(hash_table(K_T, V_T), _equ_t))equ,                  \
                    ._hash       = (CAT2(hash_table(K_T, V_T), _hash_t))hash};               \
                                                                                             \
        memcpy(t, &init, sizeof(*t));                                                        \
                                                                                             \
        CAT2(hash_table(K_T, V_T), _update_load_thresh)(t);                                  \
                                                                                             \
        return t;                                                                            \
    }                                                                                        \


static unsigned long long julie_charptr_hash(char *s) {
    unsigned long hash = 5381;
    int c;

    while ((c = *s++))
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

static int julie_charptr_equ(char *a, char *b) { return strcmp(a, b) == 0; }

static inline unsigned long long julie_value_ptr_hash(Julie_Value *value) {
    return ((unsigned long long)((void*)value)) >> 4;
}

static inline unsigned long long julie_string_id_hash(Julie_String_ID id) {
    return ((unsigned long long)((void*)id)) >> 4;
}


/* qsort() + a context argument is a total portability mess. Thanks to this guy,
   who wrote a nice wrapper and fallback so that I didn't have to. */

/* Isaac Turner 29 April 2014 Public Domain */

/*

sort_r function to be exported.

Parameters:
  base is the array to be sorted
  nel is the number of elements in the array
  width is the size in bytes of each element of the array
  compar is the comparison function
  arg is a pointer to be passed to the comparison function

void sort_r(void *base, size_t nel, size_t width,
            int (*compar)(const void *_a, const void *_b, void *_arg),
            void *arg);

*/

#define _SORT_R_INLINE inline

#if (defined __APPLE__ || defined __MACH__ || defined __DARWIN__ || \
     (defined __FreeBSD__ && !defined(qsort_r)) || defined __DragonFly__)
#  define _SORT_R_BSD
#elif (defined __GLIBC__ || (defined (__FreeBSD__) && defined(qsort_r)))
#  define _SORT_R_LINUX
#elif (defined _WIN32 || defined _WIN64 || defined __WINDOWS__ || \
       defined __MINGW32__ || defined __MINGW64__)
#  define _SORT_R_WINDOWS
#  undef _SORT_R_INLINE
#  define _SORT_R_INLINE __inline
#else
  /* Using our own recursive quicksort sort_r_simple() */
#endif

#if (defined NESTED_QSORT && NESTED_QSORT == 0)
#  undef NESTED_QSORT
#endif

#define SORT_R_SWAP(a,b,tmp) ((tmp) = (a), (a) = (b), (b) = (tmp))

/* swap a and b */
/* a and b must not be equal! */
static _SORT_R_INLINE void sort_r_swap(char *__restrict a, char *__restrict b,
                                       size_t w)
{
  char tmp, *end = a+w;
  for(; a < end; a++, b++) { SORT_R_SWAP(*a, *b, tmp); }
}

/* swap a, b iff a>b */
/* a and b must not be equal! */
/* __restrict is same as restrict but better support on old machines */
static _SORT_R_INLINE int sort_r_cmpswap(char *__restrict a,
                                         char *__restrict b, size_t w,
                                         int (*compar)(const void *_a,
                                                       const void *_b,
                                                       void *_arg),
                                         void *arg)
{
  if(compar(a, b, arg) > 0) {
    sort_r_swap(a, b, w);
    return 1;
  }
  return 0;
}

/*
Swap consecutive blocks of bytes of size na and nb starting at memory addr ptr,
with the smallest swap so that the blocks are in the opposite order. Blocks may
be internally re-ordered e.g.

  12345ab  ->   ab34512
  123abc   ->   abc123
  12abcde  ->   deabc12
*/
static _SORT_R_INLINE void sort_r_swap_blocks(char *ptr, size_t na, size_t nb)
{
  if(na > 0 && nb > 0) {
    if(na > nb) { sort_r_swap(ptr, ptr+na, nb); }
    else { sort_r_swap(ptr, ptr+nb, na); }
  }
}

/* Implement recursive quicksort ourselves */
/* Note: quicksort is not stable, equivalent values may be swapped */
static _SORT_R_INLINE void sort_r_simple(void *base, size_t nel, size_t w,
                                         int (*compar)(const void *_a,
                                                       const void *_b,
                                                       void *_arg),
                                         void *arg)
{
  char *b = (char *)base, *end = b + nel*w;

  /* for(size_t i=0; i<nel; i++) {printf("%4i", *(int*)(b + i*sizeof(int)));}
  printf("\n"); */

  if(nel < 10) {
    /* Insertion sort for arbitrarily small inputs */
    char *pi, *pj;
    for(pi = b+w; pi < end; pi += w) {
      for(pj = pi; pj > b && sort_r_cmpswap(pj-w,pj,w,compar,arg); pj -= w) {}
    }
  }
  else
  {
    /* nel > 6; Quicksort */

    int cmp;
    char *pl, *ple, *pr, *pre, *pivot;
    char *last = b+w*(nel-1), *tmp;

    /*
    Use median of second, middle and second-last items as pivot.
    First and last may have been swapped with pivot and therefore be extreme
    */
    char *l[3];
    l[0] = b + w;
    l[1] = b+w*(nel/2);
    l[2] = last - w;

    /* printf("pivots: %i, %i, %i\n", *(int*)l[0], *(int*)l[1], *(int*)l[2]); */

    if(compar(l[0],l[1],arg) > 0) { SORT_R_SWAP(l[0], l[1], tmp); }
    if(compar(l[1],l[2],arg) > 0) {
      SORT_R_SWAP(l[1], l[2], tmp);
      if(compar(l[0],l[1],arg) > 0) { SORT_R_SWAP(l[0], l[1], tmp); }
    }

    /* swap mid value (l[1]), and last element to put pivot as last element */
    if(l[1] != last) { sort_r_swap(l[1], last, w); }

    /*
    pl is the next item on the left to be compared to the pivot
    pr is the last item on the right that was compared to the pivot
    ple is the left position to put the next item that equals the pivot
    ple is the last right position where we put an item that equals the pivot

                                           v- end (beyond the array)
      EEEEEELLLLLLLLuuuuuuuuGGGGGGGEEEEEEEE.
      ^- b  ^- ple  ^- pl   ^- pr  ^- pre ^- last (where the pivot is)

    Pivot comparison key:
      E = equal, L = less than, u = unknown, G = greater than, E = equal
    */
    pivot = last;
    ple = pl = b;
    pre = pr = last;

    /*
    Strategy:
    Loop into the list from the left and right at the same time to find:
    - an item on the left that is greater than the pivot
    - an item on the right that is less than the pivot
    Once found, they are swapped and the loop continues.
    Meanwhile items that are equal to the pivot are moved to the edges of the
    array.
    */
    while(pl < pr) {
      /* Move left hand items which are equal to the pivot to the far left.
         break when we find an item that is greater than the pivot */
      for(; pl < pr; pl += w) {
        cmp = compar(pl, pivot, arg);
        if(cmp > 0) { break; }
        else if(cmp == 0) {
          if(ple < pl) { sort_r_swap(ple, pl, w); }
          ple += w;
        }
      }
      /* break if last batch of left hand items were equal to pivot */
      if(pl >= pr) { break; }
      /* Move right hand items which are equal to the pivot to the far right.
         break when we find an item that is less than the pivot */
      for(; pl < pr; ) {
        pr -= w; /* Move right pointer onto an unprocessed item */
        cmp = compar(pr, pivot, arg);
        if(cmp == 0) {
          pre -= w;
          if(pr < pre) { sort_r_swap(pr, pre, w); }
        }
        else if(cmp < 0) {
          if(pl < pr) { sort_r_swap(pl, pr, w); }
          pl += w;
          break;
        }
      }
    }

    pl = pr; /* pr may have gone below pl */

    /*
    Now we need to go from: EEELLLGGGGEEEE
                        to: LLLEEEEEEEGGGG

    Pivot comparison key:
      E = equal, L = less than, u = unknown, G = greater than, E = equal
    */
    sort_r_swap_blocks(b, ple-b, pl-ple);
    sort_r_swap_blocks(pr, pre-pr, end-pre);

    /*for(size_t i=0; i<nel; i++) {printf("%4i", *(int*)(b + i*sizeof(int)));}
    printf("\n");*/

    sort_r_simple(b, (pl-ple)/w, w, compar, arg);
    sort_r_simple(end-(pre-pr), (pre-pr)/w, w, compar, arg);
  }
}


#if defined NESTED_QSORT

  static _SORT_R_INLINE void sort_r(void *base, size_t nel, size_t width,
                                    int (*compar)(const void *_a,
                                                  const void *_b,
                                                  void *aarg),
                                    void *arg)
  {
    int nested_cmp(const void *a, const void *b)
    {
      return compar(a, b, arg);
    }

    qsort(base, nel, width, nested_cmp);
  }

#else /* !NESTED_QSORT */

  /* Declare structs and functions */

  #if defined _SORT_R_BSD

    /* Ensure qsort_r is defined */
    extern void qsort_r(void *base, size_t nel, size_t width, void *thunk,
                        int (*compar)(void *_thunk,
                                      const void *_a, const void *_b));

  #endif

  #if defined _SORT_R_BSD || defined _SORT_R_WINDOWS

    /* BSD (qsort_r), Windows (qsort_s) require argument swap */

    struct sort_r_data
    {
      void *arg;
      int (*compar)(const void *_a, const void *_b, void *_arg);
    };

    static _SORT_R_INLINE int sort_r_arg_swap(void *s,
                                              const void *a, const void *b)
    {
      struct sort_r_data *ss = (struct sort_r_data*)s;
      return (ss->compar)(a, b, ss->arg);
    }

  #endif

  #if defined _SORT_R_LINUX

    typedef int(* __compar_d_fn_t)(const void *, const void *, void *);
    extern void (qsort_r)(void *base, size_t nel, size_t width,
                          __compar_d_fn_t __compar, void *arg)
      __attribute__((nonnull (1, 4)));

  #endif

  /* implementation */

  static _SORT_R_INLINE void sort_r(void *base, size_t nel, size_t width,
                                    int (*compar)(const void *_a,
                                                  const void *_b, void *_arg),
                                    void *arg)
  {
    #if defined _SORT_R_LINUX

      #if defined __GLIBC__ && ((__GLIBC__ < 2) || (__GLIBC__ == 2 && __GLIBC_MINOR__ < 8))

        /* no qsort_r in glibc before 2.8, need to use nested qsort */
        sort_r_simple(base, nel, width, compar, arg);

      #else

        qsort_r(base, nel, width, compar, arg);

      #endif

    #elif defined _SORT_R_BSD

      struct sort_r_data tmp;
      tmp.arg = arg;
      tmp.compar = compar;
      qsort_r(base, nel, width, &tmp, sort_r_arg_swap);

    #elif defined _SORT_R_WINDOWS

      struct sort_r_data tmp;
      tmp.arg = arg;
      tmp.compar = compar;
      qsort_s(base, nel, width, sort_r_arg_swap, &tmp);

    #else

      /* Fall back to our own quicksort implementation */
      sort_r_simple(base, nel, width, compar, arg);

    #endif
  }

#endif /* !NESTED_QSORT */

#undef _SORT_R_INLINE
#undef _SORT_R_WINDOWS
#undef _SORT_R_LINUX
#undef _SORT_R_BSD


struct Julie_Array_Struct {
    unsigned long long  len;
    unsigned long long  cap;
    void               *aux;
    void               *data[];
};

#define JULIE_ARRAY_INIT        ((Julie_Array*)NULL)
#define JULIE_ARRAY_INITIAL_CAP (4)

static inline void julie_array_free(Julie_Array *array) {
    if (array != NULL) { JULIE_FREE(array); }
}

static inline unsigned julie_array_len(Julie_Array *array) {
    return array == NULL ? 0 : array->len;
}

static inline Julie_Array *julie_array_set_aux(Julie_Array *array, void *aux) {
    if (array == NULL) {
        array = JULIE_MALLOC(sizeof(Julie_Array) + (JULIE_ARRAY_INITIAL_CAP * sizeof(void*)));
        array->len = 0;
        array->cap = JULIE_ARRAY_INITIAL_CAP;
    }
    array->aux = aux;
    return array;
}

static inline Julie_Array *julie_array_push(Julie_Array *array, void *item) {
    if (array == NULL) {
        array = JULIE_MALLOC(sizeof(Julie_Array) + (JULIE_ARRAY_INITIAL_CAP * sizeof(void*)));
        array->len = 0;
        array->cap = JULIE_ARRAY_INITIAL_CAP;
        array->aux = NULL;
        goto push;
    }

    if (array->len >= array->cap) {
        array->cap += ((array->cap >> 1) > 0) ? (array->cap >> 1) : 1;
        array       = JULIE_REALLOC(array, sizeof(Julie_Array) + (array->cap * sizeof(void*)));
    }

push:;
    array->data[array->len] = item;
    array->len += 1;

    return array;
}

static inline void *julie_array_elem(Julie_Array *array, unsigned idx) {
    if (array == NULL || idx >= array->len) {
        return NULL;
    }

    return array->data[idx];
}

static inline void *julie_array_top(Julie_Array *array) {
    if (array == NULL || array->len == 0) {
        return NULL;
    }

    return array->data[array->len - 1];
}

static inline void *julie_array_pop(Julie_Array *array) {
    void *r;

    r = NULL;

    if (array != NULL && array->len > 0) {
        r = julie_array_top(array);
        array->len -= 1;
    }

    return r;
}

static inline void julie_array_erase(Julie_Array *array, unsigned idx) {
    if (array == NULL || idx >= array->len) {
        return;
    }

    memmove(array->data + idx, array->data + idx + 1, (array->len - idx - 1) * sizeof(*array->data));

    array->len -= 1;
}

#define JULIE_ARRAY_PUSH(_arrayp, _item)   ((_arrayp) = julie_array_push((_arrayp), (_item)))
#define JULIE_ARRAY_SET_AUX(_arrayp, _aux) ((_arrayp) = julie_array_set_aux((_arrayp), (_aux)))

#define ARRAY_FOR_EACH(_arrayp, _it)                                                                 \
    for (unsigned _each_i = 0;                                                                       \
         ((_arrayp) != NULL && _each_i < (_arrayp)->len && (((_it) = (_arrayp)->data[_each_i]), 1)); \
         _each_i += 1)



/*********************************************************
 *                         Core                          *
 *********************************************************/

#define JULIE_MAX_RC_POT (55ull)

struct Julie_Value_Struct {
    union {
        long long           sint;
        unsigned long long  uint;
        double              floating;
        Julie_String_ID     string_id;
        Julie_String_ID     symbol_id;
        char               *cstring;
//         Julie_Object        object;
        Julie_Array        *list;
        Julie_Fn            builtin_fn;
    };
    unsigned long long      type            : 4;                //  4
    unsigned long long      interned_string : 1;                //  5
    unsigned long long      builtin         : 1;                //  6
    unsigned long long      infix           : 1;                //  7
    unsigned long long      rc              : JULIE_MAX_RC_POT; // 64
};

#define JULIE_REF(_value)                                    \
    JULIE_ASSERT((_value)->rc < (1ull << JULIE_MAX_RC_POT)); \
    (_value)->rc += 1;

#define JULIE_UNREF(_value)                                  \
    JULIE_ASSERT((_value)->rc > 0);                          \
    (_value)->rc -= 1;



typedef Julie_Value *Julie_Value_Ptr;


typedef struct Julie_Value_Store_Block_Struct Julie_Value_Store_Block;
struct Julie_Value_Store_Block_Struct {
    unsigned long long       region_bitfield;
    unsigned long long       slots_bitfields[64];
    Julie_Value_Store_Block *next;
    Julie_Value              slots[4096];
};

#define STORE_BLOCK_ALIGN (NEXT_POT(sizeof(Julie_Value_Store_Block)))

typedef struct Julie_Value_Store_Struct {
    Julie_Value_Store_Block *head;
} Julie_Value_Store;

Julie_Value *julie_store_alloc(Julie_Value_Store *store) {
    Julie_Value_Store_Block *block;
    Julie_Value_Store_Block *prev;
    int                      err;
    unsigned long long       region;
    unsigned long long       slot;
    Julie_Value             *value;

    prev  = NULL;
    block = store->head;

    while (block != NULL) {
        if (block->region_bitfield != BITFIELD_FULL) { goto found_block; }
        prev  = block;
        block = block->next;
    }

    err = JULIE_ALIGNED_MEM((void**)&block, STORE_BLOCK_ALIGN, sizeof(*block));
    (void)err;
    JULIE_ASSERT(err == 0 && "posix_memalign failed");

    memset(block, 0, sizeof(*block));

    if (prev == NULL) {
        store->head = block;
    } else {
        prev->next = block;
    }

found_block:;
    region = CLZ(~block->region_bitfield);
    slot   = CLZ(~block->slots_bitfields[region]);

    block->slots_bitfields[region] |= (1ull << (63ull - slot));
    if (block->slots_bitfields[region] == BITFIELD_FULL) {
        block->region_bitfield |= (1ull << (63ull - region));
    }

    value = &block->slots[(region << 6ull) + slot];

    memset(value, 0, sizeof(*value));

    return value;
}

void julie_store_free(Julie_Value *value) {
    Julie_Value_Store_Block *block;
    unsigned long long       idx;
    unsigned long long       region;
    unsigned long long       slot;

    block = (void*)ALIGN_DOWN((unsigned long long)value, STORE_BLOCK_ALIGN);

    idx    = ((char*)value - (char*)block->slots) / sizeof(Julie_Value);
    region = idx >> 6ull;
    slot   = idx  & 63ull;

    JULIE_ASSERT(!!(block->slots_bitfields[region] & (1ull << (63ull - slot))) && "slot not taken");

    block->slots_bitfields[region] &= ~(1ull << (63ull - slot));
    block->region_bitfield         &= ~(1ull << (63ull - region));
}

// #define JULIE_NEW() (julie_store_alloc(&interp->store))
#define JULIE_NEW() (calloc(1, sizeof(Julie_Value)))
// #define JULIE_DEL(_value) (julie_store_free((_value)))
#define JULIE_DEL(_value) (free((_value)))




typedef struct Julie_Source_Value_Info_Struct {
    unsigned long long line;
    unsigned long long col;
    unsigned long long ind;
} Julie_Source_Value_Info;

use_hash_table(Julie_Value_Ptr, Julie_Source_Value_Info)

typedef struct Julie_Parse_Context_Struct {
    Julie_Interp *interp;
    const char *cursor;
    const char *end;
    unsigned long long          line;
    unsigned long long          col;
    unsigned long long          ind;
    unsigned long long          plevel;
    Julie_Array *parse_stack;
} Julie_Parse_Context;

struct Julie_Error_Info_Struct {
    Julie_Status status;
};

typedef char *Char_Ptr;
use_hash_table(Char_Ptr, Julie_String_ID)

use_hash_table(Julie_String_ID, Julie_Value_Ptr)

struct Julie_Interp_Struct {
    Julie_Error_Callback    error_callback;
//     Julie_Output_Callback   output_callback;
//     Julie_Eval_Callback     eval_callback;
    int                     argc;
    char                  **argv;
    Julie_Value_Store store;
    hash_table(Julie_Value_Ptr, Julie_Source_Value_Info) source_info;
    hash_table(Char_Ptr, Julie_String_ID) strings;
    hash_table(Julie_String_ID, Julie_Value_Ptr) global_symtab;
    Julie_Array *local_symtab_stack;

    Julie_Array *roots;
};



struct Julie_String_Struct {
    char               *chars;
    unsigned long long  len;
};

static Julie_String_ID julie_get_string_id(Julie_Interp *interp, const char *s) {
    Julie_String_ID    *lookup;
    Julie_String       *newstring;
    unsigned long long  len;

    lookup = hash_table_get_val(interp->strings, (char*)s);

    if (lookup == NULL) {
        newstring        = JULIE_MALLOC(sizeof(*newstring));
        len              = strlen(s);
        newstring->len   = len;
        newstring->chars = strdup(s);
        hash_table_insert(interp->strings, newstring->chars, newstring);
        lookup = hash_table_get_val(interp->strings, (char*)s);
        JULIE_ASSERT(lookup != NULL);
    }

    return *lookup;
}

static void julie_free_string(Julie_String *string) {
    JULIE_FREE(string->chars);
    string->chars = NULL;
    string->len   = 0;
}

static inline const Julie_String *julie_get_string(Julie_Interp *interp, const Julie_String_ID id) {
    (void)interp;
    return id;
}

static inline const char *julie_get_cstring(Julie_Interp *interp, const Julie_String_ID id) {
    (void)interp;
    return ((const Julie_String*)id)->chars;
}




#define _JULIE_STATUS_X(e, s) s,
const char *_julie_error_strings[] = { _JULIE_STATUS };
#undef _JULIE_STATUS_X

const char *julie_error_string(Julie_Status error) {
    return _julie_error_strings[error];
}

#define _JULIE_TYPE_X(e, s) s,
const char *_julie_type_strings[] = { _JULIE_TYPE };
#undef _JULIE_TYPE_X

const char *julie_type_string(Julie_Type type) {
    return _julie_type_strings[type];
}


static inline Julie_Value *julie_nil_value(Julie_Interp *interp) {
    Julie_Value *v;

    v = JULIE_NEW();

    v->type = JULIE_NIL;

    return v;
}

static inline Julie_Value *julie_sint_value(Julie_Interp *interp, long long sint) {
    Julie_Value *v;

    v = JULIE_NEW();

    v->type = JULIE_SINT;
    v->sint = sint;

    return v;
}

static inline Julie_Value *julie_uint_value(Julie_Interp *interp, unsigned long long uint) {
    Julie_Value *v;

    v = JULIE_NEW();

    v->type = JULIE_UINT;
    v->uint = uint;

    return v;
}

static inline Julie_Value *julie_float_value(Julie_Interp *interp, double floating) {
    Julie_Value *v;

    v = JULIE_NEW();

    v->type     = JULIE_FLOAT;
    v->floating = floating;

    return v;
}

static inline Julie_Value *julie_symbol_value(Julie_Interp *interp, const Julie_String_ID id) {
    Julie_Value *v;

    v = JULIE_NEW();

    v->type            = JULIE_SYMBOL;
    v->symbol_id       = id;
    v->interned_string = 1;

    return v;
}

static inline Julie_Value *julie_string_value(Julie_Interp *interp, const char *s) {
    Julie_Value *v;

    v = JULIE_NEW();

    v->type    = JULIE_STRING;
    v->cstring = strdup(s);

    return v;
}

static inline Julie_Value *julie_interned_string_value(Julie_Interp *interp, const Julie_String_ID id) {
    Julie_Value *v;

    v = JULIE_NEW();

    v->type            = JULIE_STRING;
    v->symbol_id       = id;
    v->interned_string = 1;

    return v;
}

static inline Julie_Value *julie_list_value(Julie_Interp *interp) {
    Julie_Value *v;

    v = JULIE_NEW();

    v->type = JULIE_LIST;
    v->list = JULIE_ARRAY_INIT;

    return v;
}

static inline Julie_Value *julie_builtin_fn_value(Julie_Interp *interp, Julie_Fn fn) {
    Julie_Value *v;

    v = JULIE_NEW();

    v->type       = JULIE_FN;
    v->builtin_fn = fn;
    v->builtin    = 1;

    return v;
}

static Julie_Value *_julie_copy(Julie_Interp *interp, Julie_Value *value, int force) {
    Julie_Value *copy;
    Julie_Value *it;

    if (value->rc && !force) { return value; }

    copy = JULIE_NEW();

    *copy    = *value;
    copy->rc = 0;

    switch (value->type) {
        case JULIE_STRING:
        case JULIE_SYMBOL:
            if (!value->interned_string) {
                copy->cstring = strdup(value->cstring);
            }
            break;

        case JULIE_LIST:
            copy->list = JULIE_ARRAY_INIT;
            ARRAY_FOR_EACH(value->list, it) {
                JULIE_ARRAY_PUSH(copy->list, _julie_copy(interp, it, 1));
            }
            break;

        case JULIE_OBJECT:
            break;
    }

    return copy;
}

static Julie_Value *julie_copy(Julie_Interp *interp, Julie_Value *value) {
    return _julie_copy(interp, value, 0);
}

static Julie_Value *julie_force_copy(Julie_Interp *interp, Julie_Value *value) {
    return _julie_copy(interp, value, 1);
}

static void _julie_free_value(Julie_Interp * interp, Julie_Value *value, int free_root) {
    Julie_Value *it;

    JULIE_ASSERT(free_root || value->rc == 0);

    if (value->rc) { return; }

    switch (value->type) {
        case JULIE_LIST:
            ARRAY_FOR_EACH(value->list, it) {
                _julie_free_value(interp, it, 1);
            }
            julie_array_free(value->list);
            break;

        case JULIE_STRING:
        case JULIE_SYMBOL:
            if (!value->interned_string) {
                JULIE_FREE(value->cstring);
            }
            break;
    }

    if (free_root) {
        JULIE_DEL(value);
    }
}

static void julie_free_value(Julie_Interp *interp, Julie_Value *value) {
    _julie_free_value(interp, value, 1);
}

static void julie_free_and_reuse_value(Julie_Interp *interp, Julie_Value *value) {
    _julie_free_value(interp, value, 0);
}

Julie_Source_Value_Info *julie_get_source_value_info(Julie_Interp *interp, Julie_Value *value) {
    Julie_Source_Value_Info *info;
    JULIE_ASSERT(interp->source_info != NULL);
    info = hash_table_get_val(interp->source_info, value);
    JULIE_ASSERT(info != NULL && "did not find value in source info table");
    return info;
}



/*********************************************************
 *                         Symbols                       *
 *********************************************************/

static int julie_symbol_starts_with_ampersand(Julie_Interp *interp, const Julie_String_ID id) {
    const Julie_String *s;

    s  = julie_get_string(interp, id);
    return s->len >= 2 && s->chars[0] == '&';
}


static hash_table(Julie_String_ID, Julie_Value_Ptr) julie_local_symtab(Julie_Interp *interp) {
    return julie_array_top(interp->local_symtab_stack);
}

static int julie_refs_outstanding(Julie_Interp *interp, Julie_Value *value) {
    Julie_Value *it;

    if (value->rc > 1) {
        return 1;
    }

    switch (value->type) {
        case JULIE_LIST:
            ARRAY_FOR_EACH(value->list, it) {
                if (julie_refs_outstanding(interp, it)) {
                    return 1;
                }
            }
            break;
    }

    return 0;
}

static Julie_Status _julie_bind(Julie_Interp *interp, const Julie_String_ID name, Julie_Value **valuep, int local) {
    int                                            ref;
    hash_table(Julie_String_ID, Julie_Value_Ptr)   symtab;
    Julie_Value                                  **lookup;
    unsigned long long                             save_rc;

    ref = julie_symbol_starts_with_ampersand(interp, name);

    if (local) {
        symtab = julie_local_symtab(interp);
        JULIE_ASSERT(symtab != NULL);
    } else {
        symtab = interp->global_symtab;
    }

    if (!ref) {
        *valuep = julie_force_copy(interp, *valuep);
    }

    lookup = hash_table_get_val(symtab, name);
    if (lookup != NULL) {
        if (*lookup != *valuep) {
            if (!ref && julie_refs_outstanding(interp, *lookup)) {
                return JULIE_ERR_RELEASE_WHILE_BORROWED;
            }

            /* Overwrite *lookup with *valuep data, preserving original address. */
            save_rc = (*lookup)->rc;
            (*lookup)->rc = 0;
            julie_free_and_reuse_value(interp, *lookup);
            **lookup = **valuep;
            (*lookup)->rc = save_rc;

            /* Free up passed in outer value. */
            memset(*valuep, 0, sizeof(**valuep));
            (*valuep)->type = JULIE_NIL;
            julie_free_value(interp, *valuep);

            /* Return address of reused value. */
            *valuep  = *lookup;
        }
    } else {
        if (ref && (*valuep)->rc == 0) {
            return JULIE_ERR_REF_OF_TRANSIENT;
        }

        JULIE_REF(*valuep);
        hash_table_insert(symtab, name, *valuep);
    }

    return JULIE_SUCCESS;
}

#if 0
static Julie_Status _julie_unbind(Julie_Interp *interp, const Julie_String_ID name, int local) {
    hash_table(Julie_String_ID, Julie_Value_Ptr)   symtab;
    Julie_Value                                  **lookup;
    Julie_Value                                   *value;

    if (local) {
        symtab = julie_array_top(interp->local_symtab_stack);
        JULIE_ASSERT(symtab != NULL);
    } else {
        symtab = interp->global_symtab;
    }

    lookup = hash_table_get_val(symtab, name);
    if (lookup == NULL) {
        return JULIE_ERR_LOOKUP;
    }

    value = *lookup;

    hash_table_delete(symtab, name);

    JULIE_UNREF(value);

    if (!julie_symbol_starts_with_ampersand(interp, name)) {
        if ((*lookup)->rc > 0) {
            return JULIE_ERR_RELEASE_WHILE_BORROWED;
        }

        julie_free_value(interp, *lookup);
        julie_store_free(*lookup);
    }

    return JULIE_SUCCESS;
}
#endif

static Julie_Status julie_bind(Julie_Interp *interp, const Julie_String_ID name, Julie_Value **valuep) {
    return _julie_bind(interp, name, valuep, 0);
}
static Julie_Status julie_bind_local(Julie_Interp *interp, const Julie_String_ID name, Julie_Value **valuep) {
    return _julie_bind(interp, name, valuep, 1);
}
#if 0
static Julie_Status julie_unbind(Julie_Interp *interp, const Julie_String_ID name) {
    return _julie_unbind(interp, name, 0);
}
static Julie_Status julie_unbind_local(Julie_Interp *interp, const Julie_String_ID name) {
    return _julie_unbind(interp, name, 1);
}
#endif

Julie_Status julie_bind_fn(Julie_Interp *interp, Julie_String_ID id, Julie_Fn fn) {
    Julie_Value *fn_val;

    fn_val = julie_builtin_fn_value(interp, fn);

    return julie_bind(interp, id, &fn_val);
}

Julie_Status julie_bind_infix_fn(Julie_Interp *interp, Julie_String_ID id, Julie_Fn fn) {
    Julie_Value *fn_val;

    fn_val = julie_builtin_fn_value(interp, fn);
    fn_val->infix = 1;

    return julie_bind(interp, id, &fn_val);
}

Julie_Value *julie_lookup(Julie_Interp *interp, const Julie_String_ID id) {
    Julie_Value                                  **lookup;
    hash_table(Julie_String_ID, Julie_Value_Ptr)   local_symtab;
    Julie_Value                                   *val;

    lookup = NULL;

    local_symtab = julie_local_symtab(interp);
    if (local_symtab != NULL) {
        lookup = hash_table_get_val(local_symtab, id);
    }

    if (lookup == NULL) {
        lookup = hash_table_get_val(interp->global_symtab, id);
    }

    if (lookup == NULL) { return NULL; }

    val = *lookup;

    return val;
}

Julie_Value *julie_lookup_local_only(Julie_Interp *interp, const Julie_String_ID id) {
    Julie_Value **lookup;

    lookup = hash_table_get_val(julie_local_symtab(interp), id);

    return lookup == NULL ? NULL : *lookup;
}


/*********************************************************
 *                         Errors                        *
 *********************************************************/

static void julie_error(Julie_Interp *interp, Julie_Error_Info *info) {
    if (interp->error_callback != NULL) {
        interp->error_callback(info);
    } else {
//         jule_free_error_info(info);
    }
}

static void julie_make_parse_error(Julie_Interp *interp, unsigned long long line, unsigned long long col, Julie_Status status) {
    Julie_Error_Info info;
    memset(&info, 0, sizeof(info));
    info.status = status;
    julie_error(interp, &info);
}

static void julie_make_bad_apply_error(Julie_Interp *interp, Julie_Value *value, Julie_Type got) {
    Julie_Error_Info info;
    memset(&info, 0, sizeof(info));
//     info.interp        = interp;
    info.status        = JULIE_ERR_BAD_APPLY;
//     info.location.line = value->line;
//     info.location.col  = value->col;
//     if (interp->cur_file != NULL) { info.file = julie_charptr_dup(julie_get_string(interp, interp->cur_file)->chars); }
//     info.got_type      = got;
    julie_error(interp, &info);
}

static void julie_make_arity_error(Julie_Interp *interp, Julie_Value *value, int wanted, int got, int at_least) {
    Julie_Error_Info info;
    memset(&info, 0, sizeof(info));
//     info.interp         = interp;
    info.status         = JULIE_ERR_ARITY;
//     info.location.line  = value->line;
//     info.location.col   = value->col;
//     if (interp->cur_file != NULL) { info.file = julie_charptr_dup(julie_get_string(interp, interp->cur_file)->chars); }
//     info.wanted_arity   = wanted;
//     info.got_arity      = got;
//     info.arity_at_least = at_least;
    julie_error(interp, &info);
}

static void julie_make_type_error(Julie_Interp *interp, Julie_Value *value, Julie_Type wanted, Julie_Type got) {
    Julie_Error_Info info;
    memset(&info, 0, sizeof(info));
//     info.interp        = interp;
    info.status        = JULIE_ERR_TYPE;
//     info.location.line = value->line;
//     info.location.col  = value->col;
//     if (interp->cur_file != NULL) { info.file = julie_charptr_dup(julie_get_string(interp, interp->cur_file)->chars); }
//     info.wanted_type   = wanted;
//     info.got_type      = got;
    julie_error(interp, &info);
}

static void julie_make_lookup_error(Julie_Interp *interp, Julie_Value *value, const Julie_String_ID id) {
    Julie_Error_Info info;
    memset(&info, 0, sizeof(info));
//     info.interp        = interp;
    info.status        = JULIE_ERR_LOOKUP;
//     info.location.line = value->line;
//     info.location.col  = value->col;
//     if (interp->cur_file != NULL) { info.file = julie_charptr_dup(julie_get_string(interp, interp->cur_file)->chars); }
//     info.sym           = julie_charptr_dup(julie_get_string(interp, id)->chars);
    julie_error(interp, &info);
}

static void julie_make_bind_error(Julie_Interp *interp, Julie_Value *value, Julie_Status status, Julie_String_ID id) {
    Julie_Error_Info info;
    memset(&info, 0, sizeof(info));
//     info.interp        = interp;
    info.status        = status;
//     info.location.line = value->line;
//     info.location.col  = value->col;
//     if (interp->cur_file != NULL) { info.file = julie_charptr_dup(julie_get_string(interp, interp->cur_file)->chars); }
//     info.sym           = id == NULL ? NULL : julie_charptr_dup(julie_get_string(interp, id)->chars);
    julie_error(interp, &info);
}

static void julie_make_lval_error(Julie_Interp *interp, Julie_Value *value, Julie_Status status) {
    Julie_Error_Info info;
    memset(&info, 0, sizeof(info));
//     info.interp        = interp;
    info.status        = status;
//     info.location.line = value->line;
//     info.location.col  = value->col;
//     if (interp->cur_file != NULL) { info.file = julie_charptr_dup(julie_get_string(interp, interp->cur_file)->chars); }
    julie_error(interp, &info);
}

static void julie_make_bad_index_error(Julie_Interp *interp, Julie_Value *value, Julie_Value *bad_index) {
    Julie_Error_Info info;
    memset(&info, 0, sizeof(info));
//     info.interp        = interp;
    info.status        = JULIE_ERR_BAD_INDEX;
//     info.location.line = value->line;
//     info.location.col  = value->col;
//     if (interp->cur_file != NULL) { info.file = julie_charptr_dup(julie_get_string(interp, interp->cur_file)->chars); }
//     info.bad_index     = bad_index;
    julie_error(interp, &info);
}

/*********************************************************
 *                        Parsing                        *
 *********************************************************/

#define PARSE_ERR_RET(_interp, _status, _line, _col)                  \
do {                                                                  \
    if ((_status) != JULIE_SUCCESS) {                                  \
        julie_make_parse_error((_interp), (_line), (_col), (_status)); \
    }                                                                 \
    return (_status);                                                 \
} while (0)

static inline int julie_is_space(int c) {
    unsigned char d = c - 9;
    return (0x80001FU >> (d & 31)) & (1U >> (d >> 5));
}

static inline int julie_is_digit(int c) {
    return (unsigned int)(('0' - 1 - c) & (c - ('9' + 1))) >> (sizeof(c) * 8 - 1);
}

static inline int julie_is_alpha(int c) {
    return (unsigned int)(('a' - 1 - (c | 32)) & ((c | 32) - ('z' + 1))) >> (sizeof(c) * 8 - 1);
}

static inline int julie_is_alnum(int c) {
    return julie_is_alpha(c) || julie_is_digit(c);
}

typedef enum {
    JULIE_TK_NONE,
    JULIE_TK_LPAREN,
    JULIE_TK_RPAREN,
    JULIE_TK_SYMBOL,
    JULIE_TK_SINT,
    JULIE_TK_HEX,
    JULIE_TK_FLOAT,
    JULIE_TK_STRING,
    JULIE_TK_EOS_ERR,
    JULIE_TK_UNEXPECTED_ERR,
} Julie_Token;

#define MORE_INPUT(_cxt)    ((_cxt)->cursor < (_cxt)->end)
#define PEEK_CHAR(_cxt, _c) ((_c) = (MORE_INPUT(_cxt) ? (*(_cxt)->cursor) : 0))
#define NEXT(_cxt)          ((_cxt)->cursor += 1)
#define SPC(_c)             (julie_is_space(_c))
#define DIG(_c)             (julie_is_digit(_c))
#define HEX(_c)             (julie_is_digit(_c) || ('a' <= (_c) && (_c) <= 'f') || ('A' <= (_c) && (_c) <= 'F'))

static Julie_Token julie_parse_token(Julie_Parse_Context *cxt) {
    int         c;
    int         last;
    const char *start;

    if (!PEEK_CHAR(cxt, c)) { return JULIE_TK_NONE; }

    if (c == '(') {
        NEXT(cxt);
        return JULIE_TK_LPAREN;
    } else if (c == ')') {
        NEXT(cxt);
        return JULIE_TK_RPAREN;
    } else if (c == '"') {
        do {
            if (c == '\n') { return JULIE_TK_EOS_ERR; }
            last = c;
            NEXT(cxt);
        } while (PEEK_CHAR(cxt, c) && (c != '"' || last == '\\'));

        NEXT(cxt);

        return JULIE_TK_STRING;
    } else if (c == '-' && ((cxt->cursor + 1) < cxt->end) && DIG(*(cxt->cursor + 1))) {
        NEXT(cxt);
        PEEK_CHAR(cxt, c);
        goto digits;

    } else if (DIG(c)) {
digits:;
        if (c == '0') {
            NEXT(cxt);
            if (PEEK_CHAR(cxt, c) && c == 'x') {
                NEXT(cxt);
                if (!PEEK_CHAR(cxt, c) || !HEX(c)) {
                    return JULIE_TK_UNEXPECTED_ERR;
                }

                do {
                    NEXT(cxt);
                } while (PEEK_CHAR(cxt, c) && HEX(c));

                return JULIE_TK_HEX;
            }
        }

        while (PEEK_CHAR(cxt, c) && DIG(c)) { NEXT(cxt); }
        if (PEEK_CHAR(cxt, c) == '.') {
            NEXT(cxt);
            while (PEEK_CHAR(cxt, c) && DIG(c)) { NEXT(cxt); }
            return JULIE_TK_FLOAT;
        }

        return JULIE_TK_SINT;
    }

    start = cxt->cursor;

    while (PEEK_CHAR(cxt, c)
    &&     !SPC(c)
    &&     c != '#'
    &&     c != '('
    &&     c != ')') {

        NEXT(cxt);
    }

    if (cxt->cursor > start) {
        return JULIE_TK_SYMBOL;
    }

    return JULIE_TK_NONE;
}

static int julie_trim_leading_ws(Julie_Parse_Context *cxt) {
    int w;
    int c;

    w = 0;

    while (PEEK_CHAR(cxt, c) && c != '\n' && SPC(c)) {
        NEXT(cxt);
        w += 1;
    }

    return w;
}

static int julie_consume_comment(Julie_Parse_Context *cxt) {
    int c;

    if (PEEK_CHAR(cxt, c) && c == '#') {
        NEXT(cxt);
        while (PEEK_CHAR(cxt, c)) {
            if (c == '\n') { break; }
            NEXT(cxt);
        }
        return 1;
    }

    return 0;
}

static Julie_Value *julie_push_list(Julie_Parse_Context *cxt) {
    Julie_Interp            *interp;
    Julie_Value             *value;
    Julie_Source_Value_Info  info;

    interp = cxt->interp;
    (void)interp;

    value = JULIE_NEW();
    value->type = JULIE_LIST;
    value->list = JULIE_ARRAY_INIT;
    JULIE_ARRAY_PUSH(cxt->parse_stack, value);

    info.ind  = cxt->ind;
    info.line = cxt->line;
    info.col  = cxt->col;

    hash_table_insert(cxt->interp->source_info, value, info);

    return value;
}

static Julie_Status julie_parse_next_value(Julie_Parse_Context *cxt, Julie_Value **valout, Julie_Token *tkout) {
    int                      status;
    Julie_Value             *val;
    int                      start_col;
    const char              *tk_start;
    Julie_Token              tk;
    const char              *tk_end;
    Julie_Value             *top;
    Julie_Value             *child;
    int                      c;
    char                    *sbuff;
    unsigned long long       slen;
    char                     tk_copy[128];
    long long                s;
    unsigned long long       u;
    double                   d;
    Julie_Source_Value_Info  info;

    status  = JULIE_SUCCESS;
    val     = NULL;
    *valout = NULL;
    *tkout  = JULIE_TK_NONE;

    cxt->col += julie_trim_leading_ws(cxt);
    if (julie_consume_comment(cxt)) { goto out; }

    tk_start = cxt->cursor;
    if ((tk = julie_parse_token(cxt)) == JULIE_TK_NONE) { goto out; }
    if (tk == JULIE_TK_UNEXPECTED_ERR) {
        status = JULIE_ERR_UNEXPECTED_TOK;
        goto out;
    }
    tk_end = cxt->cursor;

    start_col = cxt->col;

    if (tk == JULIE_TK_LPAREN) {
        julie_push_list(cxt);
        val = top = julie_array_top(cxt->parse_stack);

        cxt->col += tk_end - tk_start;

        cxt->plevel += 1;

        child = NULL;
        while ((status = julie_parse_next_value(cxt, &child, tkout)) == JULIE_SUCCESS && child != NULL) {
            JULIE_ARRAY_PUSH(top->list, child);
        }

        if (status != JULIE_SUCCESS) {
            PARSE_ERR_RET(cxt->interp, status, cxt->line, cxt->col);
        }

        if (*tkout != JULIE_TK_RPAREN) {
            PARSE_ERR_RET(cxt->interp, JULIE_ERR_MISSING_RPAREN, cxt->line, cxt->col);
        }

        cxt->plevel -= 1;

        *tkout = JULIE_TK_LPAREN;

        julie_array_pop(cxt->parse_stack);

        goto out_val;
    } else if (tk == JULIE_TK_RPAREN) {
        if (cxt->plevel <= 0) {
            PARSE_ERR_RET(cxt->interp, JULIE_ERR_EXTRA_RPAREN, cxt->line, cxt->col);
        }

        *tkout = JULIE_TK_RPAREN;

        cxt->col += tk_end - tk_start;
        goto out;
    }

    *tkout = tk;

    cxt->col += tk_end - tk_start;

    switch (tk) {
        case JULIE_TK_SYMBOL:
            if (tk_end - tk_start == 3 && strncmp(tk_start, "nil", tk_end - tk_start) == 0) {
                val = julie_nil_value(cxt->interp);
            } else {
                sbuff = alloca(tk_end - tk_start + 1);
                memcpy(sbuff, tk_start, tk_end - tk_start);
                sbuff[tk_end - tk_start] = 0;
                val = julie_symbol_value(cxt->interp, julie_get_string_id(cxt->interp, sbuff));
            }
            break;
        case JULIE_TK_STRING:
            JULIE_ASSERT(tk_start[0] == '"' && "string doesn't start with quote");
            tk_start += 1;

            sbuff = alloca(tk_end - tk_start + 1);
            slen  = 0;

            for (; tk_start < tk_end; tk_start += 1) {
                c = *tk_start;

                if (c == '"') { break; }
                if (c == '\\') {
                    tk_start += 1;
                    if (tk_start < tk_end) {
                        switch (*tk_start) {
                            case '\\':
                                break;
                            case 'n':
                                c = '\n';
                                break;
                            case 'r':
                                c = '\r';
                                break;
                            case 't':
                                c = '\t';
                                break;
                            case '"':
                                c = '"';
                                break;
                            default:
                                sbuff[slen]  = c;
                                slen        += 1;
                                c            = *tk_start;
                                goto add_char;
                        }
                    }
                    goto add_char;
                } else {
add_char:;
                    sbuff[slen] = c;
                }
                slen += 1;
            }

            sbuff[slen] = 0;

            val = julie_interned_string_value(cxt->interp, julie_get_string_id(cxt->interp, sbuff));
            break;
        case JULIE_TK_SINT:
            strncpy(tk_copy, tk_start, tk_end - tk_start);
            tk_copy[tk_end - tk_start] = 0;
            sscanf(tk_copy, "%lld", &s);
            val = julie_sint_value(cxt->interp, s);
            break;
        case JULIE_TK_HEX:
            strncpy(tk_copy, tk_start, tk_end - tk_start);
            tk_copy[tk_end - tk_start] = 0;
            sscanf(tk_copy, "%llx", &u);
            val = julie_uint_value(cxt->interp, u);
            break;
        case JULIE_TK_FLOAT:
            strncpy(tk_copy, tk_start, tk_end - tk_start);
            tk_copy[tk_end - tk_start] = 0;
            sscanf(tk_copy, "%lg", &d);
            val = julie_float_value(cxt->interp, d);
            break;
        case JULIE_TK_EOS_ERR:
            PARSE_ERR_RET(cxt->interp, JULIE_ERR_UNEXPECTED_EOS, cxt->line, start_col + (tk_end - tk_start));
            break;
        default:
            break;
    }

out_val:;

    JULIE_ASSERT(val != NULL);

    info.line = cxt->line;
    info.col  = start_col;
    info.ind  = cxt->ind;

    hash_table_insert(cxt->interp->source_info, val, info);

    *valout = val;

out:;
    return status;
}

static Julie_Status julie_parse_line(Julie_Parse_Context *cxt) {
    int                      status;
    int                      c;
    Julie_Value             *top;
    Julie_Source_Value_Info *info;
    Julie_Value             *val;
    Julie_Token              tk;

    status = JULIE_SUCCESS;

    cxt->ind = julie_trim_leading_ws(cxt);
    cxt->col = 1 + cxt->ind;

    if (!PEEK_CHAR(cxt, c))                      { goto done; }
    if (c == '\n' || julie_consume_comment(cxt)) { goto eol;  }

    while ((top = julie_array_top(cxt->parse_stack)) != NULL
    &&     (info = julie_get_source_value_info(cxt->interp, top))
    &&     cxt->ind <= info->ind) {

        julie_array_pop(cxt->parse_stack);
    }

    val = julie_push_list(cxt);
    if (top == NULL) {
        JULIE_ARRAY_PUSH(cxt->interp->roots, val);
    } else {
        JULIE_ARRAY_PUSH(top->list, val);
    }
    top = val;

    val = NULL;
    while ((status = julie_parse_next_value(cxt, &val, &tk)) == JULIE_SUCCESS && val != NULL) {
        JULIE_ARRAY_PUSH(top->list, val);
    }

    if (status != JULIE_SUCCESS) {
        PARSE_ERR_RET(cxt->interp, status, cxt->line, cxt->col);
    }

eol:;
    if (PEEK_CHAR(cxt, c)) {
        if (c == '\n') {
            NEXT(cxt);
        } else {
            PARSE_ERR_RET(cxt->interp, JULIE_ERR_UNEXPECTED_TOK, cxt->line, cxt->col);
        }
    }

done:;
    return status;
}

Julie_Status julie_parse(Julie_Interp *interp, const char *str, int size) {

    Julie_Parse_Context cxt;
    Julie_Status        status;

    memset(&cxt, 0, sizeof(cxt));

    cxt.interp      = interp;
    cxt.cursor      = str;
    cxt.end         = str + size;
    cxt.parse_stack = JULIE_ARRAY_INIT;

    status = JULIE_SUCCESS;


    while (status == JULIE_SUCCESS && MORE_INPUT(&cxt)) {
        cxt.line += 1;
        status = julie_parse_line(&cxt);
    }

    julie_array_free(cxt.parse_stack);

    return status;
}




/*********************************************************
 *                       Printing                        *
 *********************************************************/


static void _julie_string_print(Julie_Interp *interp, char **buff, int *len, int *cap, const Julie_Value *value, unsigned ind, int flags) {
    unsigned            i;
    char                b[128];
    const Julie_String  *string;
    Julie_Value         *child;
//     Julie_Value         *key;
    Julie_Value        **val;
    Julie_String_ID      sym;
    Julie_String_ID      fsym;
    union {
        Julie_Fn         f;
        void           *v;
    }                   prfn;

#define PUSHC(_c)                               \
do {                                            \
    if (*len == *cap) {                         \
        *cap <<= 1;                             \
        *buff = JULIE_REALLOC(*buff, *cap);     \
    }                                           \
    (*buff)[*len]  = (_c);                      \
    *len        += 1;                           \
} while (0)

#define PUSHSN(_s, _n)                          \
do {                                            \
    for (unsigned _i = 0; _i < (_n); _i += 1) { \
        PUSHC((_s)[_i]);                        \
    }                                           \
} while (0)

#define PUSHS(_s) PUSHSN((_s), strlen(_s))

    if (flags & JULIE_MULTILINE) {
        for (i = 0; i < ind; i += 1) { PUSHC(' '); }
    }

    switch (value->type) {
        case JULIE_NIL:
            PUSHS("nil");
            break;
        case JULIE_SINT:
            snprintf(b, sizeof(b), "%lld", value->sint);
            PUSHS(b);
            break;
        case JULIE_UINT:
            snprintf(b, sizeof(b), "0x%llx", value->uint);
            PUSHS(b);
            break;
        case JULIE_FLOAT:
            snprintf(b, sizeof(b), "%f", value->floating);
            PUSHS(b);
            break;
        case JULIE_STRING:
            string = julie_get_string(interp, value->string_id);
            if (flags & JULIE_NO_QUOTE) {
                PUSHSN(string->chars, string->len);
            } else {
                PUSHC('"');
                PUSHSN(string->chars, string->len);
                PUSHC('"');
            }
            break;
        case JULIE_SYMBOL:
            string = julie_get_string(interp, value->symbol_id);
            PUSHS(string->chars);
            break;
        case JULIE_LIST:
            PUSHC('(');
            PUSHC((flags & JULIE_MULTILINE) ? '\n' : ' ');
            ARRAY_FOR_EACH(value->list, child) {
                _julie_string_print(interp, buff, len, cap, child, (flags & JULIE_MULTILINE) ? ind + 2 : 0, flags & ~JULIE_NO_QUOTE);
                PUSHC((flags & JULIE_MULTILINE) ? '\n' : ' ');
            }
            if (flags & JULIE_MULTILINE) {
                for (i = 0; i < ind; i += 1) { PUSHC(' '); }
            }
            PUSHC(')');
            break;
#if 0
        case JULIE_OBJECT:
            PUSHC('{');
            PUSHC((flags & JULIE_MULTILINE) ? '\n' : ' ');
            hash_table_traverse((_Julie_Object)value->object, key, val) {
                _julie_string_print(interp, buff, len, cap, key, (flags & JULIE_MULTILINE) ? ind + 2 : 0, flags & ~JULIE_NO_QUOTE);
                PUSHC(':');
                _julie_string_print(interp, buff, len, cap, *val, (flags & JULIE_MULTILINE) ? ind + 2 : 0, flags & ~JULIE_NO_QUOTE);
                PUSHC((flags & JULIE_MULTILINE) ? '\n' : ' ');
            }
            if (flags & JULIE_MULTILINE) {
                for (i = 0; i < ind; i += 1) { PUSHC(' '); }
            }
            PUSHC('}');
            break;
#endif
        case JULIE_FN:
            fsym = NULL;

            if (value->builtin) {
                fsym = NULL;
                hash_table_traverse(interp->global_symtab, sym, val) {
                    if ((*val)->type == JULIE_FN
                    &&  (*val)->builtin
                    &&  (*val)->builtin_fn == value->builtin_fn) {

                        fsym = sym;
                        break;
                    }
                }
                prfn.f = value->builtin_fn;
                if (fsym != NULL) {
                    snprintf(b, sizeof(b), "<fn@%p %s>", prfn.v, fsym->chars);
                } else {
                    snprintf(b, sizeof(b), "<fn@%p>", prfn.v);
                }
                PUSHS(b);
#if 0
            } else {
                for (i = julie_len(interp->local_symtab_stack); i > 0; i -= 1) {
                    hash_table_traverse((_Julie_Symbol_Table)julie_elem(interp->local_symtab_stack, i - 1), sym, val) {
                        if ((*val) == value) {
                            fsym = sym;
                            goto found_fsym;
                        }
                    }
                }
                hash_table_traverse(interp->symtab, sym, val) {
                    if ((*val) == value) {
                        fsym = sym;
                        goto found_fsym;
                    }
                }
                goto print_tree; /* Not sure that this could ever happen, but just to be safe. */

found_fsym:;
                snprintf(b, sizeof(b), "<fn@%p> %s", (void*)value, fsym->chars);
                PUSHS(b);
                break;

print_tree:;
                if (flags & JULIE_MULTILINE) {
                    _julie_string_print(interp, buff, len, cap, value->list->data[0], ind, flags & ~JULIE_NO_QUOTE);
                    for (i = 1; i < julie_len(value->list); i += 1) {
                        PUSHC('\n');
                        _julie_string_print(interp, buff, len, cap, value->list->data[i], ind + 2, flags & ~JULIE_NO_QUOTE);
                    }
                } else {
                    PUSHC('(');
                    _julie_string_print(interp, buff, len, cap, value->list->data[0], ind, flags & ~JULIE_NO_QUOTE);
                    for (i = 1; i < julie_len(value->list); i += 1) {
                        PUSHC(' ');
                        _julie_string_print(interp, buff, len, cap, value->list->data[i], 0, flags & ~JULIE_NO_QUOTE);
                    }
                    PUSHC(')');
                }
#endif
            }
            break;
        default:
            JULIE_ASSERT(0);
            break;

    }

    PUSHC(0);
    *len -= 1;
}

char *julie_to_string(Julie_Interp *interp, const Julie_Value *value, int flags) {
    char *buff;
    int   len;
    int   cap;

    buff = JULIE_MALLOC(16);
    len  = 0;
    cap  = 16;

    _julie_string_print(interp, &buff, &len, &cap, value, 0, flags);

    return buff;
}

static void julie_output(Julie_Interp *interp, const char *s, int n_bytes) {
//     if (interp->output_callback == NULL) {
        printf("%.*s", n_bytes, s);
        fflush(stdout);
//     } else {
//         interp->output_callback(s, n_bytes);
//     }
}

static void julie_print(Julie_Interp *interp, Julie_Value *value, unsigned ind) {
    char *buff;
    int   len;
    int   cap;

    buff = JULIE_MALLOC(16);
    len  = 0;
    cap  = 16;

    _julie_string_print(interp, &buff, &len, &cap, value, ind, JULIE_NO_QUOTE | JULIE_MULTILINE);
    julie_output(interp, buff, len);
    JULIE_FREE(buff);
}

static void julie_println(Julie_Interp *interp, Julie_Value *value, unsigned ind) {
    char *buff;
    int   len;
    int   cap;

    buff = JULIE_MALLOC(16);
    len  = 0;
    cap  = 16;

    _julie_string_print(interp, &buff, &len, &cap, value, ind, JULIE_NO_QUOTE);
    julie_output(interp, buff, len);
    julie_output(interp, "\n", 1);
    JULIE_FREE(buff);
}


/*********************************************************
 *                       Builtins                        *
 *********************************************************/

static Julie_Status julie_eval(Julie_Interp *interp, Julie_Value *value, Julie_Value **result);
static Julie_Value *julie_copy(Julie_Interp *interp, Julie_Value *value);

static Julie_Status julie_args(Julie_Interp *interp, Julie_Value *expr, const char *legend, unsigned n_values, Julie_Value **values, ...) {
    Julie_Status   status;
    va_list       args;
    unsigned      count;
    unsigned      i;
    int           no_eval;
    int           deep_copy;
    int           c;
    Julie_Value   *v;
    Julie_Value  **ve_ptr;
    va_list       cleanup_args;
    unsigned      j;
    Julie_Value   *cpy;
    int           t;

    status = JULIE_SUCCESS;

    va_start(args, values);

    count = 0;
    for (i = 0; i < strlen(legend); i += 1) {
        count += legend[i] != '-' && legend[i] != '!';
    }

    i         = 0;
    no_eval   = 0;
    deep_copy = 0;
    while ((c = *legend)) {
        if (c == '-') {
            no_eval = 1;
            goto nextc;
        }
        if (c == '!') {
            deep_copy = 1;
            goto nextc;
        }

        if (i == n_values) {
            status = JULIE_ERR_ARITY;
            julie_make_arity_error(interp, expr, count, n_values, 0);
            goto out;
        }

        v = values[i];

        ve_ptr = va_arg(args, Julie_Value**);

        if (no_eval) {
            if (deep_copy) {
                *ve_ptr = julie_force_copy(interp, v);
            } else {
                *ve_ptr = julie_copy(interp, v);
            }
        } else {
            status = julie_eval(interp, v, ve_ptr);
            if (status != JULIE_SUCCESS) {
                va_start(cleanup_args, values);
                for (j = 0; j < i; j += 1) {
                    ve_ptr = va_arg(cleanup_args, Julie_Value**);
                    julie_free_value(interp, *ve_ptr);
                    *ve_ptr = NULL;
                }
                va_end(cleanup_args);
                goto out;
            }
            if (deep_copy) {
                cpy = julie_force_copy(interp, *ve_ptr);
                julie_free_value(interp, *ve_ptr);
                *ve_ptr = cpy;
            }
        }

//         (*ve_ptr)->line = v->line;
//         (*ve_ptr)->col  = v->col;

        switch (c) {
            case '0': t = JULIE_NIL;             break;
            case 's': t = JULIE_STRING;          break;
            case '$': t = JULIE_SYMBOL;          break;
            case 'l': t = JULIE_LIST;            break;
            case 'o': t = JULIE_OBJECT;          break;
            case 'i': t = _JULIE_INTEGER;        break;
            case 'n': t = _JULIE_NUMBER;         break;
            case '#': t = _JULIE_LIST_OR_OBJECT; break;
            case 'k': t = _JULIE_KEYLIKE;        break;
            case '*': t = -1;                    break;
            default:  t = JULIE_UNKNOWN;         break;
        }

        if (t == _JULIE_INTEGER && JULIE_TYPE_IS_INTEGER((*ve_ptr)->type)) {

            /* Fine. */
        } else if (t == _JULIE_NUMBER && JULIE_TYPE_IS_NUMBER((*ve_ptr)->type)) {

            /* Fine. */
        } else if (t == _JULIE_LIST_OR_OBJECT
        &&  ((*ve_ptr)->type == JULIE_LIST || (*ve_ptr)->type == JULIE_OBJECT)) {

            /* Fine. */
        } else if (t == _JULIE_KEYLIKE && JULIE_TYPE_IS_KEYLIKE((*ve_ptr)->type)) {

            /* Fine. */
        } else if (t >= 0 && (*ve_ptr)->type != t) {
            status = JULIE_ERR_TYPE;
            julie_make_type_error(interp, v, t, (*ve_ptr)->type);
            goto out;
        }

        i += 1;

        no_eval = deep_copy = 0;

nextc:;
        legend += 1;
    }

    if (i != n_values) {
        status = JULIE_ERR_ARITY;
        julie_make_arity_error(interp, expr, count, n_values, 0);
        goto out;
    }

out:;
    va_end(args);

    return status;
}

static Julie_Status julie_builtin_id(Julie_Interp *interp, Julie_Value *expr, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *value;
    Julie_Value  *ev;
    Julie_Value  *lookup;

    status = julie_args(interp, expr, "-*", n_values, values, &value);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    switch (value->type) {
        case JULIE_SINT:
        case JULIE_UINT:
        case JULIE_FLOAT:
        case JULIE_STRING:
        case JULIE_LIST:
        case JULIE_OBJECT:
            status = julie_eval(interp, value, &ev);
            if (status != JULIE_SUCCESS) {
                *result = NULL;
                goto out_free;
            }
            break;
        case JULIE_SYMBOL:
            lookup = julie_lookup(interp, value->symbol_id);
            if (lookup == NULL) {
                status = JULIE_ERR_LOOKUP;
                julie_make_lookup_error(interp, value, value->symbol_id);
                *result = NULL;
                goto out_free;
            }

            ev = julie_copy(interp, lookup);
            break;
        default:
            JULIE_ASSERT(0);
            break;
    }

    *result = ev;

out_free:;
    julie_free_value(interp, value);

out:;
    return status;
}

static Julie_Status julie_builtin_quote(Julie_Interp *interp, Julie_Value *expr, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *value;

    status = julie_args(interp, expr, "-*", n_values, values, &value);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    *result = julie_copy(interp, value);

    julie_free_value(interp, value);

out:;
    return status;
}

static Julie_Status julie_get_lval(Julie_Interp *interp, Julie_Value *value, Julie_Value **result) {
    Julie_Status status;

    if ((status = julie_eval(interp, value, result)) != JULIE_SUCCESS) { goto out; }

    if ((*result)->rc == 0) {
        julie_free_value(interp, *result);
        *result = NULL;
        status = JULIE_ERR_NOT_LVAL;
        julie_make_lval_error(interp, value, status);
        goto out;
    }

out:;
    return status;
}

static Julie_Status julie_builtin_assign(Julie_Interp *interp, Julie_Value *expr, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status     status;
    Julie_Value     *l;
    Julie_Value     *rval;
    Julie_String_ID  id;
//     Julie_Value         new_rval;
//     Julie_Value        *lval;
//     Julie_Value        *cpy;

    *result = NULL;

    if ((status = julie_args(interp, expr, "-**", n_values, values, &l, &rval)) != JULIE_SUCCESS) { goto out; }

    if (l->type == JULIE_SYMBOL) {
        id = l->interned_string ? l->symbol_id : julie_get_string_id(interp, l->cstring);

        if (julie_array_len(interp->local_symtab_stack) == 0) {
            status = julie_bind(interp, id, &rval);
        } else {
            status = julie_bind_local(interp, id, &rval);
        }
        if (status != JULIE_SUCCESS) {
            julie_make_bind_error(interp, expr, status, id);
            julie_free_value(interp, rval);
            goto out;
        }
//     } else {
// into_lval:;
//         status = julie_get_lval(interp, l, lval);
//         if (status != JULIE_SUCCESS) {
//             julie_free_value(interp, rval);
//             goto out;
//         }

//         julie_force_copy(interp, lval, rval);

//         julie_free_value(interp, rval);
    }

    julie_free_value(interp, rval);
    julie_free_value(interp, l);

    *result = rval;

out:;
    return status;
}

static Julie_Status julie_builtin_add(Julie_Interp *interp, Julie_Value *expr, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *a;
    Julie_Value  *b;

    *result = NULL;

    if ((status = julie_args(interp, expr, "nn", n_values, values, &a, &b)) != JULIE_SUCCESS) { goto out; }

    if (a->type == JULIE_SINT && b->type == JULIE_SINT) {
        *result = julie_sint_value(interp, a->sint + b->sint);
    } else if (a->type == JULIE_SINT && b->type == JULIE_UINT) {
        *result = julie_uint_value(interp, (unsigned long long)a->sint + b->uint);
    } else if (a->type == JULIE_SINT && b->type == JULIE_FLOAT) {
        *result = julie_float_value(interp, (double)a->sint + b->floating);
    } else if (a->type == JULIE_UINT && b->type == JULIE_UINT) {
        *result = julie_uint_value(interp, a->uint + b->uint);
    } else if (a->type == JULIE_UINT && b->type == JULIE_SINT) {
        *result = julie_uint_value(interp, a->uint + (unsigned long long)b->sint);
    } else if (a->type == JULIE_UINT && b->type == JULIE_FLOAT) {
        *result = julie_float_value(interp, (double)a->uint + b->floating);
    } else if (a->type == JULIE_FLOAT && b->type == JULIE_FLOAT) {
        *result = julie_float_value(interp, a->floating + b->floating);
    } else if (a->type == JULIE_FLOAT && b->type == JULIE_SINT) {
        *result = julie_float_value(interp, a->floating + (double)b->sint);
    } else if (a->type == JULIE_FLOAT && b->type == JULIE_UINT) {
        *result = julie_float_value(interp, a->floating + (double)b->uint);
    } else {
        JULIE_ASSERT(0 && "bad number type");
    }

    julie_free_value(interp, a);
    julie_free_value(interp, b);

out:;
    return status;
}

static Julie_Status julie_builtin_list(Julie_Interp *interp, Julie_Value *expr, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *list;
    unsigned      i;
    Julie_Value  *it;
    Julie_Value  *ev;
    Julie_Value  *tmp;

    (void)expr;

    status = JULIE_SUCCESS;

    list = julie_list_value(interp);

    for (i = 0; i < n_values; i += 1) {
        it     = values[i];
        status = julie_eval(interp, it, &ev);
        if (status != JULIE_SUCCESS) {
            *result = NULL;
            goto out_free;
        }

        if (ev->rc != 0) {
            tmp = julie_force_copy(interp, ev);
            julie_free_value(interp, ev);
            ev = tmp;
        }

        JULIE_ARRAY_PUSH(list->list, ev);
    }

    *result = list;
    goto out;

out_free:;
    julie_free_value(interp, list);

out:;
    return status;
}

static Julie_Status julie_builtin_elem(Julie_Interp *interp, Julie_Value *expr, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status        status;
    Julie_Value        *list;
    Julie_Value        *idx;
    unsigned long long  i;
    Julie_Value        *val;

    status = julie_args(interp, expr, "li", n_values, values, &list, &idx);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    if (idx->type == JULIE_SINT) {
        i = idx->sint;
    } else if (idx->type == JULIE_UINT) {
        i = idx->uint;
    } else {
        JULIE_ASSERT(0 && "bad number type");
    }

    if (i >= julie_array_len(list->list)) {
        status = JULIE_ERR_BAD_INDEX;
        julie_make_bad_index_error(interp, idx, julie_copy(interp, idx));
        *result = NULL;
        goto out_free;
    }

    val = julie_array_elem(list->list, i);
    if (val->rc == 0) {
        val->rc = list->rc > 0;
    }

    *result = julie_copy(interp, val);

out_free:;
    julie_free_value(interp, idx);
    julie_free_value(interp, list);

out:;
    return status;
}

/*********************************************************
 *                        Interp                         *
 *********************************************************/

static Julie_Status julie_eval(Julie_Interp *interp, Julie_Value *value, Julie_Value **result);

static Julie_Status julie_apply(Julie_Interp *interp, Julie_Value *list, Julie_Value **result) {
    Julie_Status               status;
//     Julie_String_ID            save_file;
//     Julie_Backtrace_Entry     *bt_entry;
    int            infix;
    Julie_Value *maybe_infix_fn;
    Julie_Value   *lookup;
    Julie_Value *fn;
    unsigned       i;
    Julie_Array   *args = JULIE_ARRAY_INIT;
    unsigned       n_values;
    Julie_Value  **values;

    status = JULIE_SUCCESS;

    if (julie_array_len(list->list) == 0) {
        (*result)->type = JULIE_NIL;
        return JULIE_SUCCESS;
    }

    /* Get the function value. */
    fn = NULL;

    /* Check for infix. */
    infix = 0;
    if (julie_array_len(list->list) == 3) {
        maybe_infix_fn = julie_array_elem(list->list, 1);
        if (maybe_infix_fn->type == JULIE_SYMBOL) {
            lookup = julie_lookup(interp, maybe_infix_fn->symbol_id);
            if (lookup != NULL && lookup->type == JULIE_FN && lookup->infix) {
                fn    = lookup;
                infix = 1;
            }
        } else if (maybe_infix_fn->type == JULIE_FN && maybe_infix_fn->infix) {
            fn    = maybe_infix_fn;
            infix = 1;
        }
    }

    /* Function value is first element of list -- eval. */
    if (!infix) {
        status = julie_eval(interp, julie_array_elem(list->list, 0), &fn);
        if (status != JULIE_SUCCESS) {
            goto out;
        }
    }

    /* Get argument values. */
    if (infix) {
        for (i = 0; i < julie_array_len(list->list); i += 1) {
            if (i == 1) { continue; }
            JULIE_ARRAY_PUSH(args, julie_array_elem(list->list, i));
        }
    } else {
        for (i = 1; i < julie_array_len(list->list); i += 1) {
            JULIE_ARRAY_PUSH(args, julie_array_elem(list->list, i));
        }
    }

#if 0
    /* Push a backtrace frame. */
    save_file = interp->cur_file;

    bt_entry = JULIE_MALLOC(sizeof(*bt_entry));

    bt_entry->file = interp->cur_file;
    bt_entry->fn   = fn;
    bt_entry->line = list->line;
    bt_entry->col  = list->col;
    JULIE_PUSH(interp->backtrace, bt_entry);
#endif


    n_values = julie_array_len(args);
    values   = (Julie_Value**)args->data;

    /* Evaluate function application. */
    switch (fn->type) {
        case JULIE_FN:
            if (fn->builtin) {
                status = fn->builtin_fn(interp, list, n_values, values, result);
            } else {
                JULIE_ASSERT(0 && "TODO");
            }
            break;

        case JULIE_LIST:
            if (n_values == 0) { goto id; }

            n_values = julie_array_len(list->list);
            values   = (Julie_Value**)list->list->data;
            status   = julie_builtin_elem(interp, list, n_values, values, result);
            break;

        default:
            if (n_values == 0) {
id:;
                status = julie_eval(interp, fn, result);
            } else {
                status = JULIE_ERR_BAD_APPLY;
                julie_make_bad_apply_error(interp, fn, fn->type);
            }
            break;
    }


#if 0
    interp->last_popped_builtin_fn = (bt_entry->fn->type == JULIE_FN && bt_entry->fn->builtin)
                                        ? bt_entry->fn->builtin_fn
                                        : NULL;

    julie_pop(interp->backtrace);
    JULIE_FREE(bt_entry);

    interp->cur_file = save_file;
#endif

    julie_free_value(interp, fn);

out:;
    julie_array_free(args);

    return status;
}

static Julie_Status julie_eval(Julie_Interp *interp, Julie_Value *value, Julie_Value **result) {
    Julie_Status     status;
    Julie_String_ID  id;
    Julie_Value     *lookup;

    status = JULIE_SUCCESS;

//     if (interp->eval_callback != NULL) {
//         status = interp->eval_callback(value);
//         if (status != JULIE_SUCCESS) {
//             julie_make_interp_error(interp, value, status);
//             goto out;
//         }
//     }

    *result = NULL;

    if (value->type == JULIE_LIST) {
        status = julie_apply(interp, value, result);
    } else {
        if (value->type == JULIE_SYMBOL) {
            if (value->interned_string) {
                id = value->symbol_id;
            } else {
                id = julie_get_string_id(interp, value->cstring);
            }
            if ((lookup = julie_lookup(interp, id)) == NULL) {
                status = JULIE_ERR_LOOKUP;
                julie_make_lookup_error(interp, value, id);
                goto out;
            }

            value = lookup;
        }

        *result = julie_copy(interp, value);
    }

out:;
    return status;
}


Julie_Status julie_set_error_callback(Julie_Interp *interp, Julie_Error_Callback cb) {
    interp->error_callback = cb;
    return JULIE_SUCCESS;
}

// Julie_Status julie_set_output_callback(Julie_Interp *interp, Julie_Output_Callback cb) {
//     interp->output_callback = cb;
//     return JULIE_SUCCESS;
// }

// Julie_Status julie_set_eval_callback(Julie_Interp *interp, Julie_Eval_Callback cb) {
//     interp->eval_callback = cb;
//     return JULIE_SUCCESS;
// }

Julie_Status julie_set_argv(Julie_Interp *interp, int argc, char **argv) {
    interp->argc = argc;
    interp->argv = argv;
    return JULIE_SUCCESS;
}


Julie_Status julie_init_interp(Julie_Interp *interp) {
    memset(interp, 0, sizeof(*interp));

    interp->strings     = hash_table_make_e(Char_Ptr, Julie_String_ID, julie_charptr_hash, julie_charptr_equ);
    interp->source_info = hash_table_make(Julie_Value_Ptr, Julie_Source_Value_Info, julie_value_ptr_hash);

    interp->global_symtab = hash_table_make(Julie_String_ID, Julie_Value_Ptr, julie_string_id_hash);
    interp->local_symtab_stack = JULIE_ARRAY_INIT;

    interp->roots     = JULIE_ARRAY_INIT;




#define JULIE_BIND_FN(_name, _fn)       julie_bind_fn(interp, julie_get_string_id(interp, (_name)), (_fn))
#define JULIE_BIND_INFIX_FN(_name, _fn) julie_bind_infix_fn(interp, julie_get_string_id(interp, (_name)), (_fn))

    JULIE_BIND_FN(      "`",                     julie_builtin_id);
    JULIE_BIND_FN(      "'",                     julie_builtin_quote);

    JULIE_BIND_INFIX_FN("=",                     julie_builtin_assign);
    JULIE_BIND_INFIX_FN("+",                     julie_builtin_add);
    JULIE_BIND_INFIX_FN("list",                  julie_builtin_list);

    return JULIE_SUCCESS;
}

static void julie_print_symtab(Julie_Interp *interp) {
    char             *buff;
    int               len;
    int               cap;
    Julie_String_ID   id;
    Julie_Value     **valp;

    buff = JULIE_MALLOC(16);
    len  = 0;
    cap  = 16;


    printf("┌ Symbols ───────────────────────────────────────────────────────────\n");
    hash_table_traverse(interp->global_symtab, id, valp) {
        buff[0] = 0;
        len     = 0;
        _julie_string_print(interp, &buff, &len, &cap, *valp, 0, JULIE_NO_QUOTE);
        printf("│ %-10s %p    %-30s %2llu refs\n", julie_get_cstring(interp, id), (void*)*valp, buff, (*valp)->rc);
    }
    printf("\n");

    JULIE_FREE(buff);
}

Julie_Status julie_interp(Julie_Interp *interp) {
    Julie_Status  status;
    Julie_Value  *root;
    Julie_Value  *result;

    status = JULIE_SUCCESS;

    ARRAY_FOR_EACH(interp->roots, root) {
        printf("\033[36m");
        julie_println(interp, root, 0);
        printf("\033[0m");
        result = NULL;
        status = julie_eval(interp, root, &result);
        if (status != JULIE_SUCCESS) {
            goto out;
        }
        if (result) {
            printf("\033[35m");
            printf("  → ");
            printf("\033[0m");
            julie_println(interp, result, 0);
            julie_free_value(interp, result);
        }
        printf("\033[33m");
        julie_print_symtab(interp);
        printf("\033[0m");
    }

out:;
    return status;
}

use_hash_table(Julie_Value_Ptr, int)

static void julie_free_symtab(Julie_Interp *interp, hash_table(Julie_String_ID, Julie_Value_Ptr) symtab) {
    Julie_String_ID   id;
    Julie_Value     **valp;

    hash_table_traverse(symtab, id, valp) {
        if (!julie_symbol_starts_with_ampersand(interp, id)) {
            julie_free_value(interp, *valp);
        }
    }

    hash_table_free(symtab);
}

void julie_free(Julie_Interp *interp) {
    Julie_Value             *it;
    Julie_Value_Store_Block *block;
    Julie_Value_Store_Block *next;
    char                    *key;
    Julie_String_ID         *id;


    ARRAY_FOR_EACH(interp->roots, it) {
        julie_free_value(interp, it);
    }
    julie_array_free(interp->roots);


    julie_free_symtab(interp, interp->global_symtab);

    block = interp->store.head;
    while (block != NULL) {
        next = block->next;
        JULIE_FREE(block);
        block = next;
    }

    hash_table_free(interp->source_info);

    hash_table_traverse(interp->strings, key, id) {
        (void)key;
        julie_free_string((Julie_String*)julie_get_string(interp, *id));
        JULIE_FREE((void*)*id);
    }

    hash_table_free(interp->strings);


    memset(interp, 0, sizeof(*interp));
}


/*********************************************************
 *                         Misc                          *
 *********************************************************/

Julie_Status julie_map_file_into_readonly_memory(const char *path, const char **addr, unsigned long long *size) {
    Julie_Status  status;
    FILE        *f;
    int          fd;
    struct stat  fs;

    status = JULIE_SUCCESS;
    f      = fopen(path, "r");

    if (f == NULL) { status = JULIE_ERR_FILE_NOT_FOUND; goto out; }

    fd = fileno(f);

    if      (fstat(fd, &fs) != 0) { status = JULIE_ERR_FILE_NOT_FOUND; goto out_fclose; }
    else if (S_ISDIR(fs.st_mode)) { status = JULIE_ERR_FILE_IS_DIR;    goto out_fclose; }

    *size = fs.st_size;

    if (*size == 0) {
        *addr = NULL;
        goto out_fclose;
    }

    *addr = mmap(NULL, *size, PROT_READ, MAP_SHARED, fd, 0);

    if (*addr == MAP_FAILED) { status = JULIE_ERR_MMAP_FAILED; goto out_fclose; }

out_fclose:
    fclose(f);

out:
    return status;
}

#undef STATUS_ERR_RET
#undef PARSE_ERR_RET
#undef MORE_INPUT
#undef PEEK_CHAR
#undef SPC
#undef DIG
#undef ALIGN_UP
#undef ALIGN_DOWN
#undef IS_ALIGNED
#undef IS_POWER_OF_TWO
#undef NEXT_POT_2
#undef NEXT_POT_4
#undef NEXT_POT_8
#undef NEXT_POT_16
#undef NEXT_POT_32
#undef NEXT_POT_64
#undef NEXT_POT
#undef CLZ
#undef STR
#undef _STR
#undef CAT2
#undef _CAT2
#undef CAT3
#undef _CAT3
#undef CAT4
#undef _CAT4
#undef hash_table
#undef hash_table_make
#undef hash_table_make_e
#undef hash_table_len
#undef hash_table_free
#undef hash_table_get_key
#undef hash_table_get_val
#undef hash_table_insert
#undef hash_table_delete
#undef hash_table_traverse
#undef _hash_table_slot
#undef hash_table_slot
#undef _hash_table
#undef hash_table
#undef hash_table_pretty_name
#undef _HASH_TABLE_EQU
#undef DEFAULT_START_SIZE_IDX
#undef use_hash_table
#undef STORE_BLOCK_ALIGN
#undef EVAL_STACK_BLOCK_ALIGN
#undef EVAL_STACK_BLOCK_SLOTS

#endif /* JULIE_IMPL */

#endif /* __JULIE_H__ */
