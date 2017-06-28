#include "action.h"
#include "config.h"
#include "log_parser.h"
#include "trie.h"

#include <boost/filesystem.hpp>

#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

namespace fs = boost::filesystem;

using std::cout;
using std::endl;
using std::ref;
using std::string;
using std::to_string;
using std::thread;
using std::vector;

static void CheckFileExists(const string& filename) {
  if (!fs::exists(filename)) {
    cout << "Error!\n" << filename << " doesn't exist.\n";
    exit(EXIT_FAILURE);
  }
}

static string Filename(const string& basename, size_t id) {
  return basename + "/file" + to_string(id) + ".log";
}

static void ParseSliceOfFiles(size_t start_id, const Config& config,
                             Trie& actions_aggregator) {
  for (size_t id = start_id; id <= config.FilesNum; id += config.JobsNum) {
    const auto filename = Filename(config.Directory, id);
    LogParser parser;
    parser.Parse(filename, [&](const Action& action_parsed) {
        actions_aggregator.Add(action_parsed.Properties);
    });
  }
}

int main(int argc, char* argv[]) {
  /* Prepare */
  Config config;
  config.ParseOptions(argc, argv);

  if (config.JobsNum == -1)
    config.JobsNum = std::thread::hardware_concurrency();

  for (size_t i = 1; i <= config.FilesNum; ++i)
    CheckFileExists(Filename(config.Directory, i));

  /* Aggregate */
  Trie actions_aggregator;

  vector<thread> workers(config.JobsNum);
  for (size_t id = 1; id <= config.JobsNum; ++id)
    workers[id] = thread(ParseSliceOfFiles, id, ref(config), ref(actions_aggregator));
  for (size_t id = 1; id <= config.JobsNum; ++id)
    workers[id].join();

  /* Output */
  auto Outputer = [](const Trie::Key& key, Trie::Value value) {
    cout << "Record ";
    for (int x : key)
      cout << x << ' ';
    cout << "\nCount: " << value << '\n';
  };
  actions_aggregator.Traverse(Outputer);

  return EXIT_SUCCESS;
}
