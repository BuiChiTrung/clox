#include "clox/token.hpp"
#include <chrono>

class ClockNativeFunc : public Callable {
  public:
    int arg_num = 0;
    ExprVal invoke() override {
        auto now = std::chrono::system_clock::now();
        std::chrono::duration<double> unix_time = now.time_since_epoch();

        return unix_time.count();
    }

    std::string to_string() const override { return "<native-function clock>"; }
};