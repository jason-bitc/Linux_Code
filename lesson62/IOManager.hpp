#ifndef __IOMANAGER_HPP
#define __IOMANAGER_HPP

#include "Comm.hpp"
#include "Connection.hpp"

#include <functional>
using on_message_t = std::function<std::string(std::string &)>;

#endif