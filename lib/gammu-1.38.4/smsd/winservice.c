/**
 * SMSD Windows Service
 */
/* Copyright (c) 2009 Michal Cihar <michal@cihar.com> */
/* Licensend under GNU GPL 2 */

#include <stdio.h>
#include <windows.h>
#include <winsvc.h>
#include <time.h>
#include <gammu-smsd.h>

#include "winservice.h"
#include "log.h"

char smsd_service_name[SERVICE_NAME_LENGTH] = "GammuSMSD";

/* Defined in main.c */
extern GSM_SMSDConfig *config;

SERVICE_STATUS m_ServiceStatus;

SERVICE_STATUS_HANDLE m_ServiceStatusHandle;

void service_print_error(const char *info)
{
	char *lpMsgBuf;

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), /* Default language */
		(LPTSTR) &lpMsgBuf,
		0,
		NULL
	);
	fprintf(stderr, "Error %d: %s (%s)\n", (int)GetLastError(), lpMsgBuf, info);

	LocalFree(lpMsgBuf);
}

void WINAPI SMSDServiceCtrlHandler(DWORD Opcode)
{
	switch (Opcode) {
		case SERVICE_CONTROL_PAUSE:
			m_ServiceStatus.dwCurrentState = SERVICE_PAUSED;
			break;
		case SERVICE_CONTROL_CONTINUE:
			m_ServiceStatus.dwCurrentState = SERVICE_RUNNING;
			break;
		case SERVICE_CONTROL_STOP:
			m_ServiceStatus.dwWin32ExitCode = 0;
			m_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
			m_ServiceStatus.dwCheckPoint = 0;
			m_ServiceStatus.dwWaitHint = 0;

			SetServiceStatus(m_ServiceStatusHandle,
					 &m_ServiceStatus);
			SMSD_Shutdown(config);
			break;
		case SERVICE_CONTROL_INTERROGATE:
			break;
	}
	return;
}

BOOL report_service_status(DWORD CurrentState,
			DWORD Win32ExitCode,
			DWORD WaitHint)
{
	static DWORD CheckPoint = 1;

	m_ServiceStatus.dwServiceType = SERVICE_WIN32;
	m_ServiceStatus.dwCurrentState = CurrentState;
	m_ServiceStatus.dwWin32ExitCode = Win32ExitCode;
	m_ServiceStatus.dwServiceSpecificExitCode = 0;
	m_ServiceStatus.dwWaitHint = WaitHint;

	if (CurrentState == SERVICE_START_PENDING) {
		m_ServiceStatus.dwControlsAccepted = 0;
	} else {
		m_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
	}

	if ( (CurrentState == SERVICE_RUNNING) ||
			(CurrentState == SERVICE_STOPPED) ) {
		m_ServiceStatus.dwCheckPoint = 0;
	} else {
		m_ServiceStatus.dwCheckPoint = CheckPoint++;
	}

	// Report the status of the service to the SCM.
	return SetServiceStatus( m_ServiceStatusHandle, &m_ServiceStatus );
}

void WINAPI ServiceMain(DWORD argc, LPTSTR * argv)
{
	GSM_Error error;


	m_ServiceStatusHandle = RegisterServiceCtrlHandler(smsd_service_name,
							   SMSDServiceCtrlHandler);
	if (m_ServiceStatusHandle == (SERVICE_STATUS_HANDLE) 0) {
		service_print_error("Failed to initiate service");
		SMSD_LogErrno(config, "Failed to initiate service");
		return;
	}

	if (!report_service_status(SERVICE_START_PENDING, NO_ERROR, 3000)) {
		service_print_error("Failed to report state pending");
	}

	if (!report_service_status(SERVICE_RUNNING, NO_ERROR, 0)) {
		service_print_error("Failed to report state started");
	}

	error = SMSD_MainLoop(config, FALSE, 0);
	if (error != ERR_NONE) {
		report_service_status(SERVICE_STOPPED, error, 0);
		SMSD_LogErrno(config, "Failed to start SMSD");
		return;
	} else {
		report_service_status(SERVICE_STOPPED, NO_ERROR, 0);
	}
	return;
}

