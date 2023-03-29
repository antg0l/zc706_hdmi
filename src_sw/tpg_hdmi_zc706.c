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

u16 video_mode = 1; //0 - 800x600, 1 - 1024x768
u32 tpg_height,tpg_width, clk_div;
u16 VTC_mode;

int main()
{
	int Status;
    init_platform();

    // set variables for selected mode:
    switch(video_mode)
    	{
    	case 0: // 800x600
    	{
    		// for TPG
    		tpg_height  = 600;
    		tpg_width  = 800;

    		// For the clocking wizard
    		clk_div = 0x001A; //x1A = 26 for 40MHz (800x600)

    		// For VTC
    		VTC_mode = XVTC_VMODE_SVGA;

    		break;
    	}
    	case 1: // 1024x768
    	{
    		// for TPG
    		tpg_height  = 768;
    		tpg_width  = 1024;

    		// For the clocking wizard
    		clk_div = 0x0010; //x10 = 16 for 65MHz (1024x768)

    		// For VTC
    		VTC_mode = XVTC_VMODE_XGA;

    		break;
    	}
    	case 2: // 1280x720 -- not working
    	{
    		// for TPG
    		tpg_height  = 720;
    		tpg_width  = 1280;

    		// For the clocking wizard
    		clk_div = 0x000D; //xE = 14 for 74.25MHz (1280x720)

    		// For VTC
    		VTC_mode = XVTC_VMODE_720P;

    		break;
    	}
    	case 3: // 640x480
    	{
    		// for TPG
    		tpg_height  = 480;
    		tpg_width  = 640;

    		// For the clocking wizard
    		clk_div = 0x0028; //x29 = 41

    		// For VTC
    		VTC_mode = XVTC_VMODE_VGA;

    		break;
    	}
    	// add other video formats here
    	}


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
    // Initialise the VTC
    XVtc_Config *VTC_Config = XVtc_LookupConfig(XPAR_V_TC_0_DEVICE_ID);
    XVtc_CfgInitialize(&VtcInst, VTC_Config, VTC_Config->BaseAddress);

    //XVtc_DisableGenerator(&VtcInst);

    //Initialize the clocking wizard
    XClk_Wiz_Config *ClkWiz_CfgPtr = XClk_Wiz_LookupConfig(XPAR_CLK_WIZ_0_DEVICE_ID);
    XClk_Wiz_CfgInitialize(&ClkWizInst, ClkWiz_CfgPtr, ClkWiz_CfgPtr->BaseAddr);

	/* Clocking Wizard Configuration */
    Xil_Out32(ClkWiz_CfgPtr->BaseAddr + 0x208, clk_div); //x1A = 26 for 40MHz (800x600),
    //Xil_Out32(ClkWiz_CfgPtr->BaseAddr + 0x208, 0x0010);   //x10 = 16 for 65MHz (1024x768)
    Xil_Out32(ClkWiz_CfgPtr->BaseAddr + 0x25C, 0x3);

	/* End of clocking wizard configuration */
    usleep(10);


    // Set Resolution to 800x600
    XV_tpg_Set_height(&tpg_inst, tpg_height);
    XV_tpg_Set_width(&tpg_inst, tpg_width);
    //XV_tpg_Set_height(&tpg_inst, 768);
    //XV_tpg_Set_width(&tpg_inst, 1024);

    // Set Color Space to YUV422
    XV_tpg_Set_colorFormat(&tpg_inst, 0x1);
    //XV_tpg_Set_colorFormat(&tpg_inst, 0x2);

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
    //XVtc_Config *VTC_Config = XVtc_LookupConfig(XPAR_V_TC_0_DEVICE_ID);
    //XVtc_CfgInitialize(&VtcInst, VTC_Config, VTC_Config->BaseAddress);

	/* VTC Configuration */
    XVtc_ConvVideoMode2Timing(&VtcInst,VTC_mode,&XVtc_Timingconf);
    //XVtc_ConvVideoMode2Timing(&VtcInst,XVTC_VMODE_XGA ,&XVtc_Timingconf);
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


