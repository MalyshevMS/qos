#include "klib/fmt.hpp"
#include "klib/mem.hpp"
#include <cstddef>
#include <cstdint>
#include <kernel/fs.hpp>
#include <klib/string.hpp>
#include <fs/ramfs.hpp>
#include <kernel/vconsole.hpp>

namespace Kernel::FS::RamFS {
    
    RamNode* ramfs_root = nullptr;

    static void ramnode_realloc(RamNode* node, uint32_t new_capacity) {
        if (new_capacity == 0) {
            delete[] node->buffer;
            node->buffer = nullptr;
            node->capacity = 0;
            node->size = 0;
            return;
        }

        auto new_data = new uint8_t[new_capacity];
        if (!new_data) return;

        if (node->buffer) {
            size_t copy_size = (node->size < new_capacity) ? node->size : new_capacity;
            kstd::memcpy(new_data, node->buffer, copy_size);
            delete[] node->buffer;
        }

        node->buffer = new_data;
        node->capacity = new_capacity;
    }

    static uint32_t ram_read(VFS::Node* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
        auto ramnode = static_cast<RamNode*>(node);

        if (offset >= ramnode->size) return 0;
        if (offset + size > ramnode->size) size = ramnode->size - offset;

        kstd::memcpy(buffer, ramnode->buffer + offset, size);

        return size;
    }

    static uint32_t ram_write(VFS::Node* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
        kwarn("did we get here? 1");
        auto ramnode = static_cast<RamNode*>(node);
        kwarn("did we get here? 2");

        if (offset + size > ramnode->capacity) ramnode_realloc(ramnode, (offset + size) * 2);
        kwarn("did we get here? 3");

        kstd::memcpy(ramnode->buffer + offset, buffer, size);
        kwarn("did we get here? 4");

        if (offset + size > ramnode->size) {
            ramnode->size = offset + size;
        }
        kwarn("did we get here? 5");

        // kinfo(kstd::fmt("VFS: RamFS: written {} bytes @ %x", size, ramnode));
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

