#ifndef __JULIE_H__
#define __JULIE_H__

#define _JULIE_STATUS                                                                                                             \
    _JULIE_STATUS_X(JULIE_SUCCESS,                             "No error.")                                                       \
    _JULIE_STATUS_X(JULIE_ERR_UNEXPECTED_EOS,                  "Unexpected end of input.")                                        \
    _JULIE_STATUS_X(JULIE_ERR_UNEXPECTED_TOK,                  "Unexpected token.")                                               \
    _JULIE_STATUS_X(JULIE_ERR_LINE_TOO_LONG,                   "Token starts too far into a line (column too big).")              \
    _JULIE_STATUS_X(JULIE_ERR_TOO_MANY_LINES,                  "You have exceeded the maximum number of lines in a single file.") \
    _JULIE_STATUS_X(JULIE_ERR_EXTRA_RPAREN,                    "Extraneous closing parenthesis.")                                 \
    _JULIE_STATUS_X(JULIE_ERR_MISSING_RPAREN,                  "End of line while parentheses left open.")                        \
    _JULIE_STATUS_X(JULIE_ERR_NO_INPUT,                        "Missing a top-level expression.")                                 \
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

#ifndef JULIE_MALLOC
#define JULIE_MALLOC (malloc)
#endif
#ifndef JULIE_REALLOC
#define JULIE_REALLOC (realloc)
#endif
#ifndef JULIE_FREE
#define JULIE_FREE (free)
#endif


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


