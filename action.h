#ifndef ACTION_H
#define ACTION_H

#include <string>
#include <array>

struct Action {
  static constexpr size_t NUM_PROPERTIES = 10;

  std::string Name;
  uint32_t Timestamp;
  uint32_t ActorId;
  std::array<int, NUM_PROPERTIES> Properties;
};

#endif
