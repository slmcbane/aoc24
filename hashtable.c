#include "common.h"

hashtable_result find_or_insert(hashtable *ht, intptr_t key, intptr_t value,
                                ht_hash hash, ht_predicate comp, arena *a) {
  u64 h = hash(key);
  hashtable_node **curr = &ht->root;
  while (*curr) {
    if (comp((*curr)->key, key)) {
      return (hashtable_result){*curr, false};
    }
    curr = &(*curr)->children[h & 3];
    h >>= 2;
  }

  *curr = new (a, hashtable_node, 1);
  (*curr)->key = key;
  (*curr)->value = value;
  return (hashtable_result){*curr, true};
}