Julie_Status julie_map_file_into_readonly_memory(const char *path, const char **addr, int *size) {
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



struct Julie_Array_Struct {
    unsigned  len;
    unsigned  cap;
    void     *aux;
    void     *data[];
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
        r = julie_top(array);
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


struct Julie_Value_Struct {
    union {
        long long           sint;
        unsigned long long  uint;
        double              floating;
        Julie_String_ID     string_id;
        Julie_String_ID     symbol_id;
        Julie_Object        object;
        Julie_Array        *list;
        Julie_Fn            builtin_fn;
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

typedef struct Julie_Parse_Context_Struct {
} Julie_Parse_Context;


struct Julie_Interp_Struct {
    Julie_Error_Callback    error_callback;
    Julie_Output_Callback   output_callback;
    Julie_Eval_Callback     eval_callback;
    int                     argc;
    char                  **argv;
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

Julie_Status julie_set_error_callback(Julie_Interp *interp, Julie_Error_Callback cb) {
    interp->error_callback = cb;
    return JULIE_SUCCESS;
}

Julie_Status julie_set_output_callback(Julie_Interp *interp, Julie_Output_Callback cb) {
    interp->output_callback = cb;
    return JULIE_SUCCESS;
}

Julie_Status julie_set_eval_callback(Julie_Interp *interp, Julie_Eval_Callback cb) {
    interp->eval_callback = cb;
    return JULIE_SUCCESS;
}

Julie_Status julie_set_argv(Julie_Interp *interp, int argc, char **argv) {
    interp->argc = argc;
    interp->argv = argv;
    return JULIE_SUCCESS;
}


#define STATUS_ERR_RET(_interp, _status)       \
do {                                           \
    if ((_status) != JULIE_SUCCESS) {           \
        julie_make_error((_interp), (_status)); \
    }                                          \
    return (_status);                          \
} while (0)

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

static Julie_Value *julie_push_tree(Julie_Parse_Context *cxt) {
    Julie_Value *value;

    value = _julie_value();
    value->type      = JULIE_LIST;
    value->ind_level = cxt->ind;
    value->line      = cxt->line;
    value->col       = cxt->col;
    value->list      = JULIE_ARRAY_INIT;
    JULIE_SET_AUX(value->list, (void*)cxt->interp->cur_file);

    JULIE_PUSH(cxt->stack, value);

    return value;
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

static Julie_Status julie_parse_next_value(Julie_Parse_Context *cxt, Julie_Value **valout, Julie_Token *tkout) {
    int                 status;
    Julie_Value        *val;
    int                 start_col;
    const char         *tk_start;
    Julie_Token         tk;
    const char         *tk_end;
    Julie_Value        *top;
    Julie_Value        *child;
    int                 c;
    char               *sbuff;
    unsigned long long  slen;
    char                tk_copy[128];
    long long           s;
    unsigned long long  u;
    double              d;

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

    if (cxt->col >= (1 << JULIE_MAX_COL_POT)) {
        PARSE_ERR_RET(cxt->interp, JULIE_ERR_LINE_TOO_LONG, cxt->line, cxt->col);
    }

    start_col = cxt->col;

    if (tk == JULIE_TK_LPAREN) {
        julie_push_tree(cxt);
        val = top = julie_top(cxt->stack);

        cxt->col += tk_end - tk_start;

        cxt->plevel += 1;

        child = NULL;
        while ((status = julie_parse_next_value(cxt, &child, tkout)) == JULIE_SUCCESS && child != NULL) {
            JULIE_PUSH(top->list, child);
        }

        if (status != JULIE_SUCCESS) {
            PARSE_ERR_RET(cxt->interp, status, cxt->line, cxt->col);
        }

        if (*tkout != JULIE_TK_RPAREN) {
            PARSE_ERR_RET(cxt->interp, JULIE_ERR_MISSING_RPAREN, cxt->line, cxt->col);
        }

        cxt->plevel -= 1;

        *tkout = JULIE_TK_LPAREN;

        julie_pop(cxt->stack);

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
                val = julie_nil_value();
            } else {
                sbuff = alloca(tk_end - tk_start + 1);
                memcpy(sbuff, tk_start, tk_end - tk_start);
                sbuff[tk_end - tk_start] = 0;
                val = julie_symbol_value(cxt->interp, sbuff);
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

            val = julie_string_value(cxt->interp, sbuff);
            break;
        case JULIE_TK_SINT:
            strncpy(tk_copy, tk_start, tk_end - tk_start);
            tk_copy[tk_end - tk_start] = 0;
            sscanf(tk_copy, "%lld", &s);
            val = julie_sint_value(s);
            break;
        case JULIE_TK_HEX:
            strncpy(tk_copy, tk_start, tk_end - tk_start);
            tk_copy[tk_end - tk_start] = 0;
            sscanf(tk_copy, "%llx", &u);
            val = julie_uint_value(u);
            break;
        case JULIE_TK_FLOAT:
            strncpy(tk_copy, tk_start, tk_end - tk_start);
            tk_copy[tk_end - tk_start] = 0;
            sscanf(tk_copy, "%lg", &d);
            val = julie_float_value(d);
            break;
        case JULIE_TK_EOS_ERR:
            PARSE_ERR_RET(cxt->interp, JULIE_ERR_UNEXPECTED_EOS, cxt->line, start_col + (tk_end - tk_start));
            break;
        default:
            break;
    }

out_val:;

    JULIE_ASSERT(val != NULL);

    val->ind_level = cxt->ind;
    val->line      = cxt->line;
    val->col       = start_col;

    *valout = val;

out:;
    return status;
}


static Julie_Status julie_parse_line(Julie_Parse_Context *cxt) {
    int          status;
    int          c;
    Julie_Value *top;
    Julie_Value *val;
    Julie_Token  tk;

    status = JULIE_SUCCESS;

    cxt->ind = julie_trim_leading_ws(cxt);
    cxt->col = 1 + cxt->ind;

    if (!PEEK_CHAR(cxt, c))                      { goto done; }
    if (c == '\n' || julie_consume_comment(cxt)) { goto eol;  }

    while ((top = julie_top(cxt->stack)) != NULL
    &&     cxt->ind <= top->ind_level) {

        julie_pop(cxt->stack);
    }

    val = julie_push_tree(cxt);
    if (top == NULL) {
        JULIE_PUSH(cxt->roots, val);
    } else {
        JULIE_PUSH(top->list, val);
    }
    top = val;

    val = NULL;
    while ((status = julie_parse_next_value(cxt, &val, &tk)) == JULIE_SUCCESS && val != NULL) {
        JULIE_PUSH(top->list, val);
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

static void _julie_string_print(Julie_Interp *interp, char **buff, int *len, int *cap, const Julie_Value *value, unsigned ind, int flags) {
    unsigned            i;
    char                b[128];
    const Julie_String  *string;
    Julie_Value         *child;
    Julie_Value         *key;
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
            FOR_EACH(value->list, child) {
                _julie_string_print(interp, buff, len, cap, child, (flags & JULIE_MULTILINE) ? ind + 2 : 0, flags & ~JULIE_NO_QUOTE);
                PUSHC((flags & JULIE_MULTILINE) ? '\n' : ' ');
            }
            if (flags & JULIE_MULTILINE) {
                for (i = 0; i < ind; i += 1) { PUSHC(' '); }
            }
            PUSHC(')');
            break;
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
        case JULIE_FN:
            fsym = NULL;

            if (value->builtin) {
                fsym = NULL;
                hash_table_traverse(interp->symtab, sym, val) {
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
    if (interp->output_callback == NULL) {
        printf("%.*s", n_bytes, s);
        fflush(stdout);
    } else {
        interp->output_callback(s, n_bytes);
    }
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

static Julie_Status julie_parse_nodes(Julie_Interp *interp, const char *str, int size, Julie_Array **out_nodes) {
    Julie_Parse_Context  cxt;
    Julie_Status         status;
    Julie_Value         *it;

    memset(&cxt, 0, sizeof(cxt));

    cxt.interp = interp;
    cxt.cursor = str;
    cxt.end    = str + size;

    status = JULIE_SUCCESS;
    while (status == JULIE_SUCCESS && MORE_INPUT(&cxt)) {
        cxt.line += 1;

        if (cxt.line >= (1 << JULIE_MAX_LINE_POT)) {
            PARSE_ERR_RET(cxt.interp, JULIE_ERR_TOO_MANY_LINES, cxt.line, 1);
        }

        status = julie_parse_line(&cxt);
    }

    FOR_EACH(cxt.roots, it) {
        JULIE_PUSH(*out_nodes, it);
    }

    julie_free_array(cxt.roots);
    julie_free_array(cxt.stack);

    return status;
}

Julie_Status julie_parse(Julie_Interp *interp, const char *str, int size) {
    return julie_parse_nodes(interp, str, size, &interp->roots);
}

static void julie_push_local_symtab(Julie_Interp *interp, _Julie_Symbol_Table local_symtab) {
    JULIE_PUSH(interp->local_symtab_stack, local_symtab);
}

static Julie_Status julie_pop_local_symtab(Julie_Interp *interp, Julie_Value *tree) {
    Julie_Status          status;
    _Julie_Symbol_Table   local_symtab;
    Julie_Array          *syms = JULIE_ARRAY_INIT;
    Julie_String_ID       key;
    Julie_Value         **vit;

    JULIE_ASSERT(julie_len(interp->local_symtab_stack) > 0);

    status = JULIE_SUCCESS;

    local_symtab = julie_top(interp->local_symtab_stack);

    syms = JULIE_ARRAY_INIT;
    hash_table_traverse(local_symtab, key, vit) {
        (void)vit;
        JULIE_PUSH(syms, (void*)key);
    }
    FOR_EACH(syms, key) {
        status = julie_uninstall_local(interp, key);
        if (status != JULIE_SUCCESS) {
            julie_make_install_error(interp, tree, status, key);
            goto out;
        }
    }
    julie_free_array(syms);
    hash_table_free(local_symtab);

    julie_pop(interp->local_symtab_stack);

out:;
    return status;
}

static _Julie_Symbol_Table julie_local_symtab(Julie_Interp *interp) {
    return julie_top(interp->local_symtab_stack);
}

Julie_Value *julie_lookup(Julie_Interp *interp, Julie_String_ID id) {
    Julie_Value         **lookup;
    _Julie_Symbol_Table   local_symtab;
    Julie_Value          *val;

    lookup = NULL;

    local_symtab = julie_local_symtab(interp);
    if (local_symtab != NULL) {
        lookup = hash_table_get_val(local_symtab, id);
    }

    if (lookup == NULL) {
        lookup = hash_table_get_val(interp->symtab, id);
    }

    if (lookup == NULL) { return NULL; }

    val = *lookup;

    return val;
}

Julie_Value *julie_lookup_local_only(Julie_Interp *interp, Julie_String_ID id) {
    Julie_Value **lookup;

    lookup = hash_table_get_val(julie_local_symtab(interp), id);

    return lookup == NULL ? NULL : *lookup;
}

static Julie_Status julie_install_common(Julie_Interp *interp, _Julie_Symbol_Table symtab, Julie_String_ID id, Julie_Value **val, int local) {
    Julie_Value **lookup;
    Julie_Value   tmp;

    (void)interp;

    JULIE_ASSERT((*val)->is_ref || !(*val)->in_symtab);

    lookup = hash_table_get_val(symtab, id);
    if (lookup != NULL) {
        if (*lookup != *val) {
            if ((*lookup)->is_ref) {
                JULIE_UNREF((*lookup)->ref_of);
                (*lookup)->in_symtab = 0;
                julie_free_value(interp, *lookup);
            } else {
                tmp           = **val;
                tmp.rc        = (*lookup)->rc;
                tmp.in_symtab = (*lookup)->in_symtab;
                tmp.local     = (*lookup)->local;

                **val = **lookup;

                **lookup = tmp;

                (*val)->in_symtab = 0;
                julie_free_value(interp, *val);

                *val = *lookup;
            }
        }
    } else {
        (*val)->in_symtab = 1;
        (*val)->local     = !!local;

        hash_table_insert(symtab, id, *val);
    }

    return JULIE_SUCCESS;
}

static Julie_Status julie_uninstall_common(Julie_Interp *interp, _Julie_Symbol_Table symtab, Julie_String_ID id, int do_free) {
    Julie_Value **lookup;
    Julie_Value  *val;

    (void)interp;

    lookup = hash_table_get_val(symtab, id);
    if (lookup == NULL) {
        return JULIE_ERR_LOOKUP;
    }

    val = *lookup;

    hash_table_delete(symtab, id);

    if (val->is_ref) {
        JULIE_UNREF(val->ref_of);
    }

    if (do_free) {
        val->in_symtab = 0;
        julie_free_value(interp, val);
    } else {
        val->in_symtab = 0;
    }

    return JULIE_SUCCESS;
}

Julie_Status julie_install_var(Julie_Interp *interp, Julie_String_ID id, Julie_Value **val) {
    return julie_install_common(interp, interp->symtab, id, val, 0);
}

Julie_Status julie_install_fn(Julie_Interp *interp, Julie_String_ID id, Julie_Fn fn) {
    Julie_Value *fn_val;

    fn_val = julie_builtin_value(fn);
    return julie_install_var(interp, id, &fn_val);
}

Julie_Status julie_install_infix_fn(Julie_Interp *interp, Julie_String_ID id, Julie_Fn fn) {
    Julie_Value *infix_fn;

    infix_fn = julie_builtin_value(fn);
    infix_fn->infix = 1;
    return julie_install_var(interp, id, &infix_fn);
}

Julie_Status julie_install_local(Julie_Interp *interp, Julie_String_ID id, Julie_Value **val) {
    return julie_install_common(interp, julie_local_symtab(interp), id, val, 1);
}

Julie_Status julie_uninstall_var(Julie_Interp *interp, Julie_String_ID id) {
    return julie_uninstall_common(interp, interp->symtab, id, 1);
}

Julie_Status julie_uninstall_var_no_free(Julie_Interp *interp, Julie_String_ID id) {
    return julie_uninstall_common(interp, interp->symtab, id, 0);
}

Julie_Status julie_uninstall_fn(Julie_Interp *interp, Julie_String_ID id) {
    return julie_uninstall_var(interp, id);
}

Julie_Status julie_uninstall_local(Julie_Interp *interp, Julie_String_ID id) {
    return julie_uninstall_common(interp, julie_local_symtab(interp), id, 1);
}

Julie_Status julie_uninstall_local_no_free(Julie_Interp *interp, Julie_String_ID id) {
    return julie_uninstall_common(interp, julie_local_symtab(interp), id, 0);
}

static Julie_Status julie_eval(Julie_Interp *interp, Julie_Value *value, Julie_Value **result);
static Julie_Status julie_builtin_elem(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result);
#if 0
static Julie_Status julie_builtin_field(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result);
#endif

static Julie_Status julie_invoke(Julie_Interp *interp, Julie_Value *tree, Julie_Value *fn, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    return JULIE_SUCCESS;
#if 0
    Julie_Status               status;
    Julie_String_ID            save_file;
    Julie_Backtrace_Entry     *bt_entry;
    Julie_Value               *ev;
    Julie_Value               *def_tree;
    Julie_Value               *fn_sym;
    _Julie_Symbol_Table        local_symtab;
    unsigned                  i;
    unsigned                  n_params;
    Julie_Value              **params;
    Julie_Value               *arg_sym;
    Julie_Value               *arg_val;
    Julie_Value               *expr;
    unsigned                  lambda_params;
    const Julie_Closure_Info  *closure;
    Julie_String_ID            cap_sym;
    Julie_Value              **cap_valp;
    Julie_Value               *cap_val;
    Julie_Value                builtin;
    Julie_Backtrace_Entry     *container_bt_entry;
    Julie_Value              **container_args;

    status = JULIE_SUCCESS;


    save_file = interp->cur_file;

    bt_entry = JULIE_MALLOC(sizeof(*bt_entry));

    bt_entry->file = interp->cur_file;
    bt_entry->fn   = (fn->type == JULIE_LIST || fn->type == JULIE_OBJECT)
                        ? tree
                        : fn;

    JULIE_PUSH(interp->backtrace, bt_entry);

    if (fn->type == _JULIE_TREE || fn->type == _JULIE_TREE_LINE_LEADER) {
        interp->cur_file = fn->list->aux;

        status = julie_eval(interp, tree, &ev);
        if (status != JULIE_SUCCESS) {
            *result = NULL;
            goto out;
        }
        *result = ev;
    } else if (fn->type == _JULIE_FN) {
        interp->cur_file = fn->list->aux;

        def_tree = julie_elem(fn->list, 1);

        if (def_tree->type == _JULIE_TREE || def_tree->type == _JULIE_TREE_LINE_LEADER) {
            n_params = julie_len(def_tree->list) - 1;
            params   = (Julie_Value**)def_tree->list->data + 1;

            fn_sym = julie_elem(def_tree->list, 0);
        } else if (def_tree->type == JULIE_SYMBOL) {
            n_params = 0;
            params   = NULL;

            fn_sym = def_tree;
        } else {
            status = JULIE_ERR_TYPE;
            julie_make_type_error(interp, def_tree, JULIE_SYMBOL, def_tree->type);
            *result = NULL;
            goto out;
        }

        if (n_values != n_params) {
            status = JULIE_ERR_ARITY;
            julie_make_arity_error(interp, tree, n_params, n_values, 0);
            *result = NULL;
            goto out;
        }

        local_symtab = hash_table_make(Julie_String_ID, Julie_Value_Ptr, julie_string_id_hash);

        fn = julie_copy_force(fn);
        JULIE_BORROW(fn);
        status = julie_install_common(interp, local_symtab, fn_sym->symbol_id, fn, 1);

        if (status != JULIE_SUCCESS) {
            *result = NULL;
            julie_make_install_error(interp, fn_sym, status, fn_sym->symbol_id);
            goto out;
        }

        for (i = 0; i < n_params; i += 1) {
            arg_sym = params[i];
            JULIE_ASSERT(arg_sym->type == JULIE_SYMBOL);

            status = julie_eval(interp, values[i], &ev);
            if (status != JULIE_SUCCESS) {
                *result = NULL;
                goto out;
            }

            arg_val = julie_copy_force(ev);
            julie_free_value(interp, ev);

            status = julie_install_common(interp, local_symtab, arg_sym->symbol_id, arg_val, 1);
            if (status != JULIE_SUCCESS) {
                *result = NULL;
                julie_make_install_error(interp, arg_val, status, arg_sym->symbol_id);
                goto out;
            }
        }

        julie_push_local_symtab(interp, local_symtab);

        for (i = 2; i < julie_len(fn->list); i += 1) {
            expr   = julie_elem(fn->list, i);
            status = julie_eval(interp, expr, &ev);
            if (status != JULIE_SUCCESS) {
                julie_pop(interp->local_symtab_stack);
                julie_free_symtab(local_symtab);
                *result = NULL;
                goto out;
            }
            if (i == julie_len(fn->list) - 1) {
                *result = ev;
            } else {
                julie_free_value(interp, ev);
            }
        }

        if ((*result)->local) {
            *result = julie_copy_force(*result);
        }

        JULIE_UNBORROW(fn);

        status = julie_pop_local_symtab(interp, tree);
        if (status != JULIE_SUCCESS) {
            *result = NULL;
            goto out;
        }
    } else if (fn->type == _JULIE_BUILTIN_FN) {
        status = fn->builtin_fn(interp, tree, n_values, values, result);
    } else if (fn->type == _JULIE_LAMBDA) {
        closure          = fn->list->aux;
        interp->cur_file = closure->cur_file;

        lambda_params = julie_len(fn->list) > 2;

        if (lambda_params) {
            def_tree = julie_elem(fn->list, 1);

            if (def_tree->type == _JULIE_TREE || def_tree->type == _JULIE_TREE_LINE_LEADER) {
                n_params = julie_len(def_tree->list);
                params   = (Julie_Value**)def_tree->list->data;
            } else {
                status = JULIE_ERR_TYPE;
                julie_make_type_error(interp, def_tree, JULIE_SYMBOL, def_tree->type);
                *result = NULL;
                goto out;
            }

            if (n_values != n_params) {
                status = JULIE_ERR_ARITY;
                julie_make_arity_error(interp, tree, n_params, n_values, 0);
                *result = NULL;
                goto out;
            }
        } else {
            n_params = 0;
            params   = NULL;
        }

        local_symtab = hash_table_make(Julie_String_ID, Julie_Value_Ptr, julie_string_id_hash);

        hash_table_traverse(closure->captures, cap_sym, cap_valp) {
            cap_val = julie_copy_force(*cap_valp);
            status  = julie_install_common(interp, local_symtab, cap_sym, cap_val, 1);
            if (status != JULIE_SUCCESS) {
                *result = NULL;
                julie_make_install_error(interp, cap_val, status, cap_sym);
                goto out;
            }
        }

        for (i = 0; i < n_params; i += 1) {
            arg_sym = params[i];
            JULIE_ASSERT(arg_sym->type == JULIE_SYMBOL);

            status = julie_eval(interp, values[i], &ev);
            if (status != JULIE_SUCCESS) {
                *result = NULL;
                goto out;
            }

            arg_val = julie_copy_force(ev);
            julie_free_value(interp, ev);

            status = julie_install_common(interp, local_symtab, arg_sym->symbol_id, arg_val, 1);
            if (status != JULIE_SUCCESS) {
                *result = NULL;
                julie_make_install_error(interp, arg_val, status, arg_sym->symbol_id);
                goto out;
            }
        }

        julie_push_local_symtab(interp, local_symtab);

        expr   = julie_elem(fn->list, 1 + lambda_params);
        status = julie_eval(interp, expr, &ev);
        if (status != JULIE_SUCCESS) {
            julie_pop(interp->local_symtab_stack);
            julie_free_symtab(local_symtab);
            *result = NULL;
            goto out;
        }
        *result = ev;

        if ((*result)->local) {
            *result = julie_copy_force(*result);
        }

        status = julie_pop_local_symtab(interp, tree);
        if (status != JULIE_SUCCESS) {
            *result = NULL;
            goto out;
        }
    } else if (fn->type == JULIE_LIST || fn->type == JULIE_OBJECT) {
        builtin.type = _JULIE_BUILTIN_FN;
        builtin.line = fn->line;
        builtin.col  = fn->col;

        if (fn->type == JULIE_LIST) {
            builtin.builtin_fn = julie_builtin_elem;
        } else if (fn->type == JULIE_OBJECT) {
            builtin.builtin_fn = julie_builtin_field;
        }

        container_bt_entry = JULIE_MALLOC(sizeof(*container_bt_entry));

        container_bt_entry->file = interp->cur_file;
        container_bt_entry->fn   = &builtin;

        container_args    = alloca(sizeof(*container_args) * (n_values + 1));
        container_args[0] = fn;
        memcpy(container_args + 1, values, sizeof(*container_args) * n_values);

        JULIE_PUSH(interp->backtrace, container_bt_entry);

        status = builtin.builtin_fn(interp, tree, n_values + 1, container_args, result);

        julie_pop(interp->backtrace);
        JULIE_FREE(container_bt_entry);
    } else {
        status = JULIE_ERR_BAD_APPLY;
        julie_make_bad_invoke_error(interp, fn, fn->type);
        goto out;
    }

out:;

    interp->last_popped_builtin_fn = bt_entry->fn->type == _JULIE_BUILTIN_FN
                                        ? bt_entry->fn->builtin_fn
                                        : NULL;

    julie_pop(interp->backtrace);
    JULIE_FREE(bt_entry);

    interp->cur_file = save_file;
    return status;
#endif
}

static Julie_Status julie_apply(Julie_Interp *interp, Julie_Value *list, Julie_Value **result) {
    Julie_Status               status;
    Julie_String_ID            save_file;
    Julie_Backtrace_Entry     *bt_entry;
    int            infix;
    Julie_Value *maybe_infix_fn;
    Julie_Value   *lookup;
    Julie_Value *fn;
    unsigned       i;
    Julie_Array   *args = JULIE_ARRAY_INIT;
    unsigned       n_values;
    Julie_Value  **values;

    status = JULIE_SUCCESS;

    *result = NULL;

    if (julie_len(list->list) == 0) {
        *result = julie_nil_value();
        (*result)->line = list->line;
        (*result)->col  = list->col;
        return JULIE_SUCCESS;
    }

    /* Get the function value. */
    fn = NULL;

    /* Check for infix. */
    infix = 0;
    if (julie_len(list->list) == 3) {
        maybe_infix_fn = julie_elem(list->list, 1);
        if (maybe_infix_fn->type == JULIE_SYMBOL) {
            lookup = julie_lookup(interp, maybe_infix_fn->symbol_id);
            if (lookup != NULL && lookup->type == JULIE_FN && lookup->infix) {
                fn = julie_copy(lookup);
                fn->line = maybe_infix_fn->line;
                fn->col  = maybe_infix_fn->col;
                infix = 1;
            }
        } else if (maybe_infix_fn->type == JULIE_FN && maybe_infix_fn->infix) {
            fn = julie_copy(maybe_infix_fn);
            fn->line = maybe_infix_fn->line;
            fn->col  = maybe_infix_fn->col;
            infix = 1;
        }
    }

    /* Function value is first element of list -- eval. */
    if (!infix) {
        status = julie_eval(interp, julie_elem(list->list, 0), &fn);
        if (status != JULIE_SUCCESS) {
            goto out;
        }
    }

    /* Get argument values. */
    if (infix) {
        for (i = 0; i < julie_len(list->list); i += 1) {
            if (i == 1) { continue; }
            JULIE_PUSH(args, julie_elem(list->list, i));
        }
    } else {
        for (i = 1; i < julie_len(list->list); i += 1) {
            JULIE_PUSH(args, julie_elem(list->list, i));
        }
    }

    /* Push a backtrace frame. */
    save_file = interp->cur_file;

    bt_entry = JULIE_MALLOC(sizeof(*bt_entry));

    bt_entry->file = interp->cur_file;
    bt_entry->fn   = fn;
    bt_entry->line = list->line;
    bt_entry->col  = list->col;
    JULIE_PUSH(interp->backtrace, bt_entry);


    n_values = julie_len(args);
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

            n_values = julie_len(list->list);
            values   = (Julie_Value**)list->list->data;
            status = julie_builtin_elem(interp, list, n_values, values, result);
            break;

        default:
            if (n_values == 0) {
id:;
                status = julie_eval(interp, fn, result);
            } else {
                status = JULIE_ERR_BAD_APPLY;
                julie_make_bad_invoke_error(interp, fn, fn->type);
            }
            break;
    }


    interp->last_popped_builtin_fn = (bt_entry->fn->type == JULIE_FN && bt_entry->fn->builtin)
                                        ? bt_entry->fn->builtin_fn
                                        : NULL;

    julie_pop(interp->backtrace);
    JULIE_FREE(bt_entry);

    interp->cur_file = save_file;

    julie_free_value(interp, fn);

    if (*result != NULL) {
        (*result)->line = list->line;
        (*result)->col  = list->col;
    }

out:;
    julie_free_array(args);
    return status;
}

static Julie_Status julie_eval(Julie_Interp *interp, Julie_Value *value, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *lookup;

    status  = JULIE_SUCCESS;
    *result = NULL;

    if (interp->eval_callback != NULL) {
        status = interp->eval_callback(value);
        if (status != JULIE_SUCCESS) {
            julie_make_interp_error(interp, value, status);
            goto out;
        }
    }

    switch (value->type) {
        case JULIE_NIL:
        case JULIE_SINT:
        case JULIE_UINT:
        case JULIE_FLOAT:
        case JULIE_STRING:
        case JULIE_OBJECT:
            *result = julie_copy(value);
            goto out;
        case JULIE_SYMBOL:
            if ((lookup = julie_lookup(interp, value->symbol_id)) == NULL) {
                status = JULIE_ERR_LOOKUP;
                julie_make_lookup_error(interp, value, value->symbol_id);
                *result = NULL;
                goto out;
            }
            if (lookup->is_ref) {
                lookup = lookup->ref_of;
            }
            *result = julie_copy(lookup);
            goto out;

        case JULIE_LIST:
            status = julie_apply(interp, value, result);
            goto out;

        default:
            JULIE_ASSERT(0 && "bad type in julie_eval");
    }

out:;
    return status;
#if 0
    Julie_Status   status;
    Julie_Value   *lookup;
    Julie_Value   *fn;
    Julie_Value  **arg_values;
    unsigned      n_args;

    status  = JULIE_SUCCESS;
    *result = NULL;

    if (interp->eval_callback != NULL) {
        status = interp->eval_callback(value);
        if (status != JULIE_SUCCESS) {
            julie_make_interp_error(interp, value, status);
            goto out;
        }
    }

    switch (value->type) {
        case JULIE_NIL:
        case JULIE_NUMBER:
        case JULIE_STRING:
        case JULIE_LIST:
        case JULIE_OBJECT:
            *result = julie_copy(value);
            goto out;

        case JULIE_SYMBOL:
            if ((lookup = julie_lookup(interp, value->symbol_id)) == NULL) {
                status = JULIE_ERR_LOOKUP;
                julie_make_lookup_error(interp, value, value->symbol_id);
                *result = NULL;
                goto out;
            }
            switch (lookup->type) {
                case JULIE_SYMBOL:
                case _JULIE_TREE:
                case _JULIE_TREE_LINE_LEADER:
                    status = julie_eval(interp, lookup, result);
                    goto out;
                case _JULIE_FN:
                case _JULIE_BUILTIN_FN:
                case _JULIE_LAMBDA:
                    fn          = lookup;
                    arg_values  = NULL;
                    n_args      = 0;
                    goto invoke;
                default:
                    *result = julie_copy(lookup);
                    goto out;

            }
            break;

        case _JULIE_TREE:
        case _JULIE_TREE_LINE_LEADER:
            JULIE_ASSERT(julie_len(value->list) >= 1);

            fn = julie_elem(value->list, 0);

            if (fn->type == JULIE_SYMBOL) {
                if ((lookup = julie_lookup(interp, fn->symbol_id)) == NULL) {
                    status = JULIE_ERR_LOOKUP;
                    julie_make_lookup_error(interp, value, fn->symbol_id);
                    *result = NULL;
                    goto out;
                }
                fn = lookup;
            } else {
                status = julie_eval(interp, fn, &fn);
                if (status != JULIE_SUCCESS) {
                    *result = NULL;
                    goto out;
                }
            }

            switch (fn->type) {
                case _JULIE_FN:
                case _JULIE_BUILTIN_FN:
                case _JULIE_LAMBDA:
                case JULIE_LIST:
                case JULIE_OBJECT:
                    arg_values = (Julie_Value**)value->list->data + 1;
                    n_args     = julie_len(value->list) - 1;
                    break;

                default:;
                    status = JULIE_ERR_BAD_APPLY;
                    julie_make_bad_invoke_error(interp, value, fn->type);
                    *result = NULL;
                    goto out;
                    break;
            }

invoke:;
            fn->line = value->line; /* @bad */
            fn->col  = value->col; /* @bad */

            status = julie_invoke(interp, value, fn, n_args, arg_values, result);
            if (status != JULIE_SUCCESS) {
                julie_free_value(interp, fn);
                *result = NULL;
                goto out;
            }
            julie_free_value(interp, fn);
            break;

        default:
            JULIE_ASSERT(0);
            break;
    }

out:;
    if (*result != NULL) {
        (*result)->line = value->line;
        (*result)->col  = value->col;
    }

    return status;
#endif
}



static Julie_Status julie_args(Julie_Interp *interp, Julie_Value *tree, const char *legend, unsigned n_values, Julie_Value **values, ...) {
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
            julie_make_arity_error(interp, tree, count, n_values, 0);
            goto out;
        }

        v = values[i];

        ve_ptr = va_arg(args, Julie_Value**);

        if (no_eval) {
            if (deep_copy) {
                *ve_ptr = julie_copy_force(v);
            } else {
                *ve_ptr = julie_copy(v);
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
                cpy = julie_copy_force(*ve_ptr);
                julie_free_value(interp, *ve_ptr);
                *ve_ptr = cpy;
            }
        }

        (*ve_ptr)->line = v->line;
        (*ve_ptr)->col  = v->col;

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
        julie_make_arity_error(interp, tree, count, n_values, 0);
        goto out;
    }

out:;
    va_end(args);

    return status;
}

static Julie_Status julie_builtin_typeof(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *val;

    status = julie_args(interp, tree, "*", n_values, values, &val);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    *result = julie_string_value(interp, julie_type_string(val->type));

    julie_free_value(interp, val);

out:;
    return status;
}

static Julie_Status julie_builtin_quote(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *value;

    status = julie_args(interp, tree, "-*", n_values, values, &value);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    *result = julie_copy(value);

    julie_free_value(interp, value);

out:;
    return status;
}

static Julie_Status julie_builtin_id(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *value;
    Julie_Value  *ev;
    Julie_Value  *lookup;

    status = julie_args(interp, tree, "-*", n_values, values, &value);
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

            ev = julie_copy(lookup);
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

static Julie_Status julie_get_lval(Julie_Interp *interp, Julie_Value *value, Julie_Value **result) {
    Julie_Status status;

    *result = NULL;

    status = julie_eval(interp, value, result);
    if (status != JULIE_SUCCESS) {
        goto out;
    }

    if (!(*result)->in_symtab) {
        if (!(*result)->is_ref) {
            julie_free_value(interp, *result);
            status = JULIE_ERR_NOT_LVAL;
            julie_make_lval_error(interp, value, status);
            result = NULL;
            goto out;
        }
        JULIE_ASSERT((*result)->ref_of->in_symtab && "ref of something not in symtab?");
        *result = julie_copy((*result)->ref_of);
    }

out:;
    return status;
}

static Julie_Status julie_builtin_assign(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status        status;
    Julie_Value        *l;
    Julie_Value        *rval;
    Julie_Value        *lookup;
    Julie_Value        *lval;
    Julie_Value        *cpy;
    const Julie_String *s;
    Julie_Value         tmp;

    *result = NULL;

    status = julie_args(interp, tree, "-**", n_values, values, &l, &rval);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    if (l->type == JULIE_SYMBOL) {
        s = julie_get_string(interp, l->symbol_id);
        if (s->len >= 2 && s->chars[0] == '&') {
            lookup = julie_lookup(interp, l->symbol_id);
            if (lookup == NULL) {
                if (!rval->in_symtab) {
                    status  = JULIE_ERR_REF_OF_TRANSIENT;
                    julie_make_ref_error(interp, rval, status);
                    julie_free_value(interp, rval);
                    *result = NULL;
                    goto out;
                }
            } else {
                JULIE_ASSERT(lookup->is_ref);
                goto into_lval;
            }

            rval = julie_ref_value(rval->is_ref ? rval->ref_of : rval);
            goto install;
        }

        if (!rval->is_ref && rval->in_symtab) {
            cpy = julie_copy_force(rval);
            julie_free_value(interp, rval);
            rval = cpy;
        }

install:;
        if (julie_len(interp->local_symtab_stack) == 0) {
            status = julie_install_var(interp, l->symbol_id, &rval);
        } else {
            status = julie_install_local(interp, l->symbol_id, &rval);
        }
        if (status != JULIE_SUCCESS) {
            *result = NULL;
            julie_make_install_error(interp, tree, status, l->symbol_id);
            julie_free_value(interp, rval);
            goto out;
        }
    } else {
into_lval:;
        status = julie_get_lval(interp, l, &lval);
        if (status != JULIE_SUCCESS) {
            julie_free_value(interp, rval);
            goto out;
        }

        JULIE_ASSERT(!lval->is_ref);

        if (rval->in_symtab) {
            cpy = julie_copy_force(rval);
            julie_free_value(interp, rval);
            rval = cpy;
        }

        tmp           = *rval;
        tmp.rc        = lval->rc;
        tmp.in_symtab = 1;

        *rval = *lval;
        *lval = tmp;

        rval->in_symtab = rval->rc = 0;
        julie_free_value(interp, rval);

        rval = lval;
    }

    *result = rval;

out:;
    return status;
}

static Julie_Status julie_builtin_add(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *a;
    Julie_Value  *b;

    status = julie_args(interp, tree, "nn", n_values, values, &a, &b);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    if (a->type == JULIE_SINT && b->type == JULIE_SINT) {
        *result = julie_sint_value(a->sint + b->sint);
    } else if (a->type == JULIE_SINT && b->type == JULIE_UINT) {
        *result = julie_uint_value((unsigned long long)a->sint + b->uint);
    } else if (a->type == JULIE_SINT && b->type == JULIE_FLOAT) {
        *result = julie_float_value((double)a->sint + b->floating);
    } else if (a->type == JULIE_UINT && b->type == JULIE_UINT) {
        *result = julie_uint_value(a->uint + b->uint);
    } else if (a->type == JULIE_UINT && b->type == JULIE_SINT) {
        *result = julie_uint_value(a->uint + (unsigned long long)b->sint);
    } else if (a->type == JULIE_UINT && b->type == JULIE_FLOAT) {
        *result = julie_float_value((double)a->uint + b->floating);
    } else if (a->type == JULIE_FLOAT && b->type == JULIE_FLOAT) {
        *result = julie_float_value(a->floating + b->floating);
    } else if (a->type == JULIE_FLOAT && b->type == JULIE_SINT) {
        *result = julie_float_value(a->floating + (double)b->sint);
    } else if (a->type == JULIE_FLOAT && b->type == JULIE_UINT) {
        *result = julie_float_value(a->floating + (double)b->uint);
    } else {
        JULIE_ASSERT(0 && "bad number type");
    }

    julie_free_value(interp, a);
    julie_free_value(interp, b);

out:;
    return status;
}

static Julie_Status julie_builtin_add_assign(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *a;
    Julie_Value  *b;
    Julie_Value  *lookup;

    status = julie_args(interp, tree, "-$n", n_values, values, &a, &b);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    lookup = julie_lookup(interp, a->symbol_id);
    if (lookup == NULL) {
        status = JULIE_ERR_LOOKUP;
        julie_make_lookup_error(interp, a, a->symbol_id);
        *result = NULL;
        goto out_free;
    }

    if (!JULIE_TYPE_IS_NUMBER(lookup->type)) {
        status = JULIE_ERR_TYPE;
        julie_make_type_error(interp, a, _JULIE_NUMBER, lookup->type);
        *result = NULL;
        goto out_free;
    }

    if (lookup->type == JULIE_SINT && b->type == JULIE_SINT) {
        lookup->sint += b->sint;
    } else if (lookup->type == JULIE_SINT && b->type == JULIE_UINT) {
        lookup->sint += b->uint;
    } else if (lookup->type == JULIE_SINT && b->type == JULIE_FLOAT) {
        lookup->sint += b->floating;
    } else if (lookup->type == JULIE_UINT && b->type == JULIE_UINT) {
        lookup->uint += b->uint;
    } else if (lookup->type == JULIE_UINT && b->type == JULIE_SINT) {
        lookup->uint += b->sint;
    } else if (lookup->type == JULIE_UINT && b->type == JULIE_FLOAT) {
        lookup->uint += b->floating;
    } else if (lookup->type == JULIE_FLOAT && b->type == JULIE_FLOAT) {
        lookup->floating += b->floating;
    } else if (lookup->type == JULIE_FLOAT && b->type == JULIE_SINT) {
        lookup->floating += b->sint;
    } else if (lookup->type == JULIE_FLOAT && b->type == JULIE_UINT) {
        lookup->floating += b->uint;
    } else {
        JULIE_ASSERT(0 && "bad number type");
    }

    *result = julie_copy(lookup);

out_free:;
    julie_free_value(interp, b);

out:;
    return status;
}

static Julie_Status julie_builtin_sub(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *a;
    Julie_Value  *b;

    status = julie_args(interp, tree, "nn", n_values, values, &a, &b);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    if (a->type == JULIE_SINT && b->type == JULIE_SINT) {
        *result = julie_sint_value(a->sint - b->sint);
    } else if (a->type == JULIE_SINT && b->type == JULIE_UINT) {
        *result = julie_uint_value((unsigned long long)a->sint - b->uint);
    } else if (a->type == JULIE_SINT && b->type == JULIE_FLOAT) {
        *result = julie_float_value((double)a->sint - b->floating);
    } else if (a->type == JULIE_UINT && b->type == JULIE_UINT) {
        *result = julie_uint_value(a->uint - b->uint);
    } else if (a->type == JULIE_UINT && b->type == JULIE_SINT) {
        *result = julie_uint_value(a->uint - (unsigned long long)b->sint);
    } else if (a->type == JULIE_UINT && b->type == JULIE_FLOAT) {
        *result = julie_float_value((double)a->uint - b->floating);
    } else if (a->type == JULIE_FLOAT && b->type == JULIE_FLOAT) {
        *result = julie_float_value(a->floating - b->floating);
    } else if (a->type == JULIE_FLOAT && b->type == JULIE_SINT) {
        *result = julie_float_value(a->floating - (double)b->sint);
    } else if (a->type == JULIE_FLOAT && b->type == JULIE_UINT) {
        *result = julie_float_value(a->floating - (double)b->uint);
    } else {
        JULIE_ASSERT(0 && "bad number type");
    }

    julie_free_value(interp, a);
    julie_free_value(interp, b);

out:;
    return status;
}

static Julie_Status julie_builtin_sub_assign(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *a;
    Julie_Value  *b;
    Julie_Value  *lookup;

    status = julie_args(interp, tree, "-$n", n_values, values, &a, &b);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    lookup = julie_lookup(interp, a->symbol_id);
    if (lookup == NULL) {
        status = JULIE_ERR_LOOKUP;
        julie_make_lookup_error(interp, a, a->symbol_id);
        *result = NULL;
        goto out_free;
    }

    if (!JULIE_TYPE_IS_NUMBER(lookup->type)) {
        status = JULIE_ERR_TYPE;
        julie_make_type_error(interp, a, _JULIE_NUMBER, lookup->type);
        *result = NULL;
        goto out_free;
    }

    if (lookup->type == JULIE_SINT && b->type == JULIE_SINT) {
        lookup->sint -= b->sint;
    } else if (lookup->type == JULIE_SINT && b->type == JULIE_UINT) {
        lookup->sint -= b->uint;
    } else if (lookup->type == JULIE_SINT && b->type == JULIE_FLOAT) {
        lookup->sint -= b->floating;
    } else if (lookup->type == JULIE_UINT && b->type == JULIE_UINT) {
        lookup->uint -= b->uint;
    } else if (lookup->type == JULIE_UINT && b->type == JULIE_SINT) {
        lookup->uint -= b->sint;
    } else if (lookup->type == JULIE_UINT && b->type == JULIE_FLOAT) {
        lookup->uint -= b->floating;
    } else if (lookup->type == JULIE_FLOAT && b->type == JULIE_FLOAT) {
        lookup->floating -= b->floating;
    } else if (lookup->type == JULIE_FLOAT && b->type == JULIE_SINT) {
        lookup->floating -= b->sint;
    } else if (lookup->type == JULIE_FLOAT && b->type == JULIE_UINT) {
        lookup->floating -= b->uint;
    } else {
        JULIE_ASSERT(0 && "bad number type");
    }

    *result = julie_copy(lookup);

out_free:;
    julie_free_value(interp, b);

out:;
    return status;
}

static Julie_Status julie_builtin_mul(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *a;
    Julie_Value  *b;

    status = julie_args(interp, tree, "nn", n_values, values, &a, &b);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    if (a->type == JULIE_SINT && b->type == JULIE_SINT) {
        *result = julie_sint_value(a->sint * b->sint);
    } else if (a->type == JULIE_SINT && b->type == JULIE_UINT) {
        *result = julie_uint_value((unsigned long long)a->sint * b->uint);
    } else if (a->type == JULIE_SINT && b->type == JULIE_FLOAT) {
        *result = julie_float_value((double)a->sint * b->floating);
    } else if (a->type == JULIE_UINT && b->type == JULIE_UINT) {
        *result = julie_uint_value(a->uint * b->uint);
    } else if (a->type == JULIE_UINT && b->type == JULIE_SINT) {
        *result = julie_uint_value(a->uint * (unsigned long long)b->sint);
    } else if (a->type == JULIE_UINT && b->type == JULIE_FLOAT) {
        *result = julie_float_value((double)a->uint * b->floating);
    } else if (a->type == JULIE_FLOAT && b->type == JULIE_FLOAT) {
        *result = julie_float_value(a->floating * b->floating);
    } else if (a->type == JULIE_FLOAT && b->type == JULIE_SINT) {
        *result = julie_float_value(a->floating * (double)b->sint);
    } else if (a->type == JULIE_FLOAT && b->type == JULIE_UINT) {
        *result = julie_float_value(a->floating * (double)b->uint);
    } else {
        JULIE_ASSERT(0 && "bad number type");
    }

    julie_free_value(interp, a);
    julie_free_value(interp, b);

out:;
    return status;
}

static Julie_Status julie_builtin_mul_assign(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *a;
    Julie_Value  *b;
    Julie_Value  *lookup;

    status = julie_args(interp, tree, "-$n", n_values, values, &a, &b);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    lookup = julie_lookup(interp, a->symbol_id);
    if (lookup == NULL) {
        status = JULIE_ERR_LOOKUP;
        julie_make_lookup_error(interp, a, a->symbol_id);
        *result = NULL;
        goto out_free;
    }

    if (!JULIE_TYPE_IS_NUMBER(lookup->type)) {
        status = JULIE_ERR_TYPE;
        julie_make_type_error(interp, a, _JULIE_NUMBER, lookup->type);
        *result = NULL;
        goto out_free;
    }

    if (lookup->type == JULIE_SINT && b->type == JULIE_SINT) {
        lookup->sint *= b->sint;
    } else if (lookup->type == JULIE_SINT && b->type == JULIE_UINT) {
        lookup->sint *= b->uint;
    } else if (lookup->type == JULIE_SINT && b->type == JULIE_FLOAT) {
        lookup->sint *= b->floating;
    } else if (lookup->type == JULIE_UINT && b->type == JULIE_UINT) {
        lookup->uint *= b->uint;
    } else if (lookup->type == JULIE_UINT && b->type == JULIE_SINT) {
        lookup->uint *= b->sint;
    } else if (lookup->type == JULIE_UINT && b->type == JULIE_FLOAT) {
        lookup->uint *= b->floating;
    } else if (lookup->type == JULIE_FLOAT && b->type == JULIE_FLOAT) {
        lookup->floating *= b->floating;
    } else if (lookup->type == JULIE_FLOAT && b->type == JULIE_SINT) {
        lookup->floating *= b->sint;
    } else if (lookup->type == JULIE_FLOAT && b->type == JULIE_UINT) {
        lookup->floating *= b->uint;
    } else {
        JULIE_ASSERT(0 && "bad number type");
    }

    *result = julie_copy(lookup);

out_free:;
    julie_free_value(interp, b);

out:;
    return status;
}

static Julie_Status julie_builtin_div(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *a;
    Julie_Value  *b;

    status = julie_args(interp, tree, "nn", n_values, values, &a, &b);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    if ((b->type == JULIE_SINT  && b->sint     == 0)
    ||  (b->type == JULIE_UINT  && b->uint     == 0)
    ||  (b->type == JULIE_FLOAT && b->floating == 0.0)) {

        *result = julie_nil_value();
    } else {
        if (a->type == JULIE_SINT && b->type == JULIE_SINT) {
            *result = julie_sint_value(a->sint / b->sint);
        } else if (a->type == JULIE_SINT && b->type == JULIE_UINT) {
            *result = julie_uint_value((unsigned long long)a->sint / b->uint);
        } else if (a->type == JULIE_SINT && b->type == JULIE_FLOAT) {
            *result = julie_float_value((double)a->sint / b->floating);
        } else if (a->type == JULIE_UINT && b->type == JULIE_UINT) {
            *result = julie_uint_value(a->uint / b->uint);
        } else if (a->type == JULIE_UINT && b->type == JULIE_SINT) {
            *result = julie_uint_value(a->uint / (unsigned long long)b->sint);
        } else if (a->type == JULIE_UINT && b->type == JULIE_FLOAT) {
            *result = julie_float_value((double)a->uint / b->floating);
        } else if (a->type == JULIE_FLOAT && b->type == JULIE_FLOAT) {
            *result = julie_float_value(a->floating / b->floating);
        } else if (a->type == JULIE_FLOAT && b->type == JULIE_SINT) {
            *result = julie_float_value(a->floating / (double)b->sint);
        } else if (a->type == JULIE_FLOAT && b->type == JULIE_UINT) {
            *result = julie_float_value(a->floating / (double)b->uint);
        } else {
            JULIE_ASSERT(0 && "bad number type");
        }
    }

    julie_free_value(interp, a);
    julie_free_value(interp, b);

out:;
    return status;
}

static Julie_Status julie_builtin_div_assign(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *a;
    Julie_Value  *b;
    Julie_Value  *lookup;

    status = julie_args(interp, tree, "-$n", n_values, values, &a, &b);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    lookup = julie_lookup(interp, a->symbol_id);
    if (lookup == NULL) {
        status = JULIE_ERR_LOOKUP;
        julie_make_lookup_error(interp, a, a->symbol_id);
        *result = NULL;
        goto out_free;
    }

    if (!JULIE_TYPE_IS_NUMBER(lookup->type)) {
        status = JULIE_ERR_TYPE;
        julie_make_type_error(interp, a, _JULIE_NUMBER, lookup->type);
        *result = NULL;
        goto out_free;
    }

    if ((b->type == JULIE_SINT  && b->sint     == 0)
    ||  (b->type == JULIE_UINT  && b->uint     == 0)
    ||  (b->type == JULIE_FLOAT && b->floating == 0.0)) {

        lookup->type = JULIE_NIL;
        lookup->uint = 0;
    } else {
        if (lookup->type == JULIE_SINT && b->type == JULIE_SINT) {
            lookup->sint /= b->sint;
        } else if (lookup->type == JULIE_SINT && b->type == JULIE_UINT) {
            lookup->sint /= b->uint;
        } else if (lookup->type == JULIE_SINT && b->type == JULIE_FLOAT) {
            lookup->sint /= b->floating;
        } else if (lookup->type == JULIE_UINT && b->type == JULIE_UINT) {
            lookup->uint /= b->uint;
        } else if (lookup->type == JULIE_UINT && b->type == JULIE_SINT) {
            lookup->uint /= b->sint;
        } else if (lookup->type == JULIE_UINT && b->type == JULIE_FLOAT) {
            lookup->uint /= b->floating;
        } else if (lookup->type == JULIE_FLOAT && b->type == JULIE_FLOAT) {
            lookup->floating /= b->floating;
        } else if (lookup->type == JULIE_FLOAT && b->type == JULIE_SINT) {
            lookup->floating /= b->sint;
        } else if (lookup->type == JULIE_FLOAT && b->type == JULIE_UINT) {
            lookup->floating /= b->uint;
        } else {
            JULIE_ASSERT(0 && "bad number type");
        }
    }

    *result = julie_copy(lookup);

out_free:;
    julie_free_value(interp, b);

out:;
    return status;
}

static Julie_Status julie_builtin_mod(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *a;
    Julie_Value  *b;

    status = julie_args(interp, tree, "nn", n_values, values, &a, &b);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    if ((b->type == JULIE_SINT  && b->sint     == 0)
    ||  (b->type == JULIE_UINT  && b->uint     == 0)
    ||  (b->type == JULIE_FLOAT && b->floating == 0.0)) {

        *result = julie_nil_value();
    } else {
        if (a->type == JULIE_SINT && b->type == JULIE_SINT) {
            *result = julie_sint_value(a->sint % b->sint);
        } else if (a->type == JULIE_SINT && b->type == JULIE_UINT) {
            *result = julie_uint_value((unsigned long long)a->sint % b->uint);
        } else if (a->type == JULIE_SINT && b->type == JULIE_FLOAT) {
            *result = julie_float_value(fmod((double)a->sint, b->floating));
        } else if (a->type == JULIE_UINT && b->type == JULIE_UINT) {
            *result = julie_uint_value(a->uint % b->uint);
        } else if (a->type == JULIE_UINT && b->type == JULIE_SINT) {
            *result = julie_uint_value(a->uint % (unsigned long long)b->sint);
        } else if (a->type == JULIE_UINT && b->type == JULIE_FLOAT) {
            *result = julie_float_value(fmod((double)a->uint, b->floating));
        } else if (a->type == JULIE_FLOAT && b->type == JULIE_FLOAT) {
            *result = julie_float_value(fmod(a->floating, b->floating));
        } else if (a->type == JULIE_FLOAT && b->type == JULIE_SINT) {
            *result = julie_float_value(fmod(a->floating, (double)b->sint));
        } else if (a->type == JULIE_FLOAT && b->type == JULIE_UINT) {
            *result = julie_float_value(fmod(a->floating, (double)b->uint));
        } else {
            JULIE_ASSERT(0 && "bad number type");
        }
    }

    julie_free_value(interp, a);
    julie_free_value(interp, b);

out:;
    return status;
}

static Julie_Status julie_builtin_mod_assign(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *a;
    Julie_Value  *b;
    Julie_Value  *lookup;

    status = julie_args(interp, tree, "-$n", n_values, values, &a, &b);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    lookup = julie_lookup(interp, a->symbol_id);
    if (lookup == NULL) {
        status = JULIE_ERR_LOOKUP;
        julie_make_lookup_error(interp, a, a->symbol_id);
        *result = NULL;
        goto out_free;
    }

    if (!JULIE_TYPE_IS_NUMBER(lookup->type)) {
        status = JULIE_ERR_TYPE;
        julie_make_type_error(interp, a, _JULIE_NUMBER, lookup->type);
        *result = NULL;
        goto out_free;
    }

    if ((b->type == JULIE_SINT  && b->sint     == 0)
    ||  (b->type == JULIE_UINT  && b->uint     == 0)
    ||  (b->type == JULIE_FLOAT && b->floating == 0.0)) {

        lookup->type = JULIE_NIL;
        lookup->uint = 0;
    } else {
        if (lookup->type == JULIE_SINT && b->type == JULIE_SINT) {
            lookup->sint %= b->sint;
        } else if (lookup->type == JULIE_SINT && b->type == JULIE_UINT) {
            lookup->sint %= b->uint;
        } else if (lookup->type == JULIE_SINT && b->type == JULIE_FLOAT) {
            lookup->sint = fmod((double)lookup->sint, b->floating);
        } else if (lookup->type == JULIE_UINT && b->type == JULIE_UINT) {
            lookup->uint %= b->uint;
        } else if (lookup->type == JULIE_UINT && b->type == JULIE_SINT) {
            lookup->uint %= b->sint;
        } else if (lookup->type == JULIE_UINT && b->type == JULIE_FLOAT) {
            lookup->uint = fmod((double)lookup->uint, b->floating);
        } else if (lookup->type == JULIE_FLOAT && b->type == JULIE_FLOAT) {
            lookup->floating = fmod(lookup->floating, b->floating);
        } else if (lookup->type == JULIE_FLOAT && b->type == JULIE_SINT) {
            lookup->floating = fmod(lookup->floating, (double)b->sint);
        } else if (lookup->type == JULIE_FLOAT && b->type == JULIE_UINT) {
            lookup->floating = fmod(lookup->floating, (double)b->uint);
        } else {
            JULIE_ASSERT(0 && "bad number type");
        }
    }

    *result = julie_copy(lookup);

out_free:;
    julie_free_value(interp, b);

out:;
    return status;
}

static Julie_Status julie_builtin_bit_not(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *a;

    status = julie_args(interp, tree, "i", n_values, values, &a);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    if (a->type == JULIE_SINT) {
        *result = julie_sint_value(~(a->sint));
    } else if (a->type == JULIE_UINT) {
        *result = julie_uint_value(~(a->uint));
    } else {
        JULIE_ASSERT(0 && "bad number type");
    }

    julie_free_value(interp, a);

out:;
    return status;
}

static Julie_Status julie_builtin_bit_and(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *a;
    Julie_Value  *b;

    status = julie_args(interp, tree, "ii", n_values, values, &a, &b);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    if (a->type == JULIE_SINT && b->type == JULIE_SINT) {
        *result = julie_sint_value(a->sint & b->sint);
    } else if (a->type == JULIE_SINT && b->type == JULIE_UINT) {
        *result = julie_uint_value((unsigned long long)a->sint & b->uint);
    } else if (a->type == JULIE_UINT && b->type == JULIE_UINT) {
        *result = julie_uint_value(a->uint & b->uint);
    } else if (a->type == JULIE_UINT && b->type == JULIE_SINT) {
        *result = julie_uint_value(a->uint & (unsigned long long)b->sint);
    } else {
        JULIE_ASSERT(0 && "bad number type");
    }

    julie_free_value(interp, a);
    julie_free_value(interp, b);

out:;
    return status;
}

static Julie_Status julie_builtin_bit_and_assign(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *a;
    Julie_Value  *b;
    Julie_Value  *lookup;

    status = julie_args(interp, tree, "-$i", n_values, values, &a, &b);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    lookup = julie_lookup(interp, a->symbol_id);
    if (lookup == NULL) {
        status = JULIE_ERR_LOOKUP;
        julie_make_lookup_error(interp, a, a->symbol_id);
        *result = NULL;
        goto out_free;
    }

    if (!JULIE_TYPE_IS_INTEGER(lookup->type)) {
        status = JULIE_ERR_TYPE;
        julie_make_type_error(interp, a, _JULIE_INTEGER, lookup->type);
        *result = NULL;
        goto out_free;
    }

    if (lookup->type == JULIE_SINT && b->type == JULIE_SINT) {
        lookup->sint &= b->sint;
    } else if (lookup->type == JULIE_SINT && b->type == JULIE_UINT) {
        lookup->sint &= b->uint;
    } else if (lookup->type == JULIE_UINT && b->type == JULIE_UINT) {
        lookup->uint &= b->uint;
    } else if (lookup->type == JULIE_UINT && b->type == JULIE_SINT) {
        lookup->uint &= b->sint;
    } else {
        JULIE_ASSERT(0 && "bad number type");
    }

    *result = julie_copy(lookup);

out_free:;
    julie_free_value(interp, b);

out:;
    return status;
}

static Julie_Status julie_builtin_bit_or(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *a;
    Julie_Value  *b;

    status = julie_args(interp, tree, "ii", n_values, values, &a, &b);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    if (a->type == JULIE_SINT && b->type == JULIE_SINT) {
        *result = julie_sint_value(a->sint | b->sint);
    } else if (a->type == JULIE_SINT && b->type == JULIE_UINT) {
        *result = julie_uint_value((unsigned long long)a->sint | b->uint);
    } else if (a->type == JULIE_UINT && b->type == JULIE_UINT) {
        *result = julie_uint_value(a->uint | b->uint);
    } else if (a->type == JULIE_UINT && b->type == JULIE_SINT) {
        *result = julie_uint_value(a->uint | (unsigned long long)b->sint);
    } else {
        JULIE_ASSERT(0 && "bad number type");
    }

    julie_free_value(interp, a);
    julie_free_value(interp, b);

out:;
    return status;
}

static Julie_Status julie_builtin_bit_or_assign(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *a;
    Julie_Value  *b;
    Julie_Value  *lookup;

    status = julie_args(interp, tree, "-$i", n_values, values, &a, &b);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    lookup = julie_lookup(interp, a->symbol_id);
    if (lookup == NULL) {
        status = JULIE_ERR_LOOKUP;
        julie_make_lookup_error(interp, a, a->symbol_id);
        *result = NULL;
        goto out_free;
    }

    if (!JULIE_TYPE_IS_INTEGER(lookup->type)) {
        status = JULIE_ERR_TYPE;
        julie_make_type_error(interp, a, _JULIE_INTEGER, lookup->type);
        *result = NULL;
        goto out_free;
    }

    if (lookup->type == JULIE_SINT && b->type == JULIE_SINT) {
        lookup->sint |= b->sint;
    } else if (lookup->type == JULIE_SINT && b->type == JULIE_UINT) {
        lookup->sint |= b->uint;
    } else if (lookup->type == JULIE_UINT && b->type == JULIE_UINT) {
        lookup->uint |= b->uint;
    } else if (lookup->type == JULIE_UINT && b->type == JULIE_SINT) {
        lookup->uint |= b->sint;
    } else {
        JULIE_ASSERT(0 && "bad number type");
    }

    *result = julie_copy(lookup);

out_free:;
    julie_free_value(interp, b);

out:;
    return status;
}

static Julie_Status julie_builtin_bit_xor(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *a;
    Julie_Value  *b;

    status = julie_args(interp, tree, "nn", n_values, values, &a, &b);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    if (a->type == JULIE_SINT && b->type == JULIE_SINT) {
        *result = julie_sint_value(a->sint ^ b->sint);
    } else if (a->type == JULIE_SINT && b->type == JULIE_UINT) {
        *result = julie_uint_value((unsigned long long)a->sint ^ b->uint);
    } else if (a->type == JULIE_UINT && b->type == JULIE_UINT) {
        *result = julie_uint_value(a->uint ^ b->uint);
    } else if (a->type == JULIE_UINT && b->type == JULIE_SINT) {
        *result = julie_uint_value(a->uint ^ (unsigned long long)b->sint);
    } else {
        JULIE_ASSERT(0 && "bad number type");
    }

    julie_free_value(interp, a);
    julie_free_value(interp, b);

out:;
    return status;
}

static Julie_Status julie_builtin_bit_xor_assign(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *a;
    Julie_Value  *b;
    Julie_Value  *lookup;

    status = julie_args(interp, tree, "-$i", n_values, values, &a, &b);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    lookup = julie_lookup(interp, a->symbol_id);
    if (lookup == NULL) {
        status = JULIE_ERR_LOOKUP;
        julie_make_lookup_error(interp, a, a->symbol_id);
        *result = NULL;
        goto out_free;
    }

    if (!JULIE_TYPE_IS_INTEGER(lookup->type)) {
        status = JULIE_ERR_TYPE;
        julie_make_type_error(interp, a, _JULIE_INTEGER, lookup->type);
        *result = NULL;
        goto out_free;
    }

    if (lookup->type == JULIE_SINT && b->type == JULIE_SINT) {
        lookup->sint ^= b->sint;
    } else if (lookup->type == JULIE_SINT && b->type == JULIE_UINT) {
        lookup->sint ^= b->uint;
    } else if (lookup->type == JULIE_UINT && b->type == JULIE_UINT) {
        lookup->uint ^= b->uint;
    } else if (lookup->type == JULIE_UINT && b->type == JULIE_SINT) {
        lookup->uint ^= b->sint;
    } else {
        JULIE_ASSERT(0 && "bad number type");
    }

    *result = julie_copy(lookup);

out_free:;
    julie_free_value(interp, b);

out:;
    return status;
}

static Julie_Status julie_builtin_bit_shl(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *a;
    Julie_Value  *b;

    status = julie_args(interp, tree, "nn", n_values, values, &a, &b);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    if (a->type == JULIE_SINT && b->type == JULIE_SINT) {
        *result = julie_sint_value(a->sint << b->sint);
    } else if (a->type == JULIE_SINT && b->type == JULIE_UINT) {
        *result = julie_uint_value((unsigned long long)a->sint << b->uint);
    } else if (a->type == JULIE_UINT && b->type == JULIE_UINT) {
        *result = julie_uint_value(a->uint << b->uint);
    } else if (a->type == JULIE_UINT && b->type == JULIE_SINT) {
        *result = julie_uint_value(a->uint << (unsigned long long)b->sint);
    } else {
        JULIE_ASSERT(0 && "bad number type");
    }

    julie_free_value(interp, a);
    julie_free_value(interp, b);

out:;
    return status;
}

static Julie_Status julie_builtin_bit_shl_assign(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *a;
    Julie_Value  *b;
    Julie_Value  *lookup;

    status = julie_args(interp, tree, "-$i", n_values, values, &a, &b);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    lookup = julie_lookup(interp, a->symbol_id);
    if (lookup == NULL) {
        status = JULIE_ERR_LOOKUP;
        julie_make_lookup_error(interp, a, a->symbol_id);
        *result = NULL;
        goto out_free;
    }

    if (!JULIE_TYPE_IS_INTEGER(lookup->type)) {
        status = JULIE_ERR_TYPE;
        julie_make_type_error(interp, a, _JULIE_INTEGER, lookup->type);
        *result = NULL;
        goto out_free;
    }

    if (lookup->type == JULIE_SINT && b->type == JULIE_SINT) {
        lookup->sint <<= b->sint;
    } else if (lookup->type == JULIE_SINT && b->type == JULIE_UINT) {
        lookup->sint <<= b->uint;
    } else if (lookup->type == JULIE_UINT && b->type == JULIE_UINT) {
        lookup->uint <<= b->uint;
    } else if (lookup->type == JULIE_UINT && b->type == JULIE_SINT) {
        lookup->uint <<= b->sint;
    } else {
        JULIE_ASSERT(0 && "bad number type");
    }

    *result = julie_copy(lookup);

out_free:;
    julie_free_value(interp, b);

out:;
    return status;
}

static Julie_Status julie_builtin_bit_shr(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *a;
    Julie_Value  *b;

    status = julie_args(interp, tree, "nn", n_values, values, &a, &b);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    if (a->type == JULIE_SINT && b->type == JULIE_SINT) {
        *result = julie_sint_value(a->sint >> b->sint);
    } else if (a->type == JULIE_SINT && b->type == JULIE_UINT) {
        *result = julie_uint_value((unsigned long long)a->sint >> b->uint);
    } else if (a->type == JULIE_UINT && b->type == JULIE_UINT) {
        *result = julie_uint_value(a->uint >> b->uint);
    } else if (a->type == JULIE_UINT && b->type == JULIE_SINT) {
        *result = julie_uint_value(a->uint >> (unsigned long long)b->sint);
    } else {
        JULIE_ASSERT(0 && "bad number type");
    }

    julie_free_value(interp, a);
    julie_free_value(interp, b);

out:;
    return status;
}

static Julie_Status julie_builtin_bit_shr_assign(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *a;
    Julie_Value  *b;
    Julie_Value  *lookup;

    status = julie_args(interp, tree, "-$i", n_values, values, &a, &b);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    lookup = julie_lookup(interp, a->symbol_id);
    if (lookup == NULL) {
        status = JULIE_ERR_LOOKUP;
        julie_make_lookup_error(interp, a, a->symbol_id);
        *result = NULL;
        goto out_free;
    }

    if (!JULIE_TYPE_IS_INTEGER(lookup->type)) {
        status = JULIE_ERR_TYPE;
        julie_make_type_error(interp, a, _JULIE_INTEGER, lookup->type);
        *result = NULL;
        goto out_free;
    }

    if (lookup->type == JULIE_SINT && b->type == JULIE_SINT) {
        lookup->sint >>= b->sint;
    } else if (lookup->type == JULIE_SINT && b->type == JULIE_UINT) {
        lookup->sint >>= b->uint;
    } else if (lookup->type == JULIE_UINT && b->type == JULIE_UINT) {
        lookup->uint >>= b->uint;
    } else if (lookup->type == JULIE_UINT && b->type == JULIE_SINT) {
        lookup->uint >>= b->sint;
    } else {
        JULIE_ASSERT(0 && "bad number type");
    }

    *result = julie_copy(lookup);

out_free:;
    julie_free_value(interp, b);

out:;
    return status;
}

static Julie_Status julie_builtin_equ(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *a;
    Julie_Value  *b;

    status = julie_args(interp, tree, "**", n_values, values, &a, &b);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    *result = julie_sint_value(julie_equal(a, b));

    julie_free_value(interp, a);
    julie_free_value(interp, b);

out:;
    return status;
}

static Julie_Status julie_builtin_neq(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *a;
    Julie_Value  *b;

    status = julie_args(interp, tree, "**", n_values, values, &a, &b);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    *result = julie_sint_value(!julie_equal(a, b));

    julie_free_value(interp, a);
    julie_free_value(interp, b);

out:;
    return status;
}

static Julie_Status julie_builtin_lss(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *a;
    Julie_Value  *b;

    status = julie_args(interp, tree, "nn", n_values, values, &a, &b);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    if (a->type == JULIE_SINT && b->type == JULIE_SINT) {
        *result = julie_sint_value(a->sint < b->sint);
    } else if (a->type == JULIE_SINT && b->type == JULIE_UINT) {
        *result = julie_sint_value((unsigned long long)a->sint < b->uint);
    } else if (a->type == JULIE_SINT && b->type == JULIE_FLOAT) {
        *result = julie_sint_value((double)a->sint < b->floating);
    } else if (a->type == JULIE_UINT && b->type == JULIE_UINT) {
        *result = julie_sint_value(a->uint < b->uint);
    } else if (a->type == JULIE_UINT && b->type == JULIE_SINT) {
        *result = julie_sint_value(a->uint < (unsigned long long)b->sint);
    } else if (a->type == JULIE_UINT && b->type == JULIE_FLOAT) {
        *result = julie_sint_value((double)a->uint < b->floating);
    } else if (a->type == JULIE_FLOAT && b->type == JULIE_FLOAT) {
        *result = julie_sint_value(a->floating < b->floating);
    } else if (a->type == JULIE_FLOAT && b->type == JULIE_SINT) {
        *result = julie_sint_value(a->floating < (double)b->sint);
    } else if (a->type == JULIE_FLOAT && b->type == JULIE_UINT) {
        *result = julie_sint_value(a->floating < (double)b->uint);
    } else {
        JULIE_ASSERT(0 && "bad number type");
    }

    julie_free_value(interp, a);
    julie_free_value(interp, b);

out:;
    return status;
}

static Julie_Status julie_builtin_leq(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *a;
    Julie_Value  *b;

    status = julie_args(interp, tree, "nn", n_values, values, &a, &b);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    if (a->type == JULIE_SINT && b->type == JULIE_SINT) {
        *result = julie_sint_value(a->sint <= b->sint);
    } else if (a->type == JULIE_SINT && b->type == JULIE_UINT) {
        *result = julie_sint_value((unsigned long long)a->sint <= b->uint);
    } else if (a->type == JULIE_SINT && b->type == JULIE_FLOAT) {
        *result = julie_sint_value((double)a->sint <= b->floating);
    } else if (a->type == JULIE_UINT && b->type == JULIE_UINT) {
        *result = julie_sint_value(a->uint <= b->uint);
    } else if (a->type == JULIE_UINT && b->type == JULIE_SINT) {
        *result = julie_sint_value(a->uint <= (unsigned long long)b->sint);
    } else if (a->type == JULIE_UINT && b->type == JULIE_FLOAT) {
        *result = julie_sint_value((double)a->uint <= b->floating);
    } else if (a->type == JULIE_FLOAT && b->type == JULIE_FLOAT) {
        *result = julie_sint_value(a->floating <= b->floating);
    } else if (a->type == JULIE_FLOAT && b->type == JULIE_SINT) {
        *result = julie_sint_value(a->floating <= (double)b->sint);
    } else if (a->type == JULIE_FLOAT && b->type == JULIE_UINT) {
        *result = julie_sint_value(a->floating <= (double)b->uint);
    } else {
        JULIE_ASSERT(0 && "bad number type");
    }

    julie_free_value(interp, a);
    julie_free_value(interp, b);

out:;
    return status;
}

static Julie_Status julie_builtin_gtr(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *a;
    Julie_Value  *b;

    status = julie_args(interp, tree, "nn", n_values, values, &a, &b);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    if (a->type == JULIE_SINT && b->type == JULIE_SINT) {
        *result = julie_sint_value(a->sint > b->sint);
    } else if (a->type == JULIE_SINT && b->type == JULIE_UINT) {
        *result = julie_sint_value((unsigned long long)a->sint > b->uint);
    } else if (a->type == JULIE_SINT && b->type == JULIE_FLOAT) {
        *result = julie_sint_value((double)a->sint > b->floating);
    } else if (a->type == JULIE_UINT && b->type == JULIE_UINT) {
        *result = julie_sint_value(a->uint > b->uint);
    } else if (a->type == JULIE_UINT && b->type == JULIE_SINT) {
        *result = julie_sint_value(a->uint > (unsigned long long)b->sint);
    } else if (a->type == JULIE_UINT && b->type == JULIE_FLOAT) {
        *result = julie_sint_value((double)a->uint > b->floating);
    } else if (a->type == JULIE_FLOAT && b->type == JULIE_FLOAT) {
        *result = julie_sint_value(a->floating > b->floating);
    } else if (a->type == JULIE_FLOAT && b->type == JULIE_SINT) {
        *result = julie_sint_value(a->floating > (double)b->sint);
    } else if (a->type == JULIE_FLOAT && b->type == JULIE_UINT) {
        *result = julie_sint_value(a->floating > (double)b->uint);
    } else {
        JULIE_ASSERT(0 && "bad number type");
    }

    julie_free_value(interp, a);
    julie_free_value(interp, b);

out:;
    return status;
}

static Julie_Status julie_builtin_geq(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *a;
    Julie_Value  *b;

    status = julie_args(interp, tree, "nn", n_values, values, &a, &b);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    if (a->type == JULIE_SINT && b->type == JULIE_SINT) {
        *result = julie_sint_value(a->sint >= b->sint);
    } else if (a->type == JULIE_SINT && b->type == JULIE_UINT) {
        *result = julie_sint_value((unsigned long long)a->sint >= b->uint);
    } else if (a->type == JULIE_SINT && b->type == JULIE_FLOAT) {
        *result = julie_sint_value((double)a->sint >= b->floating);
    } else if (a->type == JULIE_UINT && b->type == JULIE_UINT) {
        *result = julie_sint_value(a->uint >= b->uint);
    } else if (a->type == JULIE_UINT && b->type == JULIE_SINT) {
        *result = julie_sint_value(a->uint >= (unsigned long long)b->sint);
    } else if (a->type == JULIE_UINT && b->type == JULIE_FLOAT) {
        *result = julie_sint_value((double)a->uint >= b->floating);
    } else if (a->type == JULIE_FLOAT && b->type == JULIE_FLOAT) {
        *result = julie_sint_value(a->floating >= b->floating);
    } else if (a->type == JULIE_FLOAT && b->type == JULIE_SINT) {
        *result = julie_sint_value(a->floating >= (double)b->sint);
    } else if (a->type == JULIE_FLOAT && b->type == JULIE_UINT) {
        *result = julie_sint_value(a->floating >= (double)b->uint);
    } else {
        JULIE_ASSERT(0 && "bad number type");
    }

    julie_free_value(interp, a);
    julie_free_value(interp, b);

out:;
    return status;
}

static Julie_Status julie_builtin_not(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *a;

    status = julie_args(interp, tree, "n", n_values, values, &a);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    if (a->type == JULIE_SINT) {
        *result = julie_sint_value(!(a->sint));
    } else if (a->type == JULIE_UINT) {
        *result = julie_sint_value(!(a->uint));
    } else if (a->type == JULIE_FLOAT) {
        *result = julie_sint_value(!(a->floating));
    } else {
        JULIE_ASSERT(0 && "bad number type");
    }

    julie_free_value(interp, a);

out:;
    return status;
}

static Julie_Status julie_builtin_and(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    int           short_circuit;
    unsigned      i;
    Julie_Value  *cond;
    Julie_Value  *ev;

    status = JULIE_SUCCESS;

    if (n_values < 1) {
        status = JULIE_ERR_ARITY;
        julie_make_arity_error(interp, tree, 1, n_values, 1);
        *result = NULL;
        goto out;
    }

    short_circuit = 0;

    for (i = 0; i < n_values; i += 1) {
        cond   = values[i];
        status = julie_eval(interp, cond, &ev);
        if (status != JULIE_SUCCESS) {
            *result = NULL;
            goto out;
        }

        if (!JULIE_TYPE_IS_NUMBER(ev->type)) {
            status = JULIE_ERR_TYPE;
            julie_make_type_error(interp, ev, _JULIE_NUMBER, ev->type);
            julie_free_value(interp, ev);
            *result = NULL;
            goto out;
        }

        if (ev->type == JULIE_SINT) {
            short_circuit = ev->sint == 0;
        } else if (ev->type == JULIE_UINT) {
            short_circuit = ev->uint == 0;
        } else if (ev->type == JULIE_FLOAT) {
            short_circuit = ev->floating == 0;
        } else {
            JULIE_ASSERT(0 && "bad number type");
        }

        julie_free_value(interp, ev);

        if (short_circuit) {
            *result = julie_sint_value(0);
            goto out;
        }
    }

    *result = julie_sint_value(1);

out:;
    return status;
}

static Julie_Status julie_builtin_or(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    int          short_circuit;
    unsigned     i;
    Julie_Value  *cond;
    Julie_Value  *ev;

    status = JULIE_SUCCESS;

    if (n_values < 1) {
        status = JULIE_ERR_ARITY;
        julie_make_arity_error(interp, tree, 1, n_values, 1);
        *result = NULL;
        goto out;
    }

    short_circuit = 0;

    for (i = 0; i < n_values; i += 1) {
        cond = values[i];
        status = julie_eval(interp, cond, &ev);
        if (status != JULIE_SUCCESS) {
            *result = NULL;
            goto out;
        }

        if (!JULIE_TYPE_IS_NUMBER(ev->type)) {
            status = JULIE_ERR_TYPE;
            julie_make_type_error(interp, ev, _JULIE_NUMBER, ev->type);
            julie_free_value(interp, ev);
            *result = NULL;
            goto out;
        }

        if (ev->type == JULIE_SINT) {
            short_circuit = ev->sint != 0;
        } else if (ev->type == JULIE_UINT) {
            short_circuit = ev->uint != 0;
        } else if (ev->type == JULIE_FLOAT) {
            short_circuit = ev->floating != 0;
        } else {
            JULIE_ASSERT(0 && "bad number type");
        }

        julie_free_value(interp, ev);

        if (short_circuit) {
            *result = julie_sint_value(1);
            goto out;
        }
    }

    *result = julie_sint_value(0);

out:;
    return status;
}

static Julie_Status julie_builtin_list(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *list;
    unsigned     i;
    Julie_Value  *it;
    Julie_Value  *ev;
    Julie_Value  *tmp;

    (void)tree;

    status = JULIE_SUCCESS;

    list = julie_list_value();

    for (i = 0; i < n_values; i += 1) {
        it     = values[i];
        status = julie_eval(interp, it, &ev);
        if (status != JULIE_SUCCESS) {
            *result = NULL;
            goto out_free;
        }

        if (ev->in_symtab) {
            tmp = julie_copy_force(ev);
            julie_free_value(interp, ev);
            ev = tmp;
        }

        JULIE_PUSH(list->list, ev);
    }

    *result = list;
    goto out;

out_free:;
    julie_free_value(interp, list);

out:;
    return status;
}

static Julie_Status julie_builtin_elem(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status   status;
    Julie_Value   *list;
    Julie_Value   *idx;
    unsigned      i;
    Julie_Value   *val;

    status = julie_args(interp, tree, "li", n_values, values, &list, &idx);
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

    if (i >= julie_len(list->list)) {
        status = JULIE_ERR_BAD_INDEX;
        julie_make_bad_index_error(interp, idx, julie_copy(idx));
        *result = NULL;
        goto out_free;
    }

    val            = julie_elem(list->list, i);
    val->in_symtab = list->in_symtab;
    val->local     = list->local;

    *result = julie_copy(val);

out_free:;
    julie_free_value(interp, idx);
    julie_free_value(interp, list);

out:;
    return status;
}

static Julie_Status julie_builtin_apply(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *list;

    status = julie_args(interp, tree, "l", n_values, values, &list);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    status = julie_eval(interp, list, result);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out_free;
    }

out_free:;
    julie_free_value(interp, list);

out:;
    return status;
}

static Julie_Status julie_builtin_do(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    unsigned     i;
    Julie_Value  *it;
    Julie_Value  *ev;

    status  = JULIE_SUCCESS;
    *result = NULL;

    if (n_values < 1) {
        status = JULIE_ERR_ARITY;
        julie_make_arity_error(interp, tree, 1, n_values, 1);
        *result = NULL;
        goto out;
    }

    for (i = 0; i < n_values; i += 1) {
        it     = values[i];
        status = julie_eval(interp, it, &ev);
        if (status != JULIE_SUCCESS) {
            if (*result != NULL) {
                julie_free_value(interp, *result);
            }
            *result = NULL;
            goto out;
        }

        if (i == n_values - 1) {
            *result = ev;
        } else {
            julie_free_value(interp, ev);
        }
    }

    if (*result == NULL) {
        *result = julie_nil_value();
    }

out:;
    return status;
}

static Julie_Status julie_builtin_if(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *cond;
    unsigned     truth;
    unsigned     i;
    Julie_Value  *it;
    Julie_Value  *ev;

    status = JULIE_SUCCESS;

    if (n_values < 2) {
        status = JULIE_ERR_ARITY;
        julie_make_arity_error(interp, tree, 2, n_values, 2);
        *result = NULL;
        goto out;
    }

    status = julie_eval(interp, values[0], &cond);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    if (!JULIE_TYPE_IS_NUMBER(cond->type)) {
        status = JULIE_ERR_TYPE;
        julie_make_type_error(interp, cond, _JULIE_NUMBER, cond->type);
        goto out_free_cond;
    }

    if (cond->type == JULIE_SINT) {
        truth = !!cond->sint;
    } else if (cond->type == JULIE_UINT) {
        truth = !!cond->uint;
    } else if (cond->type == JULIE_FLOAT) {
        truth = !!cond->floating;
    } else {
        JULIE_ASSERT(0 && "bad number type");
    }

    if (truth) {
        for (i = 1; i < n_values; i += 1) {
            it     = values[i];
            status = julie_eval(interp, it, &ev);
            if (status != JULIE_SUCCESS) {
                if (*result != NULL) {
                    julie_free_value(interp, *result);
                }
                *result = NULL;
                goto out_free_cond;
            }

            if (i == n_values - 1) {
                *result = ev;
            } else {
                julie_free_value(interp, ev);
            }
        }
    }

    if (*result == NULL) {
        *result = julie_nil_value();
    }

    interp->last_if_was_true = truth;

out_free_cond:;
    julie_free_value(interp, cond);

out:;
    return status;
}

static Julie_Status julie_builtin_elif(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *cond;
    unsigned     truth;
    unsigned     i;
    Julie_Value  *it;
    Julie_Value  *ev;

    status = JULIE_SUCCESS;

    if (interp->last_popped_builtin_fn != julie_builtin_if
    &&  interp->last_popped_builtin_fn != julie_builtin_elif) {
        status  = JULIE_ERR_MUST_FOLLOW_IF;
        *result = NULL;
        julie_make_must_follow_if_error(interp, tree);
        goto out;
    }

    if (n_values < 2) {
        status = JULIE_ERR_ARITY;
        julie_make_arity_error(interp, tree, 2, n_values, 2);
        *result = NULL;
        goto out;
    }

    if (!interp->last_if_was_true) {
        status = julie_eval(interp, values[0], &cond);
        if (status != JULIE_SUCCESS) {
            *result = NULL;
            goto out;
        }


        if (!JULIE_TYPE_IS_NUMBER(cond->type)) {
            status = JULIE_ERR_TYPE;
            julie_make_type_error(interp, cond, _JULIE_NUMBER, cond->type);
            goto out_free_cond;
        }

        if (cond->type == JULIE_SINT) {
            truth = !!cond->sint;
        } else if (cond->type == JULIE_UINT) {
            truth = !!cond->uint;
        } else if (cond->type == JULIE_FLOAT) {
            truth = !!cond->floating;
        } else {
            JULIE_ASSERT(0 && "bad number type");
        }

        if (truth) {
            for (i = 1; i < n_values; i += 1) {
                it     = values[i];
                status = julie_eval(interp, it, &ev);
                if (status != JULIE_SUCCESS) {
                    if (*result != NULL) {
                        julie_free_value(interp, *result);
                    }
                    *result = NULL;
                    goto out_free_cond;
                }

                if (i == n_values - 1) {
                    *result = ev;
                } else {
                    julie_free_value(interp, ev);
                }
            }
        }

        interp->last_if_was_true = truth;

out_free_cond:;
        julie_free_value(interp, cond);
    }

    if (*result == NULL) {
        *result = julie_nil_value();
    }

out:;
    return status;
}

static Julie_Status julie_builtin_else(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    unsigned     i;
    Julie_Value  *it;
    Julie_Value  *ev;

    status  = JULIE_SUCCESS;
    *result = NULL;

    if (interp->last_popped_builtin_fn != julie_builtin_if
    &&  interp->last_popped_builtin_fn != julie_builtin_elif) {
        status  = JULIE_ERR_MUST_FOLLOW_IF;
        *result = NULL;
        julie_make_must_follow_if_error(interp, tree);
        goto out;
    }

    if (n_values < 1) {
        status = JULIE_ERR_ARITY;
        julie_make_arity_error(interp, tree, 1, n_values, 1);
        *result = NULL;
        goto out;
    }

    if (!interp->last_if_was_true) {
        for (i = 0; i < n_values; i += 1) {
            it     = values[i];
            status = julie_eval(interp, it, &ev);
            if (status != JULIE_SUCCESS) {
                if (*result != NULL) {
                    julie_free_value(interp, *result);
                }
                *result = NULL;
                goto out;
            }

            if (i == n_values - 1) {
                *result = ev;
            } else {
                julie_free_value(interp, ev);
            }
        }
    }

    if (*result == NULL) {
        *result = julie_nil_value();
    }

out:;
    return status;
}

static Julie_Status julie_builtin_while(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *_cond;
    Julie_Value  *expr;
    Julie_Value  *cond;
    int          cont;
    unsigned     i;
    Julie_Value  *_expr;
    Julie_Value  *expr_cpy;

    status = JULIE_SUCCESS;

    *result = NULL;

    if (n_values < 2) {
        status = JULIE_ERR_ARITY;
        julie_make_arity_error(interp, tree, 2, n_values, 1);
        *result = NULL;
        goto out;
    }

    _cond = values[0];
    expr  = NULL;

    for (;;) {
        status = julie_eval(interp, _cond, &cond);
        if (status != JULIE_SUCCESS) {
            *result = NULL;
            goto out;
        }

        if (!JULIE_TYPE_IS_NUMBER(cond->type)) {
            status = JULIE_ERR_TYPE;
            julie_make_type_error(interp, cond, _JULIE_NUMBER, cond->type);
            julie_free_value(interp, cond);
            goto out;
        }

        if (cond->type == JULIE_SINT) {
            cont = !!cond->sint;
        } else if (cond->type == JULIE_UINT) {
            cont = !!cond->uint;
        } else if (cond->type == JULIE_FLOAT) {
            cont = !!cond->floating;
        } else {
            JULIE_ASSERT(0 && "bad number type");
        }

        julie_free_value(interp, cond);

        if (!cont) {
            *result = expr != NULL
                        ? expr
                        : julie_nil_value();
            break;
        }

        if (expr != NULL) {
            julie_free_value(interp, expr);
        }

        for (i = 1; i < n_values; i += 1) {
            _expr  = values[i];
            status = julie_eval(interp, _expr, &expr);
            if (status != JULIE_SUCCESS) {
                *result = NULL;
                goto out;
            }

            if (i < n_values - 1) {
                julie_free_value(interp, expr);
                expr = NULL;
            }
        }

        /* Get a copy of the resulting value that we know can't be deleted while running the condition expression. */
        expr_cpy = julie_copy_force(expr);
        julie_free_value(interp, expr);
        expr = expr_cpy;
    }

out:;
    return status;
}

static Julie_Status _julie_builtin_print(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result, int nl) {
    Julie_Status  status;
    unsigned     i;
    Julie_Value  *it;
    Julie_Value  *ev;

    (void)tree;

    status = JULIE_SUCCESS;

    for (i = 0; i < n_values; i += 1) {
        it = values[i];
        status = julie_eval(interp, it, &ev);
        if (status != JULIE_SUCCESS) {
            *result = NULL;
            goto out;
        }
        julie_print(interp, ev, 0);

        if (i == n_values - 1) {
            *result = ev;
        } else {
            julie_free_value(interp, ev);
        }
    }

    if (*result == NULL) {
        *result = julie_nil_value();
    }

    if (nl) { julie_output(interp, "\n", 1); }

out:;
    return status;
}

static Julie_Status julie_builtin_print(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    return _julie_builtin_print(interp, tree, n_values, values, result, 0);
}

static Julie_Status julie_builtin_println(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    return _julie_builtin_print(interp, tree, n_values, values, result, 1);
}

static Julie_Status julie_builtin_fmt(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status        status;
    Julie_Value        *fmt;
    const Julie_String *fstring;
    unsigned           n;
    unsigned           extra;
    char               last;
    char               c;
    unsigned           i;
    Julie_Array        *strings = JULIE_ARRAY_INIT;
    Julie_Value        *it;
    Julie_Value        *ev;
    char              *s;
    int                len;
    char              *formatted;
    char              *ins;
    int                sublen;

    status = JULIE_SUCCESS;

    if (n_values < 1) {
        status = JULIE_ERR_ARITY;
        julie_make_arity_error(interp, tree, 1, n_values, 1);
        *result = NULL;
        goto out;
    }

    status = julie_eval(interp, values[0], &fmt);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    if (fmt->type != JULIE_STRING) {
        status = JULIE_ERR_TYPE;
        julie_make_type_error(interp, fmt, JULIE_STRING, fmt->type);
        *result = NULL;
        goto out_free_fmt;
    }

    fstring = julie_get_string(interp, fmt->string_id);
    n       = 0;
    extra   = 0;
    last    = 0;
    for (i = 0; i < fstring->len; i += 1) {
        c = fstring->chars[i];
        if (c == '%') {
            extra += 1;
            if (last != '\\') {
                n += 1;
            }
        }
        last = c;
    }

    if (n_values - 1 != n) {
        status = JULIE_ERR_ARITY;
        julie_make_arity_error(interp, tree, n + 1, n_values, 0);
        *result = NULL;
        goto out_free_fmt;
    }

    for (i = 1; i < n_values; i += 1) {
        it = values[i];
        status = julie_eval(interp, it, &ev);
        if (status != JULIE_SUCCESS) {
            *result = NULL;
            goto out_free_strings;
        }
        s = julie_to_string(interp, ev, JULIE_NO_QUOTE);
        JULIE_PUSH(strings, s);
        julie_free_value(interp, ev);
    }

    len = fstring->len - extra;
    FOR_EACH(strings, s) {
        len += strlen(s);
    }

    formatted = JULIE_MALLOC(len + 1);
    ins       = formatted;

    n    = 0;
    last = 0;
    for (i = 0; i < fstring->len; i += 1) {
        c = fstring->chars[i];
        if (c == '\\' && i < fstring->len - 1 && fstring->chars[i + 1] == '%') {
            /* skip */
        } else if (c == '%' && last != '\\') {
            s      = julie_elem(strings, n);
            sublen = strlen(s);
            memcpy(ins, s, sublen);
            ins += sublen;
            n += 1;
        } else {
            *ins  = c;
            ins  += 1;
        }
        last = c;
    }

    formatted[len] = 0;

    *result = julie_string_value(interp, formatted);

    JULIE_FREE(formatted);

out_free_strings:;
    FOR_EACH(strings, s) {
        JULIE_FREE(s);
    }
    julie_free_array(strings);

out_free_fmt:;
    julie_free_value(interp, fmt);

out:;
    return status;
}

static Julie_Status julie_builtin_printf(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status        status;
    Julie_Value        *fmt;
    const Julie_String *fstring;
    unsigned           n;
    unsigned           extra;
    char               last;
    char               c;
    unsigned           i;
    Julie_Array        *strings = JULIE_ARRAY_INIT;
    Julie_Value        *it;
    Julie_Value        *ev;
    char              *s;
    int                len;
    char              *formatted;
    char              *ins;
    int                sublen;

    status = JULIE_SUCCESS;

    if (n_values < 1) {
        status = JULIE_ERR_ARITY;
        julie_make_arity_error(interp, tree, 1, n_values, 1);
        *result = NULL;
        goto out;
    }

    status = julie_eval(interp, values[0], &fmt);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    if (fmt->type != JULIE_STRING) {
        status = JULIE_ERR_TYPE;
        julie_make_type_error(interp, fmt, JULIE_STRING, fmt->type);
        *result = NULL;
        goto out_free_fmt;
    }

    fstring = julie_get_string(interp, fmt->string_id);
    n       = 0;
    extra   = 0;
    last    = 0;
    for (i = 0; i < fstring->len; i += 1) {
        c = fstring->chars[i];
        if (c == '%') {
            extra += 1;
            if (last != '\\') {
                n += 1;
            }
        }
        last = c;
    }

    if (n_values - 1 != n) {
        status = JULIE_ERR_ARITY;
        julie_make_arity_error(interp, tree, n + 1, n_values, 0);
        *result = NULL;
        goto out_free_fmt;
    }

    for (i = 1; i < n_values; i += 1) {
        it = values[i];
        status = julie_eval(interp, it, &ev);
        if (status != JULIE_SUCCESS) {
            *result = NULL;
            goto out_free_strings;
        }
        s = julie_to_string(interp, ev, JULIE_NO_QUOTE);
        JULIE_PUSH(strings, s);
        julie_free_value(interp, ev);
    }

    len = fstring->len - extra;
    FOR_EACH(strings, s) {
        len += strlen(s);
    }

    formatted = JULIE_MALLOC(len + 1);
    ins       = formatted;

    n    = 0;
    last = 0;
    for (i = 0; i < fstring->len; i += 1) {
        c = fstring->chars[i];
        if (c == '\\' && i < fstring->len - 1 && fstring->chars[i + 1] == '%') {
            /* skip */
        } else if (c == '%' && last != '\\') {
            s      = julie_elem(strings, n);
            sublen = strlen(s);
            memcpy(ins, s, sublen);
            ins += sublen;
            n += 1;
        } else {
            *ins  = c;
            ins  += 1;
        }
        last = c;
    }

    formatted[len] = 0;

    *result = julie_string_value(interp, formatted);

    julie_print(interp, *result, 0);

    JULIE_FREE(formatted);

out_free_strings:;
    FOR_EACH(strings, s) {
        JULIE_FREE(s);
    }
    julie_free_array(strings);

out_free_fmt:;
    julie_free_value(interp, fmt);

out:;
    return status;
}

static Julie_Status julie_builtin_string(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *val;
    char        *s;

    status = julie_args(interp, tree, "*", n_values, values, &val);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    s = julie_to_string(interp, val, JULIE_NO_QUOTE);
    *result = julie_string_value(interp, s);
    JULIE_FREE(s);

    julie_free_value(interp, val);

out:;
    return status;
}

static Julie_Status julie_builtin_symbol(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status        status;
    Julie_Value        *str;
    const Julie_String *string;

    status = julie_args(interp, tree, "s", n_values, values, &str);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    string  = julie_get_string(interp, str->string_id);
    *result = julie_symbol_value(interp, string->chars);

    julie_free_value(interp, str);

out:;
    return status;
}

static Julie_Status julie_builtin_pad(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *w;
    Julie_Value  *val;
    int          width;
    int          ljust;
    char        *s;
    int          len;
    int          padding;
    char        *padded;

    status = julie_args(interp, tree, "i*", n_values, values, &w, &val);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    width = (int)w->sint;
    ljust = width < 0;
    s     = julie_to_string(interp, val, JULIE_NO_QUOTE);
    len   = strlen(s);

    if (ljust) { width = -width; }

    padding = width > len
                ? width - len
                : 0;

    padded = JULIE_MALLOC(len + padding + 1);
    memset(padded, ' ', len + padding);
    memcpy(padded + ((!ljust) * padding), s, len);
    padded[len + padding] = 0;

    *result = julie_string_value(interp, padded);

    JULIE_FREE(padded);
    JULIE_FREE(s);

    julie_free_value(interp, w);
    julie_free_value(interp, val);

out:;
    return status;
}

static Julie_Status julie_builtin_num_fmt(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *fmt;
    Julie_Value  *val;
    char         fbuff[128];
    char         buff[128];

    status = julie_args(interp, tree, "sn", n_values, values, &fmt, &val);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    snprintf(fbuff, sizeof(fbuff), "%%%s", julie_get_string(interp, fmt->string_id)->chars);

    if (val->type == JULIE_SINT) {
        snprintf(buff, sizeof(buff), fbuff, val->sint);
    } else if (val->type == JULIE_UINT) {
        snprintf(buff, sizeof(buff), fbuff, val->uint);
    } else if (val->type == JULIE_FLOAT) {
        snprintf(buff, sizeof(buff), fbuff, val->floating);
    } else {
        JULIE_ASSERT(0 && "bad number type");
    }

    *result = julie_string_value(interp, buff);

    julie_free_value(interp, fmt);
    julie_free_value(interp, val);

out:;
    return status;
}


static Julie_Status julie_builtin_parse_int(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *s;
    long long    i;

    status = julie_args(interp, tree, "s", n_values, values, &s);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    if (sscanf(julie_get_string(interp, s->string_id)->chars, "%lld", &i) == 1) {
        *result = julie_sint_value(i);
    } else {
        *result = julie_nil_value();
    }

    julie_free_value(interp, s);

out:;
    return status;
}

static Julie_Status julie_builtin_parse_hex(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status         status;
    Julie_Value         *s;
    unsigned long long  i;

    status = julie_args(interp, tree, "s", n_values, values, &s);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    if (sscanf(julie_get_string(interp, s->string_id)->chars, "%llx", &i) == 1) {
        *result = julie_uint_value(i);
    } else {
        *result = julie_nil_value();
    }

    julie_free_value(interp, s);

out:;
    return status;
}

static Julie_Status julie_builtin_parse_float(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *s;
    double       d;

    status = julie_args(interp, tree, "s", n_values, values, &s);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    if (sscanf(julie_get_string(interp, s->string_id)->chars, "%lg", &d) == 1) {
        *result = julie_float_value(d);
    } else {
        *result = julie_nil_value();
    }

    julie_free_value(interp, s);

out:;
    return status;
}


#if 0

static Julie_Status julie_builtin_eval(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *val;

    status = julie_args(interp, tree, "*", n_values, values, &val);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    status = julie_eval(interp, val, result);

    julie_free_value(interp, val);

out:;
    return status;
}


static Julie_Status julie_builtin_eset(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *sym;
    Julie_Value  *val;
    Julie_Value  *cpy;

    status = julie_args(interp, tree, "$*", n_values, values, &sym, &val);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    if (val->in_symtab) {
        cpy = julie_copy_force(val);
        julie_free_value(interp, val);
        val = cpy;
    }

    status = julie_install_var(interp, sym->symbol_id, val);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        julie_make_install_error(interp, tree, status, sym->symbol_id);
        julie_free_value(interp, val);
        goto out_free;
    }

    *result = val;

out_free:;
    julie_free_value(interp, sym);

out:;
    return status;
}

static Julie_Status julie_builtin_elocal(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *sym;
    Julie_Value  *val;
    Julie_Value  *cpy;

    status = julie_args(interp, tree, "$*", n_values, values, &sym, &val);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    if (val->in_symtab) {
        cpy = julie_copy_force(val);
        julie_free_value(interp, val);
        val = cpy;
    }

    status = julie_install_local(interp, sym->symbol_id, val);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        julie_make_install_error(interp, tree, status, sym->symbol_id);
        julie_free_value(interp, val);
        goto out_free;
    }

    *result = val;

out_free:;
    julie_free_value(interp, sym);

out:;
    return status;
}

static Julie_Status julie_builtin_fn(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *def_tree;
    Julie_Value  *it;
    Julie_Value  *sym;
    Julie_Value  *fn;

    status = JULIE_SUCCESS;

    if (n_values < 2) {
        status = JULIE_ERR_ARITY;
        julie_make_arity_error(interp, tree, 2, n_values, 1);
        *result = NULL;
        goto out;
    }

    def_tree = values[0];

    if (def_tree->type == _JULIE_TREE || def_tree->type == _JULIE_TREE_LINE_LEADER) {
        FOR_EACH(def_tree->list, it) {
            if (it->type != JULIE_SYMBOL) {
                status = JULIE_ERR_TYPE;
                julie_make_type_error(interp, def_tree, JULIE_SYMBOL, it->type);
                *result = NULL;
                goto out;
            }
        }
        sym = julie_elem(def_tree->list, 0);
    } else if (def_tree->type == JULIE_SYMBOL) {
        sym = def_tree;
    } else {
        status = JULIE_ERR_TYPE;
        julie_make_type_error(interp, def_tree, JULIE_SYMBOL, def_tree->type);
        *result = NULL;
        goto out;
    }

    fn       = julie_copy(tree);
    fn->type = _JULIE_FN;

    status = julie_install_var(interp, sym->symbol_id, fn);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        julie_make_install_error(interp, sym, status, sym->symbol_id);
        julie_free_value(interp, fn);
        goto out;
    }

    *result = fn;

out:;
    return status;
}

static Julie_Status julie_builtin_localfn(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *def_tree;
    Julie_Value  *it;
    Julie_Value  *sym;
    Julie_Value  *fn;

    status = JULIE_SUCCESS;

    if (n_values < 2) {
        status = JULIE_ERR_ARITY;
        julie_make_arity_error(interp, tree, 2, n_values, 1);
        *result = NULL;
        goto out;
    }

    def_tree = values[0];

    if (def_tree->type == _JULIE_TREE) {
        FOR_EACH(def_tree->list, it) {
            if (it->type != JULIE_SYMBOL) {
                status = JULIE_ERR_TYPE;
                julie_make_type_error(interp, def_tree, JULIE_SYMBOL, it->type);
                *result = NULL;
                goto out;
            }
        }
        sym = julie_elem(def_tree->list, 0);
    } else if (def_tree->type == JULIE_SYMBOL) {
        sym = def_tree;
    } else {
        status = JULIE_ERR_TYPE;
        julie_make_type_error(interp, def_tree, JULIE_SYMBOL, def_tree->type);
        *result = NULL;
        goto out;
    }

    fn       = julie_copy(tree);
    fn->type = _JULIE_FN;

    status = julie_install_local(interp, sym->symbol_id, fn);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        julie_make_install_error(interp, sym, status, sym->symbol_id);
        julie_free_value(interp, fn);
        goto out;
    }

    *result = fn;

out:;
    return status;
}

static void _julie_collect_lambda_free_variables(Julie_Interp *interp, Julie_Value *tree, Julie_Array *bounds, Julie_Array **frees) {
    Julie_Value *it;
    Julie_Value *first;

    switch (tree->type) {
        case JULIE_SYMBOL:
            FOR_EACH(bounds, it) {
                if (tree->symbol_id == it->symbol_id) { return; }
            }
            FOR_EACH(*frees, it) {
                if (tree->symbol_id == it->symbol_id) { return; }
            }

            JULIE_PUSH(*frees, tree);
            break;

        case _JULIE_TREE:
        case _JULIE_TREE_LINE_LEADER:
            first = julie_elem(tree->list, 0);

            if (first->type == JULIE_SYMBOL
            &&  ( first->symbol_id == julie_get_string_id(interp, "lambda")
               || first->symbol_id == julie_get_string_id(interp, "fn")
               || first->symbol_id == julie_get_string_id(interp, "localfn")
               || first->symbol_id == julie_get_string_id(interp, "quote"))) {

                /* Skip these forms. */
                return;
            } else {
                FOR_EACH(tree->list, it) {
                    _julie_collect_lambda_free_variables(interp, it, bounds, frees);
                }
            }
            break;
    }
}

static Julie_Status julie_builtin_lambda(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status        status;
    Julie_Value        *def_tree;
    Julie_Array        *bounds = JULIE_ARRAY_INIT;
    Julie_Value        *it;
    Julie_Value        *fn;
    Julie_Closure_Info *closure;
    Julie_Array        *frees = JULIE_ARRAY_INIT;
    Julie_Value        *lookup;

    status = JULIE_SUCCESS;

    if (n_values < 1 || n_values > 2) {
        status = JULIE_ERR_ARITY;
        julie_make_arity_error(interp, tree, 2 - (n_values == 1), n_values, 1 - (n_values == 2));
        *result = NULL;
        goto out;
    }

    if (n_values == 2) {
        def_tree = values[0];

        if (def_tree->type == _JULIE_TREE) {
            FOR_EACH(def_tree->list, it) {
                if (it->type != JULIE_SYMBOL) {
                    status = JULIE_ERR_TYPE;
                    julie_make_type_error(interp, def_tree, JULIE_SYMBOL, it->type);
                    *result = NULL;
                    goto out;
                }
                JULIE_PUSH(bounds, it);
            }
        } else {
            status = JULIE_ERR_TYPE;
            julie_make_type_error(interp, def_tree, _JULIE_TREE, def_tree->type);
            *result = NULL;
            goto out;
        }
    }

    fn       = julie_copy(tree);
    fn->type = _JULIE_LAMBDA;

    closure = JULIE_MALLOC(sizeof(*closure));

    closure->cur_file = fn->list->aux;
    closure->captures = hash_table_make(Julie_String_ID, Julie_Value_Ptr, julie_string_id_hash);

    _julie_collect_lambda_free_variables(interp, values[n_values == 2], bounds, &frees);

    FOR_EACH(frees, it) {
        lookup = julie_lookup(interp, it->symbol_id);
        if (lookup != NULL) {
            hash_table_insert(closure->captures, it->symbol_id, julie_copy_force(lookup));
        }
    }

    julie_free_array(frees);
    julie_free_array(bounds);

    JULIE_SET_AUX(fn->list, closure);

    *result = fn;

out:;
    return status;
}


static Julie_Status julie_builtin_inc(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *a;

    status = julie_args(interp, tree, "n", n_values, values, &a);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    *result = julie_number_value(a->number);

    a->number += 1;

    julie_free_value(interp, a);

out:;
    return status;
}

static Julie_Status julie_builtin_dec(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *a;

    status = julie_args(interp, tree, "n", n_values, values, &a);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    *result = julie_number_value(a->number);

    a->number -= 1;

    julie_free_value(interp, a);

out:;
    return status;
}

static Julie_Status julie_builtin_select(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *cond;
    Julie_Value  *then;

    status = JULIE_SUCCESS;

    if (n_values != 3) {
        status = JULIE_ERR_ARITY;
        julie_make_arity_error(interp, tree, 3, n_values, 0);
        *result = NULL;
        goto out;
    }

    status = julie_eval(interp, values[0], &cond);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    if (cond->type != JULIE_NUMBER) {
        status = JULIE_ERR_TYPE;
        julie_make_type_error(interp, cond, JULIE_NUMBER, cond->type);
        goto out_free_cond;
    }

    then   = values[1 + (cond->number == 0)];
    status = julie_eval(interp, then, &then);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out_free_cond;
    }

    *result = then;

out_free_cond:;
    julie_free_value(interp, cond);

out:;
    return status;
}


static Julie_Status julie_builtin_foreach(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status   status;
    Julie_Value   *sym;
    Julie_Value   *_container;
    Julie_Value   *container;
    Julie_Value   *expr;
    unsigned      i;
    unsigned      j;
    Julie_Value   *it;
    Julie_Value   *ev;
    Julie_Value  **val;

    *result = NULL;

    if (n_values < 3) {
        status = JULIE_ERR_ARITY;
        julie_make_arity_error(interp, tree, 3, n_values, 1);
        *result = NULL;
        goto out;
    }

    sym        = values[0];
    _container = values[1];

    status = julie_eval(interp, _container, &container);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    if (container->type != JULIE_LIST
    &&  container->type != JULIE_OBJECT) {
        status = JULIE_ERR_TYPE;
        julie_make_type_error(interp, container, _JULIE_LIST_OR_OBJECT, container->type);
        goto out_free;
    }

    JULIE_BORROW(container);
    JULIE_PUSH(interp->iter_vals, container);

    if (container->type == JULIE_LIST) {
        i = 0;
        FOR_EACH(container->list, it) {
            JULIE_BORROWER(it);
            status = julie_install_local(interp, sym->symbol_id, it);
            if (status != JULIE_SUCCESS) {
                *result = NULL;
                julie_make_install_error(interp, sym, status, sym->symbol_id);
                goto out_unborrow;
            }

            for (j = 2; j < n_values; j += 1) {
                expr   = values[j];
                status = julie_eval(interp, expr, &ev);
                if (status != JULIE_SUCCESS) {
                JULIE_UNBORROWER(it);
                julie_uninstall_local_no_free(interp, sym->symbol_id);
                    *result = NULL;
                    goto out_unborrow;
                }

                if (j < n_values - 1) {
                    julie_free_value(interp, ev);
                    ev = NULL;
                }
            }

            i += 1;

            if (i == julie_len(container->list)) {
                if (ev == it) {
                    ev = julie_copy_force(it);
                }
                *result = ev;
            } else {
                julie_free_value(interp, ev);
            }

            JULIE_UNBORROWER(it);
            if (julie_lookup_local_only(interp, sym->symbol_id) == it) {
                status = julie_uninstall_local_no_free(interp, sym->symbol_id);
                if (status != JULIE_SUCCESS) {
                    *result = NULL;
                    julie_make_install_error(interp, sym, status, sym->symbol_id);
                    goto out_unborrow;
                }
            }
        }

    } else {
        i = 0;
        hash_table_traverse((_Julie_Object)container->object, it, val) {
            it = *val;

            JULIE_BORROWER(it);
            status = julie_install_local(interp, sym->symbol_id, it);
            if (status != JULIE_SUCCESS) {
                *result = NULL;
                julie_make_install_error(interp, sym, status, sym->symbol_id);
                goto out_unborrow;
            }

            for (j = 2; j < n_values; j += 1) {
                expr   = values[j];
                status = julie_eval(interp, expr, &ev);
                if (status != JULIE_SUCCESS) {
                JULIE_UNBORROWER(it);
                julie_uninstall_local_no_free(interp, sym->symbol_id);
                    *result = NULL;
                    goto out_unborrow;
                }

                if (j < n_values - 1) {
                    julie_free_value(interp, ev);
                    ev = NULL;
                }
            }

            i += 1;

            if (i == hash_table_len((_Julie_Object)container->object)) {
                if (ev == it) {
                    ev = julie_copy_force(it);
                }
                *result = ev;
            } else {
                julie_free_value(interp, ev);
            }

            JULIE_UNBORROWER(it);
            if (julie_lookup_local_only(interp, sym->symbol_id) == it) {
                status = julie_uninstall_local_no_free(interp, sym->symbol_id);
                if (status != JULIE_SUCCESS) {
                    *result = NULL;
                    julie_make_install_error(interp, sym, status, sym->symbol_id);
                    goto out_unborrow;
                }
            }
        }
    }

    if (*result == NULL) {
        *result = julie_nil_value();
    }

out_unborrow:;
    julie_pop(interp->iter_vals);
    JULIE_UNBORROW(container);

out_free:;
    julie_free_value(interp, container);

out:;
    return status;
}

static Julie_Status julie_builtin_range(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *beg;
    Julie_Value  *end;
    Julie_Value  *list;
    long long    i;

    (void)tree;

    status = JULIE_SUCCESS;

    status = julie_args(interp, tree, "nn", n_values, values, &beg, &end);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    list = julie_list_value();

    if ((long long)beg->number <= (long long)end->number) {
        for (i = (long long)beg->number; i < (long long)end->number; i += 1) {
            JULIE_PUSH(list->list, julie_number_value(i));
        }
    } else {
        for (i = (long long)beg->number; i > (long long)end->number; i -= 1) {
            JULIE_PUSH(list->list, julie_number_value(i));
        }
    }

    *result = list;

    julie_free_value(interp, beg);
    julie_free_value(interp, end);

out:;
    return status;
}

static Julie_Status julie_builtin_dot(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *first;
    Julie_Value  *second;
    Julie_Value  *tmp;
    Julie_Value  *list;

    status = julie_args(interp, tree, "**", n_values, values, &first, &second);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    if (first->in_symtab) {
        tmp = julie_copy_force(first);
        julie_free_value(interp, first);
        first = tmp;
    }
    if (second->in_symtab) {
        tmp = julie_copy_force(second);
        julie_free_value(interp, second);
        second = tmp;
    }

    list = julie_list_value();

    JULIE_PUSH(list->list, first);
    JULIE_PUSH(list->list, second);

    *result = list;

out:;
    return status;
}

static Julie_Status julie_builtin_object(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *object;
    unsigned     i;
    Julie_Value  *it;
    Julie_Value  *ev;
    Julie_Value  *key;
    Julie_Value  *val;

    (void)tree;

    status = JULIE_SUCCESS;

    object = julie_object_value();

    for (i = 0; i < n_values; i += 1) {
        it     = values[i];
        status = julie_eval(interp, it, &ev);
        if (status != JULIE_SUCCESS) {
            *result = NULL;
            goto out_free_object;
        }

        if (ev->type != JULIE_LIST) {
            status = JULIE_ERR_TYPE;
            julie_make_type_error(interp, ev, JULIE_LIST, ev->type);
            *result = NULL;
            goto out_free_list;
        }

        if (julie_len(ev->list) < 2) {
            status = JULIE_ERR_MISSING_VAL;
            julie_make_interp_error(interp, it, status);
            *result = NULL;
            goto out_free_list;
        }

        key = julie_copy_force(julie_elem(ev->list, 0));
        val = julie_copy_force(julie_elem(ev->list, 1));

        if (key->type != JULIE_STRING && key->type != JULIE_NUMBER) {
            status = JULIE_ERR_OBJECT_KEY_TYPE;
            julie_make_object_key_type_error(interp, key, key->type);
            *result = NULL;
            goto out_free_list;
        }

        julie_free_value(interp, ev);

        julie_insert(object, key, val);
    }

    *result = object;
    goto out;

out_free_list:;
    julie_free_value(interp, ev);

out_free_object:;
    julie_free_value(interp, object);

out:;
    return status;
}

static Julie_Status julie_builtin_in(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status   status;
    int           found;
    Julie_Value   *container;
    Julie_Value   *key;
    Julie_Value  **lookup;
    Julie_Value   *it;

    status = JULIE_SUCCESS;

    found = 0;

    if (n_values != 2) {
        status = JULIE_ERR_ARITY;
        julie_make_arity_error(interp, tree, 2, n_values, 0);
        *result = NULL;
        goto out;
    }

    status = julie_eval(interp, values[0], &container);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    status = julie_eval(interp, values[1], &key);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out_free_container;
    }

    if (container->type == JULIE_OBJECT) {
        if (key->type != JULIE_NUMBER && key->type != JULIE_STRING) {
            status = JULIE_ERR_OBJECT_KEY_TYPE;
            julie_make_object_key_type_error(interp, key, key->type);
            *result = NULL;
            goto out_free_key;
        }

        lookup = hash_table_get_val((_Julie_Object)container->object, key);

        found = lookup != NULL;

    } else if (container->type == JULIE_LIST) {
        FOR_EACH(container->list, it) {
            if (julie_equal(key, it)) {
                found = 1;
                break;
            }
        }
    } else {
        status = JULIE_ERR_TYPE;
        julie_make_type_error(interp, container, _JULIE_LIST_OR_OBJECT, container->type);
        *result = NULL;
        goto out_free_key;
    }

    *result = julie_number_value(found);

out_free_key:;
    julie_free_value(interp, key);

out_free_container:;
    julie_free_value(interp, container);

out:;
    return status;
}


static Julie_Status julie_builtin_index(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status   status;
    Julie_Value   *list;
    Julie_Value   *val;
    unsigned      i;
    Julie_Value   *it;

    status = julie_args(interp, tree, "l*", n_values, values, &list, &val);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    i = 0;
    FOR_EACH(list->list, it) {
        if (julie_equal(val, it)) {
            *result = julie_number_value(i);
            break;
        }
        i += 1;
    }

    if (*result == NULL) {
        *result = julie_number_value(-1);
    }

    julie_free_value(interp, val);
    julie_free_value(interp, list);

out:;
    return status;
}

static Julie_Status julie_builtin_append(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status   status;
    Julie_Value   *list;
    Julie_Value   *val;
    Julie_Value   *it;

    status = julie_args(interp, tree, "l!*", n_values, values, &list, &val);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    FOR_EACH(interp->iter_vals, it) {
        if (it == list) {
            status  = JULIE_ERR_MODIFY_WHILE_ITER;
            *result = NULL;
            julie_make_install_error(interp, tree, status, values[0]->type == JULIE_SYMBOL ? values[0]->symbol_id : NULL);
            julie_free_value(interp, list);
            julie_free_value(interp, val);
            goto out;
        }
    }

    JULIE_PUSH(list->list, val);

    *result = list;

out:;
    return status;
}

static Julie_Status julie_builtin_pop(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *list;
    Julie_Value  *it;
    Julie_Value  *last;

    status = julie_args(interp, tree, "l", n_values, values, &list);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    FOR_EACH(interp->iter_vals, it) {
        if (it == list) {
            status  = JULIE_ERR_MODIFY_WHILE_ITER;
            *result = NULL;
            julie_make_install_error(interp, tree, status, values[0]->type == JULIE_SYMBOL ? values[0]->symbol_id : NULL);
            julie_free_value(interp, list);
            goto out;
        }
    }

    if (julie_len(list->list) <= 0) {
        status = JULIE_ERR_BAD_INDEX;
        julie_make_bad_index_error(interp, tree, julie_number_value(-1));
        *result = NULL;
        goto out_free;
    }

    last = julie_top(list->list);

    if (last->borrow_count) {
        julie_make_install_error(interp, tree, JULIE_ERR_RELEASE_WHILE_BORROWED, NULL);
        *result = NULL;
        goto out_free;
    }

    *result = julie_pop(list->list);
    (*result)->in_symtab      = 0;
    (*result)->local          = 0;
    (*result)->borrow_count   = 0;
    (*result)->borrower_count = 0;

out_free:;
    julie_free_value(interp, list);

out:;
    return status;
}

static Julie_Status julie_builtin_field(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *object;
    Julie_Value  *key;
    Julie_Value  *field;

    status = JULIE_SUCCESS;

    if (n_values != 2) {
        status = JULIE_ERR_ARITY;
        julie_make_arity_error(interp, tree, 2, n_values, 0);
        *result = NULL;
        goto out;
    }

    status = julie_eval(interp, values[0], &object);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    if (object->type != JULIE_OBJECT) {
        status = JULIE_ERR_TYPE;
        julie_make_type_error(interp, object, JULIE_OBJECT, object->type);
        *result = NULL;
        goto out_free_object;
    }

    status = julie_eval(interp, values[1], &key);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out_free_object;
    }

    if (key->type != JULIE_NUMBER && key->type != JULIE_STRING) {
        status = JULIE_ERR_OBJECT_KEY_TYPE;
        julie_make_object_key_type_error(interp, key, key->type);
        *result = NULL;
        goto out_free_key;
    }

    field = julie_field(object, key);

    if (field == NULL) {
        status = JULIE_ERR_BAD_INDEX;
        julie_make_bad_index_error(interp, key, julie_copy(key));
        *result = NULL;
        goto out_free_key;
    } else {
        field->in_symtab = object->in_symtab;
        field->local     = object->local;
        *result = julie_copy(field);
    }

out_free_key:;
    julie_free_value(interp, key);

out_free_object:;
    julie_free_value(interp, object);

out:;
    return status;
}

static Julie_Status julie_builtin_insert(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *object;
    Julie_Value  *key;
    Julie_Value  *val;
    Julie_Value  *it;

    status = julie_args(interp, tree, "o!k!*", n_values, values, &object, &key, &val);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    FOR_EACH(interp->iter_vals, it) {
        if (it == object) {
            status  = JULIE_ERR_MODIFY_WHILE_ITER;
            *result = NULL;
            julie_make_install_error(interp, tree, status, values[0]->type == JULIE_SYMBOL ? values[0]->symbol_id : NULL);
            julie_free_value(interp, object);
            julie_free_value(interp, key);
            julie_free_value(interp, val);
            goto out;
        }
    }

    if (julie_insert(object, key, val) == JULIE_ERR_RELEASE_WHILE_BORROWED) {
        julie_make_install_error(interp, tree, JULIE_ERR_RELEASE_WHILE_BORROWED, NULL);
        *result = NULL;
        julie_free_value(interp, object);
        julie_free_value(interp, key);
        julie_free_value(interp, val);
        goto out;
    }

    *result = object;

out:;
    return status;
}

static Julie_Status julie_builtin_delete(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *object;
    Julie_Value  *key;
    Julie_Value  *it;

    status = julie_args(interp, tree, "ok", n_values, values, &object, &key);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    FOR_EACH(interp->iter_vals, it) {
        if (it == object) {
            status  = JULIE_ERR_MODIFY_WHILE_ITER;
            *result = NULL;
            julie_make_install_error(interp, tree, status, values[0]->type == JULIE_SYMBOL ? values[0]->symbol_id : NULL);
            julie_free_value(interp, object);
            julie_free_value(interp, key);
            goto out;
        }
    }

    if (julie_delete(object, key) == JULIE_ERR_RELEASE_WHILE_BORROWED) {
        julie_make_install_error(interp, tree, JULIE_ERR_RELEASE_WHILE_BORROWED, NULL);
        *result = NULL;
        julie_free_value(interp, object);
        julie_free_value(interp, key);
        goto out;
    }

    julie_free_value(interp, key);

    *result = object;

out:;
    return status;
}

static Julie_Status julie_builtin_update_object(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status   status;
    Julie_Value   *o1;
    Julie_Value   *o2;
    Julie_Value   *key;
    Julie_Value  **val;
    Julie_Value   *it;
    Julie_Value   *kcpy;
    Julie_Value   *vcpy;

    status = julie_args(interp, tree, "oo", n_values, values, &o1, &o2);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    FOR_EACH(interp->iter_vals, it) {
        if (it == o1) {
            status  = JULIE_ERR_MODIFY_WHILE_ITER;
            *result = NULL;
            julie_make_install_error(interp, tree, status, values[0]->type == JULIE_SYMBOL ? values[0]->symbol_id : NULL);
            julie_free_value(interp, o1);
            julie_free_value(interp, o2);
            goto out;
        }
    }

    hash_table_traverse((_Julie_Object)o2->object, key, val) {
        kcpy = julie_copy_force(key);
        vcpy = julie_copy_force(*val);
        if (julie_insert(o1, kcpy, vcpy) == JULIE_ERR_RELEASE_WHILE_BORROWED) {
            julie_free_value_force(kcpy);
            julie_free_value_force(vcpy);
            julie_make_install_error(interp, tree, JULIE_ERR_RELEASE_WHILE_BORROWED, NULL);
            *result = NULL;
            julie_free_value(interp, o1);
            julie_free_value(interp, o2);
            goto out;
        }
    }

    julie_free_value(interp, o2);

    *result = o1;

out:;
    return status;
}

static Julie_Status julie_builtin_erase(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status   status;
    Julie_Value   *list;
    Julie_Value   *idx;
    Julie_Value   *it;
    unsigned      i;
    Julie_Value   *val;

    status = julie_args(interp, tree, "ln", n_values, values, &list, &idx);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    FOR_EACH(interp->iter_vals, it) {
        if (it == list) {
            status  = JULIE_ERR_MODIFY_WHILE_ITER;
            *result = NULL;
            julie_make_install_error(interp, tree, status, values[0]->type == JULIE_SYMBOL ? values[0]->symbol_id : NULL);
            julie_free_value(interp, list);
            julie_free_value(interp, idx);
            goto out;
        }
    }

    i = (int)idx->number;

    julie_free_value(interp, idx);

    if (i >= julie_len(list->list)) {
        status = JULIE_ERR_BAD_INDEX;
        julie_make_bad_index_error(interp, idx, julie_copy(idx));
        *result = NULL;
        julie_free_value(interp, list);
        goto out;
    }

    val = julie_elem(list->list, i);
    if (val->borrow_count) {
        julie_make_install_error(interp, tree, JULIE_ERR_RELEASE_WHILE_BORROWED, NULL);
        *result = NULL;
        julie_free_value(interp, list);
        goto out;
    }

    julie_erase(list->list, (unsigned)idx->number);
    julie_free_value_force(val);

    *result = list;

out:;
    return status;
}

static Julie_Status julie_builtin_keys(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status   status;
    Julie_Value   *object;
    Julie_Value   *list;
    Julie_Value   *key;
    Julie_Value  **val;

    status = julie_args(interp, tree, "o", n_values, values, &object);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    list = julie_list_value();
    hash_table_traverse((_Julie_Object)object->object, key, val) {
        (void)val;
        JULIE_PUSH(list->list, julie_copy_force(key));
    }

    julie_free_value(interp, object);

    *result = list;

out:;
    return status;
}

static Julie_Status julie_builtin_values(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status   status;
    Julie_Value   *object;
    Julie_Value   *list;
    Julie_Value   *key;
    Julie_Value  **val;

    status = julie_args(interp, tree, "o", n_values, values, &object);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    list = julie_list_value();
    hash_table_traverse((_Julie_Object)object->object, key, val) {
        (void)key;
        JULIE_PUSH(list->list, julie_copy_force(*val));
    }

    julie_free_value(interp, object);

    *result = list;

out:;
    return status;
}

typedef struct {
    Julie_Interp *interp;
    Julie_Type    sort_type;
} _Julie_Sort_Arg;

static int julie_sort_value_cmp(const void *_a, const void *_b, void *_arg) {
    int               r;
    const Julie_Value *a;
    const Julie_Value *b;
    _Julie_Sort_Arg   *arg;
    double            ad;
    double            bd;
    char             *ac;
    char             *bc;
    const char       *as;
    const char       *bs;

    r   = 0;
    a   = *(const Julie_Value**)_a;
    b   = *(const Julie_Value**)_b;
    arg = _arg;

    if (a->type == JULIE_NIL) { return -1; }
    if (b->type == JULIE_NIL) { return  1; }

    if (arg->sort_type == JULIE_NUMBER) {
        ad = a->number;
        bd = b->number;

        if      (ad == bd) { r =  0; }
        else if (ad <  bd) { r = -1; }
        else               { r =  1; }
    } else {
        JULIE_ASSERT(arg->sort_type == JULIE_STRING);

        ac = bc = NULL;
        as = a->type == JULIE_STRING
                ? julie_get_string(arg->interp, a->string_id)->chars
                : (ac = julie_to_string(arg->interp, a, JULIE_NO_QUOTE));
        bs = b->type == JULIE_STRING
                ? julie_get_string(arg->interp, b->string_id)->chars
                : (bc = julie_to_string(arg->interp, b, JULIE_NO_QUOTE));

        r = strcmp(as, bs);

        if (ac != NULL) { JULIE_FREE(ac); }
        if (bc != NULL) { JULIE_FREE(bc); }
    }

    return r;
}

static Julie_Status julie_builtin_sorted(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status     status;
    Julie_Value     *list;
    Julie_Value     *sorted;
    Julie_Type       sort_type;
    Julie_Value     *it;
    _Julie_Sort_Arg  sort_arg;

    status = julie_args(interp, tree, "l", n_values, values, &list);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    sorted = julie_copy_force(list);

    if (julie_len(sorted->list) > 0) {
        sort_type = JULIE_UNKNOWN;

        FOR_EACH(sorted->list, it) {
            if (JULIE_TYPE_IS_KEYLIKE(it->type)) {
                if (it->type == JULIE_STRING) {
                    sort_type = JULIE_STRING;
                } else if (sort_type != JULIE_STRING) {
                    sort_type = JULIE_NUMBER;
                }
            } else {
                status = JULIE_ERR_TYPE;
                julie_make_type_error(interp, list, _JULIE_KEYLIKE, it->type);
                *result = NULL;
                julie_free_value(interp, sorted);
                goto out_free;
            }
        }

        JULIE_ASSERT(sort_type);

        sort_arg.interp    = interp;
        sort_arg.sort_type = sort_type;

        sort_r(sorted->list->data, julie_len(sorted->list), sizeof(*(sorted->list->data)), julie_sort_value_cmp, &sort_arg);
    }

    *result = sorted;

out_free:;
    julie_free_value(interp, list);

out:;
    return status;
}

static Julie_Status julie_builtin_map(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *f;
    Julie_Value  *list;
    Julie_Value  *t;
    Julie_Value  *mapped;
    Julie_Value  *it;
    Julie_Value  *ev;

    status = julie_args(interp, tree, "*l", n_values, values, &f, &list);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    t = ((tree->type == _JULIE_TREE || tree->type == _JULIE_TREE_LINE_LEADER) && julie_len(tree->list) > 1)
            ? julie_elem(tree->list, 1)
            : tree;

    mapped = julie_list_value();

    FOR_EACH(list->list, it) {
        status = julie_invoke(interp, t, f, 1, &it, &ev);
        if (status != JULIE_SUCCESS) {
            julie_free_value(interp, mapped);
            *result = NULL;
            goto out_free;
        }
        if (ev->in_symtab || ev->borrower_count) {
            /* Make sure we get a value that can be owned by our new list. */
            ev = julie_copy_force(ev);
        }
        JULIE_PUSH(mapped->list, ev);
    }

    *result = mapped;

out_free:;
    julie_free_value(interp, list);
    julie_free_value(interp, f);

out:;
    return status;
}

static Julie_Status julie_builtin_filter(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *f;
    Julie_Value  *list;
    Julie_Value  *t;
    Julie_Value  *filtered;
    Julie_Value  *it;
    Julie_Value  *ev;

    status = julie_args(interp, tree, "*l", n_values, values, &f, &list);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    t = ((tree->type == _JULIE_TREE || tree->type == _JULIE_TREE_LINE_LEADER) && julie_len(tree->list) > 1)
            ? julie_elem(tree->list, 1)
            : tree;

    filtered = julie_list_value();

    FOR_EACH(list->list, it) {
        status = julie_invoke(interp, t, f, 1, &it, &ev);
        if (status != JULIE_SUCCESS) {
            julie_free_value(interp, filtered);
            *result = NULL;
            goto out_free;
        }
        if (ev->type != JULIE_NUMBER) {
            status = JULIE_ERR_TYPE;
            julie_make_type_error(interp, ev, JULIE_NUMBER, ev->type);
            julie_free_value(interp, ev);
            julie_free_value(interp, filtered);
            *result = NULL;
            goto out_free;
        }
        if (ev->number != 0) {
            JULIE_PUSH(filtered->list, julie_copy_force(it));
        }
        julie_free_value(interp, ev);
    }

    *result = filtered;

out_free:;
    julie_free_value(interp, list);
    julie_free_value(interp, f);

out:;
    return status;
}

static Julie_Status julie_builtin_reduce(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *f;
    Julie_Value  *acc;
    Julie_Value  *list;
    Julie_Value  *t;
    Julie_Value  *it;
    Julie_Value  *arg_pass[2];
    Julie_Value  *ev;

    status = julie_args(interp, tree, "**l", n_values, values, &f, &acc, &list);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    t = ((tree->type == _JULIE_TREE || tree->type == _JULIE_TREE_LINE_LEADER) && julie_len(tree->list) > 1)
            ? julie_elem(tree->list, 1)
            : tree;

    FOR_EACH(list->list, it) {
        arg_pass[0] = acc;
        arg_pass[1] = it;
        status = julie_invoke(interp, t, f, 2, arg_pass, &ev);
        if (status != JULIE_SUCCESS) {
            julie_free_value(interp, acc);
            *result = NULL;
            goto out_free;
        }
        julie_free_value(interp, acc);
        acc = ev;
    }

    *result = acc;

out_free:;
    julie_free_value(interp, list);
    julie_free_value(interp, f);

out:;
    return status;
}

static Julie_Status julie_parse_nodes(Julie_Interp *interp, const char *str, int size, Julie_Array **out_nodes);

static Julie_Status julie_builtin_eval_file(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status        status;
    Julie_Value        *path;
    const Julie_String *pstring;
    Julie_String_ID     save_file;
    const char        *mem;
    int                size;
    Julie_Array        *nodes = JULIE_ARRAY_INIT;
    unsigned           i;
    Julie_Value        *it;
    Julie_Value        *ev;

    status = julie_args(interp, tree, "s", n_values, values, &path);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    pstring = julie_get_string(interp, path->string_id);
    julie_free_value(interp, path);

    status = julie_map_file_into_readonly_memory(pstring->chars, &mem, &size);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        julie_make_file_error(interp, tree, status, pstring->chars);
        goto out_free;
    }

    save_file        = interp->cur_file;
    interp->cur_file = pstring;

    status = julie_parse_nodes(interp, mem, size, &nodes);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out_restore_file;
    }

    i = 0;
    FOR_EACH(nodes, it) {
        status = julie_eval(interp, it, &ev);
        if (status != JULIE_SUCCESS) {
            *result = NULL;
            goto out_restore_file;
        }

        i += 1;

        if (i == julie_len(nodes)) {
            *result = ev;
        } else {
            julie_free_value(interp, ev);
        }
    }

    if (*result != NULL) {
        *result = julie_copy_force(*result);
    } else {
        *result = julie_nil_value();
    }

