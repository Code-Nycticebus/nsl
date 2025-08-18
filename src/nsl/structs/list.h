#ifndef _NSL_LIST_H_
#define _NSL_LIST_H_

#include "nsl/core/arena.h"
#include "nsl/defines.h"

#include <stdlib.h>

#define nsl_list_first(list) (NSL_ASSERT((list)->len > 0), (list)->items[0])
#define nsl_list_last(list)  (NSL_ASSERT((list)->len > 0), (list)->items[(list)->len - 1])
#define nsl_list_pop(list)   (NSL_ASSERT((list)->len > 0), (list)->items[--(list)->len])

#define nsl_list_is_empty(list) (!(list)->len)
#define nsl_list_clear(list) ((list)->len = 0)

#define nsl_list_free(list)                                                    \
  do {                                                                         \
    nsl_arena_free_chunk((list)->arena, (list)->items);                        \
    (list)->items = NULL;                                                      \
    (list)->len = 0;                                                           \
    (list)->cap = 0;                                                           \
  } while (0)

#define nsl_list_copy(src, dest)                                               \
  do {                                                                         \
    nsl_list_resize((dest), (src)->len);                                       \
    for (usize __c_i = 0; __c_i < (src)->len; __c_i++) {                       \
      (dest)->items[__c_i] = (src)->items[__c_i];                              \
    }                                                                          \
    (dest)->len = (src)->len;                                                  \
  } while (0)

#define nsl_list_resize(list, size)                                            \
  do {                                                                         \
    if (size < (list)->cap) {                                                  \
      break;                                                                   \
    }                                                                          \
    (list)->cap = size;                                                        \
    (list)->items = nsl_arena_realloc_chunk(                                   \
        (list)->arena, (list)->items, (list)->cap * sizeof(*(list)->items));   \
  } while (0)

#define nsl_list_reserve(list, size)                                           \
  do {                                                                         \
    const usize __rs = (list)->len + size;                                     \
    if (__rs < (list)->cap) {                                                  \
      break;                                                                   \
    }                                                                          \
    usize __ns = (list)->cap == 0 ? 5 : (list)->cap;                           \
    while (__ns < __rs) {                                                      \
      __ns *= 2;                                                               \
    }                                                                          \
    nsl_list_resize(list, __ns);                                               \
  } while (0)

#define nsl_list_push(list, ...)                                               \
  do {                                                                         \
    nsl_list_reserve((list), 1);                                               \
    (list)->items[(list)->len++] = (__VA_ARGS__);                              \
  } while (0)

#define nsl_list_extend(list, count, ...)                                      \
  do {                                                                         \
    nsl_list_reserve((list), (count));                                         \
    for (usize __e_i = 0; __e_i < (count); __e_i++) {                          \
      (list)->items[(list)->len + __e_i] = (__VA_ARGS__)[__e_i];               \
    }                                                                          \
    (list)->len += count;                                                      \
  } while (0)

#define nsl_list_extend_static(list, ...)                                      \
  do {                                                                         \
    nsl_list_extend(list, NSL_ARRAY_LEN(__VA_ARGS__), __VA_ARGS__);            \
  } while (0)

#define nsl_list_extend_list(list, other)                                      \
  do {                                                                         \
    nsl_list_reserve((list), (other)->len);                                    \
    for (usize __e_i = 0; __e_i < (other)->len; __e_i++) {                     \
      (list)->items[(list)->len + __e_i] = (other)->items[__e_i];              \
    }                                                                          \
    (list)->len += (other)->len;                                               \
  } while (0)

#define nsl_list_insert(list, value, idx)                                      \
  do {                                                                         \
    nsl_list_reserve(list, 1);                                                 \
    for (usize __r_i = idx + 1; __r_i < (list)->len + 1; __r_i++) {            \
      (list)->items[__r_i] = (list)->items[__r_i - 1];                         \
    }                                                                          \
    (list)->items[idx] = value;                                                \
    (list)->len++;                                                             \
  } while (0)

#define nsl_list_remove(list, idx)                                             \
  do {                                                                         \
    for (usize __r_i = idx + 1; __r_i < (list)->len; __r_i++) {                \
      (list)->items[__r_i - 1] = (list)->items[__r_i];                         \
    }                                                                          \
    (list)->len--;                                                             \
  } while (0)

#define nsl_list_map(src, dest, map)                                           \
  do {                                                                         \
    nsl_list_reserve((dest), (src)->len);                                      \
    for (usize __m_i = 0; __m_i < (src)->len; __m_i++) {                       \
      (dest)->items[__m_i] = map((src)->items[__m_i]);                         \
    }                                                                          \
    (dest)->len = (src)->len;                                                  \
  } while (0)

#define nsl_list_map_ctx(src, dest, map, ctx)                                  \
  do {                                                                         \
    nsl_list_reserve((dest), (src)->len);                                      \
    for (usize __m_i = 0; __m_i < (src)->len; __m_i++) {                       \
      (dest)->items[__m_i] = map(ctx, (src)->items[__m_i]);                    \
    }                                                                          \
    (dest)->len = (src)->len;                                                  \
  } while (0)

#define nsl_list_filter(src, dest, filter)                                     \
  do {                                                                         \
    nsl_list_reserve((dest), (src)->len);                                      \
    usize __f_count = 0;                                                       \
    for (usize __f_i = 0; __f_i < (src)->len; __f_i++) {                       \
      if (filter((src)->items[__f_i])) {                                       \
        (dest)->items[__f_count++] = (src)->items[__f_i];                      \
      }                                                                        \
    }                                                                          \
    (dest)->len = __f_count;                                                   \
  } while (0)

#define nsl_list_filter_ctx(src, dest, filter, ctx)                            \
  do {                                                                         \
    nsl_list_reserve((dest), (src)->len);                                      \
    usize __f_count = 0;                                                       \
    for (usize __f_i = 0; __f_i < (src)->len; __f_i++) {                       \
      if (filter((ctx), (src)->items[__f_i])) {                                \
        (dest)->items[__f_count++] = (src)->items[__f_i];                      \
      }                                                                        \
    }                                                                          \
    (dest)->len = __f_count;                                                   \
  } while (0)

#define nsl_list_sort(src, sort)                                               \
  qsort((src)->items, (src)->len, sizeof((src)->items[0]), sort)

#define nsl_list_reverse(list)                                                 \
  do {                                                                         \
    nsl_list_reserve((list), 1);                                               \
    for (usize __r_i = 0; __r_i < (list)->len - __r_i - 1; __r_i++) {          \
      usize __last_idx = (list)->len - __r_i - 1;                              \
      (list)->items[(list)->len] = (list)->items[__r_i];                       \
      (list)->items[__r_i] = (list)->items[__last_idx];                        \
      (list)->items[__last_idx] = (list)->items[(list)->len];                  \
    }                                                                          \
  } while (0)

#define nsl_list_for_each(T, iter, da)                                         \
  if ((da)->len)                                                               \
    for (T iter = (da)->items; iter <= &(da)->items[(da)->len - 1]; iter++)

#endif // _NSL_LIST_H_
