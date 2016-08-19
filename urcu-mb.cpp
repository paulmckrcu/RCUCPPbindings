#include <unistd.h>
#include "urcu-mb.hpp"

static class std::rcu_mb _rm;
class std::rcu_domain &rm = _rm;
