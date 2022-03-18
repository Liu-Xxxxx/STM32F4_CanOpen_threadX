#include "canopen_drv.h"
#include "OD_gateway.h"

extern CAN_HandleTypeDef hcan1;

/* ��̬���� ------------------------------------------------------------------*/

/* ��ʱ��TIM��ر��� */
uint32_t NextTime = 0; //��һ�δ���ʱ�����
uint32_t last_time_set = 0;
/* ��̬���� ------------------------------------------------------------------*/
uint32_t TxMailbox;

/************************************************
�������� �� CANRcv_DateFromISR
��    �� �� CAN��������
��    �� �� RxMsg --- ��������(����)
�� �� ֵ �� ��
*************************************************/
void CANRcv_DateFromISR(canRecvHander_def *RxMsg)
{
	static Message msg;
	uint8_t i = 0;

	msg.cob_id = RxMsg->rx_header.StdId; // CAN-ID

	if (CAN_RTR_REMOTE == RxMsg->rx_header.RTR)
		msg.rtr = 1; //Զ��֡
	else
		msg.rtr = 0; //����֡

	msg.len = RxMsg->rx_header.DLC; //����

#ifdef SEND_BASE_INFO
	printf("RX ID: 0x%X, Len: %d, ", msg.cob_id, msg.len);
#endif

	for (i = 0; i < RxMsg->rx_header.DLC; i++) //����
	{
		msg.data[i] = RxMsg->buf[i];

#ifdef SEND_BASE_INFO
		printf("0x%X ", msg.data[i]);
#endif
	}

#ifdef SEND_BASE_INFO
	printf("\n");
#endif

	canDispatch(&OD_gateway_Data, &msg); //����Э����ؽӿ�
}

/********************************** CANOpen�ӿں���(���Լ�ʵ��) **********************************/
/************************************************
�������� �� canSend
��    �� �� CAN����
��    �� �� notused --- δʹ�ò���
			m --------- ��Ϣ����
�� �� ֵ �� 0:ʧ��  1:�ɹ�
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
�������� �� setTimer
��    �� �� Set the timer for the next alarm.
��    �� �� value --- ����
�� �� ֵ �� ��
*************************************************/
void setTimer(TIMEVAL value)
{
	last_time_set = HAL_GetTick();
	NextTime = last_time_set + value;
}

/************************************************
�������� �� getElapsedTime
��    �� �� Return the elapsed time to tell the Stack how much time is spent since last call.
��    �� �� ��
�� �� ֵ �� (���ŵ�)ʱ��
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
�������� �� TIMx_DispatchFromISR
��    �� �� ��ʱ����(�Ӷ�ʱ���ж�)
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void TIMx_DispatchFromISR(void)
{
	if (NextTime == HAL_GetTick())
	{
		TimeDispatch();
	}
}

/**** Copyright (C)2018 strongerHuang. All Rights Reserved **** END OF FILE ****/
