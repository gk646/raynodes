// Copyright (c) 2023 gk646
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
#define CX_FINISHED
#ifndef CXSTRUCTS_BINARYTREE_H
#  define CXSTRUCTS_BINARYTREE_H

#  include <algorithm>
#  include <deque>
#  include <iostream>
#  include <stdexcept>
#  include <vector>
#  include "../cxconfig.h"

namespace cxhelper {  // namespace to hide helper structs
/**
 * Helper struct for the BinaryTree
 * @tparam T type
 */
template <typename T>
struct TreeNode {
  TreeNode* left_;
  TreeNode* right_;
  T data_;
  inline explicit TreeNode(const T& val) : data_(val), left_(nullptr), right_(nullptr) {}

  inline TreeNode(const T& val, TreeNode* left, TreeNode* right)
      : data_(val), left_(left), right_(right) {}
};
}  // namespace cxhelper
namespace cxstructs {
using namespace cxhelper;
/** <h2> Binary Tree.</h2>
 *
 * A Binary Tree is a fundamental data structure that uses a hierarchical model. Each element, known as a node,
 * has at most two children: <b>left child</b> and <b>right child</b>.
 *
 * The tree starts from the <b>root node</b>, and any node with no children is called a <b>leaf node</b>.
 *<br><br>
 * It is most useful for data that can be organized hierarchically. Each node should be a type (T) that supports
 * comparison using the '\<' operator, to enable insertion and searching operations.
 * <br><br>
 * While accessing or searching for a specific element in a binary tree requires traversing potentially half the tree,
 * a balanced binary tree allows for faster lookup times of O(log N).
 */
template <typename T>
class BinaryTree {
  using TNode = TreeNode<T>;
  TNode* root_;
  uint_32_cx size_;

  inline int subTreeDepth(TNode* node) {
    if (!node) {
      return 0;
    } else {
      int left = subTreeDepth(node->left_);
      int right = subTreeDepth(node->right_);
      return std::max(left, right) + 1;
    }
  }
  inline void insert(const T& val, TNode* node) {
    if (val < node->data_) {
      if (!node->left_) {
        node->left_ = new TNode(val);
      } else insert(val, node->left_);
    } else {
      if (!node->right_) {
        node->right_ = new TNode(val);
      } else insert(val, node->right_);
    }
  }
  inline bool contains(const T& val, TNode* node) const {
    if (node) {
      if (val < node->data_) {
        if (node->left_) {
          return contains(val, node->left_);
        }
        return false;
      } else if (val > node->data_) {
        if (node->right_) {
          return contains(val, node->right_);
        }
        return false;
      } else {
        return true;
      }
    }
    return false;
  }
  inline TNode* erase(const T& val, TNode* node) {
    if (!node) {
      return node;
    }

    if (val < node->data_) {
      node->left_ = erase(val, node->left_);
    } else if (val > node->data_) {
      node->right_ = erase(val, node->right_);
    } else {
      if (!node->left_) {
        TNode* temp = node->right_;
        delete node;
        size_--;
        return temp;
      } else if (!node->right_) {
        TNode* temp = node->left_;
        delete node;
        size_--;
        return temp;
      }
      TNode* temp = minValueNode(node->right_);
      node->data_ = temp->data_;
      node->right_ = erase(temp->data_, node->right_);
      size_--;
    }
    return node;
  }
  inline TNode* minValueNode(TNode* node) {
    TNode* current = node;

    while (current && current->left_ != nullptr) {
      current = current->left_;
    }
    return current;
  }

 public:
  BinaryTree() : root_(nullptr), size_(0){};
  //no moving around yet
  BinaryTree(const BinaryTree&) = delete;
  BinaryTree& operator=(const BinaryTree&) = delete;
  BinaryTree(BinaryTree&&) = delete;
  BinaryTree& operator=(BinaryTree&&) = delete;
  ~BinaryTree() {
    std::deque<TNode*> nodesToDelete;
    nodesToDelete.push_back(root_);

    while (!nodesToDelete.empty()) {
      TNode* node = nodesToDelete.back();
      nodesToDelete.pop_back();

      if (node->left_) {
        nodesToDelete.push_back(node->left_);
      }
      if (node->right_) {
        nodesToDelete.push_back(node->right_);
      }

      delete node;
    }
  }
  /**
   * Gives access to the root node of the tree
   * @return the root node
   */
  [[nodiscard]] TNode* get_root() const { return root_; }

  /**
   * Inverts this BinaryTree starting from the given node
   */
  inline void invert(TNode* node) {
    if (node == nullptr) {
      return;
    }
    std::swap(node->left_, node->right_);
    invert(node->left_);
    invert(node->right_);
  }

  /**
   * Inverts this BinaryTree starting from the root
   */
  inline void invert() { invert(root_); }

  /**
   * Inserts the given element into the tree at the right position
   * @param val - the inserted value
   */
  inline void insert(const T& val) {
    if (root_) {
      insert(val, root_);
      size_++;
    } else {
      size_++;
      root_ = new TNode(val);
    }
  }

  /**
   * Recursively searches the tree for the given value
   * @param val - the value to search for
   * @return - true if the tree contained the given value, false otherwise
   */
  [[nodiscard]] inline bool contains(const T& val) const { return contains(val, root_); }

