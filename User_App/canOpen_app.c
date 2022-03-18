#include "User_App.h"

extern CAN_HandleTypeDef hcan1;

TX_THREAD cnaOpenRecv_thread;
TX_EVENT_FLAGS_GROUP canOpen_flags;
uint32_t canOpenRecv_STK[DEF_THREAD_STK_SIZE / 4];

static canRecvHander_def RxMsg;

void canOpenRecv_entry(ULONG input);

void CANOpen_Config(void)
{
    /*##-1- Configure the CAN Filter ###########################################*/
    CAN_FilterTypeDef sFilterConfig;
    sFilterConfig.FilterBank = 0;
    sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
    sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
    sFilterConfig.FilterIdHigh = 0x0000;
    sFilterConfig.FilterIdLow = 0x0000;
    sFilterConfig.FilterMaskIdHigh = 0x0000;
    sFilterConfig.FilterMaskIdLow = 0x0000;
    sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
    sFilterConfig.FilterActivation = ENABLE;
    sFilterConfig.SlaveStartFilterBank = 14;

    if (HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig) != HAL_OK)
    {
        /* Filter configuration Error */
        Error_Handler();
    }

    /*##-2- Start the CAN peripheral ###########################################*/
    if (HAL_CAN_Start(&hcan1) != HAL_OK)
    {
        /* Start Error */
        Error_Handler();
    }

    /*##-3- Activate CAN  notification #######################################*/
    HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
}

void CANOpen_Driver_Init(void)
{
    uint32_t sta;
    sta = tx_thread_create(&cnaOpenRecv_thread,
                           "canOpenRecv_thread",
                           canOpenRecv_entry,
                           NULL,
                           canOpenRecv_STK,
                           DEF_THREAD_STK_SIZE,
                           CANOPEN_RECV_PRIO,
                           CANOPEN_RECV_PRIO,
                           TX_NO_TIME_SLICE,
                           TX_AUTO_START);
    if (sta != TX_SUCCESS)
    {
        printf("canOpenRecv_thread create failed: %d\n", sta);
        Error_Handler();
    }

    sta = tx_event_flags_create(&canOpen_flags, "canOpen_flags");
    if (sta != TX_SUCCESS)
    {
        printf("canOpen_flags create failed: %d\n", sta);
        Error_Handler();
    }
}

void canOpenRecv_entry(ULONG input)
{
    uint32_t sta;
    ULONG actual_events;

    CANOpen_Config();
    setNodeId(&OD_gateway_Data, ID_gateway);
    setState(&OD_gateway_Data, Initialisation);
    setState(&OD_gateway_Data, Operational);

    while (1)
    {
        sta = tx_event_flags_get(&canOpen_flags,
                                 CANOPEN_RECV_FLAG,
                                 TX_AND_CLEAR,
                                 &actual_events,
                                 TX_WAIT_FOREVER);
        if (sta != TX_SUCCESS)
        {
            printf("canopen recv get flags failed: %d\n", sta);
            Error_Handler();
        }

        CANRcv_DateFromISR(&RxMsg);
    }
}
/**
 * @brief  Rx Fifo 0 message pending callback
 * @param  hcan: pointer to a CAN_HandleTypeDef structure that contains
 *         the configuration information for the specified CAN.
 * @retval None
 */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    uint32_t sta;
    /* Get RX message */
    if (HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &RxMsg.rx_header, RxMsg.buf) != HAL_OK)
    {
        /* Reception Error */
        printf("can get txmessage failed\n");
        Error_Handler();
    }
    tx_event_flags_set(&canOpen_flags, CANOPEN_RECV_FLAG, TX_OR);
    if (sta != TX_SUCCESS)
    {
        printf("canopen set recv flag failed :%d\n");
        Error_Handler();
    }
}

void _heartbeatError(CO_Data *d, UNS8 heartbeatID)
{
    printf("heartbeatError: %d\n", heartbeatID);
}
void _post_SlaveBootup(CO_Data *d, UNS8 SlaveID)
{
    printf("slaveBootup: %d\n", SlaveID);
}
void _post_SlaveStateChange(CO_Data *d, UNS8 nodeId, e_nodeState newNodeState)
{
    printf("slaveID: %d StateChange: %d\n", nodeId, newNodeState);
}
void _nodeguardError(CO_Data *d, UNS8 id)
{
    printf("nodeguardError: %d\n", id);
}
