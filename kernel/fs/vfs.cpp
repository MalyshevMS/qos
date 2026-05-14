#include <cstdint>
#include <kernel/fs.hpp>
#include <kernel/vconsole.hpp>
#include <klib/cstring.hpp>
#include <klib/fmt.hpp>
#include <klib/map.hpp>
#include <klib/mem.hpp>
#include <klib/string.hpp>

namespace Kernel::FS::VFS {

using namespace kstd;

Node* vfs_root = nullptr;

Node* finddir_default(Node* node, const char* name) {
    auto str = string(name);
    auto it = node->map.find(str);

    if (it == node->map.end()) return nullptr;

    return it.second();
}

Node* readdir_default(Node* node, uint32_t index) {
    if (index >= node->map.size()) return nullptr;

    auto it = node->map.begin();
    for (uint32_t i = 0; i < index; i++) it++;

    return it.second();
}

Node* create_node(const char* name) {
    auto node = new Node;
    memset(node, 0, sizeof(Node));
    memcpy(node->name, name, strlen(name));

    kinfo(fmt("VFS: created new node '{}' @ %x", name, node));

    return node;
}

Node* find_node(const char* path) {
    if (!path || path[0] == '\0') return nullptr;

    Node* current = vfs_root;

    size_t i = 0;
    if (path[0] == '/') {
        i = 1;
    }

    char buffer[256];
    
    while (path[i] != '\0') {
        size_t b_idx = 0;

        while (path[i] != '/' && path[i] != '\0' && b_idx < 255) {
            buffer[b_idx++] = path[i++];
        }
        buffer[b_idx] = '\0';

        if (path[i] == '/') {
            while (path[i] == '/') i++;
        }

        if (b_idx > 0) {
            if (current->type != FS_DIR) {
                return nullptr;
            }

            Node* next = current->finddir(current, buffer);
            if (!next) {
                return nullptr;
            }
            current = next;
        }
    }

    return current;
}

void init() {
    vfs_root = create_node("/");
    vfs_root->type = FS_DIR;
    vfs_root->finddir = finddir_default;
}

}; // namespace Kernel::FS::VFS
