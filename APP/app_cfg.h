/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                          (c) Copyright 2003-2006; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                      APPLICATION CONFIGURATION
*
*                                     ST Microelectronics STM32
*                                              with the
*                                   STM3210B-LK1 Evaluation Board
*
* Filename      : app_cfg.h
* Version       : V1.10
* Programmer(s) : BH3NVN
*********************************************************************************************************
*/

#ifndef  __APP_CFG_H__
#define  __APP_CFG_H__

/*
*********************************************************************************************************
*                                       MODULE ENABLE / DISABLE
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                              TASKS NAMES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            TASK PRIORITIES
*********************************************************************************************************
*/

#define  	APP_TASK_START_PRIO               8
#define 	APP_TASK_EHTRECDATA_PRIO					10
#define 	APP_TASK_CARD_PRIO								11
#define 	APP_TASK_DOWN_FILE_PRIO						9
#define 	APP_TASK_UPLOAD_PRIO							12


/*
*********************************************************************************************************
*                                            TASK STACK SIZES
*                             Size of the task stacks (# of OS_STK entries)
*********************************************************************************************************
*/

#define  	APP_TASK_START_STK_SIZE               512
#define 	APP_TASK_EHTRECDATA_STK_SIZE					1024+200
#define 	APP_TASK_CARD_STK_SIZE								4096
#define		APP_TASK_DOWN_STK_SIZE								512
#define		APP_TASK_UPLOAD_STK_SIZE							512

/*
*********************************************************************************************************
*                                                  LIB
*********************************************************************************************************
*/

#define  uC_CFG_OPTIMIZE_ASM_EN                 DEF_ENABLED
#define  LIB_STR_CFG_FP_EN                      DEF_DISABLED

#endif
