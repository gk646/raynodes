#include <catch_amalgamated.hpp>

#include "TestUtil.h"

// This tests correct deletion of action
// Preventing double deletions
TEST_CASE("Fuzzy Test", "[Actions]") {
  auto ec = TestUtil::getBasicContext();

  auto copyNode = [](EditorContext& ec) {
    ec.core.copy(ec);
  };

  auto deleteNode = [](EditorContext& ec) {
    ec.core.erase(ec);
  };

  auto pasteNode = [](EditorContext& ec) {
    ec.core.paste(ec);
  };

  auto createNode = [](EditorContext& ec) {
    ec.core.createNode(ec, "Vec3", {});
  };

  const std::vector<EditorFunction> funcs{createNode, createNode, copyNode, deleteNode, pasteNode};

  // Select a random node
  TestUtil::fuzzTest(ec, funcs, 1000, [](EditorContext& ec) {
    ec.core.selectedNodes.clear();
    int size = ec.core.nodes.size();
    if (size == 0) return;
    srand(time(nullptr));
    for (int i = 0; i < 5; ++i) {
      int num = rand() % size;
      auto* chosen = ec.core.nodes[num];
      ec.core.selectedNodes.insert({chosen->uID, chosen});
    }
    printf("%d\n", size);
  });

  // Deletes all remaining actions
  ec.core.resetEditor(ec);

  REQUIRE(ec.core.actionQueue.size() == 1);  // New canvas actions
}