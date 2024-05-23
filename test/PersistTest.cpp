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

TEST_CASE("Test correct file creation and count", "[Persist]") {
  TestUtil::SetupCWD();
  auto* testPath = "./res/__GEN1__.rn";
  auto ec = TestUtil::getBasicContext();
  constexpr int testSize = 10;

  for (int i = 0; i < testSize; ++i) {
    ec.core.createAddNode(ec, "Text", {0, 0});
  }
  REQUIRE(ec.core.nodes.size() == testSize);

  ec.persist.openedFilePath = testPath;
  ec.core.hasUnsavedChanges = true;
  ec.persist.saveProject(ec);

  // Reset state
  ec.core.resetEditor(ec);

  REQUIRE(std::filesystem::exists(testPath) == true);

  ec.persist.importProject(ec);
  REQUIRE(ec.core.nodes.size() == testSize);
  REQUIRE(std::filesystem::exists(testPath) == true);
}

TEST_CASE("Teset correct data persistence", "[Persist]") {
  TestUtil::SetupCWD();
  auto* testPath = "./res/__GEN1__.rn";
  auto ec = TestUtil::getBasicContext();
  ec.persist.openedFilePath = testPath;

  constexpr int testInt = 5;
  constexpr const char* testString = "Test";
  constexpr const char* testFloat = "3.141592";

  // Create nodes
  {
    ec.core.createAddNode(ec, "Text", {})->getComponent<TextFieldC<>>("Text")->textField.buffer = testString;
    ec.core.createAddNode(ec, "Int", {})->getComponent<MathC>("Int")->selectedMode = testInt;
    auto vec2 = ec.core.createAddNode(ec, "Vec2", {})->getComponent<Vec2C<>>("Vec2")->textFields;
    vec2[0].buffer = testFloat, vec2[1].buffer = testFloat;
    auto vec3 = ec.core.createAddNode(ec, "Vec3", {})->getComponent<Vec3C<>>("Vec3")->textFields;
    vec3[0].buffer = testFloat, vec3[1].buffer = testFloat, vec3[2].buffer = testFloat;
  }

  // Persist cycle
  ec.core.hasUnsavedChanges = true;
  ec.persist.saveProject(ec);

  // Reset state
  ec.core.resetEditor(ec);

  ec.persist.importProject(ec);

  // Test values
  {
    REQUIRE(ec.core.getNode(NodeID(0))->getComponent<TextFieldC<>>("Text")->textField.buffer == testString);
    REQUIRE(ec.core.getNode(NodeID(1))->getComponent<MathC>("Int")->selectedMode == testInt);
    auto vec2 = ec.core.getNode(NodeID(2))->getComponent<Vec2C<>>("Vec2")->textFields;
    REQUIRE(vec2[0].buffer == testFloat);
    REQUIRE(vec2[1].buffer == testFloat);
    auto vec3 = ec.core.getNode(NodeID(3))->getComponent<Vec3C<>>("Vec3")->textFields;
    REQUIRE(vec3[0].buffer == testFloat);
    REQUIRE(vec3[1].buffer == testFloat);
    REQUIRE(vec3[2].buffer == testFloat);
  }
}

TEST_CASE("Benchmark saving and loading", "[Persist]") {
  TestUtil::SetupCWD();
  auto* testPath = "./res/__GEN2__.rn";
  auto ec = TestUtil::getBasicContext();

  constexpr int testSize = 1000;

  for (int i = 0; i < testSize; ++i) {
    ec.core.createAddNode(ec, "Text", {0, 0});
  }

  REQUIRE(ec.core.nodes.size() == testSize);

  ec.persist.openedFilePath = testPath;
  ec.core.hasUnsavedChanges = true;

  BENCHMARK("Save") {
    return ec.persist.saveProject(ec);
  };

  BENCHMARK("Load") {
    return ec.persist.importProject(ec);
  };

  REQUIRE(ec.core.nodes.size() == testSize);
}