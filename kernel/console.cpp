#include <cfg/asm.txx>
#include <kernel/console.hpp>
#include <kernel/serial.hpp>
#include <kernel/power.hpp>
#include <kernel/vconsole.hpp>
#include <driver/keyboard.hpp>
#include <driver/pci.hpp>
#include <driver/disk.hpp>
#include <driver/timer.hpp>

using namespace kstd;
using namespace Driver;

namespace Kernel::Console {
    static bool running = true;
    static string prompt;
    static int prompt_y;

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
        kprintln("Available commands:");
        kprintln("    clear - clears screen");
        kprintln("    echo - outputs a string");
        kprintln("    kinfo - simulate kinfo() - basic kernel info messages");
        kprintln("    kdebug - simulate kdebug() - debug messages");
        kprintln("    kwarn - simulate kwarn() - warnings");
        kprintln("    kpanic - simulate kpanic() - kernel panic (will cause real panic)");
        kprintln("    watch - watch command output every second, press Ctrl+C to exit");
        kprintln("    info - system info");
        kprintln("    tickp - show tick period (in femtoseconds)");
        kprintln("    freq - show CPU frequency, calibrated at boot (Hz and MHz)");
        kprintln("    ctsc - show current CPU frequency (recalibrates TSC)");
        kprintln("    uptime - get machine uptime (in milliseconds)");
        kprintln("    lspci - list PCI devices");
        kprintln("    satainfo - list SATA devices");
        kprintln("    rfs <device> - read first sector from SATA device");
        kprintln("    sleep - wait for 5 seconds");
        kprintln("    divz - cause kernel panic (divide int32 by zero)");
        kprintln("    reboot - reboot the system");
        kprintln("    exit/poweroff - power off the system");
    }

    void clear() {
        kclear();
        prompt_y = 0;
    }

    void echo(const string& args) {
        if (args.empty()) {
            kprintln();
        } else {
            kprintln(args);
        }
    }

    void lspci() {
        auto devs = PCI::get_devices();
        auto count = PCI::get_device_count();

        for (int i = 0; i < count; i++) {
            auto& dev = devs[i];
            auto classname = PCI::get_class_name(dev.class_code);
            kprintln(fmt("ID:%x:%x Class:%x Sub:%x - {}", dev.vendor_id, dev.device_id, dev.class_code, dev.subclass, classname));
        }
    }

    void satainfo() {
        int count = Disk::device_count();
        kprintln(fmt("SATA devices: {}", count));

        for (int i = 0; i < count; i++) {
            uint8_t port;
            uint32_t sig;
            if (Disk::get_device_info(i, port, sig)) {
                kprintln(fmt("  [{}] Port {} Signature %x", i, port, sig));
            }
        }
    }

    void rfs(const string& args) {
        int device_index = 0;
        if (!args.empty()) {
            device_index = 0;
            for (size_t i = 0; i < args.size(); i++) {
                if (args[i] < '0' || args[i] > '9') {
                    kprintln("Invalid device index.");
                    return;
                }
                device_index = device_index * 10 + (args[i] - '0');
            }
        }

        int count = Disk::device_count();
        if (count == 0) {
            kprintln("No SATA devices available.");
            return;
        }
        if (device_index < 0 || device_index >= count) {
            kprintln(fmt("Device index out of range: {}", device_index));
            return;
        }

        auto buffer = new uint8_t[512];
        bool ok = Disk::read_sectors(device_index, buffer, 0, 1);
        if (!ok) {
            kprintln("Disk read failed.");
            delete[] buffer;
            return;
        }

        int idx = 0;
        for (int i = 0; i < 16; i++) {
            for (int j = 0; j < 32; j++) {
                kprint(fmt("{}", (char)buffer[idx]));
                idx++;
            }

            kprintln();
        }

        delete[] buffer;
    }

    void sleep() {
        kprintln("Sleeping for 5 seconds");
        Timer::sleep(5000);
        kprintln("Wake up!");
    }

    void uptime() {
        kprintln(fmt("Uptime: {} milliseconds", Timer::ktime_ms()));
    }

    void freq() {
        auto freq = Timer::frequency();
        kprintln(fmt("CPU Frequency (calibrated at boot): {} MHz ({} Hz)", freq / 1'000'000, freq));
    }

    void tickp() {
        kprintln(fmt("Tick period: {} femtoseconds", Timer::tick_period()));
    }

    void ctsc() {
        auto freq = Timer::calibrate_tsc();
        kprintln(fmt("CPU frequency (current): {} MHz ({} Hz)", freq / 1'000'000, freq));
    }

    void info() {
        kprintln("=== System info ===");
        kprintln("Arch: x86");
        kprintln("Resolution: 80x25 (VGA)");
        freq();
        ctsc();
        uptime();
    }

    void divz() {
        kprintln("Testing division by zero (for int32_t)...");
        auto zero = 0;
        auto res = 4171 / zero;
        kprintln(fmt("{}", res));
    }

    void systemd() {
        kcolor(0x1F);
        clear();
        kprintln("                                       /\\            ");
        kprintln("                                      /  \\           ");
        kprintln("                                     / /\\ \\          ");
        kprintln("                            ________/ /__\\ \\________ ");
        kprintln("                            \\  ____  ______ _____  / ");
        kprintln("                             \\ \\  / /      \\ \\  / /  ");
        kprintln("                              \\ \\/ /        \\ \\/ /   ");
        kprintln("                               \\  /          \\  /    ");
        kprintln("                               /  \\          /  \\    ");
        kprintln("                              / /\\ \\        / /\\ \\   ");
        kprintln("                             / /__\\_\\______/ /__\\ \\  ");
        kprintln("                            /_______ _____  _______\\ ");
        kprintln("                                    \\ \\  / /         ");
        kprintln("                                     \\ \\/ /          ");
        kprintln("                                      \\  /           ");
        kprintln("                                       \\/            ");
        kprintln();
        kprintln("SystemD RedHat (C) CyberWeapon (TM).");

        kprint("Installing GCC Exploit        [..............................]");
        kprint("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
        for (int i = 0; i < 30; i++) {
            kprint("#");
            Timer::sleep(100);
        }
        kprintln("] Done.");

        // int b = 0;
        // int c = a 
        kprint("Installing RedHat CyberWeapon [..............................]");
        kprint("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
        for (int i = 0; i < 30; i++) {
            kprint("#");
            Timer::sleep(300);
        }
        kprintln("] Done.");

        kprint("Activating nanorobots         [..............................]");
        kprint("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
        for (int i = 0; i < 30; i++) {
            kprint("#");
            Timer::sleep(250);
        }
        kprintln("] Done.");
        kprint("Big brother is watching you.");

        INT_DISABLE;
        CPU_HALT;
        Timer::sleep(10);
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
                kprintln(fmt("Wathcing command '{}' (every second). Press Ctrl+C to exit.", args));
                execute_command(args);
                Timer::sleep(1000);
            }
        } else if (cmd == "clear") {
            clear();
        } else if (cmd == "echo") {
            echo(args);
        } else if (cmd == "kinfo") {
            kinfo(args);
        } else if (cmd == "kdebug") {
            kdebug(args);
        } else if (cmd == "kwarn") {
            kwarn(args);
        } else if (cmd == "kpanic") {
            kpanic(args);
        } else if (cmd == "info") {
            info();
        } else if (cmd == "lspci") {
            lspci();
        } else if (cmd == "satainfo") {
            satainfo();
        } else if (cmd == "rfs") {
            rfs(args);
        } else if (cmd == "sleep") {
            sleep();
        } else if (cmd == "divz") {
            divz();
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
        } else if (cmd == "systemd") {
            systemd();
        } else if (cmd == "poweroff" || cmd == "exit") {
            Hardware::poweroff();
            kprintln("If you see this message, your ACPI controller is broken");
        } else {
            string err_msg = "Command not found: ";
            err_msg += cmd;
            kprintln(err_msg);
        }
    }

    void init() {
        running = true;
        prompt = "(kernel)> ";

        kdebug("Welcome to QOS!");
        kprintln("You are now in kernel console.");
        kprint(prompt);
        int cx, cy;
        get_cursor(cx, cy);
        prompt_y = cy;
    }

    void run() {
        string input;

        while (running) {
            if (Keyboard::has_data()) {
                auto sc = Keyboard::getscan();
                auto ch = Keyboard::scantochar(sc);

                if (ch != 0 && ch != '\n' && ch != '\b') {
                    kprint(string(ch));
                    input += ch;
                }

                if (sc == Keyboard::SCANCODE_BACKSPACE) {
                    if (!input.empty()) {
                        input.erase(input.size() - 1, 1);
                        kprint("\b");
                        int cx, cy;
                        get_cursor(cx, cy);
                        kputc(cx, cy, ' ');
                    }
                }

                if (sc == Keyboard::SCANCODE_ENTER) {
                    kprintln();
                    
                    if (!input.empty()) {
                        // Serial::println("Command: '{}'", input);
                        execute_command(input);
                    }

                    input.clear();
                    kprint(prompt);
                    int cx, cy;
                    get_cursor(cx, cy);
                    prompt_y = cy;
                }
            }

            CPU_HALT;
        }
    }

} // namespace Kernel::Console
