#ifndef QHT_H
#define QHT_H

#include <libft.h>

#define DEFINE_QHT(T, KTYPE, VTYPE)                                        \
typedef struct s_##T {                                                     \
	VTYPE *arr;                                                            \
	uint8_t *harr;                                                         \
	uint64_t size;                                                         \
} T;                                                                       \
static inline uint64_t T##_hash(KTYPE key)                                 \
{                                                                          \
	const uint8_t *p = (uint8_t *)&key;                                    \
	size_t len = sizeof(key);                                              \
    uint64_t hash = 14695981039346656037ULL;                               \
    while (len--) {                                                        \
        hash ^= *p++;                                                      \
        hash *= 1099511628211ULL;                                          \
    }                                                                      \
    return (hash);                                                         \
}                                                                          \
static inline bool T##_eq(KTYPE a, KTYPE b)                                \
{                                                                          \
	return (a == b);                                                       \
}                                                                          \
static T *T##_create(uint64_t size)                                        \
{                                                                          \
	T *htbl = NULL;                                                        \
	LIBFT_ALLOC(htbl, sizeof(T));                                          \
	*htbl = (T){0};                                                        \
	htbl->size = size;                                                     \
	LIBFT_ALLOC(htbl->arr, htbl->size * sizeof(VTYPE));                    \
	LIBFT_ALLOCZ(htbl->harr, htbl->size);                                  \
	return (htbl);                                                         \
}                                                                          \
static void T##_init(T *htbl, uint64_t size)                               \
{                                                                          \
	assert(NULL != htbl);                                                  \
	*htbl = (T){0};                                                        \
	htbl->size = size;                                                     \
	LIBFT_ALLOC(htbl->arr, htbl->size * sizeof(VTYPE));                    \
	LIBFT_ALLOCZ(htbl->harr, htbl->size);                                  \
}                                                                          \
static bool T##_has(const T *htbl, KTYPE key)                              \
{                                                                          \
	if (htbl->arr == NULL) return false;                                   \
	uint64_t hash = T##_hash(key);                                         \
	uint64_t idx = hash % htbl->size;                                      \
	return (htbl->harr[idx] > 0);                                          \
}                                                                          \
static VTYPE T##_get(const T *htbl, KTYPE key)                             \
{                                                                          \
	assert(htbl->size > 0);                                                \
	uint64_t hash = T##_hash(key);                                         \
	uint64_t idx = hash % htbl->size;                                      \
	return (htbl->arr[idx]);                                               \
}                                                                          \
static VTYPE T##_get_or_default(const T *htbl, KTYPE key, VTYPE deflt)     \
{                                                                          \
	if (htbl->arr == NULL) return (deflt);                                 \
	uint64_t hash = T##_hash(key);                                         \
	uint64_t idx = hash % htbl->size;                                      \
	if (htbl->harr[idx] == 0) return (deflt);                              \
	return (htbl->arr[idx]);                                               \
}                                                                          \
static bool T##_set(const T *htbl, KTYPE key, VTYPE value)                 \
{                                                                          \
    assert(htbl->size > 0);                                                \
	uint64_t hash = T##_hash(key);                                         \
	uint64_t idx = hash % htbl->size;                                      \
	if (htbl->harr[idx] > 0) return false;                                 \
	htbl->arr[idx] = value;                                                \
	htbl->harr[idx]++;                                                     \
	return true;                                                           \
}                                                                          \
static bool T##_reset(const T *htbl, KTYPE key, VTYPE value)               \
{                                                                          \
    assert(htbl->size > 0);                                                \
	uint64_t hash = T##_hash(key);                                         \
	uint64_t idx = hash % htbl->size;                                      \
	if (htbl->harr[idx] == 0) htbl->harr[idx]++;                           \
	htbl->arr[idx] = value;                                                \
	return true;                                                           \
}                                                                          \
static bool T##_unset(const T *htbl, KTYPE key)                            \
{                                                                          \
    assert(htbl->size > 0);                                                \
	uint64_t hash = T##_hash(key);                                         \
	uint64_t idx = hash % htbl->size;                                      \
	if (htbl->harr[idx] == 0) return false;                                \
	htbl->harr[idx] = 0;                                                   \
	return true;                                                           \
}                                                                          \
static void	T##_clear(T *htbl)                                             \
{                                                                          \
	LIBFT_FREE(htbl->arr);                                                 \
	LIBFT_FREE(htbl->harr);                                                \
	*htbl = (T){0};                                                        \
}                                                                          \
static void	T##_del(T *htbl)                                               \
{                                                                          \
	LIBFT_FREE(htbl->arr);                                                 \
	LIBFT_FREE(htbl->harr);                                                \
	LIBFT_FREE(htbl);                                                      \
}                                                                          \
static T *T##_clone(T *htbl)                                               \
{                                                                          \
	T *clone = NULL;                                                       \
	LIBFT_ALLOC(clone, sizeof(T));                                         \
	*clone = *htbl;                                                        \
	clone->arr = ft_memdup(htbl->arr, htbl->size * sizeof(VTYPE));         \
	clone->harr = ft_memdup(htbl->harr, htbl->size);                       \
	return (clone);                                                        \
}

