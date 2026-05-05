#include <kernel/fs.hpp>
#include <kernel/vconsole.hpp>
#include <klib/mem.hpp>
#include <klib/cstring.hpp>
#include <klib/fmt.hpp>

namespace Kernel::FS {

using namespace kstd;

VFSNode* vfs_root = nullptr;
VFSNode* node_proc = nullptr;
VFSNode* node_version = nullptr;

uint32_t version_read(VFSNode* node, uint32_t offset, uint32_t size, uint8_t* buff) {
    const char* data = "QOS v1.0\n";
    uint32_t len = strlen(data);

    if (offset >= len) return 0; // End of file
    if (offset + size > len) size = len - offset; // Truncate

    memcpy(buff, (uint8_t*)data + offset, size);

    return size;
}

VFSNode* proc_finddir(VFSNode* node, const char* name) {
    if (memcmp(name, "version", 8) == 0) return node_version; // Hardcode first

    return nullptr;
}

VFSNode* root_finddir(VFSNode* node, const char* name) {
    if (memcmp(name, "proc", 5) == 0) return node_proc;

    return nullptr;
}

extern "C" void vfs_init() {
    // "/"
    vfs_root = new VFSNode;
    memset(vfs_root, 0, sizeof(VFSNode));
    memcpy(vfs_root->name, "/", 2);
    vfs_root->type = FS_DIR;
    vfs_root->finddir = root_finddir;
    
    // "/proc"
    node_proc = new VFSNode;
    memset(node_proc, 0, sizeof(VFSNode));
    memcpy(node_proc->name, "proc", 5);
    node_proc->type = FS_DIR;
    node_proc->finddir = proc_finddir;
    
    // "/proc/version"
    node_version = new VFSNode;
    memset(node_version, 0, sizeof(VFSNode));
    memcpy(node_version, "version", 8);
    node_version->type = FS_FILE;
    node_version->read = version_read;
    node_version->size = 10; // Yes, hardcoding again

    kinfo(fmt("VFS: '/' at %x", vfs_root));
    kinfo(fmt("VFS: '/proc' at %x", node_proc));
    kinfo(fmt("VFS: '/proc/version' at %x", node_version));
}

}; // namespace Kernel::FS