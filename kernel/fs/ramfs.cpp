#include <kernel/fs.hpp>
#include <klib/string.hpp>
#include <fs/ramfs.hpp>

namespace Kernel::FS::RamFS {
    
    RamNode* ramfs_root = nullptr;

    void mount() {
        ramfs_root = static_cast<RamNode*>(VFS::create_node("ram"));
        ramfs_root->finddir = VFS::finddir_default;
        ramfs_root->type = VFS::FS_DIR;

        VFS::vfs_root->map["ram"] = ramfs_root;
    }

    RamNode* create_dir(RamNode* parent, const char* name) {
        auto dir = static_cast<RamNode*>(parent->finddir(parent, name));
        if (dir != nullptr) return dir;

        dir = static_cast<RamNode*>(VFS::create_node(name));
        dir->finddir = VFS::finddir_default;
        dir->type = VFS::FS_DIR;

        parent->map[name] = dir;

        return dir;
    }

    RamNode* create_file(RamNode* parent_dir, const char* name) {
        auto file = static_cast<RamNode*>(parent_dir->finddir(parent_dir, name));
        if (file != nullptr) return file;

        file = static_cast<RamNode*>(VFS::create_node(name));
        file->type = VFS::FS_FILE;
        // TODO: read/write
    }

};

