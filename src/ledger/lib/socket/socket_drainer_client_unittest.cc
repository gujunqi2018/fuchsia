// Copyright 2017 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/ledger/lib/socket/socket_drainer_client.h"

#include "src/ledger/lib/loop_fixture/test_loop_fixture.h"
#include "src/ledger/lib/socket/socket_pair.h"

namespace socket {
namespace {

using SocketDrainerClientTest = ledger::TestLoopFixture;

// Regression test for LE-229.
TEST_F(SocketDrainerClientTest, DoNotCallOnDelete) {
  socket::SocketPair socket;
  socket.socket1.reset();

  auto drainer = std::make_unique<SocketDrainerClient>();
  bool called = false;
  drainer->SetOnDiscardable([&called] { called = true; });
  drainer->Start(std::move(socket.socket2), [&drainer](const std::string& v) { drainer.reset(); });
  RunLoopUntilIdle();
  EXPECT_FALSE(called);
}

}  // namespace
}  // namespace socket
