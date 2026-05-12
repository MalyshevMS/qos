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

Node* create_node(const char* name) {
    auto node = new Node;
    memset(node, 0, sizeof(Node));
    memcpy(node->name, name, strlen(name));

    kinfo(fmt("VFS: created new node '{}' @ %x", name, node));

    return node;
}

Node* find_node(const char* path) {
    Node* current = nullptr;
    if (path[0] == '/') current = vfs_root;
    // TODO: check for ~ when you add /home/...

    auto len = strlen(path);

    if (path[len - 1] == '/') len--;

    string buffer = "";

    for (int i = 1; i < len; i++) {
        if (path[i] == '/') {
            current = current->finddir(current, buffer.c_str());
            if (!current) return nullptr;
            buffer = "";
        } else {
            buffer += path[i];
        }
    }

    current = current->finddir(current, buffer.c_str());

    return current;
}

void init() {
    vfs_root = create_node("/");
    vfs_root->type = FS_DIR;
    vfs_root->finddir = finddir_default;
}

}; // namespace Kernel::FS::VFS
