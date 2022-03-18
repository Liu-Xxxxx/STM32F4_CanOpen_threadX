#include "User_App.h"
#include "canopen_drv.h"
#include "OD_gateway.h"

void CANOpen_Driver_Init(void);

TX_THREAD init_thread;
uint32_t init_thread_STK[DEF_THREAD_STK_SIZE/4];
void init_thread_entry(ULONG input);

void user_app_entry(void)
{
	uint32_t sta;
	printf("gateway init\n");
	sta = tx_thread_create(&init_thread,
							"init_thread",
							init_thread_entry,
							NULL,
							init_thread_STK,
							DEF_THREAD_STK_SIZE,
							INIT_THREAD_PRIO,
							INIT_THREAD_PRIO,
							TX_NO_TIME_SLICE,
							TX_AUTO_START );
	if(sta != TX_SUCCESS)
	{
		printf("init thread create failed: %d\n",sta);
		Error_Handler();
	}
}

void init_thread_entry(ULONG input)
{
	CANOpen_Driver_Init();
	while(1)
	{
		tx_thread_sleep(1);
		HAL_IncTick();
		TIMx_DispatchFromISR();
	}
}
