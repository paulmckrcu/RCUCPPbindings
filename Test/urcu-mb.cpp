#include <unistd.h>
#include "urcu-mb.hpp"

static class rcu_mb _rm;
class rcu_domain &rm = _rm;
