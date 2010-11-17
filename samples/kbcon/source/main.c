/* Sample msgDialog
 */ 

#include <psl1ght/lv2.h>
#include <psl1ght/lv2/timer.h>

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>

#include <sysutil/video.h>
#include <rsx/gcm.h>
#include <rsx/reality.h>
#include <rsx/screen.h>

#include <io/pad.h>
#include <io/kb.h>

#include <sysutil/events.h>
#include <sysutil/msgdialog.h>

#include <dbgcon/dbgcon.h>

void* con_handle = NULL;
static volatile u32 receive_exit_request = 0;
extern u64 sys_time_get_system_time();

/* Override printf to print in console */
#define printf(fmt, ...) con_printf(con_handle, fmt, ##__VA_ARGS__);

void appCleanup(void)
{
	sysUnregisterCallback(EVENT_SLOT0);
	ioPadEnd();
	ioKbEnd();	
	con_exit(con_handle);
}

static void eventHandle(u64 status, u64 param, void * userdata)
{
    (void)param;
    (void)userdata;
	if(status == EVENT_REQUEST_EXITAPP){
		printf("Quit exitapp requested\n");
		exit(0);
	}else{
		printf("Unhandled event: 0x%08llX\n", (unsigned long long int)status);
	}
}

void con_help(void)
{
	printf("Press pad button X to exit\n");
	printf("Press pad button O to printf_xy(x=0,y=0) timing informations\n");
	printf("Press pad button /\\ to printf() timing informations\n");
	printf("Press pad button [] to change fgcolor\n");
	printf("Press pad button SELECT to clear console framebuffer\n");
	printf("Press pad button START to display help\n");
}

