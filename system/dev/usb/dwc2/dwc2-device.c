// Copyright 2018 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <ddk/debug.h>

#include "dwc2.h"

static void dwc2_ep0_out_start(dwc_usb_t* dwc)
{
    printf("dwc2_ep0_out_start\n");

	deptsiz0_t doeptsize0 = {0};
	depctl_t doepctl = {0};

	doeptsize0.supcnt = 3;
	doeptsize0.pktcnt = 1;
	doeptsize0.xfersize = 8*3;
    regs->depout[0].doeptsiz = doeptsize0.val;

	doepctl.epena = 1;
    regs->depout[0].doepctl = doepctl;

//	flush_cpu_cache();
}

static void dwc_ep0_complete_request(dwc_usb_t* dwc) {
    printf("dwc_ep0_complete_request\n");

}

/*

static void dwc_otg_read_setup_packet(u32 *_dest)
{
	// Get the 8 bytes of a setup transaction data
	// Pop 2 DWORDS off the receive data FIFO into memory
	_dest[0] = dwc_read_reg32(DWC_REG_DATA_FIFO_START);
	_dest[1] = dwc_read_reg32(DWC_REG_DATA_FIFO_START);
}

static zx_status_t dwc3_handle_setup(dwc3_t* dwc, usb_setup_t* setup, void* buffer, size_t length,
                                     size_t* out_actual) {
    zx_status_t status;

    if (setup->bmRequestType == (USB_DIR_OUT | USB_TYPE_STANDARD | USB_RECIP_DEVICE)) {
        // handle some special setup requests in this driver
        switch (setup->bRequest) {
        case USB_REQ_SET_ADDRESS:
            zxlogf(TRACE, "SET_ADDRESS %d\n", setup->wValue);
            dwc3_set_address(dwc, setup->wValue);
            *out_actual = 0;
            return ZX_OK;
        case USB_REQ_SET_CONFIGURATION:
            zxlogf(TRACE, "SET_CONFIGURATION %d\n", setup->wValue);
            dwc3_reset_configuration(dwc);
            dwc->configured = false;
            status = usb_dci_control(&dwc->dci_intf, setup, buffer, length, out_actual);
            if (status == ZX_OK && setup->wValue) {
                dwc->configured = true;
                dwc3_start_eps(dwc);
            }
            return status;
        default:
            // fall through to usb_dci_control()
            break;
        }
    } else if (setup->bmRequestType == (USB_DIR_OUT | USB_TYPE_STANDARD | USB_RECIP_INTERFACE) &&
               setup->bRequest == USB_REQ_SET_INTERFACE) {
        zxlogf(TRACE, "SET_INTERFACE %d\n", setup->wValue);
        dwc3_reset_configuration(dwc);
        dwc->configured = false;
        status = usb_dci_control(&dwc->dci_intf, setup, buffer, length, out_actual);
        if (status == ZX_OK) {
            dwc->configured = true;
            dwc3_start_eps(dwc);
        }
        return status;
    }

    return usb_dci_control(&dwc->dci_intf, setup, buffer, length, out_actual);
}
*/

