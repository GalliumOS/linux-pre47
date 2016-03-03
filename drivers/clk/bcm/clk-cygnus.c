/*
 * Copyright (C) 2014 Broadcom Corporation
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation version 2.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/kernel.h>
#include <linux/err.h>
#include <linux/clk-provider.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/clkdev.h>
#include <linux/of_address.h>
#include <linux/delay.h>

#include <dt-bindings/clock/bcm-cygnus.h>
#include "clk-iproc.h"

#define reg_val(o, s, w) { .offset = o, .shift = s, .width = w, }

#define aon_val(o, pw, ps, is) { .offset = o, .pwr_width = pw, \
	.pwr_shift = ps, .iso_shift = is }

#define sw_ctrl_val(o, s) { .offset = o, .shift = s, }

#define asiu_div_val(o, es, hs, hw, ls, lw) \
		{ .offset = o, .en_shift = es, .high_shift = hs, \
		.high_width = hw, .low_shift = ls, .low_width = lw }

#define reset_val(o, rs, prs, kis, kiw, kps, kpw, kas, kaw) { .offset = o, \
	.reset_shift = rs, .p_reset_shift = prs, .ki_shift = kis, \
	.ki_width = kiw, .kp_shift = kps, .kp_width = kpw, .ka_shift = kas, \
	.ka_width = kaw }

#define vco_ctrl_val(uo, lo) { .u_offset = uo, .l_offset = lo }

#define enable_val(o, es, hs, bs) { .offset = o, .enable_shift = es, \
	.hold_shift = hs, .bypass_shift = bs }

#define asiu_gate_val(o, es) { .offset = o, .en_shift = es }

static void __init cygnus_armpll_init(struct device_node *node)
{
	iproc_armpll_setup(node);
}
CLK_OF_DECLARE(cygnus_armpll, "brcm,cygnus-armpll", cygnus_armpll_init);

static const struct iproc_pll_ctrl genpll = {
	.flags = IPROC_CLK_AON | IPROC_CLK_PLL_HAS_NDIV_FRAC |
		IPROC_CLK_PLL_NEEDS_SW_CFG,
	.aon = aon_val(0x0, 2, 1, 0),
	.reset = reset_val(0x0, 11, 10, 4, 3, 0, 4, 7, 3),
	.sw_ctrl = sw_ctrl_val(0x10, 31),
	.ndiv_int = reg_val(0x10, 20, 10),
	.ndiv_frac = reg_val(0x10, 0, 20),
	.pdiv = reg_val(0x14, 0, 4),
	.vco_ctrl = vco_ctrl_val(0x18, 0x1c),
	.status = reg_val(0x28, 12, 1),
};

static const struct iproc_clk_ctrl genpll_clk[] = {
	[BCM_CYGNUS_GENPLL_AXI21_CLK] = {
		.channel = BCM_CYGNUS_GENPLL_AXI21_CLK,
		.flags = IPROC_CLK_AON,
		.enable = enable_val(0x4, 6, 0, 12),
		.mdiv = reg_val(0x20, 0, 8),
	},
	[BCM_CYGNUS_GENPLL_250MHZ_CLK] = {
		.channel = BCM_CYGNUS_GENPLL_250MHZ_CLK,
		.flags = IPROC_CLK_AON,
		.enable = enable_val(0x4, 7, 1, 13),
		.mdiv = reg_val(0x20, 10, 8),
	},
	[BCM_CYGNUS_GENPLL_IHOST_SYS_CLK] = {
		.channel = BCM_CYGNUS_GENPLL_IHOST_SYS_CLK,
		.flags = IPROC_CLK_AON,
		.enable = enable_val(0x4, 8, 2, 14),
		.mdiv = reg_val(0x20, 20, 8),
	},
	[BCM_CYGNUS_GENPLL_ENET_SW_CLK] = {
		.channel = BCM_CYGNUS_GENPLL_ENET_SW_CLK,
		.flags = IPROC_CLK_AON,
		.enable = enable_val(0x4, 9, 3, 15),
		.mdiv = reg_val(0x24, 0, 8),
	},
	[BCM_CYGNUS_GENPLL_AUDIO_125_CLK] = {
		.channel = BCM_CYGNUS_GENPLL_AUDIO_125_CLK,
		.flags = IPROC_CLK_AON,
		.enable = enable_val(0x4, 10, 4, 16),
		.mdiv = reg_val(0x24, 10, 8),
	},
	[BCM_CYGNUS_GENPLL_CAN_CLK] = {
		.channel = BCM_CYGNUS_GENPLL_CAN_CLK,
		.flags = IPROC_CLK_AON,
		.enable = enable_val(0x4, 11, 5, 17),
		.mdiv = reg_val(0x24, 20, 8),
	},
};

static void __init cygnus_genpll_clk_init(struct device_node *node)
{
	iproc_pll_clk_setup(node, &genpll, NULL, 0, genpll_clk,
			    ARRAY_SIZE(genpll_clk));
}
CLK_OF_DECLARE(cygnus_genpll, "brcm,cygnus-genpll", cygnus_genpll_clk_init);

static const struct iproc_pll_ctrl lcpll0 = {
	.flags = IPROC_CLK_AON | IPROC_CLK_PLL_NEEDS_SW_CFG,
	.aon = aon_val(0x0, 2, 5, 4),
	.reset = reset_val(0x0, 31, 30, 27, 3, 23, 4, 19, 4),
	.sw_ctrl = sw_ctrl_val(0x4, 31),
	.ndiv_int = reg_val(0x4, 16, 10),
	.pdiv = reg_val(0x4, 26, 4),
	.vco_ctrl = vco_ctrl_val(0x10, 0x14),
	.status = reg_val(0x18, 12, 1),
};

static const struct iproc_clk_ctrl lcpll0_clk[] = {
	[BCM_CYGNUS_LCPLL0_PCIE_PHY_REF_CLK] = {
		.channel = BCM_CYGNUS_LCPLL0_PCIE_PHY_REF_CLK,
		.flags = IPROC_CLK_AON,
		.enable = enable_val(0x0, 7, 1, 13),
		.mdiv = reg_val(0x8, 0, 8),
	},
	[BCM_CYGNUS_LCPLL0_DDR_PHY_CLK] = {
		.channel = BCM_CYGNUS_LCPLL0_DDR_PHY_CLK,
		.flags = IPROC_CLK_AON,
		.enable = enable_val(0x0, 8, 2, 14),
		.mdiv = reg_val(0x8, 10, 8),
	},
	[BCM_CYGNUS_LCPLL0_SDIO_CLK] = {
		.channel = BCM_CYGNUS_LCPLL0_SDIO_CLK,
		.flags = IPROC_CLK_AON,
		.enable = enable_val(0x0, 9, 3, 15),
		.mdiv = reg_val(0x8, 20, 8),
	},
	[BCM_CYGNUS_LCPLL0_USB_PHY_REF_CLK] = {
		.channel = BCM_CYGNUS_LCPLL0_USB_PHY_REF_CLK,
		.flags = IPROC_CLK_AON,
		.enable = enable_val(0x0, 10, 4, 16),
		.mdiv = reg_val(0xc, 0, 8),
	},
	[BCM_CYGNUS_LCPLL0_SMART_CARD_CLK] = {
		.channel = BCM_CYGNUS_LCPLL0_SMART_CARD_CLK,
		.flags = IPROC_CLK_AON,
		.enable = enable_val(0x0, 11, 5, 17),
		.mdiv = reg_val(0xc, 10, 8),
	},
	[BCM_CYGNUS_LCPLL0_CH5_UNUSED] = {
		.channel = BCM_CYGNUS_LCPLL0_CH5_UNUSED,
		.flags = IPROC_CLK_AON,
		.enable = enable_val(0x0, 12, 6, 18),
		.mdiv = reg_val(0xc, 20, 8),
	},
};

static void __init cygnus_lcpll0_clk_init(struct device_node *node)
{
	iproc_pll_clk_setup(node, &lcpll0, NULL, 0, lcpll0_clk,
			    ARRAY_SIZE(lcpll0_clk));
}
CLK_OF_DECLARE(cygnus_lcpll0, "brcm,cygnus-lcpll0", cygnus_lcpll0_clk_init);

/*
 * MIPI PLL VCO frequency parameter table
 */
