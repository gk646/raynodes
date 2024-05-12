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
#ifndef CXSTRUCTS_SRC_CXSTRUCTS_HASHGRID_H_
#  define CXSTRUCTS_SRC_CXSTRUCTS_HASHGRID_H_

#  include <unordered_map>
#  include <vector>
#  include "../cxconfig.h"

namespace cxstructs {

/**
 * HashGrid for a square space
 * A HashGrid divides the space into cells of uniform size, and entities are assigned to these cells based on their spatial coordinates. <br>
 * This spatial partitioning allows for efficient querying and management of entities based on their locations.
 *
 */
template <typename EntityID = uint32_t>
struct HashGrid {
  using size_type = uint_32_cx;
  using GridID = size_type;

 private:
  float cellSize = 0;
  float totalSpaceSize = 0;
  size_type gridSize = 0;
  std::unordered_map<GridID, std::vector<EntityID>> cells;

 public:
  explicit HashGrid(float cellSize, float spaceSize)
      : cellSize(cellSize), totalSpaceSize(spaceSize),
        gridSize(static_cast<size_type>(spaceSize / cellSize)) {
    cells.reserve(gridSize * gridSize);
    cells.max_load_factor(1.0F);
  };
  HashGrid() = default;
  HashGrid(const HashGrid&) = delete;
  HashGrid& operator=(const HashGrid&) = delete;
  HashGrid(HashGrid&&) = delete;
  HashGrid& operator=(HashGrid&& other) noexcept {
    if (this == &other) {
      return *this;
    }

    cells = std::move(other.cells);
    cellSize = other.cellSize;
    totalSpaceSize = other.totalSpaceSize;
    gridSize = other.gridSize;

    other.cellSize = 0;
    other.totalSpaceSize = 0;
    other.gridSize = 0;

    return *this;
  }
  inline const std::vector<EntityID>& operator[](GridID g) noexcept { return cells[g]; }
  [[nodiscard]] inline GridID getGridID(float x, float y) const noexcept {
    return static_cast<int>(x / cellSize) + static_cast<int>(y / cellSize) * gridSize;
  }
  inline void getGridIDs(int32_t (&gridIDs)[4], float x, float y, float width,
                         float height) const noexcept {
    int topLeftGridX = static_cast<int>(x / cellSize);
    int topLeftGridY = static_cast<int>(y / cellSize);
    int bottomRightGridX = static_cast<int>((x + width) / cellSize);
    int bottomRightGridY = static_cast<int>((y + height) / cellSize);

    std::fill(std::begin(gridIDs), std::end(gridIDs), -1);

    gridIDs[0] = topLeftGridX + topLeftGridY * gridSize;
    gridIDs[1] = (topLeftGridX != bottomRightGridX) ? bottomRightGridX + topLeftGridY * gridSize
                                                    : -1;  // Top-right
    gridIDs[2] = (topLeftGridY != bottomRightGridY) ? topLeftGridX + bottomRightGridY * gridSize
                                                    : -1;  // Bottom-left
    gridIDs[3] = (topLeftGridX != bottomRightGridX && topLeftGridY != bottomRightGridY)
                     ? bottomRightGridX + bottomRightGridY * gridSize
                     : -1;  // Bottom-right
  }
  inline void clear() {
    for (auto& pair : cells) {
      pair.second.clear();
    }
  };
  inline void setupNew(float newCellSize, uint_16_cx newSpaceSize, bool optimized = true) {
    if (optimized) {
      float value = cellSize / totalSpaceSize;
      cellSize = newSpaceSize * value;
      cells.reserve(std::pow(totalSpaceSize / cellSize, 2));
    } else {
      cellSize = newCellSize;
      totalSpaceSize = newSpaceSize;
    }
    cells.clear();
  }
  inline void insert(float x, float y, EntityID entityID) {
    CX_ASSERT(x < totalSpaceSize && y < totalSpaceSize, "x or y is larger than spaceSize");
    cells[getGridID(x, y)].emplace_back(entityID);
  }

#  ifdef CX_INCLUDE_TESTS
  static void TEST() {
    float spaceSize = 100;
    float cellSize = 5;

    HashGrid hashGrid{cellSize, spaceSize};

    for (uint_fast32_t i = 0; i < spaceSize; i++) {
      for (uint_fast32_t j = 0; j < spaceSize; j++) {
        hashGrid.insert(i, j, i);
      }
    }

    hashGrid.insert(99, 99, 1);
    hashGrid.insert(1, 1, 2);

    std::vector<uint32_t> coll;
    coll.reserve(60);

    now();
    hashGrid.containedInRectCollect(0, 0, 99, 99, coll);
    printTime<std::chrono::nanoseconds>("lookup");
    std::cout << coll.size() << std::endl;
  }
#  endif
};

}  // namespace cxstructs

#endif  //CXSTRUCTS_SRC_CXSTRUCTS_HASHGRID_H_
