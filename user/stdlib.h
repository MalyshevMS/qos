inline void exit(int code) {
    asm volatile (
        "movl $1, %%eax\n"
        "movl %0, %%ebx\n"
        "int $0x80"
        :
        : "r"(code)
        : "eax", "ebx"
    );
}

inline void sleep(int ms) {
    asm volatile (
        "movl $2, %%eax\n"
        "movl %0, %%ebx\n"
        "int $0x80"
        :
        : "r"(ms)
        : "eax", "ebx"
    );
}

inline void print(const char *text) {
    asm volatile(
        "movl $3, %%eax\n"
        "movl %0, %%ebx\n"
        "int $0x80"
        :
        : "r"(text)
        : "eax", "ebx"
    );
}
