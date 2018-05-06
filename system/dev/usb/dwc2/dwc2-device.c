// Copyright 2018 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <ddk/debug.h>

#include "dwc2.h"

static void dwc_flush_fifo(dwc_usb_t* dwc, const int num) {
    union dwc_core_reset core_reset;

    core_reset.val = 0;
	core_reset.txfflsh = 1;
	core_reset.txfnum = num;
	regs->core_reset = core_reset;
	
    uint32_t count = 0;
	do {
	    core_reset = regs->core_reset;
		if (++count > 10000)
			break;
	} while (core_reset.txfflsh == 1);

    zx_nanosleep(zx_deadline_after(ZX_USEC(1)));

	if (num == 0) {
		return;
    }

    core_reset.val = 0;
	core_reset.rxfflsh = 1;
	regs->core_reset = core_reset;

	count = 0;
	do {
	    core_reset = regs->core_reset;
		if (++count > 10000)
			break;
	} while (core_reset.rxfflsh == 1);

    zx_nanosleep(zx_deadline_after(ZX_USEC(1)));
}

void dwc_handle_reset_irq(dwc_usb_t* dwc) {
/*
	depctl_data_t doepctl = {0};
	daint_data_t daintmsk = {0};
	dcfg_data_t dcfg = {0};
	depctl_data_t diepctl = {0};
	depctl_data_t diepctl_rd = {0};
	grstctl_t resetctl = {0};
	dctl_data_t dctl = {0};
	gintsts_data_t gintsts = {0};
	int i = 0;
*/
	zxlogf(INFO, "USB RESET\n");

#if 0
	/* Clear the Remote Wakeup Signalling */
	dctl.b.rmtwkupsig = 1;
	dwc_modify_reg32( DWC_REG_DCTL,dctl.d32, 0 );
#endif

	for (int i = 0; i < MAX_EPS_CHANNELS; i++) {
	    uint32_t diepctl = regs->depin[i].diepctl;

        if (diepctl & DWC_DIEPCTL_EPENA) {
            // disable all active IN EPs
    	    regs->depin[i].diepctl = DWC_DIEPCTL_SNAK | DWC_DIEPCTL_EPDIS;
        }

        regs->depin[i].diepctl = DWC_DOEPCTL_SNAK;
	}

	/* Flush the NP Tx FIFO */
	dwc_flush_fifo(dwc, 0);

	/* Flush the Learning Queue */
	regs->core_reset.intknqflsh = 1;

    // EPO IN and OUT
	regs->daintmsk = (1 < DWC_EP_IN_SHIFT) | (1 < DWC_EP_OUT_SHIFT);

    union dwc_doepmsk doepmsk;
    doepmsk.val = 0;
	doepmsk.setup = 1;
	doepmsk.xfercompl = 1;
	doepmsk.ahberr = 1;
	doepmsk.epdisabled = 1;
	regs->doepmsk = doepmsk;

    union dwc_diepmsk diepmsk;
    diepmsk.val = 0;
	diepmsk.xfercompl = 1;
	diepmsk.timeout = 1;
	diepmsk.epdisabled = 1;
	diepmsk.ahberr = 1;
	regs->diepmsk = diepmsk;

#if 0	
	/* Reset Device Address */
	dcfg.d32 = dwc_read_reg32(DWC_REG_DCFG);
	dcfg.b.devaddr = 0;
	dwc_write_reg32(DWC_REG_DCFG, dcfg.d32);

	/* setup EP0 to receive SETUP packets */
	ep0_out_start();

	/* Clear interrupt */
	gintsts.b.usbreset = 1;
	dwc_write_reg32 (DWC_REG_GINTSTS, gintsts.d32);

	flush_cpu_cache();
#endif
}
