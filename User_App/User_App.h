#ifndef __USER_APP_H
#define __USER_APP_H

#include "main.h"
#include "SEGGER_RTT.h"
#include "tx_api.h"
#include <stdio.h>
#include "canopen_drv.h"
#include "OD_gateway.h"

// #define SEND_BASE_INFO
enum nodeId_name
{
    ID_gateway = 0,
    ID_hc_board,
    ID_sol_driver,
    ID_psnr_board,
    ID_psnr_bd_dau,
    ID_meter1_2,
    ID_meter3_4,
    ID_meter5_6,
};

#define DEF_THREAD_STK_SIZE 1024
#define INIT_THREAD_PRIO 2
#define CANOPEN_RECV_PRIO 15

#define CANOPEN_RECV_FLAG 0x01

#define printf(...) SEGGER_RTT_printf(0, __VA_ARGS__)
#endif