/**************************************************************************/
/**************************************************************************/
/**************************************************************************/

#define DEFINE_QHT_H(T, KTYPE, VTYPE, HASH_FUNC, HASH_EQ)                  \
typedef struct s_##T {                                                     \
	VTYPE *arr;                                                            \
	uint8_t *harr;                                                         \
	uint64_t size;                                                         \
} T;                                                                       \
static T *T##_create(uint64_t size)                                        \
{                                                                          \
	T *htbl = NULL;                                                        \
	LIBFT_ALLOC(htbl, sizeof(T));                                          \
	*htbl = (T){0};                                                        \
	htbl->size = size;                                                     \
	LIBFT_ALLOC(htbl->arr, htbl->size * sizeof(VTYPE));                    \
	LIBFT_ALLOCZ(htbl->harr, htbl->size);                                  \
	return (htbl);                                                         \
}                                                                          \
static void T##_init(T *htbl, uint64_t size)                               \
{                                                                          \
	assert(NULL != htbl);                                                  \
	*htbl = (T){0};                                                        \
	htbl->size = size;                                                     \
	LIBFT_ALLOC(htbl->arr, htbl->size * sizeof(VTYPE));                    \
	LIBFT_ALLOCZ(htbl->harr, htbl->size);                                  \
}                                                                          \
static bool T##_has(const T *htbl, KTYPE key)                              \
{                                                                          \
	if (htbl->arr == NULL) return false;                                   \
	uint64_t hash = HASH_FUNC(key);                                        \
	uint64_t idx = hash % htbl->size;                                      \
	return (htbl->harr[idx] > 0);                                          \
}                                                                          \
static VTYPE T##_get(const T *htbl, KTYPE key)                             \
{                                                                          \
	assert(htbl->size > 0);                                                \
	uint64_t hash = HASH_FUNC(key);                                        \
	uint64_t idx = hash % htbl->size;                                      \
	return (htbl->arr[idx]);                                               \
}                                                                          \
static VTYPE T##_get_or_default(const T *htbl, KTYPE key, VTYPE deflt)     \
{                                                                          \
	if (htbl->arr == NULL) return (deflt);                                 \
	uint64_t hash = HASH_FUNC(key);                                        \
	uint64_t idx = hash % htbl->size;                                      \
	if (htbl->harr[idx] == 0) return (deflt);                              \
	return (htbl->arr[idx]);                                               \
}                                                                          \
static bool T##_set(const T *htbl, KTYPE key, VTYPE value)                 \
{                                                                          \
    assert(htbl->size > 0);                                                \
	uint64_t hash = HASH_FUNC(key);                                        \
	uint64_t idx = hash % htbl->size;                                      \
	if (htbl->harr[idx] > 0) return false;                                 \
	htbl->arr[idx] = value;                                                \
	htbl->harr[idx]++;                                                     \
	return true;                                                           \
}                                                                          \
static bool T##_reset(const T *htbl, KTYPE key, VTYPE value)               \
{                                                                          \
    assert(htbl->size > 0);                                                \
	uint64_t hash = HASH_FUNC(key);                                        \
	uint64_t idx = hash % htbl->size;                                      \
	if (htbl->harr[idx] == 0) htbl->harr[idx]++;                           \
	htbl->arr[idx] = value;                                                \
	return true;                                                           \
}                                                                          \
static bool T##_unset(const T *htbl, KTYPE key)                            \
{                                                                          \
    assert(htbl->size > 0);                                                \
	uint64_t hash = HASH_FUNC(key);                                        \
	uint64_t idx = hash % htbl->size;                                      \
	if (htbl->harr[idx] == 0) return false;                                \
	htbl->harr[idx] = 0;                                                   \
	return true;                                                           \
}                                                                          \
static void	T##_clear(T *htbl)                                             \
{                                                                          \
	LIBFT_FREE(htbl->arr);                                                 \
	LIBFT_FREE(htbl->harr);                                                \
	*htbl = (T){0};                                                        \
}                                                                          \
static void	T##_del(T *htbl)                                               \
{                                                                          \
	LIBFT_FREE(htbl->arr);                                                 \
	LIBFT_FREE(htbl->harr);                                                \
	LIBFT_FREE(htbl);                                                      \
}                                                                          \
static T *T##_clone(T *htbl)                                               \
{                                                                          \
	T *clone = NULL;                                                       \
	LIBFT_ALLOC(clone, sizeof(T));                                         \
	*clone = *htbl;                                                        \
	clone->arr = ft_memdup(htbl->arr, htbl->size * sizeof(VTYPE));         \
	clone->harr = ft_memdup(htbl->harr, htbl->size);                       \
	return (clone);                                                        \
}

