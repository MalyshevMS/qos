#include <cfg/asm.txx>
#include <kernel/console.hpp>
#include <kernel/vga.hpp>
#include <kernel/serial.hpp>
#include <kernel/power.hpp>
#include <driver/keyboard.hpp>
#include <driver/disk.hpp>
#include <driver/timer.hpp>

using namespace kstd;
using namespace Driver;

namespace Kernel::Console {
    static int cursor_x = 0;
    static int cursor_y = 0;
    static int prompt_y = 0;
    static bool running = true;
    static string prompt;

    static void newline() {
        cursor_x = 0;
        cursor_y++;

        if (cursor_y >= Vga::height) {
            Vga::scroll_up();
            cursor_y = Vga::height - 1;
        }
    }

    static void print_char(char ch) {
        if (ch == '\n') {
            newline();
        } else if (ch == '\r') {
            cursor_x = 0;
        } else if (ch == '\b') {
            if (cursor_x > 0) {
                cursor_x--;
                Vga::putc(cursor_x, cursor_y, ' ');
            }
        } else {
            Vga::putc(cursor_x, cursor_y, ch);
            cursor_x++;

            if (cursor_x >= Vga::width) {
                newline();
            }
        }
        
        Vga::update_cursor(cursor_x, cursor_y);
    }

    static void print(const string& str) {
        for (size_t i = 0; i < str.size(); i++) {
            print_char(str[i]);
        }
    }

    static void print_cstring(const char* str) {
        if (!str) return;
        while (*str) {
            print_char(*str++);
        }
    }

    static void println(const string& str = "") {
        print(str);
        print_char('\n');
    }

    static string get_command(const string& input) {
        size_t pos = input.find(' ');

        if (pos == string::npos) {
            return input;
        }

        return input.substr(0, pos);
    }

    static string get_args(const string& input) {
        size_t pos = input.find(' ');
        if (pos == string::npos) {
            return "";
        }

        while (pos < input.size() && input[pos] == ' ') {
            pos++;
        }

        if (pos >= input.size()) {
            return "";
        }

        return input.substr(pos);
    }

    void help() {
        println("Available commands:");
        println("    clear - clears screen");
        println("    echo - outputs a string");
        println("    watch - watch command output every second, press Ctrl+C to exit");
        println("    info - system info");
        println("    tickp - show tick period (in femtoseconds)");
        println("    freq - show CPU frequency, calibrated at boot (Hz and MHz)");
        println("    ctsc - show current CPU frequency (recalibrates TSC)");
        println("    uptime - get machine uptime (in milliseconds)");
        println("    rfs - read first sector");
        println("    sleep - wait for 5 seconds");
        println("    reboot - reboot the system");
        println("    exit/poweroff - power off the system");
    }

    void clear() {
        Vga::clear();
        cursor_x = 0;
        cursor_y = 0;
        prompt_y = 0;
    }

    void echo(const string& args) {
        if (args.empty()) {
            print_char('\n');
        } else {
            println(args);
        }
    }

    void rfs() {
        auto buffer = new uint8_t[512];
        Disk::read_sectors_ATA_PIO(buffer, 0, 1);
        int idx = 0;

        for (int i = 0; i < 16; i++) {
            for (int j = 0; j < 32; j++) {
                print(fmt("{}", (char)buffer[idx]));
                idx++;
            }

            println();
        }

        delete buffer;
    }

    void sleep() {
        println("Sleeping for 5 seconds");
        Timer::sleep(5000);
        println("Wake up!");
    }

    void uptime() {
        println(fmt("Uptime: {} milliseconds", Timer::ktime_ms()));
    }

    void freq() {
        auto freq = Timer::frequency();
        println(fmt("CPU Frequency (calibrated at boot): {} MHz ({} Hz)", freq / 1'000'000, freq));
    }

    void tickp() {
        println(fmt("Tick period: {} femtoseconds", Timer::tick_period()));
    }

    void ctsc() {
        auto freq = Timer::calibrate_tsc();
        println(fmt("CPU frequency (current): {} MHz ({} Hz)", freq / 1'000'000, freq));
    }

    void info() {
        println("=== System info ===");
        println("Arch: x86");
        println("Resolution: 80x25 (VGA)");
        freq();
        ctsc();
        uptime();
    }

    void execute_command(const string& input) {
        if (input.empty()) {
            return;
        }

        string cmd = get_command(input);
        string args = get_args(input);

        if (cmd == "help") {
            help();
        } else if (cmd == "watch") {
            while (Keyboard::getscan() != Keyboard::SCANCODE_C || !Keyboard::is_ctrl_pressed()) {
                clear();
                println(fmt("Wathcing command '{}' (every second). Press Ctrl+C to exit.", args));
                execute_command(args);
                Timer::sleep(1000);
            }
        } else if (cmd == "clear") {
            clear();
        } else if (cmd == "echo") {
            echo(args);
        } else if (cmd == "info") {
            info();
        } else if (cmd == "rfs") {
            rfs();
        } else if (cmd == "sleep") {
            sleep();
        } else if (cmd == "uptime") {
            uptime();
        } else if (cmd == "reboot") {
            Hardware::reboot();
        } else if (cmd == "freq") {
            freq();
        } else if (cmd == "tickp") {
            tickp();
        } else if (cmd == "ctsc") {
            ctsc();
        } else if (cmd == "poweroff" || cmd == "exit") {
            Hardware::poweroff();
            println("If you see this message, your ACPI controller is broken");
        } else {
            string err_msg = "Command not found: ";
            err_msg += cmd;
            println(err_msg);
        }
    }

    void init() {
        Vga::clear();
        cursor_x = 0;
        cursor_y = 0;
        prompt_y = 0;
        running = true;
        prompt = "(kernel)> ";

        tickp();
        println("Welcome to QOS!");
        println("You are now in kernel console.");
        help();
        print(prompt);
        prompt_y = cursor_y;
    }

    void run() {
        string input;

        while (running) {
            if (Keyboard::has_data()) {
                auto sc = Keyboard::getscan();
                auto ch = Keyboard::scantochar(sc);

                if (ch != 0 && ch != '\n' && ch != '\b') {
                    print_char(ch);
                    input += ch;
                }

                if (sc == Keyboard::SCANCODE_BACKSPACE) {
                    if (!input.empty()) {
                        input.erase(input.size() - 1, 1);
                        print_char('\b');
                    }
                }

                if (sc == Keyboard::SCANCODE_ENTER) {
                    print_char('\n');
                    
                    if (!input.empty()) {
                        Serial::println("Command: '{}'", input);
                        execute_command(input);
                    }

                    input.clear();
                    print(prompt);
                    prompt_y = cursor_y;
                }
            }

            Vga::update_cursor(cursor_x, cursor_y);
            CPU_HALT;
        }
    }

} // namespace Kernel::Console