static const struct iproc_pll_vco_param mipipll_vco_params[] = {
	/* rate (Hz) ndiv_int ndiv_frac pdiv */
	{ 750000000UL,   30,     0,        1 },
	{ 1000000000UL,  40,     0,        1 },
	{ 1350000000ul,  54,     0,        1 },
	{ 2000000000UL,  80,     0,        1 },
	{ 2100000000UL,  84,     0,        1 },
	{ 2250000000UL,  90,     0,        1 },
	{ 2500000000UL,  100,    0,        1 },
	{ 2700000000UL,  54,     0,        0 },
	{ 2975000000UL,  119,    0,        1 },
	{ 3100000000UL,  124,    0,        1 },
	{ 3150000000UL,  126,    0,        1 },
};

static const struct iproc_pll_ctrl mipipll = {
	.flags = IPROC_CLK_PLL_ASIU | IPROC_CLK_PLL_HAS_NDIV_FRAC |
		 IPROC_CLK_NEEDS_READ_BACK,
	.aon = aon_val(0x0, 4, 17, 16),
	.asiu = asiu_gate_val(0x0, 3),
	.reset = reset_val(0x0, 11, 10, 4, 3, 0, 4, 7, 4),
	.ndiv_int = reg_val(0x10, 20, 10),
	.ndiv_frac = reg_val(0x10, 0, 20),
	.pdiv = reg_val(0x14, 0, 4),
	.vco_ctrl = vco_ctrl_val(0x18, 0x1c),
	.status = reg_val(0x28, 12, 1),
};