#if 0
static void pcd_setup(pcd_struct_t *_pcd)
{
	struct usb_ctrlrequest	ctrl = _pcd->setup_pkt.req;
	struct usb_request *req = &gadget_wrapper.req;
	struct usb_req_flag *req_flag = &gadget_wrapper.req_flag;
	struct usb_gadget_driver *driver = gadget_wrapper.driver;
	struct usb_gadget *gadget = &gadget_wrapper.gadget;
	dwc_ep_t	*ep0 = &_pcd->dwc_eps[0].dwc_ep;

	if (0 == req_flag->request_enable)
		return;

	_pcd->status = 0;
	req_flag->request_enable = 0;

	if (ctrl.bRequestType & USB_DIR_IN) {
		ep0->is_in = 1;
		_pcd->ep0state = EP0_IN_DATA_PHASE;
	} else {
		ep0->is_in = 0;
		_pcd->ep0state = EP0_OUT_DATA_PHASE;
	}

	if ((ctrl.bRequestType & USB_TYPE_MASK) != USB_TYPE_STANDARD) {
		/* handle non-standard (class/vendor) requests in the gadget driver */
		printf("Vendor requset\n");
		f_dwc_otg_ep_req_start(_pcd, 0, 1, req);
		return;
	}

	/** @todo NGS: Handle bad setup packet? */
	switch (ctrl.bRequest) {
	case USB_REQ_GET_STATUS:
		_pcd->status = 0;
		req->buf = (char *)&_pcd->status;
		req->length = 2;
		f_dwc_otg_ep_req_start(_pcd, 0, 1, req);
		break;
	case USB_REQ_SET_ADDRESS:
		if (USB_RECIP_DEVICE == ctrl.bRequestType) {
			dcfg_data_t dcfg = {0};
			dcfg.b.devaddr = ctrl.wValue;
			dwc_modify_reg32(DWC_REG_DCFG, 0, dcfg.d32);
			do_setup_status_phase(_pcd, 1);
		}
		break;
	case USB_REQ_SET_INTERFACE:
	case USB_REQ_SET_CONFIGURATION:
		/* Configuration changed */
		req_flag->request_config = 1;
	default:
		DBG("Call the Gadget Driver's setup functions\n");
		/* Call the Gadget Driver's setup functions */
		driver->setup(gadget, &ctrl);
		break;
	}

	return;
}
#endif

static void dwc_handle_ep0(dwc_usb_t* dwc) {
    printf("dwc_handle_ep0\n");

/*    
  	pcd_struct_t * _pcd = &gadget_wrapper.pcd;
	dwc_ep_t * ep0 = &_pcd->dwc_eps[0].dwc_ep;
	struct usb_req_flag *req_flag = &gadget_wrapper.req_flag;
*/
	switch (dwc->ep0_state) {
	case EP0_STATE_IDLE:
printf("XXXX pcd_setup\n");
//		req_flag->request_config = 0;
//		pcd_setup(_pcd);
		break;

	case EP0_STATE_DATA_IN:
//		if (ep0->xfer_count < ep0->total_len)
//			printf("FIX ME!! dwc_otg_ep0_continue_transfer!\n");
//		else
			dwc_ep0_complete_request(dwc);
		break;

	case EP0_STATE_DATA_OUT:
		dwc_ep0_complete_request(dwc);
		break;

	case EP0_STATE_STATUS:
		dwc_ep0_complete_request(dwc);
		/* OUT for next SETUP */
		dwc->ep0_state = EP0_STATE_IDLE;
//		ep0->stopped = 1;
//		ep0->is_in = 0;
		break;

	case EP0_STATE_STALL:
	case EP0_STATE_DISCONNECT:
	default:
		printf("EP0 state is %d, should not get here pcd_setup()\n", dwc->ep0_state);
		break;
    }
}

static void dwc_complete_ep(dwc_usb_t* dwc, uint32_t ep_num, int is_in) {
    printf("dwc_complete_ep\n");

#if 0
	deptsiz_data_t deptsiz;
	pcd_struct_t *pcd = &gadget_wrapper.pcd;
	dwc_ep_t * ep;
	u32 epnum = ep_num;

	if (ep_num) {
		if (!is_in)
			epnum = ep_num + 1;
	}

	ep = &pcd->dwc_eps[epnum].dwc_ep;

	if (is_in) {
		pcd->dwc_eps[epnum].req->actual = ep->xfer_len;
		deptsiz.d32 = dwc_read_reg32(DWC_REG_IN_EP_TSIZE(ep_num));
		if (deptsiz.b.xfersize == 0 && deptsiz.b.pktcnt == 0 &&
                    ep->xfer_count == ep->xfer_len) {
			ep->start_xfer_buff = 0;
			ep->xfer_buff = 0;
			ep->xfer_len = 0;
		}
		pcd->dwc_eps[epnum].req->status = 0;
	} else {
		deptsiz.d32 = dwc_read_reg32(DWC_REG_OUT_EP_TSIZE(ep_num));
		pcd->dwc_eps[epnum].req->actual = ep->xfer_count;
		ep->start_xfer_buff = 0;
		ep->xfer_buff = 0;
		ep->xfer_len = 0;
		pcd->dwc_eps[epnum].req->status = 0;
	}

	if (pcd->dwc_eps[epnum].req->complete) {
		pcd->dwc_eps[epnum].req->complete((struct usb_ep *)(pcd->dwc_eps[epnum].priv), pcd->dwc_eps[epnum].req);
	}
#endif
}