s32 main(s32 argc, const char* argv[])
{
	int i;
	PadInfo2 padinfo2;
	PadData paddata;
	KbInfo kbinfo;
	KbData kbdata;
	VideoResolution screen_res;
	t_con_dimensions con_dim;
	
	screen_init(&screen_res);
	con_dim.width = screen_res.width;
	con_dim.height = screen_res.height;	
	con_handle = con_init(&con_dim);
	if(con_handle == NULL)
		return 0;
	
	printf("Start Test\n");
	
	ioPadInit(MAX_PORT_NUM);
	ioKbInit(MAX_KB_PORT_NUM);
	
	atexit(appCleanup);
	
	sysRegisterCallback(EVENT_SLOT0, eventHandle, NULL);

	/* Con Test */
	printf("Test 12345 with longgggggggggggggggg Textttttttttttt !!!!!!!!!!!!!!!!!!!!!!!\n");
	printf("Test 12345 with hugggggggggggggggggggggggggggeeeeeeeeeeeeeeeee Texttttttttttttttttttttttttttttttttttttttttttttttttttttttttt sizeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee !!!!!!!!!!!!!!!!!!!!!!! 0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ\n");
	printf("Test 1!\n");
	printf("Test 2!\n");
	printf("Test 3!\n");
	printf("Test 4!\n");
	printf("Test 5!\n");
	printf("Test 6!\n");
	printf("Test 7!\n");
	printf("Test 8!\n");
	printf("Test 9!\n");

	u32 w, h;
	con_dimensions_get(con_handle, &w, &h );
	printf("printf() This console has %u columns and %u rows\n", w, h );
	con_printf(con_handle, "con_printf() This console has %u columns and %u rows\n", w, h );

	con_fontsize_get(con_handle, &w, &h);
	printf("printf() This console has fontsize in pixel width=%u height=%u\n", w, h );	
	
	u64 beforeUsleep1=sys_time_get_system_time();	
	usleep(1);
	u64 afterUsleep1=sys_time_get_system_time();		
	
	u64 beforeUsleep100=sys_time_get_system_time();	
	usleep(100);
	u64 afterUsleep100=sys_time_get_system_time();	
	
	u64 beforeUsleep10000=sys_time_get_system_time();		
	usleep(10000);
	u64 afterUsleep10000=sys_time_get_system_time();	

	u64 uswait1Time = afterUsleep1 - beforeUsleep1;	
	u64 uswait100Time = afterUsleep100 - beforeUsleep100;	
	u64 uswait10000Time = afterUsleep10000 - beforeUsleep10000;
	printf("Timing check usleep(1)=%llu usleep(100)=%llu usleep(10000)=%llu\n",
			uswait1Time, uswait100Time, uswait10000Time);
	
	printf("You shall plug an USB keyboard to use this demo and to type text in console\n");
	con_help();
	
	u32 fgcolor = DEFAULT_FG_COLOR;
	u32 fgcol_state = 0;
	while(receive_exit_request == 0)
	{		
		u64 startFrame=sys_time_get_system_time();
		/* Check the pads. */
		u64 beforePad=sys_time_get_system_time();
		if( ioPadGetInfo2(&padinfo2) == 0)
		{
			for(i=0; i<MAX_PORT_NUM; i++)
			{
				if(padinfo2.port_status[i])
				{
					ioPadGetData(i, &paddata);
					if(paddata.BTN_CROSS){
						printf("PadButton \"X\" Exit.\r\n");
						receive_exit_request = 1;					
					}				
				}		
			}
		} else
		{
			/* Error exit */
			printf("ioPadGetInfo2 error Exit.\r\n");
			receive_exit_request = 1;
		}	
		u64 afterPad=sys_time_get_system_time();

		/* Keyboard */
		u64 beforeKb=sys_time_get_system_time();		
		/* Check Keyboards and output keys */
		ioKbGetInfo(&kbinfo);
		for(i=0; i<MAX_KEYBOARDS; i++)
		{
			if(kbinfo.status[i])
			{
				if( (ioKbRead(i, &kbdata)) == 0)
				{
					for(int j=0; j<kbdata.nb_keycode;j++)
					{
						u16 keycode;
						/*
						KbMkey mkey;
						KbLed led;
						*/
						keycode = kbdata.keycode[j];
						/*
						mkey.mkeys = kbdata.mkey.mkeys;
						led.leds = kbdata.led.leds;
						*/
						if(keycode != 0)
							printf("%c",(keycode&0x00FF));							
					}
				}
			}
		}	
		u64 afterKb=sys_time_get_system_time();
		
		u64 beforeWaitFlip=sys_time_get_system_time();
		waitFlip(); // Wait for the last flip to finish, so we can draw to the old buffer
		u64 afterWaitFlip=sys_time_get_system_time();
		
		/* Draw here in framebuffer buffers[currentBuffer]->ptr */
		u64 beforeCon_draw=sys_time_get_system_time();		
		con_draw(con_handle, buffers[currentBuffer]->ptr, buffers[currentBuffer]->width, buffers[currentBuffer]->height);
		u64 afterCon_draw=sys_time_get_system_time();	
		
		u64 beforeFlip=sys_time_get_system_time();			
		flip(currentBuffer); // Flip buffer onto screen
		currentBuffer = !currentBuffer;
		u64 afterFlip=sys_time_get_system_time();			
		
		/* Check system events */
		u64 beforeSysCheckCallback=sys_time_get_system_time();		
		sysCheckCallback();
		u64 afterSysCheckCallback=sys_time_get_system_time();

		u64 endFrame=sys_time_get_system_time();
		
		/* Timing statistics */
		u64 padTime=afterPad-beforePad;
		u64 kbTime=afterKb-beforeKb;
		u64 flipWaitTime=afterWaitFlip-beforeWaitFlip;
		u64 con_drawTime=afterCon_draw-beforeCon_draw;
		u64 flipTime=afterFlip-beforeFlip;
		u64 syscheckCBTime=afterSysCheckCallback-beforeSysCheckCallback;
		u64 totalTime=endFrame-startFrame;

		if(paddata.BTN_TRIANGLE)
		{
			/* Print timing */
			printf("Timeus pad=%llu, kb=%llu, Waitflip=%llu, condraw=%llu, flip=%llu sysCheckCB=%llu total=%llu=%.02f fps\n",
					padTime,
					kbTime,
					flipWaitTime,
					con_drawTime,
					flipTime,
					syscheckCBTime,
					totalTime,
					(1.0F/((float)(totalTime)/1000000.0F))
					);
		}		

		if(paddata.BTN_SQUARE)
		{
			/* Change FG color */
			printf("Before fgcolor=0x%08X\n", fgcolor);
			if(fgcol_state<=128)
			{
				/* Color change from WHITE to BLACK */
				fgcolor -= 0x00020202;
				fgcolor |= 0xFF000000;
			}else if(fgcol_state>128 && fgcol_state<=256)
			{
				/* Color change from WHITE to BLUE */
				fgcolor -= 0x00000002;
				fgcolor |= 0xFF000000;
			}else if(fgcol_state>256 && fgcol_state<=384)
			{
				/* Color change from WHITE to GREEN */
				fgcolor -= 0x00000200;
				fgcolor |= 0xFF000000;	
			}else if(fgcol_state>384 && fgcol_state<=512)
			{
				/* Color change from WHITE to RED */
				fgcolor -= 0x00020000;
				fgcolor |= 0xFF000000;	
			}else
			{
				fgcol_state = 0;
			}
			fgcol_state++;
			
			if( (fgcolor == 128) || (fgcolor == 256) || (fgcolor == 384) || (fgcolor == 512))			
				fgcolor = DEFAULT_FG_COLOR;
				
			con_fgcolor_set(con_handle, fgcolor);
			printf("After fgcolor=0x%08X\n", fgcolor);
		}

		if(paddata.BTN_SELECT)
		{
			con_clear(con_handle);
		}	
		
		if(paddata.BTN_START)
		{
			con_help();
		}		

		if(paddata.BTN_CIRCLE)
		{
			/* Print timing */
			con_printf_xy(con_handle, 0, 0, 
							" Timeus pad=%llu, kb=%llu, Waitflip=%llu, condraw=%llu, flip=%llu sysCheckCB=%llu total=%llu=%.02f fps      \n",
							padTime,
							kbTime,
							flipWaitTime,
							con_drawTime,
							flipTime,
							syscheckCBTime,
							totalTime,
							(1.0F/((float)(totalTime)/1000000.0F))
							);
		}

	}
	
	return 0;
}

