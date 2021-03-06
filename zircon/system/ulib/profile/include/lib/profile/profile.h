// Copyright 2019 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <lib/svc/service.h>
#include <zircon/compiler.h>

__BEGIN_CDECLS

const zx_service_provider_t* profile_get_service_provider(void);

__END_CDECLS
