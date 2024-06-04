#include <errno.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#include "klib/khash.h"
#include "klib/kbtree.h"

#include <stdbool.h>
#include "qp/Tbl.h"

// Define the hashmap with char* keys and long values
KHASH_MAP_INIT_STR(klib_hashmap, ssize_t)

ssize_t max_word_frequency_khash(const char *const words[], size_t len) {
    int ret = 0;
    khash_t(klib_hashmap) *hash = kh_init(klib_hashmap);

    for (size_t i = 0; i < len; i++) {
        khint_t k = kh_put(klib_hashmap, hash, words[i], &ret);
        kh_value(hash, k) = kh_exist(hash, k)? kh_value(hash, k) + 1 : 1;
    }

    ssize_t max_freq = 0;
    for (khint_t k = kh_begin(hash); k!= kh_end(hash); k++) {
        max_freq = kh_exist(hash, k)? (kh_value(hash, k) > max_freq? kh_value(hash, k) : max_freq) : max_freq;
    }

    kh_destroy(klib_hashmap, hash);
    return max_freq;
}

typedef struct {
    const char* key;
    ssize_t count;
} elem_t;

#define elem_cmp(a, b) (strcmp((a).key, (b).key))
KBTREE_INIT(klib_btree, elem_t, elem_cmp)

ssize_t max_word_frequency_kbtree(const char* words[], size_t len)
{
    kbtree_t(klib_btree)* b;

    b = kb_init(klib_btree, KB_DEFAULT_SIZE);
    for (size_t i = 0; i < len; ++i) {
        elem_t t = (elem_t){ .key = words[i], .count = 1 };
        elem_t* p = kb_getp(klib_btree, b, &t); // kb_get() also works
        
        if (!p) 
            kb_putp(klib_btree, b, &t);
        else 
            ++p->count;
    }

    ssize_t max_freq = 0;

    kbitr_t itr;
    kb_itr_first(klib_btree, b, &itr);
    for (; kb_itr_valid(&itr); kb_itr_next(klib_btree, b, &itr)) {
        elem_t kv = kb_itr_key(elem_t, &itr);

        max_freq = (max_freq > kv.count) ? max_freq : kv.count;
    }
    kb_destroy(klib_btree, b);
    return max_freq;
}

ssize_t max_word_frequency_qptrie(const char* words[], size_t len) {
    Tbl *table = NULL;
    for (size_t i = 0; i < len; ++i) {
        size_t key_len = strlen(words[i]);
        // Abuse the fact that Tget returns NULL on missing key...
        ssize_t val = (ssize_t) Tgetl(table, words[i], key_len);
        // Val must be word aligned, so we increase the counter by sizeof(size_t)
        table = Tsetl(table, words[i], key_len, (void*) (val + sizeof(size_t)));
    }

    ssize_t max_freq = 0;
    const char* key = NULL;
    void* val = NULL;
    while(Tnext(table, &key, &val)) {
        ssize_t freq = (ssize_t) Tget(table, key); 
        max_freq = (max_freq > freq) ? max_freq : freq;

        table = Tset(table, key, NULL);
    }

    return max_freq / sizeof(size_t);
}

// int main(int argc, const char** argv) {
//     printf("%zd\n", max_word_frequency_khash(argv + 1, argc - 1));
//     printf("%zd\n", max_word_frequency_kbtree(argv + 1, argc - 1));
//     printf("%zd\n", max_word_frequency_qptrie(argv + 1, argc - 1));
// }
