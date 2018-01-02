// ClientPipeComm.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>

#define PIPE_NAME TEXT("\\\\.\\pipe\\myPipe.Comm")


HANDLE hPipeHandleReadWrite = INVALID_HANDLE_VALUE;
int MAX_WAIT_FOR_PIPE = 30000;
bool WAIT_FOREVER_FOR_PIPE = true;
BOOL bOpened = false;
BOOL bComplete = false;

DWORD Initialize();
DWORD WriteMessage(char * stMsg, int iMsgLen);
int main()
{
	Initialize();
	char* stBuffer1 = "Hello1";
	char* stBuffer2 = "Hello2";
	char* stBuffer3 = "Hello3";
	WriteMessage(stBuffer1, 6);
	WriteMessage(stBuffer2, 6);
	WriteMessage(stBuffer3, 6);
	system("pause");
    return 0;
}

DWORD Initialize()
{
	DWORD dwRetCode = 0;
	// Skip opening the pipe if we have already established a connection
	if (!bOpened || hPipeHandleReadWrite == INVALID_HANDLE_VALUE)
	{
		printf("\nRead/Write Pipe not already connected");

		int iWaitCount = 0;
		hPipeHandleReadWrite = CreateFile(PIPE_NAME, (GENERIC_READ | GENERIC_WRITE), (FILE_SHARE_WRITE | FILE_SHARE_READ), NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		while (hPipeHandleReadWrite == INVALID_HANDLE_VALUE)
		{
			// If we are not waiting forever and the pipe connect wait count is exceeded exit the loop
			if (iWaitCount > MAX_WAIT_FOR_PIPE && !WAIT_FOREVER_FOR_PIPE)
			{
				printf("\nFailed to create pipe connection in the given time: %d", GetLastError());
				dwRetCode = 1;
				break;
			}

			// Otherwise sleep a short while and 
			iWaitCount++;
			Sleep(10);

			hPipeHandleReadWrite = CreateFile((LPCWSTR)PIPE_NAME, (GENERIC_READ | GENERIC_WRITE), (FILE_SHARE_WRITE | FILE_SHARE_READ), NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			printf("Error:%d\n", GetLastError());
		}

		if (hPipeHandleReadWrite != INVALID_HANDLE_VALUE)
		{
			printf("\nPipe cocnnection established successfully");
			char* stBuffer = "Connected";
			dwRetCode = WriteMessage(stBuffer, 9);
			bOpened = true;
		}
		else
		{
			dwRetCode = 1;
		}
	}

	return dwRetCode;
}

DWORD WriteMessage(char * stMsg, int iMsgLen)
{
	DWORD dwRetCode = 0;
	printf("\nWrite Starts");

	if (hPipeHandleReadWrite != INVALID_HANDLE_VALUE)
	{
		DWORD dwBytes = 0;
		BOOL bdone = false;
		bdone = WriteFile(hPipeHandleReadWrite, stMsg, iMsgLen, &dwBytes, NULL);

		if (bdone == false)
		{
			DWORD dwError = 0;
			dwError = GetLastError();

			if (dwError != ERROR_IO_PENDING || dwError != ERROR_SUCCESS)
			{
				if (dwError == ERROR_BROKEN_PIPE)
				{
					dwRetCode = 1;
					printf("\nFailed to write to pipe: ERROR_BROKEN_PIPE");
				}
				else
				{
					printf("\nFailed to write to pipe, LastErrorCode: %d",dwRetCode);
				}
			}
		}
	}
	else
	{
		printf("\nPipe is an INVALID_HANDLE_VALUE, cannot write to pipe.");
		dwRetCode = 1;
	}

	printf("\nWrite Exit");
	return dwRetCode;
}
