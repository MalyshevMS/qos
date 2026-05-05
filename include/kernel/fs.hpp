#pragma once

#include <cstdint>

namespace Kernel {
namespace FS {
    enum Type {
        FS_FILE,    // Regular file
        FS_DIR,     // Directory
        FS_CHAR,    // Char device
        FS_VIRT     // Virtual
    };

    struct VFSNode {
        char name[256];
        uint32_t size;
        Type type;

        uint32_t (*read)(VFSNode* node, uint32_t offset, uint32_t size, uint8_t* buffer);
        uint32_t (*write)(VFSNode* node, uint32_t offset, uint32_t size, uint8_t* buffer);

        VFSNode* (*readdir)(VFSNode* node, uint32_t index);
        VFSNode* (*finddir)(VFSNode* node, const char* name);
    };

    extern VFSNode* vfs_root;
}; // namespace FS
}; // namespace Kernel