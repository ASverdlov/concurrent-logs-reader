#define BOOST_TEST_MODULE MyTest
#include <boost/test/included/unit_test.hpp>

#include "trie.h"

#include <set>

using std::set;
using std::pair;

BOOST_AUTO_TEST_CASE( add_test )
{
  Trie trie;

  auto key1 = Trie::Key{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  auto key2 = Trie::Key{1, 1, 3, 4, 5, 6, 7, 8, 9, 10};

  trie.Add(key1);
  trie.Add(key2);

  BOOST_CHECK_EQUAL( trie.Get(key1), 1 );
  BOOST_CHECK_EQUAL( trie.Get(key2), 1 );
}

BOOST_AUTO_TEST_CASE( traverse_test )
{
  Trie trie;

  trie.Add({1, 2, 3, 4, 5, 6, 7, 8, 9, 19});
  trie.Add({1, 2, 3, 4, 5, 6, 7, 8, 9, 19});
  trie.Add({1, 2, 3, 4, 5, 6, 7, 8, 9, 19});

  trie.Add({1, 3, 3, 4, 5, 6, 7, 8, 9, 19});

  set<pair<Trie::Key, Trie::Value>> canonical_values = {
    { {1, 2, 3, 4, 5, 6, 7, 8, 9, 19}, 3 },
    { {1, 3, 3, 4, 5, 6, 7, 8, 9, 19}, 1 }
  };

  set<pair<Trie::Key, Trie::Value>> values;
  {
    // Fill in
    auto Emitter = [&](const Trie::Key& key, Trie::Value value) {
      values.insert(make_pair(key, value));
    };
    trie.Traverse(Emitter);
  }
  BOOST_ASSERT( values == canonical_values );
}

