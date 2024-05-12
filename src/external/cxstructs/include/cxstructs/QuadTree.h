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
#ifndef CXSTRUCTS_SRC_DATASTRUCTURES_QUADTREE_H_
#  define CXSTRUCTS_SRC_DATASTRUCTURES_QUADTREE_H_

#  include "../cxconfig.h"
#  include "Geometry.h"
#  include "vec.h"
#  include <utility>

//used in kNN 2D

namespace cxstructs {

/**
 * <h2>QuadTree</h2>
 * is a tree data structure in which each internal node has exactly four children: north-west, north-east, south-west and south-east.<p>
 * QuadTrees are most often used to partition a two-dimensional space by recursively subdividing it into four quadrants or regions.
 *<p>
 * You can then insert any custom Type with x() and y() position getters and they will be placed into the right partition.
 * You can then for example retrieve all element in a subrectangle to quickly do operations on them.
 */
template <typename T>
class QuadTree {

  uint_16_cx max_depth_;
  uint_16_cx max_points_;
  // rectangle bounds
  Rect bounds_;

  // the points contained in this QuadTree
  vec<T> vec_;

  // the 4 children
  QuadTree* top_left_;
  QuadTree* top_right_;
  QuadTree* bottom_left_;
  QuadTree* bottom_right_;
  /**
     * @brief Subdivides the QuadTree into four smaller QuadTrees and distributing elements
     */
  inline void split() noexcept {
    const auto half_width = bounds_.width() / 2;
    auto half_height = bounds_.height() / 2;
    top_left_ = new QuadTree({bounds_.x(), bounds_.y(), half_width, half_height}, max_depth_ - 1,
                             max_points_);
    top_right_ = new QuadTree({bounds_.x() + half_width, bounds_.y(), half_width, half_height},
                              max_depth_ - 1, max_points_);
    bottom_left_ = new QuadTree({bounds_.x(), bounds_.y() + half_height, half_width, half_height},
                                max_depth_ - 1, max_points_);
    bottom_right_ =
        new QuadTree({bounds_.x() + half_width, bounds_.y() + half_height, half_width, half_height},
                     max_depth_ - 1, max_points_);

    for (const auto& e : vec_) {
      insert_subtrees(e);
    }
    vec_.clear();
  };
  inline void insert_subtrees(const T& e) const noexcept {
    if (e.x() > bounds_.x() + bounds_.width() / 2) {
      if (e.y() > bounds_.y() + bounds_.height() / 2) {
        bottom_right_->insert(e);
      } else {
        top_right_->insert(e);
      }
    } else {
      if (e.y() > bounds_.y() + bounds_.height() / 2) {
        bottom_left_->insert(e);
      } else {
        top_left_->insert(e);
      }
    }
  }
  inline void size_subtrees(uint_32_cx& current) const noexcept {
    current += vec_.size();
    if (top_right_) {
      top_right_->size_subtrees(current);
      top_left_->size_subtrees(current);
      bottom_right_->size_subtrees(current);
      bottom_left_->size_subtrees(current);
    }
  }
  inline void count_subrect_subtrees(const Rect& bound, uint_32_cx& count) noexcept {
    if (bound.intersects(bounds_)) {
      for (const auto& point : vec_) {
        if (bound.contains(point)) {
          count++;
        }
      }
      if (top_right_) {
        top_right_->count_subrect_subtrees(bound, count);
        top_left_->count_subrect_subtrees(bound, count);
        bottom_left_->count_subrect_subtrees(bound, count);
        bottom_right_->count_subrect_subtrees(bound, count);
      }
    }
  }
  inline void accumulate_subrect_subtrees(const Rect& bound, vec<T*>& list) const noexcept {
    if (bound.intersects(bounds_)) {
      auto arr = vec_.get_raw();
      for (uint_fast32_t i = 0; i < vec_.size(); i++) {
        if (bound.contains(arr[i])) {
          list.push_back(&arr[i]);
        }
      }
      if (top_right_) {
        top_right_->accumulate_subrect_subtrees(bound, list);
        top_left_->accumulate_subrect_subtrees(bound, list);
        bottom_left_->accumulate_subrect_subtrees(bound, list);
        bottom_right_->accumulate_subrect_subtrees(bound, list);
      }
    }
  }
  inline void erase_point(const T& e) const noexcept {
    if (e.x() > bounds_.x() + bounds_.width() / 2) {
      if (e.y() > bounds_.y() + bounds_.height() / 2) {
        bottom_right_->erase(e);
      } else {
        top_right_->erase(e);
      }
    } else {
      if (e.y() > bounds_.y() + bounds_.height() / 2) {
        bottom_left_->erase(e);
      } else {
        top_left_->erase(e);
      }
    }
  }