static void dwc_flush_fifo(dwc_usb_t* dwc, const int num) {
    union dwc_core_reset core_reset = {0};

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
	zxlogf(INFO, "USB RESET\n");

	/* Clear the Remote Wakeup Signalling */
	regs->dctl.rmtwkupsig = 1;

	for (int i = 0; i < MAX_EPS_CHANNELS; i++) {
	    depctl_t diepctl = regs->depin[i].diepctl;

        if (diepctl.epena) {
            // disable all active IN EPs
            diepctl.snak = 1;
            diepctl.epdis = 1;
    	    regs->depin[i].diepctl = diepctl;
        }

        regs->depout[i].doepctl.snak = 1;
	}

	/* Flush the NP Tx FIFO */
	dwc_flush_fifo(dwc, 0);

	/* Flush the Learning Queue */
	regs->core_reset.intknqflsh = 1;

    // EPO IN and OUT
	regs->daintmsk = (1 < DWC_EP_IN_SHIFT) | (1 < DWC_EP_OUT_SHIFT);

    union dwc_doepmsk doepmsk = {0};
	doepmsk.setup = 1;
	doepmsk.xfercompl = 1;
	doepmsk.ahberr = 1;
	doepmsk.epdisabled = 1;
	regs->doepmsk = doepmsk;

    union dwc_diepmsk diepmsk = {0};
	diepmsk.xfercompl = 1;
	diepmsk.timeout = 1;
	diepmsk.epdisabled = 1;
	diepmsk.ahberr = 1;
	regs->diepmsk = diepmsk;

	/* Reset Device Address */
	regs->dcfg.devaddr = 0;

	/* setup EP0 to receive SETUP packets */
	dwc2_ep0_out_start(dwc);

	/* Clear interrupt */
	union dwc_core_interrupts core_interrupts = {0};
	core_interrupts.usbreset = 1;
	regs->core_interrupts = core_interrupts;

//	flush_cpu_cache();
}

void dwc_handle_enumdone_irq(dwc_usb_t* dwc) {
	zxlogf(INFO, "SPEED ENUM\n");

    dwc->ep0_state = EP0_STATE_IDLE;

    depctl_t diepctl = regs->depin[0].diepctl;
    diepctl.mps = DWC_DEP0CTL_MPS_64;
    regs->depin[0].diepctl = diepctl;

    depctl_t doepctl = regs->depout[0].doepctl;
    doepctl.epena = 1;
    regs->depout[0].doepctl = doepctl;

    union dwc_dctl dctl = {0};
    dctl.cgnpinnak = 1;
    regs->dctl = dctl;

	/* high speed */
	regs->core_configuration.usbtrdtim = 5;

	/* Clear interrupt */
	union dwc_core_interrupts core_interrupts = {0};
	core_interrupts.enumdone = 1;
	regs->core_interrupts = core_interrupts;
}