static const struct iproc_clk_ctrl mipipll_clk[] = {
	[BCM_CYGNUS_MIPIPLL_CH0_UNUSED] = {
		.channel = BCM_CYGNUS_MIPIPLL_CH0_UNUSED,
		.flags = IPROC_CLK_NEEDS_READ_BACK,
		.enable = enable_val(0x4, 12, 6, 18),
		.mdiv = reg_val(0x20, 0, 8),
	},
	[BCM_CYGNUS_MIPIPLL_CH1_LCD] = {
		.channel = BCM_CYGNUS_MIPIPLL_CH1_LCD,
		.flags = IPROC_CLK_NEEDS_READ_BACK,
		.enable = enable_val(0x4, 13, 7, 19),
		.mdiv = reg_val(0x20, 10, 8),
	},
	[BCM_CYGNUS_MIPIPLL_CH2_V3D] = {
		.channel = BCM_CYGNUS_MIPIPLL_CH2_V3D,
		.flags = IPROC_CLK_NEEDS_READ_BACK,
		.enable = enable_val(0x4, 14, 8, 20),
		.mdiv = reg_val(0x20, 20, 8),
	},
	[BCM_CYGNUS_MIPIPLL_CH3_UNUSED] = {
		.channel = BCM_CYGNUS_MIPIPLL_CH3_UNUSED,
		.flags = IPROC_CLK_NEEDS_READ_BACK,
		.enable = enable_val(0x4, 15, 9, 21),
		.mdiv = reg_val(0x24, 0, 8),
	},
	[BCM_CYGNUS_MIPIPLL_CH4_UNUSED] = {
		.channel = BCM_CYGNUS_MIPIPLL_CH4_UNUSED,
		.flags = IPROC_CLK_NEEDS_READ_BACK,
		.enable = enable_val(0x4, 16, 10, 22),
		.mdiv = reg_val(0x24, 10, 8),
	},
	[BCM_CYGNUS_MIPIPLL_CH5_UNUSED] = {
		.channel = BCM_CYGNUS_MIPIPLL_CH5_UNUSED,
		.flags = IPROC_CLK_NEEDS_READ_BACK,
		.enable = enable_val(0x4, 17, 11, 23),
		.mdiv = reg_val(0x24, 20, 8),
	},
};

static void __init cygnus_mipipll_clk_init(struct device_node *node)
{
	iproc_pll_clk_setup(node, &mipipll, mipipll_vco_params,
			    ARRAY_SIZE(mipipll_vco_params), mipipll_clk,
			    ARRAY_SIZE(mipipll_clk));
}
CLK_OF_DECLARE(cygnus_mipipll, "brcm,cygnus-mipipll", cygnus_mipipll_clk_init);

static const struct iproc_asiu_div asiu_div[] = {
	[BCM_CYGNUS_ASIU_KEYPAD_CLK] = asiu_div_val(0x0, 31, 16, 10, 0, 10),
	[BCM_CYGNUS_ASIU_ADC_CLK] = asiu_div_val(0x4, 31, 16, 10, 0, 10),
	[BCM_CYGNUS_ASIU_PWM_CLK] = asiu_div_val(0x8, 31, 16, 10, 0, 10),
};

static const struct iproc_asiu_gate asiu_gate[] = {
	[BCM_CYGNUS_ASIU_KEYPAD_CLK] = asiu_gate_val(0x0, 7),
	[BCM_CYGNUS_ASIU_ADC_CLK] = asiu_gate_val(0x0, 9),
	[BCM_CYGNUS_ASIU_PWM_CLK] = asiu_gate_val(IPROC_CLK_INVALID_OFFSET, 0),
};

static void __init cygnus_asiu_init(struct device_node *node)
{
	iproc_asiu_setup(node, asiu_div, asiu_gate, ARRAY_SIZE(asiu_div));
}
CLK_OF_DECLARE(cygnus_asiu_clk, "brcm,cygnus-asiu-clk", cygnus_asiu_init);
