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

#include "import/RnImport.h"
#include "TestUtil.h"

TEST_CASE("Test a simple import of a rn file") {
  TestUtil::SetupCWD();
  auto rn = raynodes::importRN("res/Test1.rn");

  REQUIRE(rn.connCnt == 1);
  REQUIRE(rn.nodeCnt == 2);
  REQUIRE(rn.templateCnt == 10);

  REQUIRE(rn.connections[0].fromNode == 0);
  REQUIRE(rn.connections[0].fromComponent == -1);
  REQUIRE(rn.connections[0].fromPin == 0);

  // String data test
  auto str = rn.getComponentData<raynodes::STRING>(0, "TextField");
  auto str1 = rn.getComponentData<raynodes::STRING>(0, 0);
  REQUIRE(str == "Test1");
  REQUIRE(str == str1);

  str = rn.getComponentData<raynodes::STRING>(1, "TextField");
  str1 = rn.getComponentData<raynodes::STRING>(1, 0);
  REQUIRE(str == "Test2");
  REQUIRE(str == str1);

  // Bool test

  // Connection test

  //Test single
  {
    auto conn = rn.getConnection(0, -1);

    REQUIRE(conn.isValid() == true);
    REQUIRE(conn.fromNode == 0);
    REQUIRE(conn.fromComponent == -1);  // node-to-node connection
    REQUIRE(conn.fromPin == 0);
    REQUIRE(conn.toNode == 1);
    REQUIRE(conn.toComponent == -1);  // node-to-node connection
    REQUIRE(conn.toPin == 0);
  }

  // Test multiple
  {
    auto conns = rn.getConnections<4>(0, -1);
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
}

TEST_CASE("Benchmark the import") {
  TestUtil::SetupCWD();
  auto* path = "./res/__GEN2__.rn";
  BENCHMARK("Import") {
    auto rn = raynodes::importRN(path);
  };
}