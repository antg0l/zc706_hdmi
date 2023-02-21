/******************************************************************************
 *  Copyright (c) 2018, Xilinx, Inc.
 *  All rights reserved.
 * 
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions are met:
 *
 *  1.  Redistributions of source code must retain the above copyright notice, 
 *     this list of conditions and the following disclaimer.
 *
 *  2.  Redistributions in binary form must reproduce the above copyright 
 *      notice, this list of conditions and the following disclaimer in the 
 *      documentation and/or other materials provided with the distribution.
 *
 *  3.  Neither the name of the copyright holder nor the names of its 
 *      contributors may be used to endorse or promote products derived from 
 *      this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  OR BUSINESS INTERRUPTION). HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
 *  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
 *  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
 *  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *****************************************************************************/

/******************************************************************************
 * @file iic_utils.h
 *
 * Library files for IIC control on ZC702
 *
 * <pre>
 * MODIFICATION HISTORY:
 *
 * Ver   Who          Date     Changes
 * ----- ------------ -------- -----------------------------------------------
 * 1.00  Florent Werb 12/05/18 Initial Version
 * </pre>
 *
 *****************************************************************************/

#ifndef SRC_IIC_UTILS_H_
#define SRC_IIC_UTILS_H_

	#include "xparameters.h"
	#include "xiicps.h"
	#include "sleep.h"

	#define IIC_SCLK_RATE		100000

	//ADV7511
	#define ADV7511_HPD_CTRL_MASK	0x40 // bit 6 = state of HPD
	#define ADV7511_HDP_REG_ADDR	0x42


	int ps_iic_init(u16 DeviceId, XIicPs* IicPs);
	int iic_write( XIicPs *IicPs, u8 Address, u8 Data, s32 ByteCount);
	int iic_write2( XIicPs *IicPs, u8 Address, u8 Register, u8 Data );
	int iic_read( XIicPs *IicPs, u8 Address, u8 Register, u8 *Data, int ByteCount);
	void set_iic_mux( XIicPs *IicPs, u8 MuxSelect, u8 Address);
	int check_hdmi_hpd_status(XIicPs *IicPs, u8 Address);
	void configure_adv7511(XIicPs *IicPs, u8 Address);
	void configure_adv7511_zc702(XIicPs *IicPs, u8 Address);
	void configure_adv7511_zc706(XIicPs *IicPs, u8 Address);
	void wait_for_monitor(XIicPs *IicPs, u8 Address);

#endif /* SRC_IIC_UTILS_H_ */
