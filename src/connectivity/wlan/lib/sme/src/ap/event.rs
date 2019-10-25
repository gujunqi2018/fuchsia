// Copyright 2018 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

use fuchsia_zircon::{self as zx, prelude::DurationNum};

use crate::{timer::TimeoutDuration, MacAddr};

pub const START_TIMEOUT_SECONDS: i64 = 5;

#[derive(Debug, Clone)]
pub enum Event {
    Sme { event: SmeEvent },
    Client { addr: MacAddr, event: ClientEvent },
}

impl TimeoutDuration for Event {
    fn timeout_duration(&self) -> zx::Duration {
        match self {
            Event::Sme { event } => event.timeout_duration(),
            Event::Client { event, .. } => event.timeout_duration(),
        }
    }
}

#[derive(Debug, Clone)]
pub enum SmeEvent {
    StartTimeout,
}

impl SmeEvent {
    pub fn timeout_duration(&self) -> zx::Duration {
        match self {
            SmeEvent::StartTimeout => START_TIMEOUT_SECONDS.seconds(),
        }
    }
}

#[derive(Debug, Clone)]
pub enum RsnaTimeout {
    Request,
    Negotiation,
}

#[derive(Debug, Clone)]
pub enum ClientEvent {
    AssociationTimeout,
    RsnaTimeout(RsnaTimeout),
}

impl ClientEvent {
    pub fn timeout_duration(&self) -> zx::Duration {
        match self {
            // We only use schedule_at, so we ignore these timeout durations here.
            // TODO(tonyy): Switch everything to use schedule_at, maybe?
            ClientEvent::AssociationTimeout => 0.seconds(),
            ClientEvent::RsnaTimeout { .. } => 0.seconds(),
        }
    }
}
