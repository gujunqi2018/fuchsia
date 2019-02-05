// Copyright 2019 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GARNET_LIB_RUST_WLAN_MLME_C_BINDINGS_H_
#define GARNET_LIB_RUST_WLAN_MLME_C_BINDINGS_H_

// Warning:
// This file was autogenerated by cbindgen.
// Do not modify this file manually.

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/**
 * An input buffer will always be returned to its original owner when no longer
 * being used. An input buffer is used for every buffer handed from C++ to Rust.
 */
typedef struct {
  /**
   * Returns the buffer's ownership and free it.
   */
  void (*free_buffer)(void *raw);
  /**
   * Pointer to the buffer's underlying data structure.
   */
  void *raw;
  /**
   * Pointer to the start of the buffer's data portion and its length.
   */
  uint8_t *data;
  uintptr_t len;
} rust_mlme_in_buf_t;

typedef struct {
  /**
   * Acquire a `InBuf` with a given minimum length from the provider.
   * The provider must release the underlying buffer's ownership and transfer it
   * to this crate. The buffer will be returned via the `free_buffer` callback
   * when it's no longer used.
   */
  rust_mlme_in_buf_t (*take_buffer)(uintptr_t min_len);
} rust_mlme_buffer_provider_ops_t;

/**
 * An output buffer requires its owner to manage the underlying buffer's memory
 * themselves. An output buffer is used for every buffer handed from Rust to
 * C++.
 */
typedef struct {
  /**
   * Pointer to the buffer's underlying data structure.
   */
  void *raw;
  /**
   * Pointer to the start of the buffer's data portion and the amount of bytes
   * written.
   */
  uint8_t *data;
  uintptr_t written_bytes;
} rust_mlme_out_buf_t;

extern "C" int32_t rust_mlme_is_valid_open_auth_resp(rust_mlme_in_buf_t buf,
                                                     bool has_body_aligned);

#endif /* GARNET_LIB_RUST_WLAN_MLME_ABI_H_ */
