all:
	g++ main.cpp config.cpp trie.cpp log_parser.cpp -g -lboost_program_options -lboost_filesystem -lboost_system -std=c++11

test:
	g++ tests.cpp trie.cpp -g -lboost_system -std=c++11
