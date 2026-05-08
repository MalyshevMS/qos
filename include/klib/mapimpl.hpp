#pragma once

#include <klib/map.hpp>

namespace kstd {

template <typename Key, typename Value>
typename map<Key, Value>::Node *
map<Key, Value>::find_node(const Key &key) const {
  Node *current = root;
  while (current) {
    if (equal(current->key, key)) {
      return current;
    } else if (less(key, current->key)) {
      current = current->left;
    } else {
      current = current->right;
    }
  }
  return nullptr;
}

template <typename Key, typename Value>
typename map<Key, Value>::Node *map<Key, Value>::find_min(Node *node) const {
  if (!node)
    return nullptr;
  while (node->left) {
    node = node->left;
  }
  return node;
}

template <typename Key, typename Value>
typename map<Key, Value>::Node *map<Key, Value>::find_max(Node *node) const {
  if (!node)
    return nullptr;
  while (node->right) {
    node = node->right;
  }
  return node;
}

template <typename Key, typename Value>
typename map<Key, Value>::Node *
map<Key, Value>::get_successor(Node *node) const {
  if (!node)
    return nullptr;

  if (node->right) {
    return find_min(node->right);
  }

  Node *parent = node->parent;
  while (parent && node == parent->right) {
    node = parent;
    parent = parent->parent;
  }
  return parent;
}

template <typename Key, typename Value>
typename map<Key, Value>::Node *
map<Key, Value>::get_predecessor(Node *node) const {
  if (!node)
    return nullptr;

  if (node->left) {
    return find_max(node->left);
  }

  Node *parent = node->parent;
  while (parent && node == parent->left) {
    node = parent;
    parent = parent->parent;
  }
  return parent;
}

template <typename Key, typename Value>
void map<Key, Value>::rotate_left(Node *node) {
  if (!node || !node->right)
    return;

  Node *right_child = node->right;
  node->right = right_child->left;

  if (right_child->left) {
    right_child->left->parent = node;
  }

  right_child->parent = node->parent;
  if (!node->parent) {
    root = right_child;
  } else if (node == node->parent->left) {
    node->parent->left = right_child;
  } else {
    node->parent->right = right_child;
  }

  right_child->left = node;
  node->parent = right_child;
}

template <typename Key, typename Value>
void map<Key, Value>::rotate_right(Node *node) {
  if (!node || !node->left)
    return;

  Node *left_child = node->left;
  node->left = left_child->right;

  if (left_child->right) {
    left_child->right->parent = node;
  }

  left_child->parent = node->parent;
  if (!node->parent) {
    root = left_child;
  } else if (node == node->parent->right) {
    node->parent->right = left_child;
  } else {
    node->parent->left = left_child;
  }

  left_child->right = node;
  node->parent = left_child;
}

template <typename Key, typename Value>
void map<Key, Value>::fix_insert(Node *node) {
  while (node != root && node->parent && node->parent->color == RED) {
    if (node->parent == node->parent->parent->left) {
      Node *uncle = node->parent->parent->right;

      if (uncle && uncle->color == RED) {
        node->parent->color = BLACK;
        uncle->color = BLACK;
        node->parent->parent->color = RED;
        node = node->parent->parent;
      } else {
        if (node == node->parent->right) {
          node = node->parent;
          rotate_left(node);
        }
        node->parent->color = BLACK;
        node->parent->parent->color = RED;
        rotate_right(node->parent->parent);
      }
    } else {
      Node *uncle = node->parent->parent->left;

      if (uncle && uncle->color == RED) {
        node->parent->color = BLACK;
        uncle->color = BLACK;
        node->parent->parent->color = RED;
        node = node->parent->parent;
      } else {
        if (node == node->parent->left) {
          node = node->parent;
          rotate_right(node);
        }
        node->parent->color = BLACK;
        node->parent->parent->color = RED;
        rotate_left(node->parent->parent);
      }
    }
  }
  root->color = BLACK;
}

template <typename Key, typename Value>
void map<Key, Value>::fix_erase(Node *node) {
  while (node != root && get_color(node) == BLACK) {
    if (node == node->parent->left) {
      Node *sibling = node->parent->right;

      if (sibling && sibling->color == RED) {
        sibling->color = BLACK;
        node->parent->color = RED;
        rotate_left(node->parent);
        sibling = node->parent->right;
      }

      if (sibling) {
        if ((!sibling->left || sibling->left->color == BLACK) &&
            (!sibling->right || sibling->right->color == BLACK)) {
          sibling->color = RED;
          node = node->parent;
        } else if (!sibling->right || sibling->right->color == BLACK) {
          if (sibling->left)
            sibling->left->color = BLACK;
          sibling->color = RED;
          rotate_right(sibling);
          sibling = node->parent->right;
        }

        sibling->color = node->parent->color;
        node->parent->color = BLACK;
        if (sibling->right)
          sibling->right->color = BLACK;
        rotate_left(node->parent);
        node = root;
      } else {
        node = node->parent;
      }
    } else {
      Node *sibling = node->parent->left;

      if (sibling && sibling->color == RED) {
        sibling->color = BLACK;
        node->parent->color = RED;
        rotate_right(node->parent);
        sibling = node->parent->left;
      }

      if (sibling) {
        if ((!sibling->right || sibling->right->color == BLACK) &&
            (!sibling->left || sibling->left->color == BLACK)) {
          sibling->color = RED;
          node = node->parent;
        } else if (!sibling->left || sibling->left->color == BLACK) {
          if (sibling->right)
            sibling->right->color = BLACK;
          sibling->color = RED;
          rotate_left(sibling);
          sibling = node->parent->left;
        }

        sibling->color = node->parent->color;
        node->parent->color = BLACK;
        if (sibling->left)
          sibling->left->color = BLACK;
        rotate_right(node->parent);
        node = root;
      } else {
        node = node->parent;
      }
    }
  }
  if (node)
    node->color = BLACK;
}

template <typename Key, typename Value>
typename map<Key, Value>::Node *
map<Key, Value>::insert_node(Node *node, const Key &key, const Value &value,
                             Node *parent) {

  if (!node) {
    Node *new_node = new Node(key, value);
    new_node->parent = parent;
    return new_node;
  }

  if (equal(key, node->key)) {
    node->value = value;
    return node;
  }

  if (less(key, node->key)) {
    node->left = insert_node(node->left, key, value, node);
  } else {
    node->right = insert_node(node->right, key, value, node);
  }

  return node;
}

template <typename Key, typename Value>
void map<Key, Value>::insert(const Key &key, const Value &value) {
  bool was_empty = (root == nullptr);

  if (was_empty) {
    root = new Node(key, value);
    root->parent = nullptr;
    root->color = BLACK;
    sz = 1;
  } else {
    Node *target = find_node(key);
    if (target) {
      target->value = value;
      return;
    }

    root = insert_node(root, key, value, nullptr);
    sz++;
    fix_insert(root);
  }
}

template <typename Key, typename Value>
typename map<Key, Value>::Node *map<Key, Value>::erase_node(Node *node,
                                                            const Key &key) {

  if (!node)
    return nullptr;

  if (less(key, node->key)) {
    node->left = erase_node(node->left, key);
    if (node->left)
      node->left->parent = node;
  } else if (less(node->key, key)) {
    node->right = erase_node(node->right, key);
    if (node->right)
      node->right->parent = node;
  } else {
    // Node found
    if (!node->left) {
      Node *right = node->right;
      if (right)
        right->parent = node->parent;
      delete node;
      return right;
    } else if (!node->right) {
      Node *left = node->left;
      if (left)
        left->parent = node->parent;
      delete node;
      return left;
    } else {
      // Both children exist
      Node *successor = find_min(node->right);
      node->key = successor->key;
      node->value = successor->value;
      node->right = erase_node(node->right, successor->key);
      if (node->right)
        node->right->parent = node;
    }
  }

  return node;
}

template <typename Key, typename Value>
bool map<Key, Value>::erase(const Key &key) {
  if (!find_node(key)) {
    return false;
  }
  root = erase_node(root, key);
  if (root)
    root->parent = nullptr;
  sz--;
  return true;
}

template <typename Key, typename Value>
void map<Key, Value>::clear_node(Node *node) {
  if (!node)
    return;
  clear_node(node->left);
  clear_node(node->right);
  delete node;
}

template <typename Key, typename Value> void map<Key, Value>::clear() {
  clear_node(root);
  root = nullptr;
  sz = 0;
}

template <typename Key, typename Value>
Value &map<Key, Value>::operator[](const Key &key) {
  Node *node = find_node(key);
  if (!node) {
    insert(key, Value());
    node = find_node(key);
  }
  return node->value;
}

template <typename Key, typename Value>
Value &map<Key, Value>::at(const Key &key) {
  Node *node = find_node(key);
  if (!node) {
    // In a real implementation, should throw exception
    // For kernel code, we just return a default value
    static Value default_val;
    return default_val;
  }
  return node->value;
}

template <typename Key, typename Value>
const Value &map<Key, Value>::at(const Key &key) const {
  Node *node = find_node(key);
  if (!node) {
    static const Value default_val;
    return default_val;
  }
  return node->value;
}

template <typename Key, typename Value>
typename map<Key, Value>::iterator map<Key, Value>::find(const Key &key) {
  return iterator(find_node(key));
}

template <typename Key, typename Value>
const typename map<Key, Value>::iterator
map<Key, Value>::find(const Key &key) const {
  return const_cast<map *>(this)->find(key);
}

template <typename Key, typename Value>
size_t map<Key, Value>::count(const Key &key) const {
  return find_node(key) ? 1 : 0;
}

template <typename Key, typename Value>
bool map<Key, Value>::contains(const Key &key) const {
  return find_node(key) != nullptr;
}

template <typename Key, typename Value>
typename map<Key, Value>::iterator map<Key, Value>::begin() {
  return iterator(find_min(root));
}

template <typename Key, typename Value>
const typename map<Key, Value>::iterator map<Key, Value>::begin() const {
  return const_cast<map *>(this)->begin();
}

template <typename Key, typename Value>
typename map<Key, Value>::iterator map<Key, Value>::rbegin() {
  return iterator(find_max(root));
}

template <typename Key, typename Value>
const typename map<Key, Value>::iterator map<Key, Value>::rbegin() const {
  return const_cast<map *>(this)->rbegin();
}

} // namespace kstd
