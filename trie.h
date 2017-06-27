#ifndef TRIE_H
#define TRIE_H

#include "action.h"

#include <atomic>
#include <list>
#include <mutex>
#include <unordered_map>

/*
 * TRIE data structure implementation.
 *
 * Description:
 * -----------
 * This is basically a map (Array<int, 10> -> Unsigned) implemented via
 * tree. For each leaf the array of numbers written on edges from ROOT_VERTEX
 * down to it is the *key*. The counter inside leaf is the *value*.
 *
 * Insert complexity: O(1)
 */

class Trie {
public:
  static constexpr size_t DEPTH = Action::NUM_PROPERTIES;

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
    std::unordered_map<int, Node*> Next; /* Next node by edge value */
    unsigned Counter;
  };

private:
  inline Node* RootNode();
  inline const Node* RootNode() const;

  void Traverse(const Node* node, size_t level,
                Key& path_array, OnRecordCallback OnRecord) const;

  inline Node* NewNode();

  mutable std::mutex NodesAddMtx;
  std::list<Node> Nodes;
};

#endif
