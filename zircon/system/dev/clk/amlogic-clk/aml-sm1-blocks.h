// Copyright 2020 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <soc/aml-meson/sm1-clk.h>
#include "aml-clk-blocks.h"
#include "aml-g12a-blocks.h"

static constexpr meson_clk_gate_t sm1_clk_gates[] = {
        // SYS CPU Clock gates.
    {.reg = kHhiSysCpuClkCntl1, .bit = 24},  // CLK_SYS_PLL_DIV16
    {.reg = kHhiSysCpuClkCntl1, .bit = 1},   // CLK_SYS_CPU_CLK_DIV16

    {.reg = kHhiGclkMpeg0, .bit = 0},   // CLK_DDR
    {.reg = kHhiGclkMpeg0, .bit = 1},   // CLK_DOS
    {.reg = kHhiGclkMpeg0, .bit = 2},   // CLK_ALOCKER
    {.reg = kHhiGclkMpeg0, .bit = 3},   // CLK_MIPI_DSI_HOST
    {.reg = kHhiGclkMpeg0, .bit = 4},   // CLK_ETH_PHY
    {.reg = kHhiGclkMpeg0, .bit = 5},   // CLK_ISA
    {.reg = kHhiGclkMpeg0, .bit = 6},   // CLK_PL301
    {.reg = kHhiGclkMpeg0, .bit = 7},   // CLK_PERIPHS
    {.reg = kHhiGclkMpeg0, .bit = 8},   // CLK_SPICC_0
    {.reg = kHhiGclkMpeg0, .bit = 9},   // CLK_I2C
    {.reg = kHhiGclkMpeg0, .bit = 10},  // CLK_SANA
    {.reg = kHhiGclkMpeg0, .bit = 11},  // CLK_SD
    {.reg = kHhiGclkMpeg0, .bit = 12},  // CLK_RNG0
    {.reg = kHhiGclkMpeg0, .bit = 13},  // CLK_UART0
    {.reg = kHhiGclkMpeg0, .bit = 14},  // CLK_SPICC_1
    {.reg = kHhiGclkMpeg0, .bit = 19},  // CLK_HIU_REG
    {.reg = kHhiGclkMpeg0, .bit = 20},  // CLK_MIPI_DSI_PHY
    {.reg = kHhiGclkMpeg0, .bit = 23},  // CLK_ASSIST_MISC
    {.reg = kHhiGclkMpeg0, .bit = 24},  // CLK_EMMC_A
    {.reg = kHhiGclkMpeg0, .bit = 25},  // CLK_EMMC_B
    {.reg = kHhiGclkMpeg0, .bit = 26},  // CLK_EMMC_C
    {.reg = kHhiGclkMpeg0, .bit = 28},  // CLK_ACODEC

    {.reg = kHhiGclkMpeg1, .bit = 0},   // CLK_AUDIO
    {.reg = kHhiGclkMpeg1, .bit = 3},   // CLK_ETH_CORE
    {.reg = kHhiGclkMpeg1, .bit = 4},   // CLK_DEMUX
    {.reg = kHhiGclkMpeg1, .bit = 11},  // CLK_AIFIFO
    {.reg = kHhiGclkMpeg1, .bit = 13},  // CLK_ADC
    {.reg = kHhiGclkMpeg1, .bit = 16},  // CLK_UART1
    {.reg = kHhiGclkMpeg1, .bit = 20},  // CLK_G2D
    {.reg = kHhiGclkMpeg1, .bit = 23},  // CLK_RESET
    {.reg = kHhiGclkMpeg1, .bit = 24},  // CLK_PCIE_COMB
    {.reg = kHhiGclkMpeg1, .bit = 25},  // CLK_PARSER
    {.reg = kHhiGclkMpeg1, .bit = 26},  // CLK_USB_GENERAL
    {.reg = kHhiGclkMpeg1, .bit = 27},  // CLK_PCIE_PHY
    {.reg = kHhiGclkMpeg1, .bit = 29},  // CLK_AHB_ARB0

    {.reg = kHhiGclkMpeg2, .bit = 1},   // CLK_AHB_DATA_BUS
    {.reg = kHhiGclkMpeg2, .bit = 2},   // CLK_AHB_CTRL_BUS
    {.reg = kHhiGclkMpeg2, .bit = 3},   // CLK_HTX_HDCP22
    {.reg = kHhiGclkMpeg2, .bit = 4},   // CLK_HTX_PCLK
    {.reg = kHhiGclkMpeg2, .bit = 6},   // CLK_BT656
    {.reg = kHhiGclkMpeg2, .bit = 8},   // CLK_USB1_TO_DDR
    {.reg = kHhiGclkMpeg2, .bit = 11},  // CLK_MMC_PCLK
    {.reg = kHhiGclkMpeg2, .bit = 15},  // CLK_UART2
    {.reg = kHhiGclkMpeg2, .bit = 25},  // CLK_VPU_INTR
    {.reg = kHhiGclkMpeg2, .bit = 30},  // CLK_GIC

    {.reg = kHhiGclk2Other, .bit = 1},   // CLK_VCLK2_VENCI0
    {.reg = kHhiGclk2Other, .bit = 2},   // CLK_VCLK2_VENCI1
    {.reg = kHhiGclk2Other, .bit = 3},   // CLK_VCLK2_VENCP0
    {.reg = kHhiGclk2Other, .bit = 4},   // CLK_VCLK2_VENCP1
    {.reg = kHhiGclk2Other, .bit = 5},   // CLK_VCLK2_VENCT0
    {.reg = kHhiGclk2Other, .bit = 6},   // CLK_VCLK2_VENCT1
    {.reg = kHhiGclk2Other, .bit = 7},   // CLK_VCLK2_OTHER
    {.reg = kHhiGclk2Other, .bit = 8},   // CLK_VCLK2_ENCI
    {.reg = kHhiGclk2Other, .bit = 9},   // CLK_VCLK2_ENCP
    {.reg = kHhiGclk2Other, .bit = 10},  // CLK_DAC_CLK
    {.reg = kHhiGclk2Other, .bit = 14},  // CLK_AOCLK_GATE
    {.reg = kHhiGclk2Other, .bit = 16},  // CLK_IEC958_GATE
    {.reg = kHhiGclk2Other, .bit = 20},  // CLK_ENC480P
    {.reg = kHhiGclk2Other, .bit = 21},  // CLK_RNG1
    {.reg = kHhiGclk2Other, .bit = 22},  // CLK_VCLK2_ENCT
    {.reg = kHhiGclk2Other, .bit = 23},  // CLK_VCLK2_ENCL
    {.reg = kHhiGclk2Other, .bit = 24},  // CLK_VCLK2_VENCLMMC
    {.reg = kHhiGclk2Other, .bit = 25},  // CLK_VCLK2_VENCL
    {.reg = kHhiGclk2Other, .bit = 26},  // CLK_VCLK2_OTHER1

    // SM1 Specific Clock Gates.
    {.reg = kHhiGclkMpeg1, .bit = 18},  // CLK_CSI_DIG
    {.reg = kHhiGclkMpeg1, .bit = 19},  // CLK_NNA
    {.reg = kHhiGclkMpeg1, .bit = 28},  // CLK_PARSER1
    {.reg = kHhiGclkMpeg2, .bit = 16},  // CLK_CSI_HOST
    {.reg = kHhiGclkMpeg2, .bit = 17},  // CLK_CSI_ADPAT
    {.reg = kHhiGclkMpeg2, .bit = 22},  // CLK_TEMP_SENSOR
    {.reg = kHhiGclkMpeg2, .bit = 29},  // CLK_CSI_PHY
};

static_assert(sm1_clk::CLK_SM1_COUNT == countof(sm1_clk_gates),
              "sm1_clk_gates[] and CLK_SM1_COUNT count mismatch");

// clang-format off