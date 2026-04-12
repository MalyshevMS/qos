#pragma once

#include <klib/string.hpp>

namespace Kernel {
namespace Console {
    void init();

    void run();

    void execute_command(const kstd::string& input);

    void help();
    void clear();
    void echo(const kstd::string& args);
    void info();
    void exit();
    
} // namespace Console
} // namespace Kernel
