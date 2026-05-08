#pragma once

#include <cstddef>

namespace kstd {

template <typename Key, typename Value> class map {
private:
  enum Color { RED, BLACK };

  struct Node {
    Key key;
    Value value;
    Node *left;
    Node *right;
    Node *parent;
    Color color;

    Node(const Key &k, const Value &v)
        : key(k), value(v), left(nullptr), right(nullptr), parent(nullptr),
          color(RED) {}
  };

  Node *root;
  size_t sz;

  Node *find_node(const Key &key) const;
  Node *insert_node(Node *node, const Key &key, const Value &value,
                    Node *parent);
  Node *erase_node(Node *node, const Key &key);
  void clear_node(Node *node);

  void rotate_left(Node *node);
  void rotate_right(Node *node);
  void fix_insert(Node *node);
  void fix_erase(Node *node);

  Node *get_successor(Node *node) const;
  Node *get_predecessor(Node *node) const;
  Node *find_min(Node *node) const;
  Node *find_max(Node *node) const;
  Color get_color(Node *node) const { return node ? node->color : BLACK; }

public:
  class iterator {
  private:
    Node *current;
    friend class map;
    iterator(Node *node) : current(node) {}

  public:
    iterator() : current(nullptr) {}

    iterator &operator++() {
      if (!current)
        return *this;
      if (current->right) {
        current = current->right;
        while (current->left)
          current = current->left;
      } else {
        Node *parent = current->parent;
        while (parent && current == parent->right) {
          current = parent;
          parent = parent->parent;
        }
        current = parent;
      }
      return *this;
    }

    iterator operator++(int) {
      iterator tmp = *this;
      ++(*this);
      return tmp;
    }

    iterator &operator--() {
      if (!current)
        return *this;
      if (current->left) {
        current = current->left;
        while (current->right)
          current = current->right;
      } else {
        Node *parent = current->parent;
        while (parent && current == parent->left) {
          current = parent;
          parent = parent->parent;
        }
        current = parent;
      }
      return *this;
    }

    iterator operator--(int) {
      iterator tmp = *this;
      --(*this);
      return tmp;
    }

    bool operator==(const iterator &other) const {
      return current == other.current;
    }
    bool operator!=(const iterator &other) const {
      return current != other.current;
    }

    Key &first() { return current->key; }
    const Key &first() const { return current->key; }

    Value &second() { return current->value; }
    const Value &second() const { return current->value; }

    Key *operator->() { return &current->key; }
    const Key *operator->() const { return &current->key; }
  };

  map() : root(nullptr), sz(0) {}
  ~map() { clear(); }

  map(const map &) = delete;
  map &operator=(const map &) = delete;

  Value &operator[](const Key &key);
  Value &at(const Key &key);
  const Value &at(const Key &key) const;

  size_t size() const { return sz; }
  bool empty() const { return sz == 0; }

  void insert(const Key &key, const Value &value);
  bool erase(const Key &key);
  void clear();

  iterator find(const Key &key);
  const iterator find(const Key &key) const;
  size_t count(const Key &key) const;
  bool contains(const Key &key) const;

  iterator begin();
  const iterator begin() const;
  iterator end() { return iterator(nullptr); }
  const iterator end() const { return iterator(nullptr); }

  iterator rbegin();
  const iterator rbegin() const;
  iterator rend() { return iterator(nullptr); }
  const iterator rend() const { return iterator(nullptr); }

private:
  static bool less(const Key &a, const Key &b) { return a < b; }
  static bool equal(const Key &a, const Key &b) {
    return !(less(a, b)) && !(less(b, a));
  }
};

} // namespace kstd

#include <klib/mapimpl.hpp>
