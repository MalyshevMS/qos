#include "stdlib.h"

extern "C" void user_main() {
    sleep(500);

    for (int i = 0; i < 10; i++) {
        print("Hello, world!\n");
    }
    
    exit(1234);
}