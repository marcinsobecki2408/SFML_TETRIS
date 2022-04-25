#include <iostream>
#include <windows.h> 
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include <string>
#include <windows.h> 

bool stopClient = false;

BOOL WINAPI consoleHandler(DWORD signal) {

    if (signal == CTRL_C_EVENT)
        stopClient = true;

    return TRUE;
}

int main()
{
    HANDLE hPipe;
    DWORD dwWritten;
    std::string message;

    hPipe = CreateFile(TEXT("\\\\.\\pipe\\TetrisPipe"), 
                        GENERIC_WRITE, 
                        0,
                        NULL,
                        OPEN_EXISTING,
                        0,
                        NULL);
    if (!SetConsoleCtrlHandler(consoleHandler, TRUE)) 
    {
        printf("\nERROR: Could not set control handler"); 
        return 1;
    }

    if (hPipe != INVALID_HANDLE_VALUE)
    {
        while(true)
        {
            if(stopClient)
                break;
            std::getline(std::cin, message);
            if(!message.empty())
                message.append("\n");
            WriteFile(hPipe,
                message.c_str(),
                message.length() + 1,   // = length of string + terminating '\0' !!!
                &dwWritten,
                NULL);
        }
        CloseHandle(hPipe);
    }

    return 0;
}