out_restore_file:;
    interp->cur_file = save_file;

out_free:;
    FOR_EACH(nodes, it) {
        julie_free_value_force(it);
    }
    julie_free_array(nodes);

out:;
    return status;
}

static Julie_Status julie_builtin_use_package(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status        status;
    Julie_Value        *name;
    const Julie_String *name_string;

    if (interp->use_package_forbidden) {
        *result = NULL;
        status  = JULIE_ERR_USE_PACKAGE_FORBIDDEN;
        julie_make_forbidden_error(interp, tree, status);
        goto out;
    }

    status = julie_args(interp, tree, "s", n_values, values, &name);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    name_string = julie_get_string(interp, name->string_id);
    julie_free_value(interp, name);

    status = julie_load_package(interp, name_string->chars, result);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        julie_make_load_package_error(interp, tree, status, name_string->chars, dlerror());
        goto out;
    }

out:;
    return status;
}

static Julie_Status julie_builtin_add_package_directory(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status        status;
    Julie_Value        *path;
    const Julie_String *pstring;

    if (interp->add_package_directory_forbidden) {
        *result = NULL;
        status  = JULIE_ERR_USE_PACKAGE_FORBIDDEN;
        julie_make_forbidden_error(interp, tree, status);
        goto out;
    }

    status = julie_args(interp, tree, "s", n_values, values, &path);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    pstring = julie_get_string(interp, path->string_id);
    julie_free_value(interp, path);

    julie_add_package_directory(interp, pstring->chars);

    *result = julie_string_value(interp, pstring->chars);

out:;
    return status;
}

