#include "action.h"
#include "config.h"
#include "log_parser.h"
#include "trie.h"

#include <boost/filesystem.hpp>

#include <iostream>
#include <string>

#include <cstdlib>

namespace fs = boost::filesystem;

using std::cout;
using std::endl;
using std::string;
using std::to_string;

int main(int argc, char* argv[]) {
  Config config;
  config.ParseOptions(argc, argv);

  Trie actions_aggregator;

  for (size_t i = 1; i <= config.FilesNum; ++i) {
    const auto filename = config.Directory + "/file" + to_string(i) + ".log";

    if (!fs::exists(filename)) {
      cout << "ERROR! " << filename << " doesn't exist.\n";
      return EXIT_FAILURE;
    }

    LogParser parser;
    parser.Parse(filename, [&](const Action& action_parsed) {
        actions_aggregator.Add(action_parsed.Properties);
    });
  }

  // Output aggregated actions
  auto Outputer = [](const Trie::Key& key, Trie::Value value) {
    cout << "Record ";
    for (int x : key)
      cout << x << ' ';
    cout << "\nCount: " << value << '\n';
  };
  actions_aggregator.Traverse(Outputer);

  return EXIT_SUCCESS;
}
