#pragma once

#include <cstdint>
#include <klib/string.hpp>
#include <klib/map.hpp>

namespace Kernel {
namespace FS {
namespace VFS {

    enum Type {
        FS_FILE,    // Regular file
        FS_DIR,     // Directory
        FS_CHAR,    // Char device
        FS_VIRT     // Virtual
    };

    struct Node {
        char name[256];
        uint32_t size;
        Type type;

        uint32_t (*read)(Node*  node, uint32_t offset, uint32_t size, uint8_t* buffer);
        uint32_t (*write)(Node* node, uint32_t offset, uint32_t size, uint8_t* buffer);

        Node* (*finddir)(Node* node, const char* name);
        Node* (*readdir)(Node* node, uint32_t index);

        Node* (*create_file)(Node* node, const char* name);
        Node* (*create_dir)(Node* node, const char* name);

        kstd::map<kstd::string, Node*> map;

        virtual ~Node() {}
    };

    void init();

    Node* finddir_default(Node* node, const char* name);
    Node* readdir_default(Node* node, uint32_t index);
    Node* create_node(const char* name);
    Node* find_node(const char* path);

    extern Node* vfs_root;
}; // namespace VFS
}; // namespace FS
}; // namespace Kernel
