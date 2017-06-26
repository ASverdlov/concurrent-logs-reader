#include "trie.h"

/* Debug */
#include <iostream>
using std::cout;
using std::endl;

Trie::Trie() 
  : Nodes(1, Node()) 
{ 
}

size_t Trie::NewNode() {
  Nodes.push_back(Node{});
  return Nodes.size() - 1u;
}

void Trie::Add(const Key& record) {
  std::lock_guard<std::mutex> nodes_lock(NodesMtx);

  size_t v = 0;
  for (int level = 0; level < DEPTH; ++level) {
    int edge = record[level];

    if (!Nodes[v].Next.count(edge))
      Nodes[v].Next[edge] = NewNode();
    v = Nodes[v].Next[edge];
  }

  ++Nodes[v].Counter;
}

Trie::Value Trie::Get(const Key& record) const {
  std::lock_guard<std::mutex> nodes_lock(NodesMtx);

  size_t v = 0;
  for (int level = 0; level < DEPTH; ++level) {
    int edge = record[level];
    v = Nodes[v].Next.at(edge);
  }
  return Nodes[v].Counter;
}

void Trie::Traverse(int current_vertex, size_t level, 
                    Key& key, OnRecordCallback OnRecord) const {
  const auto& node = Nodes[current_vertex];

  if (level == DEPTH) {
    OnRecord(key, node.Counter);
    return;
  }
  for (auto& p : node.Next) {
    int edge = p.first;
    size_t next_vertex = p.second;

    key[level] = edge;
    Traverse(next_vertex, level + 1, key, OnRecord);
  }
}

void Trie::Traverse(OnRecordCallback OnRecord) const {
  Key key;
  Traverse(ROOT_VERTEX, 0u, key, OnRecord);
}
