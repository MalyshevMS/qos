#include <kernel/fs.hpp>
#include <kernel/vconsole.hpp>
#include <klib/cstring.hpp>
#include <klib/fmt.hpp>
#include <klib/map.hpp>
#include <klib/mem.hpp>
#include <klib/string.hpp>

namespace Kernel::FS {

using namespace kstd;

VFSNode* vfs_root = nullptr;

VFSNode* vfs_finddir(VFSNode* node, const char* name) {
    auto str = string(name);
    auto it = node->map.find(str);

    if (it == node->map.end()) return nullptr;

    return it.second();
}

void vfs_init() {
    vfs_root = new VFSNode;
    memset(vfs_root, 0, sizeof(VFSNode));
    memcpy(vfs_root->name, "/", 2);
    vfs_root->type = FS_DIR;
    vfs_root->finddir = vfs_finddir;

    kinfo(fmt("VFS: '/'@%x", vfs_root));
}

}; // namespace Kernel::FS
