#include "action.h"
#include "config.h"
#include "log_parser.h"
#include "trie.h"

#include <boost/filesystem.hpp>

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

namespace fs = boost::filesystem;

using std::cout;
using std::endl;
using std::ofstream;
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
  cout << "# of processors in use: " << config.JobsNum << endl;

  for (size_t i = 1; i <= config.FilesNum; ++i)
    CheckFileExists(Filename(config.Directory, i));

  /* Aggregate */
  Trie actions_aggregator;

  vector<thread> workers(config.JobsNum);
  for (size_t id = 0; id < config.JobsNum; ++id)
    workers[id] = thread(ParseSliceOfFiles, id + 1, ref(config), ref(actions_aggregator));
  for (size_t id = 0; id < config.JobsNum; ++id)
    workers[id].join();

  /* Output */
  auto output_file = ofstream(config.OutputFile);
  auto StatisticsSaver = [&](const Trie::Key& key, Trie::Value value) {
    output_file << "Properties: ";
    for (int x : key)
      output_file << x << ' ';
    output_file << "\nCount: " << value << '\n';
  };
  actions_aggregator.Traverse(StatisticsSaver);

  return EXIT_SUCCESS;
}
