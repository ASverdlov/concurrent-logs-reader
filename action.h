#ifndef ACTION_H
#define ACTION_H

#include <string>
#include <array>

struct Action {
  std::string Name;
  uint32_t Timestamp;
  uint32_t ActorId;
  std::array<int, 10> Properties;
};

#endif
