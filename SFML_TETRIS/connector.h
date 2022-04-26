#ifndef SFML_TETRIS_CONNECTOR_H
#define SFML_TETRIS_CONNECTOR_H

#include <string>
using std::string;

#define BUF_SIZE 300

#ifdef _WIN32 || _WIN64

#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>

class connector {
    HANDLE hPipe = NULL;
    char buffer[BUF_SIZE];
    DWORD dwRead;
    DWORD dwWritten;
    string recievedMessage;
    DWORD totalAvailableBytes;

public:
    connector(string name);
    ~connector();
    string read();
    void write(char* buf, int size);
    void clear();

};

#else

#include <sys/stat.h>
#include <csignal>

#include <cstring>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <fstream>

class connector {
    char buffer[BUF_SIZE];
    string name;
    FILE *pipe;

public:
    explicit connector(string name);
    ~connector();
    string read();
    void write(char* buf, int size);
    void clear();
};

#endif


#endif //SFML_TETRIS_CONNECTOR_H
