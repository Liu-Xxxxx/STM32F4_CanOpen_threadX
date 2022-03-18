#ifndef _CANOPEN_DRV_H
#define _CANOPEN_DRV_H

/* 包含的头文件 --------------------------------------------------------------*/
#include "User_App.h"

typedef struct {
	CAN_TxHeaderTypeDef tx_header;
	uint8_t buf[8];
}canSendHander_def;

typedef struct {
	CAN_RxHeaderTypeDef rx_header;
	uint8_t buf[8];
}canRecvHander_def;


/* 函数申明 ------------------------------------------------------------------*/
void CANSend_Date(canSendHander_def *TxMsg);
void CANRcv_DateFromISR(canRecvHander_def *RxMsg);
void TIMx_DispatchFromISR(void);


#endif /* _CANOPEN_DRV_H */
