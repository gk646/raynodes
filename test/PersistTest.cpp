// Copyright (c) 2024 gk646
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

#include <catch_amalgamated.hpp>
#include <filesystem>

#include "TestUtil.h"

TEST_CASE("Test correct saving and loading", "PersistTest1") {
  TestUtil::SetupCWD();
  auto* testPath = "./res/__GEN1__.rn";
  auto ec = TestUtil::getBasicContext();
  constexpr int testSize = 10;

  for (int i = 0; i < testSize; ++i) {
    auto node = ec.core.createNode(ec, "DummyN", {0, 0});
    auto* comp = node->getComponent<TextOutputC>("TextField");
  }

  REQUIRE(ec.core.nodes.size() == testSize);

  ec.persist.openedFilePath = testPath;
  ec.core.hasUnsavedChanges = true;
  ec.persist.saveToFile(ec);

  REQUIRE(ec.core.nodes.size() == testSize);
  REQUIRE(std::filesystem::exists(testPath) == true);

  ec.persist.loadFromFile(ec);

  REQUIRE(ec.core.nodes.size() == testSize);
  REQUIRE(std::filesystem::exists(testPath) == true);
}

TEST_CASE("Benchmark saving and loading", "PersistBench1") {
  TestUtil::SetupCWD();
  auto* testPath = "./res/__GEN2__.rn";
  auto ec = TestUtil::getBasicContext();

  constexpr int testSize = 1000;

  for (int i = 0; i < testSize; ++i) {
    ec.core.createNode(ec, "DummyN", {0, 0});
  }

  REQUIRE(ec.core.nodes.size() == testSize);

  ec.persist.openedFilePath = testPath;
  ec.core.hasUnsavedChanges = true;

  BENCHMARK("Save") {
    return ec.persist.saveToFile(ec);
  };

  BENCHMARK("Load") {
    return ec.persist.loadFromFile(ec);
  };

  REQUIRE(ec.core.nodes.size() == testSize);
}