gboolean install_smsd_service(SMSD_Parameters * params)
{
	char config_name[MAX_PATH], program_name[MAX_PATH], commandline[3 * MAX_PATH];
	char service_display_name[MAX_PATH];

	HANDLE schSCManager, schService;
	char description[] = "Gammu SMS Daemon service";
	SERVICE_DESCRIPTION service_description;

	if (GetModuleFileName(NULL, program_name, sizeof(program_name)) == 0)
		return FALSE;

	if (GetFullPathName(params->config_file, sizeof(config_name), config_name, NULL) == 0)
		return FALSE;

	sprintf(commandline, "\"%s\" -S -c \"%s\" -n \"%s\" -f %d",
		program_name, config_name, smsd_service_name, params->max_failures);

	sprintf(service_display_name, "Gammu SMSD Service (%s)", smsd_service_name);

	schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (schSCManager == NULL) {
		return FALSE;
	}

	schService = CreateService(schSCManager,
				   smsd_service_name,
				   service_display_name, // service name to display
				   SERVICE_ALL_ACCESS,	// desired access
				   SERVICE_WIN32_OWN_PROCESS,	// service type
				   SERVICE_AUTO_START,	// start type
				   SERVICE_ERROR_NORMAL,	// error control type
				   commandline,	// service's binarygammu-smsd
				   NULL,	// no load ordering group
				   NULL,	// no tag identifier
				   NULL,	// no dependencies
				   NULL,	// LocalSystem account
				   NULL);	// no password

	if (schService == NULL) {
		return FALSE;
	}

	service_description.lpDescription = description;
	if (ChangeServiceConfig2(schService, SERVICE_CONFIG_DESCRIPTION, &service_description) == 0) {
		return FALSE;
	}

	CloseServiceHandle(schService);
	return TRUE;
}

gboolean uninstall_smsd_service(void)
{
	HANDLE schSCManager;

	SC_HANDLE hService;

	schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (schSCManager == NULL) {
		return FALSE;
	}

	hService = OpenService(schSCManager, smsd_service_name, SERVICE_ALL_ACCESS);

	if (hService == NULL) {
		return FALSE;
	}
	if (DeleteService(hService) == 0) {
		return FALSE;
	}
	if (CloseServiceHandle(hService) == 0) {
		return FALSE;
	}

	return TRUE;
}

gboolean stop_smsd_service(void)
{
	HANDLE schSCManager;

	SC_HANDLE hService;
	SERVICE_STATUS sstatus;

	schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (schSCManager == NULL)
		return FALSE;
	hService =
	    OpenService(schSCManager, smsd_service_name, SERVICE_ALL_ACCESS);
	if (hService == NULL)
		return FALSE;
	if (ControlService(hService, SERVICE_CONTROL_STOP, &sstatus) == 0)
		return FALSE;
	if (CloseServiceHandle(hService) == 0)
		return FALSE;

	return TRUE;
}

gboolean start_smsd_service(void)
{
	HANDLE schSCManager;

	SC_HANDLE hService;

	schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (schSCManager == NULL) {
		return FALSE;
	}

	hService = OpenService(schSCManager, smsd_service_name, SERVICE_ALL_ACCESS);

	if (hService == NULL) {
		return FALSE;
	}
	if (StartService(hService, 0, NULL) == 0) {
		return FALSE;
	}
	if (CloseServiceHandle(hService) == 0) {
		return FALSE;
	}

	return TRUE;
}

gboolean start_smsd_service_dispatcher(void)
{
	SERVICE_TABLE_ENTRY DispatchTable[] = {
		{smsd_service_name, ServiceMain},
		{NULL, NULL}
	};

	return StartServiceCtrlDispatcher(DispatchTable);
}

/* How should editor hadle tabs in this file? Add editor commands here.
 * vim: noexpandtab sw=8 ts=8 sts=8:
 */