/**************************************************************************/
/**************************************************************************/
/**************************************************************************/

#define DEFINE_QHT_FS(T, KTYPE, VTYPE, SIZE)                               \
typedef struct s_##T {                                                     \
	VTYPE arr[SIZE];                                                       \
	uint8_t harr[SIZE];                                                    \
	bool init;                                                             \
} T;                                                                       \
static inline uint64_t T##_hash(KTYPE key)                                 \
{                                                                          \
	const uint8_t *p = (uint8_t *)&key;                                    \
	size_t len = sizeof(key);                                              \
    uint64_t hash = 14695981039346656037ULL;                               \
    while (len--) {                                                        \
        hash ^= *p++;                                                      \
        hash *= 1099511628211ULL;                                          \
    }                                                                      \
    return (hash);                                                         \
}                                                                          \
static inline bool T##_eq(KTYPE a, KTYPE b)                                \
{                                                                          \
	return (a == b);                                                       \
}                                                                          \
static void T##_init(T *htbl)                                              \
{                                                                          \
	assert(NULL != htbl);                                                  \
	ft_memset(htbl->harr, 0, SIZE);                                        \
	htbl->init = true;                                                     \
}                                                                          \
static bool T##_has(const T *htbl, KTYPE key)                              \
{                                                                          \
	uint64_t hash = T##_hash(key);                                         \
	uint64_t idx = hash % SIZE;                                            \
	return (htbl->harr[idx] > 0);                                          \
}                                                                          \
static VTYPE T##_get(const T *htbl, KTYPE key)                             \
{                                                                          \
	uint64_t hash = T##_hash(key);                                         \
	uint64_t idx = hash % SIZE;                                            \
	assert(htbl->harr[idx] > 0);                                           \
	return (htbl->arr[idx]);                                               \
}                                                                          \
static VTYPE T##_get_or_default(const T *htbl, KTYPE key, VTYPE deflt)     \
{                                                                          \
	uint64_t hash = T##_hash(key);                                         \
	uint64_t idx = hash % SIZE;                                            \
	if (htbl->harr[idx] == 0) return (deflt);                              \
	return (htbl->arr[idx]);                                               \
}                                                                          \
static bool T##_set(T *htbl, KTYPE key, VTYPE value)                       \
{                                                                          \
	uint64_t hash = T##_hash(key);                                         \
	uint64_t idx = hash % SIZE;                                            \
	if (htbl->harr[idx]) return false;                                     \
	htbl->arr[idx] = value;                                                \
	htbl->harr[idx]++;                                                     \
	return true;                                                           \
}                                                                          \
static bool T##_reset(T *htbl, KTYPE key, VTYPE value)                     \
{                                                                          \
	uint64_t hash = T##_hash(key);                                         \
	uint64_t idx = hash % SIZE;                                            \
	if (htbl->harr[idx] == 0) htbl->harr[idx]++;                           \
	htbl->arr[idx] = value;                                                \
	return true;                                                           \
}                                                                          \
static bool T##_unset(T *htbl, KTYPE key)                                  \
{                                                                          \
	uint64_t hash = T##_hash(key);                                         \
	uint64_t idx = hash % SIZE;                                            \
	if (htbl->harr[idx] == 0) return false;                                \
	htbl->harr[idx] = 0;                                                   \
	return true;                                                           \
}                                                                          \
static void	T##_clear(T *htbl)                                             \
{                                                                          \
	ft_memset(htbl->harr, 0, SIZE);                                        \
}                                                                          \
static void	T##_del(T *htbl)                                               \
{                                                                          \
	LIBFT_FREE(htbl);                                                      \
}

