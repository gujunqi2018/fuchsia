/*
 * Copyright (c) 2018 The Fuchsia Authors
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "brcm_hw_ids.h"
#include "common.h"

#include <ddk/binding.h>
#include <ddk/device.h>
#include <ddk/driver.h>
#include <ddk/platform-defs.h>
#include <zircon/types.h>
#include <hw/pci.h>

zx_status_t brcmfmac_bind(void* ctx, zx_device_t* device) {
    BRCMF_DBG(TRACE, "Enter");
    return brcmfmac_module_init(device);
}

static constexpr zx_driver_ops_t brcmfmac_driver_ops = []() {
    zx_driver_ops_t ops = {};
    ops.version = DRIVER_OPS_VERSION;
    ops.bind = brcmfmac_bind;
    return ops;
}();

ZIRCON_DRIVER_BEGIN(brcmfmac, brcmfmac_driver_ops, "zircon", "0.1", 33)
    BI_GOTO_IF(EQ, BIND_PROTOCOL, ZX_PROTOCOL_COMPOSITE, 5910),
    BI_GOTO_IF(EQ, BIND_PROTOCOL, ZX_PROTOCOL_USB, 758),
    BI_ABORT_IF(NE, BIND_PROTOCOL, ZX_PROTOCOL_PCI),
    BI_ABORT_IF(NE, BIND_PCI_VID, BRCM_PCIE_VENDOR_ID_BROADCOM),
    BI_ABORT_IF(NE, BIND_PCI_CLASS, PCI_CLASS_NETWORK),
    BI_ABORT_IF(NE, BIND_PCI_SUBCLASS, PCI_SUBCLASS_NETWORK_OTHER),
    BI_MATCH_IF(EQ, BIND_PCI_DID, BRCM_PCIE_4350_DEVICE_ID),
    BI_MATCH_IF(EQ, BIND_PCI_DID, BRCM_PCIE_4356_DEVICE_ID),
    BI_MATCH_IF(EQ, BIND_PCI_DID, BRCM_PCIE_43567_DEVICE_ID),
    BI_MATCH_IF(EQ, BIND_PCI_DID, BRCM_PCIE_43570_DEVICE_ID),
    BI_MATCH_IF(EQ, BIND_PCI_DID, BRCM_PCIE_4358_DEVICE_ID),
    BI_MATCH_IF(EQ, BIND_PCI_DID, BRCM_PCIE_4359_DEVICE_ID),
    BI_MATCH_IF(EQ, BIND_PCI_DID, BRCM_PCIE_43602_DEVICE_ID),
    BI_MATCH_IF(EQ, BIND_PCI_DID, BRCM_PCIE_43602_2G_DEVICE_ID),
    BI_MATCH_IF(EQ, BIND_PCI_DID, BRCM_PCIE_43602_5G_DEVICE_ID),
    BI_MATCH_IF(EQ, BIND_PCI_DID, BRCM_PCIE_43602_RAW_DEVICE_ID),
    BI_MATCH_IF(EQ, BIND_PCI_DID, BRCM_PCIE_4365_DEVICE_ID),
    BI_MATCH_IF(EQ, BIND_PCI_DID, BRCM_PCIE_4365_2G_DEVICE_ID),
    BI_MATCH_IF(EQ, BIND_PCI_DID, BRCM_PCIE_4365_5G_DEVICE_ID),
 // BRCMF_PCIE_DEVICE_SUB(0x4365, BRCM_PCIE_VENDOR_ID_BROADCOM, 0x4365), // TODO(cphoenix): support
    BI_MATCH_IF(EQ, BIND_PCI_DID, BRCM_PCIE_4366_DEVICE_ID),
    BI_MATCH_IF(EQ, BIND_PCI_DID, BRCM_PCIE_4366_2G_DEVICE_ID),
    BI_MATCH_IF(EQ, BIND_PCI_DID, BRCM_PCIE_4366_5G_DEVICE_ID),
    BI_MATCH_IF(EQ, BIND_PCI_DID, BRCM_PCIE_4371_DEVICE_ID),
    BI_ABORT(),
    BI_LABEL(758),
    BI_ABORT_IF(NE, BIND_USB_VID, 0x043e),
    BI_MATCH_IF(EQ, BIND_USB_PID, 0x3101),
    BI_ABORT(),
    // Composite binding used for SDIO.
    BI_LABEL(5910),
    BI_ABORT_IF(NE, BIND_PLATFORM_DEV_VID, PDEV_VID_BROADCOM),
    BI_ABORT_IF(NE, BIND_PLATFORM_DEV_DID, PDEV_DID_BCM_WIFI),
    BI_MATCH_IF(EQ, BIND_PLATFORM_DEV_PID, PDEV_PID_BCM4356),
    BI_MATCH_IF(EQ, BIND_PLATFORM_DEV_PID, PDEV_PID_BCM43458),
ZIRCON_DRIVER_END(brcmfmac)
