// Copyright 2017 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "asm.h"

// The memset implementation is verbatim from cortex-strings.
// But we also need to define aliases for the sanitizer runtime.

#include "third_party/lib/cortex-strings/src/aarch64/memset.S"

#ifdef HIDDEN
    .hidden memset
#else
ALIAS(memset, __unsanitized_memset)
ASAN_WEAK_ALIAS(memset)

// This name is called from inside libc to avoid going through the PLT.
.hidden __libc_memset
ALIAS(memset, __libc_memset)
#endif
