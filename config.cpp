#include "config.h"

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include <iostream>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

using std::cout;
using std::endl;

void Config::ParseOptions(int argc, char* argv[]) {
  /* Options description */
  po::options_description optional_desc("Optional options");
  optional_desc.add_options()
      ("help,h", "produce help message")
      ("output_file,o", po::value<std::string>(&OutputFile)->default_value("agr.txt"), "output file")
  ;

  po::options_description required_desc("Required options");
  required_desc.add_options()
      ("path,p", po::value<std::string>(&Directory)->required(), "directory with logs")
      ("jobs,j", po::value<unsigned>(&JobsNum)->required(), "threads number")
      ("count,c", po::value<unsigned>(&FilesNum)->required(), "logs number")
  ;

  po::options_description all_desc("Options");
  all_desc.add(optional_desc).add(required_desc);

  /* Parse */
  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, all_desc), vm);

  if (vm.count("help")) {
      cout << all_desc << endl;
      exit(0);
  }

  try {
    po::notify(vm);    
  } catch (po::error& e) {
    cout << e.what() << endl;
    cout << all_desc << endl;
    exit(0);
  }
}
