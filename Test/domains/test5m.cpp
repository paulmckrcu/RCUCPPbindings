#include "urcu-mb.hpp"

static std::rcu_mb _rm;
std::rcu_domain &rm = _rm;
