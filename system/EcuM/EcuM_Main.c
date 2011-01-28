/* -------------------------------- Arctic Core ------------------------------
 * Arctic Core - the open source AUTOSAR platform http://arccore.com
 *
 * Copyright (C) 2009  ArcCore AB <contact@arccore.com>
 *
 * This source code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by the
 * Free Software Foundation; See <http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt>.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 * -------------------------------- Arctic Core ------------------------------*/

//lint -emacro(904,VALIDATE,VALIDATE_RV,VALIDATE_NO_RV) //904 PC-Lint exception to MISRA 14.7 (validate macros).

#include "EcuM.h"
#include "EcuM_Cbk.h"
#include "EcuM_Internals.h"
#if defined(USE_DEM)
#include "Dem.h"
#endif
#if defined(USE_NVM)
#include "Nvm.h"
#endif

static uint32 internal_data_run_state_timeout = 0;
#if defined(USE_NVM)
static uint32 internal_data_go_off_one_state_timeout = 0;
#endif


void EcuM_enter_run_mode(void)
{
	internal_data.current_state = ECUM_STATE_APP_RUN;
	EcuM_OnEnterRUN(); /** @req EcuM2308 */
	//TODO: Call ComM_EcuM_RunModeIndication(NetworkHandleType Channel) for all channels that have requested run.
	internal_data_run_state_timeout = internal_data.config->EcuMRunMinimumDuration / ECUM_MAIN_FUNCTION_PERIOD; /** @req EcuM2310 */
}

static inline void enter_post_run_mode(void)
{
	internal_data.current_state = ECUM_STATE_APP_POST_RUN;
}

static inline void enter_prep_shutdown_mode(void)
{
	internal_data.current_state = ECUM_STATE_PREP_SHUTDOWN;
	EcuM_OnPrepShutdown();
}

static inline void enter_go_sleep_mode(void)
{
	internal_data.current_state = ECUM_STATE_GO_SLEEP;
	EcuM_OnGoSleep();
}

static inline void enter_go_off_one_mode(void)
{
	internal_data.current_state = ECUM_STATE_GO_OFF_ONE;
	EcuM_OnGoOffOne();

#if defined(USE_COMM)
	ComM_DeInit();
#endif

#if defined(USE_NVM)

	// Start NvM_WriteAll and timeout timer
	NvM_WriteAll();

	internal_data_go_off_one_state_timeout = internal_data.config->EcuMNvramWriteAllTimeout / ECUM_MAIN_FUNCTION_PERIOD;
#endif
}

static inline boolean hasRunRequests(void)
{
	uint32 result = internal_data.run_requests;

#if defined(USE_COMM)
	result |= internal_data.run_comm_requests;
#endif

	return (result != 0);
}

static inline boolean hasPostRunRequests(void)
{
	return (internal_data.postrun_requests != 0);
}

void EcuM_MainFunction(void)
{
#if defined(USE_NVM)
static NvM_RequestResultType writeAllResult;
#endif

VALIDATE_NO_RV(internal_data.initiated, ECUM_MAINFUNCTION_ID, ECUM_E_NOT_INITIATED);

	if (internal_data.current_state == ECUM_STATE_APP_RUN)
	{
		if (internal_data_run_state_timeout)
		{
			internal_data_run_state_timeout--;
		}

		if ((!hasRunRequests()) && (internal_data_run_state_timeout == 0))
		{
			EcuM_OnExitRun();	/** @req EcuM2865 */
			enter_post_run_mode();/** @req EcuM2865 */
			/*lint --e(904)*/ return;
		}
	}

	if (internal_data.current_state == ECUM_STATE_APP_POST_RUN)
	{
		if (hasRunRequests())
		{
			EcuM_enter_run_mode(); /** @req EcuM2866 */
			/*lint --e(904)*/ return;
		}

		if (!hasPostRunRequests())
		{
			EcuM_OnExitPostRun(); /** @req EcuM2761 */
			enter_prep_shutdown_mode();/** @req EcuM2761 */
			/*lint --e(904)*/ return;
		}
	}

	if (internal_data.current_state == ECUM_STATE_PREP_SHUTDOWN)
	{
#if defined(USE_DEM)
		// DEM shutdown
		Dem_Shutdown();
#endif

		// Switch shutdown mode
		if ((internal_data.shutdown_target == ECUM_STATE_OFF) || (internal_data.shutdown_target == ECUM_STATE_RESET)) {
			enter_go_off_one_mode();
		}

		if (internal_data.shutdown_target == ECUM_STATE_SLEEP) {
			enter_go_sleep_mode();
		}
	}

	if (internal_data.current_state == ECUM_STATE_GO_OFF_ONE)
	{
#if defined(USE_NVM)
		if (internal_data_go_off_one_state_timeout)
			internal_data_go_off_one_state_timeout--;

		// Wait for the NVM job (NvmWriteAll) to terminate
		NvM_GetErrorStatus(0, &writeAllResult);
		if ((writeAllResult != NVM_REQ_PENDING) || (internal_data_go_off_one_state_timeout == 0))
		{
			ShutdownOS(E_OK);
		}
#else
		ShutdownOS(E_OK);
#endif
	}

	if (internal_data.current_state == ECUM_STATE_GO_SLEEP)
	{
		// TODO: Fill out
	}

}
