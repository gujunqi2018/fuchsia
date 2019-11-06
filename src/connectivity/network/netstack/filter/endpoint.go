// Copyright 2018 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Filter endpoint implements a LinkEndpoint interface, which can wrap another
// LinkEndpoint.

package filter

import (
	"sync/atomic"

	"github.com/google/netstack/tcpip"
	"github.com/google/netstack/tcpip/buffer"
	"github.com/google/netstack/tcpip/stack"
)

var _ stack.LinkEndpoint = (*Endpoint)(nil)
var _ stack.NetworkDispatcher = (*Endpoint)(nil)

type Endpoint struct {
	filter     *Filter
	dispatcher stack.NetworkDispatcher
	enabled    uint32
	stack.LinkEndpoint
}

// New creates a new Filter endpoint by wrapping a lower LinkEndpoint.
func NewEndpoint(filter *Filter, lower stack.LinkEndpoint) *Endpoint {
	return &Endpoint{
		filter:       filter,
		LinkEndpoint: lower,
		enabled:      1,
	}
}

func (e *Endpoint) Enable() {
	atomic.StoreUint32(&e.enabled, 1)
}

func (e *Endpoint) Disable() {
	atomic.StoreUint32(&e.enabled, 0)
}

func (e *Endpoint) IsEnabled() bool {
	return atomic.LoadUint32(&e.enabled) == 1
}

// DeliverNetworkPacket implements stack.NetworkDispatcher.
func (e *Endpoint) DeliverNetworkPacket(linkEP stack.LinkEndpoint, dstLinkAddr, srcLinkAddr tcpip.LinkAddress, protocol tcpip.NetworkProtocolNumber, vv buffer.VectorisedView, linkHeader buffer.View) {
	if atomic.LoadUint32(&e.enabled) == 1 {
		hdr := buffer.NewPrependableFromView(vv.First())
		payload := vv
		payload.RemoveFirst()

		if e.filter.Run(Incoming, protocol, hdr, payload) != Pass {
			return
		}
	}
	e.dispatcher.DeliverNetworkPacket(e, dstLinkAddr, srcLinkAddr, protocol, vv, linkHeader)
}

// Attach implements stack.LinkEndpoint.
func (e *Endpoint) Attach(dispatcher stack.NetworkDispatcher) {
	e.dispatcher = dispatcher
	e.LinkEndpoint.Attach(e)
}

// WritePacket implements stack.LinkEndpoint.
func (e *Endpoint) WritePacket(r *stack.Route, gso *stack.GSO, hdr buffer.Prependable, payload buffer.VectorisedView, protocol tcpip.NetworkProtocolNumber) *tcpip.Error {
	if atomic.LoadUint32(&e.enabled) == 1 && e.filter.Run(Outgoing, protocol, hdr, payload) != Pass {
		return nil
	}
	return e.LinkEndpoint.WritePacket(r, gso, hdr, payload, protocol)
}

// WritePackets implements stack.LinkEndpoint.
func (e *Endpoint) WritePackets(r *stack.Route, gso *stack.GSO, hdrs []stack.PacketDescriptor, payload buffer.VectorisedView, protocol tcpip.NetworkProtocolNumber) (int, *tcpip.Error) {
	if atomic.LoadUint32(&e.enabled) == 0 {
		return e.LinkEndpoint.WritePackets(r, gso, hdrs, payload, protocol)
	}
	h := make([]stack.PacketDescriptor, 0, len(hdrs))
	for _, hdr := range hdrs {
		if e.filter.Run(Outgoing, protocol, hdr.Hdr, payload) != Pass {
			continue
		}
		h = append(h, hdr)
	}

	return e.LinkEndpoint.WritePackets(r, gso, h, payload, protocol)
}
