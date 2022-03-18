#include "canopen_drv.h"
#include "OD_gateway.h"

extern CAN_HandleTypeDef hcan1;

/* 静态变量 ------------------------------------------------------------------*/

/* 定时器TIM相关变量 */
uint32_t NextTime = 0; //下一次触发时间计数
uint32_t last_time_set = 0;
/* 静态申明 ------------------------------------------------------------------*/
uint32_t TxMailbox;

/************************************************
函数名称 ： CANRcv_DateFromISR
功    能 ： CAN接收数据
参    数 ： RxMsg --- 接收数据(队列)
返 回 值 ： 无
*************************************************/
void CANRcv_DateFromISR(canRecvHander_def *RxMsg)
{
	static Message msg;
	uint8_t i = 0;

	msg.cob_id = RxMsg->rx_header.StdId; // CAN-ID

	if (CAN_RTR_REMOTE == RxMsg->rx_header.RTR)
		msg.rtr = 1; //远程帧
	else
		msg.rtr = 0; //数据帧

	msg.len = RxMsg->rx_header.DLC; //长度

#ifdef SEND_BASE_INFO
	printf("RX ID: 0x%X, Len: %d, ", msg.cob_id, msg.len);
#endif

	for (i = 0; i < RxMsg->rx_header.DLC; i++) //数据
	{
		msg.data[i] = RxMsg->buf[i];

#ifdef SEND_BASE_INFO
		printf("0x%X ", msg.data[i]);
#endif
	}

#ifdef SEND_BASE_INFO
	printf("\n");
#endif

	canDispatch(&OD_gateway_Data, &msg); //调用协议相关接口
}

/********************************** CANOpen接口函数(需自己实现) **********************************/
/************************************************
函数名称 ： canSend
功    能 ： CAN发送
参    数 ： notused --- 未使用参数
			m --------- 消息参数
返 回 值 ： 0:失败  1:成功
*************************************************/
unsigned char canSend(CAN_PORT notused, Message *m)
{
	uint8_t i;
	static canSendHander_def TxMsg;
	HAL_StatusTypeDef sta;

	TxMsg.tx_header.StdId = m->cob_id;

	if (m->rtr)
		TxMsg.tx_header.RTR = CAN_RTR_REMOTE;
	else
		TxMsg.tx_header.RTR = CAN_RTR_DATA;

	TxMsg.tx_header.IDE = CAN_ID_STD;
	TxMsg.tx_header.DLC = m->len;

#ifdef SEND_BASE_INFO
	printf("TX ID: 0x%X, Len: %d, ", m->cob_id, m->len);
#endif

	for (i = 0; i < m->len; i++)
	{
		TxMsg.buf[i] = m->data[i];
#ifdef SEND_BASE_INFO
		printf("0x%X ", m->data[i]);
#endif
	}
#ifdef SEND_BASE_INFO
	printf("\n");
#endif
	while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0)
		;
	sta = HAL_CAN_AddTxMessage(&hcan1, &TxMsg.tx_header, TxMsg.buf, &TxMailbox);
	return sta;
}

/************************************************
函数名称 ： setTimer
功    能 ： Set the timer for the next alarm.
参    数 ： value --- 参数
返 回 值 ： 无
*************************************************/
void setTimer(TIMEVAL value)
{
	last_time_set = HAL_GetTick();
	NextTime = last_time_set + value;
}

/************************************************
函数名称 ： getElapsedTime
功    能 ： Return the elapsed time to tell the Stack how much time is spent since last call.
参    数 ： 无
返 回 值 ： (消逝的)时间
*************************************************/
TIMEVAL getElapsedTime(void)
{
	uint32_t timer;
	timer = HAL_GetTick();
	if (timer >= NextTime)
		return timer - NextTime;
	else
		return HAL_MAX_DELAY - last_time_set + NextTime;
}

/************************************************
函数名称 ： TIMx_DispatchFromISR
功    能 ： 定时调度(从定时器中断)
参    数 ： 无
返 回 值 ： 无
*************************************************/
void TIMx_DispatchFromISR(void)
{
	if (NextTime == HAL_GetTick())
	{
		TimeDispatch();
	}
}

/**** Copyright (C)2018 strongerHuang. All Rights Reserved **** END OF FILE ****/
