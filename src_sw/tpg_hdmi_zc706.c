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
 * @file tpg_hdmi_zc702.c
 *
 * Main file for Video Series 21
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

#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"

#include "iic_utils.h"
#include "xv_tpg.h"
#include "zc706_hw.h"

#include "xvtc.h"
#include "xclk_wiz.h"

XIicPs IicPs_inst;
XV_tpg tpg_inst;
XV_tpg_Config *tpg_config;

XVtc VtcInst;
XVtc_Timing XVtc_Timingconf, XVtc_Timingconf1;

XClk_Wiz ClkWizInst;

int main()
{
	int Status;
    init_platform();

    print("TPG application on ZC706 using on-board HDMI\n\r");

    //Configure the PS IIC Controller
    ps_iic_init(XPAR_XIICPS_0_DEVICE_ID, &IicPs_inst);

    // Set the iic mux to the ADV7511
    set_iic_mux(&IicPs_inst, ZC702_I2C_SELECT_HDMI, ZC702_I2C_MUX_ADDR);

    //Wait for the monitor to be connected
    wait_for_monitor(&IicPs_inst, ZC702_HDMI_ADDR);

    // ADV7511 Basic Configuration
    configure_adv7511(&IicPs_inst,ZC702_HDMI_ADDR);
    // ADV7511 ZC702 Specific configuration
    configure_adv7511_zc706(&IicPs_inst,ZC702_HDMI_ADDR);

    xil_printf("HDMI Setup Complete!\r\n");

    /* Insert the code for the TPG here */
    Status = XV_tpg_Initialize(&tpg_inst, XPAR_V_TPG_0_DEVICE_ID);
    if(Status!= XST_SUCCESS)
    {
            xil_printf("TPG configuration failed\r\n");
            return(XST_FAILURE);
    }

    //Initialize the clocking wizard
    XClk_Wiz_Config *ClkWiz_CfgPtr = XClk_Wiz_LookupConfig(XPAR_CLK_WIZ_0_DEVICE_ID);
    XClk_Wiz_CfgInitialize(&ClkWizInst, ClkWiz_CfgPtr, ClkWiz_CfgPtr->BaseAddr);

	/* Clocking Wizard Configuration */
    Xil_Out32(ClkWiz_CfgPtr->BaseAddr + 0x208, 0x001A);
    Xil_Out32(ClkWiz_CfgPtr->BaseAddr + 0x25C, 0x3);

	/* End of clocking wizard configuration */


    // Set Resolution to 800x600
    XV_tpg_Set_height(&tpg_inst, 600);
    XV_tpg_Set_width(&tpg_inst, 800);

    // Set Color Space to YUV422
    XV_tpg_Set_colorFormat(&tpg_inst, 0x1);

    // Change the pattern to color bar
    XV_tpg_Set_bckgndId(&tpg_inst, XTPG_BKGND_RAINBOW_COLOR);

    // Set Overlay to moving box
    // Set the size of the box
    XV_tpg_Set_boxSize(&tpg_inst, 10);
    // Set the speed of the box
    XV_tpg_Set_motionSpeed(&tpg_inst, 2);
    // Enable the moving box
    XV_tpg_Set_ovrlayId(&tpg_inst, 1);

    //Start the TPG
    XV_tpg_EnableAutoRestart(&tpg_inst);
    XV_tpg_Start(&tpg_inst);
    xil_printf("TPG started!\r\n");

    /* End of TPG code*/


    // Initialise the VTC
    XVtc_Config *VTC_Config = XVtc_LookupConfig(XPAR_V_TC_0_DEVICE_ID);
    XVtc_CfgInitialize(&VtcInst, VTC_Config, VTC_Config->BaseAddress);

	/* VTC Configuration */
    XVtc_ConvVideoMode2Timing(&VtcInst,XVTC_VMODE_SVGA,&XVtc_Timingconf);
    XVtc_SetGeneratorTiming(&VtcInst, &XVtc_Timingconf);
    XVtc_RegUpdate(&VtcInst);
    //uint16_t aa = XVtc_GetGeneratorVideoMode(&VtcInst);
    //XVtc_SetGeneratorVideoMode(&VtcInst, XVTC_VMODE_SVGA);
    //XVtc_GetGeneratorTiming(&VtcInst, &XVtc_Timingconf1);
    //XVtc_SetGeneratorTiming(&VtcInst, &XVtc_Timingconf);
    //XVtc_RegUpdate(&VtcInst);

	/* End of VTC Configuration */

    //Start the VTC generator
    XVtc_EnableGenerator(&VtcInst);

	while(1);


    cleanup_platform();
    return 0;
}


