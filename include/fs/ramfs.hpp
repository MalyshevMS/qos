#pragma once

#include <kernel/fs.hpp>

namespace Kernel {
namespace FS {
namespace RamFS {

    struct RamNode : public VFS::Node {
        uint8_t* buffer;
        uint32_t capacity;
    };


    void mount();
    RamNode* create_dir(RamNode* parent, const char* name);
    RamNode* create_file(RamNode* parent_dir, const char* name);

    extern RamNode* ramfs_root;

}; // namespace RamFS
}; // namespace FS
}; // namespace Kernel;
