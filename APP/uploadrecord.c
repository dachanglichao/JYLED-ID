#include  "os_cpu.h"
#include  "app_cfg.h"
#include  "ucos_ii.h"
#include  "include.h"
#include "stdio.h"
#include "cJSON.h"
#include "stdlib.h"

void	Active_UpOffRe(void);

extern u8 isReg;
void App_TaskUploadRecord(void* p_arg)
{
	while(1)
	{
		if(isReg == 0)
		{
			
		}else
		{
			Active_UpOffRe();
		}
			
		OSTimeDlyHMSM(0, 0, 1, 0);//将任务挂起500ms	 		
	}
}


