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

typedef struct Julie_Error_Info_Struct Julie_Error_Info;
typedef void (*Julie_Error_Callback)(Julie_Error_Info *info);
typedef struct Julie_Interp_Struct Julie_Interp;
typedef struct Julie_Array_Struct  Julie_Array;
typedef struct Julie_Value_Struct  Julie_Value;

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

    #define ALIGN_UP(x, align)      ((__typeof(x))((((unsigned long long)(x)) + ((unsigned long long)align)) & ~(((unsigned long long)align) - 1ull)))
    #define ALIGN_DOWN(x, align)    ((__typeof(x))(((unsigned long long)(x)) & ~(((unsigned long long)align) - 1ull)))
    #define IS_ALIGNED(x, align)    (!(((unsigned long long)(x)) & (((unsigned long long)align) - 1ull)))
    #define IS_POWER_OF_TWO(x)      ((x) != 0 && IS_ALIGNED((x), (x)))

#define NEXT_POT_2(x)   (           (x) | (           (x) >>  1ull))
#define NEXT_POT_4(x)   ( NEXT_POT_2(x) | ( NEXT_POT_2(x) >>  2ull))
#define NEXT_POT_8(x)   ( NEXT_POT_4(x) | ( NEXT_POT_4(x) >>  4ull))
#define NEXT_POT_16(x)  ( NEXT_POT_8(x) | ( NEXT_POT_8(x) >>  8ull))
#define NEXT_POT_32(x)  (NEXT_POT_16(x) | (NEXT_POT_16(x) >> 16ull))
#define NEXT_POT_64(x)  (NEXT_POT_32(x) | (NEXT_POT_32(x) >> 32ull))
#define NEXT_POT(x)     (NEXT_POT_64((x) - 1ull) + 1ull)

#define CLZ(_val) (__builtin_clzll((_val) | 1ull))

#define BITFIELD_FULL (0xFFFFFFFFFFFFFFFF)

#ifndef JULIE_MALLOC
#define JULIE_MALLOC (malloc)
#endif
#ifndef JULIE_REALLOC
#define JULIE_REALLOC (realloc)
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


static inline unsigned long long julie_value_ptr_hash(Julie_Value *value) {
    return ((unsigned long long)((void*)value)) >> 4;
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

struct Julie_Value_Struct {
    union {
        long long           sint;
        unsigned long long  uint;
        double              floating;
//         Julie_String_ID     string_id;
//         Julie_String_ID     symbol_id;
//         Julie_Object        object;
        Julie_Array        *list;
//         Julie_Fn            builtin_fn;
        Julie_Value        *ref_of;
    };
    unsigned long long      type     :                  4; //  4
//     unsigned long long      in_symtab:                  1; //  5
//     unsigned long long      local    :                  1; //  6
//     unsigned long long      builtin  :                  1; //  7
//     unsigned long long      infix    :                  1; //  8
//     unsigned long long      quoted   :                  1; //  9
//     unsigned long long      is_ref   :                  1; // 10
//     unsigned long long      rc       :   JULIE_MAX_RC_POT; // 28
//     unsigned long long      line     : JULIE_MAX_LINE_POT; // 44
//     unsigned long long      col      :  JULIE_MAX_COL_POT; // 54
//     unsigned long long      ind_level:  JULIE_MAX_COL_POT; // 64
};

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

Julie_Value *julie_store_alloc(Julie_Value_Store *store, Julie_Type type) {
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

    err = posix_memalign((void**)&block, STORE_BLOCK_ALIGN, sizeof(*block));
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
    value->type = type;

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

struct Julie_Interp_Struct {
    Julie_Error_Callback    error_callback;
//     Julie_Output_Callback   output_callback;
//     Julie_Eval_Callback     eval_callback;
    int                     argc;
    char                  **argv;
    Julie_Value_Store store;
    hash_table(Julie_Value_Ptr, Julie_Source_Value_Info) source_info;

    Julie_Array *roots;
};


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

Julie_Source_Value_Info *julie_get_source_value_info(Julie_Interp *interp, Julie_Value *value) {
    Julie_Source_Value_Info *info;
    JULIE_ASSERT(interp->source_info != NULL);
    info = hash_table_get_val(interp->source_info, value);
    JULIE_ASSERT(info != NULL && "did not find value in source info table");
    return info;
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
    Julie_Value             *value;
    Julie_Source_Value_Info  info;

    value = julie_store_alloc(&cxt->interp->store, JULIE_LIST);
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
                val = julie_store_alloc(&cxt->interp->store, JULIE_NIL);
            } else {
                sbuff = alloca(tk_end - tk_start + 1);
                memcpy(sbuff, tk_start, tk_end - tk_start);
                sbuff[tk_end - tk_start] = 0;
                val = julie_store_alloc(&cxt->interp->store, JULIE_SYMBOL);
                /* @todo: string interning -- set value here */
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

            val = julie_store_alloc(&cxt->interp->store, JULIE_STRING);
            /* @todo: string interning -- set value here */
            break;
        case JULIE_TK_SINT:
            strncpy(tk_copy, tk_start, tk_end - tk_start);
            tk_copy[tk_end - tk_start] = 0;
            sscanf(tk_copy, "%lld", &s);
            val = julie_store_alloc(&cxt->interp->store, JULIE_SINT);
            val->sint = s;
            break;
        case JULIE_TK_HEX:
            strncpy(tk_copy, tk_start, tk_end - tk_start);
            tk_copy[tk_end - tk_start] = 0;
            sscanf(tk_copy, "%llx", &u);
            val = julie_store_alloc(&cxt->interp->store, JULIE_UINT);
            val->uint = u;
            break;
        case JULIE_TK_FLOAT:
            strncpy(tk_copy, tk_start, tk_end - tk_start);
            tk_copy[tk_end - tk_start] = 0;
            sscanf(tk_copy, "%lg", &d);
            val = julie_store_alloc(&cxt->interp->store, JULIE_FLOAT);
            val->floating = d;
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
 *                        Interp                         *
 *********************************************************/

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

    interp->source_info = hash_table_make(Julie_Value_Ptr, Julie_Source_Value_Info, julie_value_ptr_hash);

    interp->roots     = JULIE_ARRAY_INIT;

    return JULIE_SUCCESS;
}

Julie_Status julie_interp(Julie_Interp *interp) {
    Julie_Status  status;

    status = JULIE_SUCCESS;

#if 0
    ARRAY_FOR_EACH(interp->roots, root) {
        status = julie_eval(interp, root, &result);
        if (status != JULIE_SUCCESS) {
            goto out;
        }
        if (result) {
            julie_free_value(interp, result);
        }
    }
#endif

    return status;
}

void julie_free(Julie_Interp *interp) {
    Julie_Value_Store_Block *block;
    Julie_Value_Store_Block *next;

    julie_array_free(interp->roots);

    block = interp->store.head;
    while (block != NULL) {
        next = block->next;
        free(block);
        block = next;
    }

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

#endif /* JULIE_IMPL */

#endif /* __JULIE_H__ */