static Julie_Status julie_builtin_exit(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *exit_code;
    int          code;

    (void)tree;

    *result   = NULL;
    exit_code = NULL;

    if (n_values >= 1) {
        status = julie_eval(interp, values[0], &exit_code);
        if (status != JULIE_SUCCESS) {
            *result = NULL;
            goto out;
        }

        if (exit_code->type != JULIE_NUMBER) {
            status = JULIE_ERR_TYPE;
            julie_make_type_error(interp, exit_code, JULIE_NUMBER, exit_code->type);
            goto out_free;
        }
    }

    code = exit_code != NULL ? (int)exit_code->number : 0;

    exit(code);

out_free:;
    if (exit_code != NULL) {
        julie_free_value(interp, exit_code);
    }

out:;
    return status;
}

static Julie_Status julie_builtin_len(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status        status;
    Julie_Value        *ev;
    const Julie_String *string;

    status = julie_args(interp, tree, "*", n_values, values, &ev);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    switch (ev->type) {
        case JULIE_NIL:
            *result = julie_number_value(0);
            break;
        case JULIE_NUMBER:
            *result = julie_copy(ev);
            break;
        case JULIE_STRING:
            string  = julie_get_string(interp, ev->string_id);
            *result = julie_number_value(string->len);
            break;
        case JULIE_SYMBOL:
            string  = julie_get_string(interp, ev->symbol_id);
            *result = julie_number_value(string->len);
            break;
        case JULIE_LIST:
            *result = julie_number_value(julie_len(ev->list));
            break;
        case JULIE_OBJECT:
            *result = julie_number_value(hash_table_len((_Julie_Object)ev->object));
            break;
        case _JULIE_TREE:
        case _JULIE_TREE_LINE_LEADER:
        case _JULIE_LAMBDA:
            *result = julie_number_value(julie_len(ev->list));
            break;
        case _JULIE_BUILTIN_FN:
            *result = julie_number_value(0);
            break;
        default:
            JULIE_ASSERT(0);
            break;
    }

    julie_free_value(interp, ev);

out:;
    return status;
}

