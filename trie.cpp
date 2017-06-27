#include "trie.h"

/* Debug */
#include <iostream>
using std::cout;
using std::endl;

Trie::Trie() 
  : Nodes(1, Node()) 
{ 
}

inline Trie::Node* Trie::NewNode() {
  Nodes.push_back(Node{});
  return &Nodes.back();
}

inline Trie::Node* Trie::RootNode() {
  return &Nodes.front();
}

inline const Trie::Node* Trie::RootNode() const {
  return &Nodes.front();
}

void Trie::Add(const Key& record) {
  auto* node = RootNode();

  for (int level = 0; level < DEPTH; ++level) {
    int edge = record[level];
    {
      std::lock_guard<std::mutex> nodes_add_lock(NodesAddMtx);
      if (!node->Next.count(edge))
        node->Next[edge] = NewNode();
    }
    node = node->Next[edge];
  }

  ++(node->Counter);
}

Trie::Value Trie::Get(const Key& record) const {
  auto* node = RootNode();
  for (int level = 0; level < DEPTH; ++level) {
    int edge = record[level];
    node = node->Next.at(edge);
  }
  return node->Counter;
}

void Trie::Traverse(const Node* node, size_t level, 
                    Key& key, OnRecordCallback OnRecord) const {
  if (level == DEPTH) {
    OnRecord(key, node->Counter);
    return;
  }
  for (auto& p : node->Next) {
    int edge = p.first;
    auto* next_node = p.second;

    key[level] = edge;
    Traverse(next_node, level + 1, key, OnRecord);
  }
}

void Trie::Traverse(OnRecordCallback OnRecord) const {
  std::lock_guard<std::mutex> nodes_add_lock(NodesAddMtx);

  Key key;
  Traverse(RootNode(), 0u, key, OnRecord);
}