void dwc_handle_rxstsqlvl_irq(dwc_usb_t* dwc) {
printf("dwc_handle_rxstsqlvl_irq\n");
/*
	gintmsk_data_t gintmask = {0};
	gintsts_data_t gintsts = {0};
	dwc_ep_t *ep;
	pcd_struct_t *pcd = &gadget_wrapper.pcd;
	struct usb_req_flag *req_flag = &gadget_wrapper.req_flag;
	device_grxsts_data_t status;
*/

	/* Disable the Rx Status Queue Level interrupt */
    regs->core_interrupt_mask.rxstsqlvl = 0;
//	gintmask.b.rxstsqlvl = 1;
//	dwc_modify_reg32(DWC_REG_GINTMSK, gintmask.d32, 0);

	/* Get the Status from the top of the FIFO */
	 union receive_status_pop rsp = regs->receive_status_pop;
	if (rsp.epnum != 0)
		rsp.epnum = 2;
	/* Get pointer to EP structure */
//	ep = &pcd->dwc_eps[status.b.epnum].dwc_ep;

	switch (rsp.pktsts) {
	case DWC_STS_DATA_UPDT:
printf("DWC_STS_DATA_UPDT\n");
/*
		if (status.b.bcnt && ep->xfer_buff) {
			dwc_otg_read_packet(ep->xfer_buff, status.b.bcnt);
			ep->xfer_count += status.b.bcnt;
			ep->xfer_buff += status.b.bcnt;
		}
*/
		break;

	case DWC_DSTS_SETUP_UPDT:
printf("DWC_STS_DATA_UPDT\n");
{
    volatile uint32_t* fifo = (uint32_t *)((uint8_t *)regs + 0x1000);
    uint32_t* dest = (uint32_t*)&dwc->cur_setup;
    dest[0] = *fifo;
    dest[1] = *fifo;
printf("SETUP bmRequestType: 0x%02x bRequest: %u wValue: %u wIndex: %u wLength: %u\n",
       dwc->cur_setup.bmRequestType, dwc->cur_setup.bRequest, dwc->cur_setup.wValue,
       dwc->cur_setup.wIndex, dwc->cur_setup.wLength); 
}

//		dwc_otg_read_setup_packet(gadget_wrapper.pcd.setup_pkt.d32);
//		req_flag->request_enable = 1;
//		ep->xfer_count += status.b.bcnt;
		break;

	case DWC_DSTS_GOUT_NAK:
	case DWC_STS_XFER_COMP:
	case DWC_DSTS_SETUP_COMP:
	default:
printf("other\n");
		break;
	}


	/* Enable the Rx Status Queue Level interrupt */
    regs->core_interrupt_mask.rxstsqlvl = 1;

	/* Clear interrupt */
	union dwc_core_interrupts core_interrupts = {0};
	core_interrupts.rxstsqlvl = 1;
	regs->core_interrupts = core_interrupts;
}