static Julie_Status julie_builtin_empty(Julie_Interp *interp, Julie_Value *tree, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    Julie_Value  *ev;

    status = julie_args(interp, tree, "#", n_values, values, &ev);
    if (status != JULIE_SUCCESS) {
        *result = NULL;
        goto out;
    }

    if (ev->type == JULIE_LIST) {
        *result = julie_number_value(julie_len(ev->list) == 0);
    } else if (ev->type == JULIE_OBJECT) {
        *result = julie_number_value(hash_table_len((_Julie_Object)ev->object) == 0);
    }

    julie_free_value(interp, ev);

out:;
    return status;
}

#endif

Julie_Status julie_init_interp(Julie_Interp *interp) {
    memset(interp, 0, sizeof(*interp));

    interp->roots     = JULIE_ARRAY_INIT;
    interp->strings   = hash_table_make_e(Char_Ptr, Julie_String_ID, julie_charptr_hash, julie_charptr_equ);
    interp->symtab    = hash_table_make(Julie_String_ID, Julie_Value_Ptr, julie_string_id_hash);
    interp->iter_vals = JULIE_ARRAY_INIT;

#define JULIE_INSTALL_FN(_name, _fn)       julie_install_fn(interp, julie_get_string_id(interp, (_name)), (_fn))
#define JULIE_INSTALL_INFIX_FN(_name, _fn) julie_install_infix_fn(interp, julie_get_string_id(interp, (_name)), (_fn))

    JULIE_INSTALL_FN(      "typeof",                julie_builtin_typeof);
    JULIE_INSTALL_FN(      "`",                     julie_builtin_id);
    JULIE_INSTALL_FN(      "'",                     julie_builtin_quote);

    JULIE_INSTALL_INFIX_FN("=",                     julie_builtin_assign);

    JULIE_INSTALL_INFIX_FN("+",                     julie_builtin_add);
    JULIE_INSTALL_INFIX_FN("+=",                    julie_builtin_add_assign);
    JULIE_INSTALL_INFIX_FN("-",                     julie_builtin_sub);
    JULIE_INSTALL_INFIX_FN("-=",                    julie_builtin_sub_assign);
    JULIE_INSTALL_INFIX_FN("*",                     julie_builtin_mul);
    JULIE_INSTALL_INFIX_FN("*=",                    julie_builtin_mul_assign);
    JULIE_INSTALL_INFIX_FN("/",                     julie_builtin_div);
    JULIE_INSTALL_INFIX_FN("/=",                    julie_builtin_div_assign);
    JULIE_INSTALL_INFIX_FN("%",                     julie_builtin_mod);
    JULIE_INSTALL_INFIX_FN("%=",                    julie_builtin_mod_assign);

    JULIE_INSTALL_FN(      "~",                     julie_builtin_bit_not);
    JULIE_INSTALL_INFIX_FN("&",                     julie_builtin_bit_and);
    JULIE_INSTALL_INFIX_FN("&=",                    julie_builtin_bit_and_assign);
    JULIE_INSTALL_INFIX_FN("|",                     julie_builtin_bit_or);
    JULIE_INSTALL_INFIX_FN("|=",                    julie_builtin_bit_or_assign);
    JULIE_INSTALL_INFIX_FN("^",                     julie_builtin_bit_xor);
    JULIE_INSTALL_INFIX_FN("^=",                    julie_builtin_bit_xor_assign);
    JULIE_INSTALL_INFIX_FN("<<",                    julie_builtin_bit_shl);
    JULIE_INSTALL_INFIX_FN("<<=",                   julie_builtin_bit_shl_assign);
    JULIE_INSTALL_INFIX_FN(">>",                    julie_builtin_bit_shr);
    JULIE_INSTALL_INFIX_FN(">>=",                   julie_builtin_bit_shr_assign);

    JULIE_INSTALL_INFIX_FN("==",                    julie_builtin_equ);
    JULIE_INSTALL_INFIX_FN("!=",                    julie_builtin_neq);
    JULIE_INSTALL_INFIX_FN("<",                     julie_builtin_lss);
    JULIE_INSTALL_INFIX_FN("<=",                    julie_builtin_leq);
    JULIE_INSTALL_INFIX_FN(">",                     julie_builtin_gtr);
    JULIE_INSTALL_INFIX_FN(">=",                    julie_builtin_geq);
    JULIE_INSTALL_FN(      "not",                   julie_builtin_not);
    JULIE_INSTALL_INFIX_FN("and",                   julie_builtin_and);
    JULIE_INSTALL_INFIX_FN("or",                    julie_builtin_or);

    JULIE_INSTALL_FN(      "list",                  julie_builtin_list);
    JULIE_INSTALL_FN(      "elem",                  julie_builtin_elem);

    JULIE_INSTALL_FN(      "apply",                 julie_builtin_apply);

    JULIE_INSTALL_FN(      "do",                    julie_builtin_do);
    JULIE_INSTALL_FN(      "if",                    julie_builtin_if);
    JULIE_INSTALL_FN(      "elif",                  julie_builtin_elif);
    JULIE_INSTALL_FN(      "else",                  julie_builtin_else);
    JULIE_INSTALL_FN(      "while",                 julie_builtin_while);

    JULIE_INSTALL_FN(      "print",                 julie_builtin_print);
    JULIE_INSTALL_FN(      "println",               julie_builtin_println);
    JULIE_INSTALL_FN(      "string",                julie_builtin_string);
    JULIE_INSTALL_FN(      "symbol",                julie_builtin_symbol);
    JULIE_INSTALL_FN(      "pad",                   julie_builtin_pad);
    JULIE_INSTALL_FN(      "fmt",                   julie_builtin_fmt);
    JULIE_INSTALL_FN(      "num-fmt",               julie_builtin_num_fmt);
    JULIE_INSTALL_FN(      "printf",                julie_builtin_printf);
    JULIE_INSTALL_FN(      "parse-int",             julie_builtin_parse_int);
    JULIE_INSTALL_FN(      "parse-hex",             julie_builtin_parse_hex);
    JULIE_INSTALL_FN(      "parse-float",           julie_builtin_parse_float);

#if 0
    JULIE_INSTALL_FN("eval",                  julie_builtin_eval);
    JULIE_INSTALL_FN("set",                   julie_builtin_set);
    JULIE_INSTALL_FN("local",                 julie_builtin_local);
    JULIE_INSTALL_FN("ref",                   julie_builtin_ref);
    JULIE_INSTALL_FN("eset",                  julie_builtin_eset);
    JULIE_INSTALL_FN("elocal",                julie_builtin_elocal);
    JULIE_INSTALL_FN("eref",                  julie_builtin_eref);
    JULIE_INSTALL_FN("fn",                    julie_builtin_fn);
    JULIE_INSTALL_FN("localfn",               julie_builtin_localfn);
    JULIE_INSTALL_FN("lambda",                julie_builtin_lambda);
    JULIE_INSTALL_FN("`",                     julie_builtin_id);
    JULIE_INSTALL_FN("quote",                 julie_builtin_quote);
    JULIE_INSTALL_FN("'",                     julie_builtin_quote);
    JULIE_INSTALL_FN("++",                    julie_builtin_inc);
    JULIE_INSTALL_FN("--",                    julie_builtin_dec);
    JULIE_INSTALL_FN("select",                julie_builtin_select);
    JULIE_INSTALL_FN("while",                 julie_builtin_while);
    JULIE_INSTALL_FN("foreach",               julie_builtin_foreach);
    JULIE_INSTALL_FN("range",                 julie_builtin_range);
    JULIE_INSTALL_FN(".",                     julie_builtin_dot);
    JULIE_INSTALL_FN("elem",                  julie_builtin_elem);
    JULIE_INSTALL_FN("index",                 julie_builtin_index);
    JULIE_INSTALL_FN("append",                julie_builtin_append);
    JULIE_INSTALL_FN("pop",                   julie_builtin_pop);
    JULIE_INSTALL_FN("object",                julie_builtin_object);
    JULIE_INSTALL_FN("in",                    julie_builtin_in);
    JULIE_INSTALL_FN("field",                 julie_builtin_field);
    JULIE_INSTALL_FN("insert",                julie_builtin_insert);
    JULIE_INSTALL_FN("delete",                julie_builtin_delete);
    JULIE_INSTALL_FN("update-object",         julie_builtin_update_object);
    JULIE_INSTALL_FN("erase",                 julie_builtin_erase);
    JULIE_INSTALL_FN("len",                   julie_builtin_len);
    JULIE_INSTALL_FN("empty",                 julie_builtin_empty);
    JULIE_INSTALL_FN("keys",                  julie_builtin_keys);
    JULIE_INSTALL_FN("values",                julie_builtin_values);
    JULIE_INSTALL_FN("sorted",                julie_builtin_sorted);
    JULIE_INSTALL_FN("map",                   julie_builtin_map);
    JULIE_INSTALL_FN("filter",                julie_builtin_filter);
    JULIE_INSTALL_FN("reduce",                julie_builtin_reduce);
    JULIE_INSTALL_FN("eval-file",             julie_builtin_eval_file);
    JULIE_INSTALL_FN("use-package",           julie_builtin_use_package);
    JULIE_INSTALL_FN("add-package-directory", julie_builtin_add_package_directory);
    JULIE_INSTALL_FN("exit",                  julie_builtin_exit);
#endif

#undef JULIE_INSTALL_FN

    return JULIE_SUCCESS;
}

