
#pragma once
#include <string>
#include <sys/types.h>
#include <variant>

const uint MAX_ARGS_NUM = 255;
// std::monostate to present nil in Lox
constexpr std::monostate NIL{};

const std::string INIT_METHOD = "init";