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

void init() {
    vfs_root = create_node("/");
    vfs_root->type = FS_DIR;
    vfs_root->finddir = finddir_default;
}

}; // namespace Kernel::FS::VFS