/**************************************************************************/
/**************************************************************************/
/**************************************************************************/

#define DEFINE_QHT_HFS(T, KTYPE, VTYPE, SIZE, HASH_FUNC, HASH_EQ)          \
typedef struct s_##T {                                                     \
	VTYPE arr[SIZE];                                                       \
	uint8_t harr[SIZE];                                                    \
	bool init;                                                             \
} T;                                                                       \
static void T##_init(T *htbl)                                              \
{                                                                          \
	assert(NULL != htbl);                                                  \
	ft_memset(htbl->harr, 0, SIZE);                                        \
	htbl->init = true;                                                     \
}                                                                          \
static bool T##_has(const T *htbl, KTYPE key)                              \
{                                                                          \
	uint64_t hash = HASH_FUNC(key);                                        \
	uint64_t idx = hash % SIZE;                                            \
	return (htbl->harr[idx] > 0);                                          \
}                                                                          \
static VTYPE T##_get(const T *htbl, KTYPE key)                             \
{                                                                          \
	uint64_t hash = HASH_FUNC(key);                                        \
	uint64_t idx = hash % SIZE;                                            \
	assert(htbl->harr[idx] > 0);                                           \
	return (htbl->arr[idx]);                                               \
}                                                                          \
static VTYPE T##_get_or_default(const T *htbl, KTYPE key, VTYPE deflt)     \
{                                                                          \
	uint64_t hash = HASH_FUNC(key);                                        \
	uint64_t idx = hash % SIZE;                                            \
	if (htbl->harr[idx] == 0) return (deflt);                              \
	return (htbl->arr[idx]);                                               \
}                                                                          \
static bool T##_set(T *htbl, KTYPE key, VTYPE value)                       \
{                                                                          \
	uint64_t hash = HASH_FUNC(key);                                        \
	uint64_t idx = hash % SIZE;                                            \
	if (htbl->harr[idx]) return false;                                     \
	htbl->arr[idx] = value;                                                \
	htbl->harr[idx]++;                                                     \
	return true;                                                           \
}                                                                          \
static bool T##_reset(T *htbl, KTYPE key, VTYPE value)                     \
{                                                                          \
	uint64_t hash = HASH_FUNC(key);                                        \
	uint64_t idx = hash % SIZE;                                            \
	if (htbl->harr[idx] == 0) htbl->harr[idx]++;                           \
	htbl->arr[idx] = value;                                                \
	return true;                                                           \
}                                                                          \
static bool T##_unset(T *htbl, KTYPE key)                                  \
{                                                                          \
	uint64_t hash = HASH_FUNC(key);                                        \
	uint64_t idx = hash % SIZE;                                            \
	if (htbl->harr[idx] == 0) return false;                                \
	htbl->harr[idx] = 0;                                                   \
	return true;                                                           \
}                                                                          \
static void	T##_clear(T *htbl)                                             \
{                                                                          \
	ft_memset(htbl->harr, 0, SIZE);                                        \
}                                                                          \
static void	T##_del(T *htbl)                                               \
{                                                                          \
	LIBFT_FREE(htbl);                                                      \
}

#endif
