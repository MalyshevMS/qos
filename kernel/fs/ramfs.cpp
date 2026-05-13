#include <klib/fmt.hpp>
#include <klib/mem.hpp>
#include <cstddef>
#include <cstdint>
#include <kernel/fs.hpp>
#include <klib/string.hpp>
#include <fs/ramfs.hpp>
#include <kernel/vconsole.hpp>

namespace Kernel::FS::RamFS {

    using namespace kstd;
    
    RamNode* ramfs_root = nullptr;

    static void ramnode_realloc(RamNode* node, uint32_t new_capacity) {
        kwarn("Called ramnode_realloc()");
        kwarn(fmt("Useful info: node=%x, buf=%x, cap={}, sz={}", node, node->buffer, node->capacity, node->size));
    
        if (new_capacity == 0) {
            kwarn("new_capacity==0: deleting data");
            delete[] node->buffer;
            node->buffer = nullptr;
            node->capacity = 0;
            node->size = 0;
            kwarn(fmt("ramnode_realloc() info: buff=%x, cap={}, sz={}",
                       node->buffer, node->capacity, node->size));
            return;
        }

        uint8_t* new_data = new uint8_t[new_capacity];
        kwarn(fmt("new_data allocated @ %x", new_data));

        if (node->buffer) {
            memcpy(new_data, node->buffer, node->size);
            kwarn(fmt("Copied {} bytes from %x to %x", node->size, node->buffer, new_data));
            delete[] node->buffer;
        }

        node->buffer = new_data;
        kwarn(fmt("Node buffer is now pointing to %x", node->buffer));
        kwarn(fmt("cap={}, new_cap={}", node->capacity, new_capacity));
        node->capacity = new_capacity;
        kwarn(fmt("cap={}, new_cap={}", node->capacity, new_capacity));
        kwarn(fmt("cap={}, new_cap={}", node->capacity, new_capacity));
        kwarn(fmt("cap={}, new_cap={}", node->capacity, new_capacity));
        kwarn(fmt("cap={}, new_cap={}", node->capacity, new_capacity));
        
        kwarn(fmt("ramnode_realloc() info: buff=%x, cap={}, sz={}",
                   node->buffer, node->capacity, node->size));
    }

    static uint32_t ram_read(VFS::Node* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
        auto ramnode = static_cast<RamNode*>(node);

        if (offset >= ramnode->size) return 0;
        if (offset + size > ramnode->size) size = ramnode->size - offset;

        memcpy(buffer, ramnode->buffer + offset, size);

        return size;
    }

    static uint32_t ram_write(VFS::Node* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
        auto ramnode = static_cast<RamNode*>(node);

        kwarn(fmt("VFS: RamFS: write info: addr=%x, off={}, sz={}, calc=%x", ramnode, offset, size, ramnode + offset));

        if (offset + size > ramnode->capacity) ramnode_realloc(ramnode, (offset + size) * 2);

        memcpy(ramnode->buffer + offset, buffer, size);

        if (offset + size > ramnode->size) {
            ramnode->size = offset + size;
        }

        // kinfo(fmt("VFS: RamFS: written {} bytes @ %x", size, ramnode));
        return size;
    }

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
        file->read = ram_read;
        file->write = ram_write;
        file->capacity = 0;
        file->buffer = nullptr;

        parent_dir->map[name] = file;

        return file;
    }

};

