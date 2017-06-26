#ifndef TRIE_H
#define TRIE_H

#include "action.h"

#include <atomic>
#include <mutex>
#include <unordered_map>
#include <vector>

/*
 * TRIE data structure implementation.
 *
 * Description:
 * -----------
 * This is basically a map (Array<int, 10> -> Unsigned) implemented via
 * tree. For each leaf the array of numbers written on edges from ROOT_VERTEX
 * down to it is the *key*. The counter inside leaf is the *value*.
 *
 * Advantage over std::map is insert complexity: O(1) vs O(log(n)).
 * Advantage over std::unordered_map is space efficiency.
 *
 * Possible improvements:
 * ---------------------
 * std::vector only allows for multiple reads or single write.
 * We could write our own lock-free container to store nodes in.
 */

class Trie {
public:
  static constexpr size_t DEPTH = 10;

  using Key = std::array<int, DEPTH>;
  using Value = unsigned;
  using OnRecordCallback = std::function<void(const Key&, Value)>;

public:
  /* Constructs a Trie with one vertex (root). */
  Trie();

  /* Increments value */
  void Add(const Key& key);

  /* Access value by key */
  Value Get(const Key& key) const;

  /* Invokes OnRecord(key, value) for each record in the trie */
  void Traverse(OnRecordCallback OnRecord) const;

private:
  struct Node {
    std::unordered_map<int, size_t> Next; /* Next node by edge value */
    unsigned Counter;
  };

private:
  static constexpr size_t ROOT_VERTEX = 0;

  void Traverse(int current_vertex, size_t level,
                Key& path_array, OnRecordCallback OnRecord) const;

  size_t NewNode();

  mutable std::mutex NodesMtx;
  std::vector<Node> Nodes;
};

#endif
