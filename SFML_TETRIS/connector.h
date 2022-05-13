#ifndef SFML_TETRIS_CONNECTOR_H
#define SFML_TETRIS_CONNECTOR_H

#include <string>
using std::string;

#define BUF_SIZE 300

#ifdef _WIN32

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
public:
    char buffer[BUF_SIZE];
    string name;
    FILE *pipe;

    connector(string name);
    ~connector();
};

class innector : public connector {
    std::ifstream in;
public:
    innector(string name) : connector(name), in(name, std::ios::in) {};
    ~innector();
    string read();
};

class outnector : public connector {
    std::ofstream out;
public:
    outnector(string name) : connector(name), out(name, std::ios::out) {}
    ~outnector();
    void write(char* buf, int size);
};

#endif


#endif //SFML_TETRIS_CONNECTOR_H