void dwc_handle_inepintr_irq(dwc_usb_t* dwc) {
printf("dwc_handle_inepintr_irq\n");

#if 0
	diepint_data_t diepint = {0};
	gintmsk_data_t intr_mask = {0};
	gintsts_data_t gintsts = {0};
	u32 ep_intr;
	u32 epnum = 0;

	/* Read in the device interrupt bits */
	ep_intr = dwc_read_reg32(DWC_REG_DAINT);

	ep_intr = (dwc_read_reg32(DWC_REG_DAINT) &
		dwc_read_reg32(DWC_REG_DAINTMSK));
	ep_intr =(ep_intr & 0xffff);

	/* Clear the INEPINT in GINTSTS */
	/* Clear all the interrupt bits for all IN endpoints in DAINT */
	gintsts.b.inepint = 1;
	dwc_write_reg32(DWC_REG_GINTSTS, gintsts.d32);
	dwc_write_reg32(DWC_REG_DAINT, 0xFFFF);
	flush_cpu_cache();

	/* Service the Device IN interrupts for each endpoint */
	while (ep_intr) {
		if (ep_intr & 0x1) {
			diepint.d32 = (dwc_read_reg32(DWC_REG_IN_EP_INTR(epnum)) &
				dwc_read_reg32(DWC_REG_DAINTMSK));

			/* Transfer complete */
			if (diepint.b.xfercompl) {
				/* Disable the NP Tx FIFO Empty Interrrupt  */
				intr_mask.b.nptxfempty = 1;
				dwc_modify_reg32(DWC_REG_GINTMSK, intr_mask.d32, 0);
				/* Clear the bit in DIEPINTn for this interrupt */
				CLEAR_IN_EP_INTR(epnum, xfercompl);
				/* Complete the transfer */
				if (0 == epnum) {
					handle_ep0();
				} else {
					dwc_complete_ep(dwc, epnum, 1);
					if (diepint.b.nak)
						CLEAR_IN_EP_INTR(epnum, nak);
				}
			}
			/* Endpoint disable  */
			if (diepint.b.epdisabled) {
				/* Clear the bit in DIEPINTn for this interrupt */
				CLEAR_IN_EP_INTR(epnum, epdisabled);
			}
			/* AHB Error */
			if (diepint.b.ahberr) {
				/* Clear the bit in DIEPINTn for this interrupt */
				CLEAR_IN_EP_INTR(epnum, ahberr);
			}
			/* TimeOUT Handshake (non-ISOC IN EPs) */
			if (diepint.b.timeout) {
				handle_in_ep_timeout_intr(epnum);
				CLEAR_IN_EP_INTR(epnum, timeout);
			}
			/** IN Token received with TxF Empty */
			if (diepint.b.intktxfemp) {
				CLEAR_IN_EP_INTR(epnum, intktxfemp);
			}
			/** IN Token Received with EP mismatch */
			if (diepint.b.intknepmis) {
				CLEAR_IN_EP_INTR(epnum, intknepmis);
			}
			/** IN Endpoint NAK Effective */
			if (diepint.b.inepnakeff) {
				CLEAR_IN_EP_INTR(epnum, inepnakeff);
			}
		}
		epnum++;
		ep_intr >>= 1;
	}
#endif
}

void dwc_handle_outepintr_irq(dwc_usb_t* dwc) {
	uint32_t epnum = 0;

	/* Read in the device interrupt bits */
	uint32_t ep_intr = regs->daint & 0xFFFF0000;
	ep_intr >>= DWC_EP_OUT_SHIFT;

	/* Clear the interrupt */
	union dwc_core_interrupts core_interrupts = {0};
	core_interrupts.outepintr = 1;
	regs->core_interrupts = core_interrupts;
	regs->daint = 0xFFFF0000;

	while (ep_intr) {
		if (ep_intr & 1) {
		    doepint_t doepint = regs->depout[epnum].doepint;

			/* Transfer complete */
			if (doepint.xfercompl) {
				/* Clear the bit in DOEPINTn for this interrupt */
			    doepint_t clear = {0};
			    clear.xfercompl = 1;
			    regs->depout[epnum].doepint = clear;

				if (epnum == 0) {
				    clear.val = 0;
				    clear.setup = 1;
    			    regs->depout[epnum].doepint = clear;
					dwc_handle_ep0(dwc);
				} else {
					dwc_complete_ep(dwc, epnum, 0);
				}
			}
			/* Endpoint disable  */
			if (doepint.epdisabled) {
				/* Clear the bit in DOEPINTn for this interrupt */
			    doepint_t clear = {0};
                clear.epdisabled = 1;
			    regs->depout[epnum].doepint = clear;
			}
			/* AHB Error */
			if (doepint.ahberr) {
			    doepint_t clear = {0};
                clear.ahberr = 1;
			    regs->depout[epnum].doepint = clear;
			}
			/* Setup Phase Done (contr0l EPs) */
			if (doepint.setup) {
			    doepint_t clear = {0};
                clear.setup = 1;
			    regs->depout[epnum].doepint = clear;
			}
		}
		epnum++;
		ep_intr >>= 1;
	}
}