  /**
   * Erases the first occurrence of a node with this value
   * @param val - the node-value to search for
   * @return true if a deletion happened
   */
  inline bool erase(const T& val) {
    auto temp = size_;
    root_ = erase(val, root_);
    return (temp - 1 == size_);
  }
  /**
  @brief Removes all nodes from the binary tree<p>
   After the operation, the tree becomes empty and its size is 0
   **/
  inline void clear() {
    std::deque<TNode*> nodesToDelete;
    nodesToDelete.push_back(root_);

    while (!nodesToDelete.empty()) {
      TNode* node = nodesToDelete.back();
      nodesToDelete.pop_back();

      if (node->left_) {
        nodesToDelete.push_back(node->left_);
      }
      if (node->right_) {
        nodesToDelete.push_back(node->right_);
      }

      delete node;
    }
    size_ = 0;
    root_ = nullptr;
  }
  /**
   * @return the total amount of nodes in the tree
   */
  [[nodiscard]] inline uint_32_cx size() const { return size_; }
  /**
     * @brief Checks if the BinaryTree is empty.
     *
     * @return true if the BinaryTree is empty, false otherwise.
     */
  [[nodiscard]] inline bool empty() const { return size_ == 0; }
  /**
   *
   * @return the maximum depth of the tree
   */
  uint_32_cx maxDepth() { return subTreeDepth(root_); }

  class InOrderIterator {
    std::deque<TNode*> nodes;

   public:
    explicit InOrderIterator(TNode* root) { pushLeft(root); }

    void pushLeft(TNode* node) {
      while (node) {
        nodes.push_back(node);
        node = node->left_;
      }
    }

    T& operator*() { return nodes.back()->data_; }

    InOrderIterator& operator++() {
      TNode* node = nodes.back();
      nodes.pop_back();
      pushLeft(node->right_);
      return *this;
    }

    bool operator!=(const InOrderIterator& other) { return !nodes.empty(); }
  };
  /**
   * In order tree traversal
   * @return a in order tree traversal begin iterator
   */
  InOrderIterator begin() { return InOrderIterator(root_); }
  /**
   * In order tree traversal
* @return a in order tree traversal end iterator

   */
  InOrderIterator end() { return InOrderIterator(nullptr); }

#  ifdef CX_INCLUDE_TESTS
  static void TEST() {
    std::cout << "BINARY SEARCH TREE TESTS" << std::endl;

    // Test insert and contained
    std::cout << "  Testing insertion and contained method..." << std::endl;
    BinaryTree<int> bt1;
    bt1.insert(1);
    bt1.insert(2);
    CX_ASSERT(bt1.contains(1), "");

    CX_ASSERT(bt1.contains(2), "");

    // Test size
    std::cout << "  Testing size method..." << std::endl;
    CX_ASSERT(bt1.size() == 2, "");

    // Test erase
    std::cout << "  Testing erase method..." << std::endl;
    bt1.erase(1);
    CX_ASSERT(!bt1.contains(1), "");
    CX_ASSERT(bt1.size() == 1, "");

    // Test erase non-existing value
    std::cout << "  Testing erase non-existing value..." << std::endl;
    bool erased = bt1.erase(5);  // Should return false as 5 doesn't exist
    CX_ASSERT(erased == false, "");

    // Test newly created tree
    std::cout << "  Testing newly created tree..." << std::endl;
    BinaryTree<int> bt2;
    CX_ASSERT(bt2.empty(), "");
    CX_ASSERT(bt2.size() == 0, "");

    // Test insert and contained
    std::cout << "  Testing insertion and contained method..." << std::endl;
    bt2.insert(10);
    CX_ASSERT(bt2.contains(10), "");
    CX_ASSERT(!bt2.empty(), "");
    CX_ASSERT(bt2.size() == 1, "");

    // Test insert multiple elements
    std::cout << "  Testing insertion of multiple elements..." << std::endl;
    bt2.insert(15);
    CX_ASSERT(bt2.contains(15), "");
    bt2.insert(5);
    CX_ASSERT(bt2.contains(5), "");
    CX_ASSERT(!bt2.contains(4), "");
    CX_ASSERT(bt2.size() == 3, "");

    // Test maxDepth method
    std::cout << "  Testing maxDepth method..." << std::endl;
    CX_ASSERT(bt2.maxDepth() == 2, "");  // Check if depth is correct after insertions
    // Test invert method
    std::cout << "  Testing invert method..." << std::endl;
    std::vector<int> normalTraversal, invertedTraversal;
    for (auto it = bt2.begin(); it != bt2.end(); ++it) {
      normalTraversal.push_back(*it);
    }

    bt2.invert();

    for (auto it = bt2.begin(); it != bt2.end(); ++it) {
      invertedTraversal.push_back(*it);
    }
    CX_ASSERT(normalTraversal != invertedTraversal, "");

    std::cout << "  Testing sorted traversal..." << std::endl;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(0, 100);
    bt2.clear();
    for (uint_fast32_t i = 0; i < 100; i++) {
      bt2.insert(distr(gen));
    }
    int prev_num = 0;
    for (const auto& num : bt2) {
      CX_ASSERT(num >= prev_num, "sort check");
      prev_num = num;
    }
  }
#  endif
};
}  // namespace cxstructs
#endif  // CXSTRUCTS_BINARYTREE_H