Julie_Status julie_interp(Julie_Interp *interp) {
    Julie_Status  status;
    Julie_Value  *root;
    Julie_Value  *result;

    status = JULIE_SUCCESS;

    if (julie_len(interp->roots) == 0) {
        return JULIE_ERR_NO_INPUT;
    }

    FOR_EACH(interp->roots, root) {
        status = julie_eval(interp, root, &result);
        if (status != JULIE_SUCCESS) {
            goto out;
        }
        if (result) {
            julie_free_value(interp, result);
        }
    }

out:;
    return status;
}

void julie_free(Julie_Interp *interp) {
    _Julie_Symbol_Table    symtab;
    Julie_Value           *it;
    char                 *key;
    Julie_String_ID       *id;
    void                 *handle;
    Julie_Backtrace_Entry *bt;


    while ((symtab = julie_pop(interp->local_symtab_stack)) != NULL) {
        julie_free_symtab(interp, symtab);
    }
    julie_free_array(interp->local_symtab_stack);

    julie_free_symtab(interp, interp->symtab);

    FOR_EACH(interp->package_values, it) {
        julie_free_value_force(interp, it);
    }
    julie_free_array(interp->package_values);

    FOR_EACH(interp->roots, it) {
        julie_free_value_force(interp, it);
    }
    julie_free_array(interp->roots);

    hash_table_traverse(interp->strings, key, id) {
        (void)key;
        julie_free_string((Julie_String*)julie_get_string(interp, *id));
        JULIE_FREE((void*)*id);
    }
    hash_table_free(interp->strings);

    FOR_EACH(interp->package_handles, handle) {
        dlclose(handle);
    }
    julie_free_array(interp->package_handles);

    julie_free_array(interp->package_dirs);

    FOR_EACH(interp->backtrace, bt) {
        JULIE_FREE(bt);
    }
    julie_free_array(interp->backtrace);

    memset(interp, 0, sizeof(*interp));
}


static void P(Julie_Interp *interp, Julie_Value *val) {
    julie_print(interp, val, 0);
    printf("\n");
}


#undef STATUS_ERR_RET
#undef PARSE_ERR_RET
#undef MORE_INPUT
#undef PEEK_CHAR
#undef SPC
#undef DIG

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
