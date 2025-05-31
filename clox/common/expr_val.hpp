#pragma once
#include <memory>
#include <variant>

class LoxCallable;
class LoxClass;
class LoxInstance;

using ExprVal =
    std::variant<double, bool, std::string, std::shared_ptr<LoxCallable>,
                 std::shared_ptr<LoxInstance>, std::monostate>;