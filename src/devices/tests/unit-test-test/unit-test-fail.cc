// Copyright 2019 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdio.h>

#include <ddk/binding.h>
#include <ddk/device.h>
#include <ddk/driver.h>
#include <ddk/platform-defs.h>

static zx_device_t* dev = NULL;

static void unbind(void* ctx) { device_unbind_reply(dev); }

static constexpr zx_protocol_device_t dev_ops = []() {
  zx_protocol_device_t ops = {};
  ops.version = DEVICE_OPS_VERSION;
  ops.unbind = unbind;
  return ops;
}();

static zx_status_t bind(void* ctx, zx_device_t* parent) {
  device_add_args_t args = {};
  args.version = DEVICE_ADD_ARGS_VERSION;
  args.name = "unit-test-fail";
  args.ops = &dev_ops;
  args.flags = DEVICE_ADD_NON_BINDABLE;

  zx_status_t status = device_add(parent, &args, &dev);
  if (status != ZX_OK) {
  }
  return status;
}

static bool run_unit_tests(void* ctx, zx_device_t* parent, zx_handle_t channel) { return false; }

static constexpr zx_driver_ops_t driver_ops = []() -> zx_driver_ops_t {
  zx_driver_ops_t ops = {};
  ops.version = DRIVER_OPS_VERSION;
  ops.bind = bind;
  ops.run_unit_tests = run_unit_tests;
  return ops;
}();

// clang-format off
ZIRCON_DRIVER_BEGIN(unit_test_fail, driver_ops, "zircon", "0.1", 2)
    BI_ABORT_IF_AUTOBIND,
    BI_MATCH_IF(EQ, BIND_PROTOCOL, ZX_PROTOCOL_TEST),
ZIRCON_DRIVER_END(unit_test_fail)
    // clang-format on
