#include <klib/fmt.hpp>
#include <klib/mem.hpp>
#include <cstdint>
#include <kernel/fs.hpp>
#include <klib/string.hpp>
#include <klib/cstring.hpp>
#include <fs/ramfs.hpp>
#include <kernel/vconsole.hpp>

namespace Kernel::FS::RamFS {

    using namespace kstd;
    
    RamNode* ramfs_root = nullptr;

    static void ramnode_realloc(RamNode* node, uint32_t new_capacity) {
        if (new_capacity == 0) {
            delete[] node->buffer;
            node->buffer = nullptr;
            node->capacity = 0;
            node->size = 0;
            return;
        }

        uint8_t* new_data = new uint8_t[new_capacity];

        if (node->buffer) {
            memcpy(new_data, node->buffer, node->size);
            delete[] node->buffer;
        }

        node->buffer = new_data;
        node->capacity = new_capacity;
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


        if (offset + size > ramnode->capacity) {
            uint32_t new_capacity = (offset + size) * 2;
            if (new_capacity < offset + size) new_capacity = offset + size;
            ramnode_realloc(ramnode, new_capacity);
        }

        if (!ramnode->buffer) {
            return 0;
        }

        memcpy(ramnode->buffer + offset, buffer, size);

        if (offset + size > ramnode->size) {
            ramnode->size = offset + size;
        }

        kinfo(fmt("RamFS: written {} bytes @ %x", size, ramnode->buffer + offset));
        return size;
    }

    void mount() {
        ramfs_root = create_node("ram");
        
        ramfs_root->finddir = VFS::finddir_default;
        ramfs_root->type = VFS::FS_DIR;

        kinfo(fmt("RamFS: mounted ramfs_root @ %x", ramfs_root));

        VFS::vfs_root->map["ram"] = ramfs_root;
    }

    RamNode* create_node(const char* name) {
        auto node = new RamNode;
        memset(node, 0, sizeof(RamNode));
        memcpy(node->name, name, strlen(name));

        return node;
    }

    RamNode* create_dir(RamNode* parent, const char* name) {
        auto dir = static_cast<RamNode*>(parent->finddir(parent, name));
        if (dir != nullptr) return dir;

        dir = create_node(name);
        
        dir->finddir = VFS::finddir_default;
        dir->type = VFS::FS_DIR;

        parent->map[name] = dir;

        return dir;
    }

    RamNode* create_file(RamNode* parent_dir, const char* name) {
        auto file = static_cast<RamNode*>(parent_dir->finddir(parent_dir, name));
        if (file != nullptr) return file;

        file = create_node(name);

        file->type = VFS::FS_FILE;
        file->read = ram_read;
        file->write = ram_write;

        parent_dir->map[name] = file;

        return file;
    }

};