 public:
  /*no default constructor to prevent modifications and subtree mismatch
   * Setting new bound invalidates the lower layers!
   * Also, no moving(copying) the quad tree yet
  */
  /**
     * @brief Constructs a new QuadTree object.
     *
     *When the max_points is reached the given quad tree will split up distributing all its element to the
     * correct subtrees. <p>
     * This can happen a total of max_depth amount of times. This balancing needs to be adjusted to the use case.
     *
     * @param initial_bounds A Rect object that defines the boundary of this QuadTree.
     * @param max_depth maximum depth of the whole tree
     * @param max_points maximum amount of points per each node
     */
  explicit QuadTree(Rect initial_bounds, uint_16_cx max_depth = 10, uint_32_cx max_points = 50)
      : max_depth_(max_depth), max_points_(max_points), bounds_(std::move(initial_bounds)),
        top_left_(nullptr), top_right_(nullptr), bottom_left_(nullptr), bottom_right_(nullptr){};
  QuadTree(const QuadTree&) = delete;
  QuadTree& operator=(const QuadTree&) = delete;
  QuadTree(QuadTree&&) = delete;
  QuadTree& operator=(QuadTree&&) = delete;
  ~QuadTree() {
    if (top_right_) {
      delete top_right_;
      top_right_ = nullptr;
    }
    if (top_left_) {
      delete top_left_;
      top_left_ = nullptr;
    }
    if (bottom_right_) {
      delete bottom_right_;
      bottom_right_ = nullptr;
    }
    if (bottom_left_) {
      delete bottom_left_;
      bottom_left_ = nullptr;
    }
  }
  /**
     * @brief Inserts a element into the QuadTree.
     *
     * @param p The element to be inserted.
     */
  inline void insert(const T& e) {
    if (!bounds_.contains(e)) {
      return;
    }
    if (!top_right_) {
      if (vec_.size() < max_points_) {
        vec_.push_back(e);
        return;
      } else {
        if (max_depth_ > 0) {
          split();
        } else {
          vec_.push_back(e);
          CX_WARNING(false, "|QuadTree.h| Reached max depth | large insertions now will slow "
                            "down the tree");
          return;
        }
      }
    }

    insert_subtrees(e);
  }

  /**
   * Number of points contained in the given rectangle bound
   * @param bound the rectangle to search in
   * @return number of points
   */
  inline uint_32_cx count_subrect(const Rect& bound) {
    uint_32_cx count = 0;
    count_subrect_subtrees(bound, count);
    return count;
  }
  /**
   * Retrieves all elements that are contained in the given bound as a iterable list of pointers
   * @param bound the rectangle to search in
   * @return a list of pointers to the objects
   */
  inline vec<T*> get_subrect(const Rect& bound) {
    vec<T*> retval;
    accumulate_subrect_subtrees(bound, retval);
    return retval;
  }
  /**
   * Removes the first occurence of that object from the quadtree<p>
   * Uses operator== to check for equality
   * @param e the element to erase
   */
  inline void erase(const T& e) {
    if (!top_right_) {
      vec_.erase(e);
    } else {
      erase_point(e);
    }
  }
  /**
   * Clears the QuadTree of all elements including its own subtrees
   */
  inline void clear() {
    if (top_right_) {
      delete top_right_;
      top_right_ = nullptr;
    }
    if (top_left_) {
      delete top_left_;
      top_left_ = nullptr;
    }
    if (bottom_right_) {
      delete bottom_right_;
      bottom_right_ = nullptr;
    }
    if (bottom_left_) {
      delete bottom_left_;
      bottom_left_ = nullptr;
    }

    vec_.clear();
  };
  /**
   * Actively iterates down the tree summing all subtree sizes<p>
   * <b>This can bits_get slow on large trees </b>
   * @return the max depth of the tree
   */
  [[nodiscard]] inline uint_32_cx size() const {
    uint_32_cx size = 0;
    size_subtrees(size);
    return size;
  }
  /**
   * Actively iterates down the tree for its max depth<p>
   * <b>This can bits_get slow on large trees </b>
   * @return the max depth of the tree
   */
  [[nodiscard]] inline uint_16_cx depth() const {
    auto tree = this->top_right_;
    uint_16_cx depth = 0;
    while (tree) {
      tree = tree->top_right_;
      depth++;
    }
    return depth;
  }
  /**
   * Sets a new bound for the root tree.
   *
   * <b>DOES NOT</b> recalculate the bound of already existing subtrees
   * @param new_bound
   */
  inline void set_bounds(const Rect& new_bound) noexcept { bounds_ = new_bound; }
  [[nodiscard]] inline const Rect& get_bounds() const noexcept { return bounds_; }
  class Iterator {};
#  ifdef CX_INCLUDE_TESTS
  static void TEST() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> distr(0, 200);
    std::cout << "TESTING QUAD TREE" << std::endl;

    std::cout << "   Testing insert..." << std::endl;
    QuadTree<Point> tree({0, 0, 200, 200});
    for (uint_fast32_t i = 0; i < 1000; i++) {
      tree.insert({distr(gen), distr(gen)});
    }

    CX_ASSERT(tree.depth() == 3, "");
    CX_ASSERT(tree.size() == 1000, "");

    std::cout << "   Testing count subrect..." << std::endl;
    CX_ASSERT(tree.count_subrect({0, 0, 200, 200}) == 1000, "");
    CX_ASSERT(tree.count_subrect({10, 10, 50, 50}) > 10, "");
    tree.insert({2, 2});
    CX_ASSERT(tree.size() == 1001, "");
    tree.erase({2, 2});
    CX_ASSERT(tree.size() == 1000, "");

    std::cout << "   Testing max capacity..." << std::endl;
    QuadTree<Point> tree1({0, 0, 200, 200}, 2, 10);
    for (uint_fast32_t i = 0; i < 100000; i++) {
      tree.insert({distr(gen), distr(gen)});
    }

    std::cout << "   Testing object retrieval..." << std::endl;
    tree1.clear();
    tree1.insert({2, 2});
    for (auto ptr : tree1.get_subrect({0, 0, 2, 2})) {
      CX_ASSERT(*ptr == Point(2, 2), "");
    }
  }
#  endif
};
}  // namespace cxstructs
#endif  //CXSTRUCTS_SRC_DATASTRUCTURES_QUADTREE_H_
