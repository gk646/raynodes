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

#include "RnImport.h"
#include "TestUtil.h"

TEST_CASE("Test getComponentData", "[Import]") {
  TestUtil::SetupCWD();
  auto rn = raynodes::importRN("res/Test1.rn");

  REQUIRE(rn.connCnt == 4);
  REQUIRE(rn.nodeCnt == 6);

  // String data test
  {
    auto str = rn.getComponentData<raynodes::STRING>(1, "TextField");
    auto str1 = rn.getComponentData<raynodes::STRING>(1, 0);
    REQUIRE(str == "Test2");
    REQUIRE(str == str1);

    // String Aggregate test
    str = rn.getComponentData<raynodes::STRING>(5, "DisplayText");
    REQUIRE(str == "Display");
    str1 = rn.getComponentData<raynodes::STRING>(5, "Choice2");
    REQUIRE(str1 == "C2");
    auto str2 = rn.getComponentData<raynodes::STRING>(5, "Choice4");
    REQUIRE(str2 == "C4");

    auto str3 = rn.getComponentData<raynodes::STRING>(5, 1);
    REQUIRE(str == str3);
    auto str4 = rn.getComponentData<raynodes::STRING>(5, 3);
    REQUIRE(str1 == str4);
    auto str5 = rn.getComponentData<raynodes::STRING>(5, 5);
    REQUIRE(str2 == str5);
  }
  // Bool test

  // Integer test
  {
    auto val = rn.getComponentData<raynodes::INTEGER>(4, "Operation");
    REQUIRE(val == 1);
  }

  {
    // Float test
    auto val = rn.getComponentData<raynodes::FLOAT>(2, "Number");
    REQUIRE(val == 2.33);
  }

  // Vec3
  /**
   Vec3 test{1.1, 1.22, 1.333};
   auto val = rn.getComponentData<raynodes::VECTOR_3>(3, "Vector3");
   REQUIRE(val.x == test.x);
   REQUIRE(val.y == test.y);
   REQUIRE(val.z == test.z);
   */
}

TEST_CASE("Test getConnectionOut", "[Import]") {
  TestUtil::SetupCWD();
  auto rn = raynodes::importRN("res/Test1.rn");
  {
    auto conns = rn.getConnectionsOut<4>(1, -1);
    const auto conn = conns[0];
    REQUIRE(conn.isValid() == true);
    REQUIRE(conn.fromNode == 1);
    REQUIRE(conn.fromComponent == -1);  // node-to-node connection
    REQUIRE(conn.fromPin == 0);
    REQUIRE(conn.toNode == 5);
    REQUIRE(conn.toComponent == -1);  // node-to-node connection
    REQUIRE(conn.toPin == 0);

    // Correct invalidation
    for (int i = 1; i < 4; ++i) {
      REQUIRE(conns[i].isValid() == false);
    }
  }
  // Test vector
  {
    auto conns = rn.getConnectionsOut(1, -1);
    const auto conn = conns[0];
    REQUIRE(conn.isValid() == true);
    REQUIRE(conn.fromNode == 1);
    REQUIRE(conn.fromComponent == -1);  // node-to-node connection
    REQUIRE(conn.fromPin == 0);
    REQUIRE(conn.toNode == 5);
    REQUIRE(conn.toComponent == -1);  // node-to-node connection
    REQUIRE(conn.toPin == 0);

    for (const auto c : conns) {
      REQUIRE(c.isValid() == true);
    }
  }
  // 2 nodes connection into the same input
  {
    auto conns = rn.getConnectionsIn(5, -1);
    REQUIRE(conns.size() == 2);
  }
  // No connections
  REQUIRE(rn.getConnectionsOut(2, -1).empty());
  REQUIRE(rn.getConnectionsOut<1>(2, -1)[0].isValid() == false);
}
TEST_CASE("Test getConnectionIn", "[Import]") {
  TestUtil::SetupCWD();
  auto rn = raynodes::importRN("res/Test1.rn");

  // Test multiple
  {
    auto conns = rn.getConnectionsIn<4>(1, -1);
    const auto conn = conns[0];
    REQUIRE(conn.isValid() == true);
    REQUIRE(conn.fromNode == 0);
    REQUIRE(conn.fromComponent == -1);  // node-to-node connection
    REQUIRE(conn.fromPin == 0);
    REQUIRE(conn.toNode == 1);
    REQUIRE(conn.toComponent == -1);  // node-to-node connection
    REQUIRE(conn.toPin == 0);

    for (int i = 1; i < 4; ++i) {
      REQUIRE(conns[i].isValid() == false);
    }
  }
  // Test multiple vector
  {
    auto conns = rn.getConnectionsIn(1, -1);
    const auto conn = conns[0];
    REQUIRE(conn.isValid() == true);
    REQUIRE(conn.fromNode == 0);
    REQUIRE(conn.fromComponent == -1);  // node-to-node connection
    REQUIRE(conn.fromPin == 0);
    REQUIRE(conn.toNode == 1);
    REQUIRE(conn.toComponent == -1);  // node-to-node connection
    REQUIRE(conn.toPin == 0);

    for (const auto c : conns) {
      REQUIRE(c.isValid() == true);
    }
  }

  // No connections
  REQUIRE(rn.getConnectionsIn(2, -1).empty());
  REQUIRE(rn.getConnectionsIn<1>(2, -1)[0].isValid() == false);
}
TEST_CASE("Test getNodes", "[Import]") {
  TestUtil::SetupCWD();
  auto rn = raynodes::importRN("res/Test1.rn");
  // Test array
  {
    auto nodes = rn.getNodes<4>("TextField");
    REQUIRE(nodes[0] == 0);
    REQUIRE(nodes[1] == 1);
    REQUIRE(nodes[2] == UINT16_MAX);
    REQUIRE(nodes[3] == UINT16_MAX);
  }
  // Test multiple vector
  {
    auto nodes = rn.getNodes("TextField");
    REQUIRE(nodes.size() == 2);
    REQUIRE(nodes[0] == 0);
    REQUIRE(nodes[1] == 1);
  }
}

TEST_CASE("Benchmark the import", "[Import]") {
  TestUtil::SetupCWD();
  auto* path = "./res/__GEN2__.rn";
  BENCHMARK("Import") {
    auto rn = raynodes::importRN(path);
  };
}