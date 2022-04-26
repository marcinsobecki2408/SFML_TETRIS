#include "connector.h"

#ifdef _WIN32 || _WIN64

connector::connector(string name) {
    // "\\\\.\\pipe\\TetrisPipe"
    hPipe = CreateNamedPipe(TEXT(name),
                            PIPE_ACCESS_DUPLEX,
                            PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,   // FILE_FLAG_FIRST_PIPE_INSTANCE is not needed but forces CreateNamedPipe(..) to fail if the pipe already exists...
                            1,
                            BUF_SIZE * 16,
                            BUF_SIZE * 16,
                            NMPWAIT_USE_DEFAULT_WAIT,
                            NULL);
    // Get message from pipe client
    if (hPipe != INVALID_HANDLE_VALUE)
    {
        ConnectNamedPipe(hPipe, NULL);   // wait for someone to connect to the pipe
    }
}

connector::~connector() {
    // Pipe shutdown
    DisconnectNamedPipe(hPipe);
}

string connector::read() {
    if (FALSE == PeekNamedPipe(hPipe,
                               0,
                               0,
                               0,
                               &total_available_bytes,
                               0))
    {
        // Handle failure. (Me)
    }
        // If there is some data, read it
    else if (total_available_bytes > 0)
    {
        if (ReadFile(hPipe, buffer, sizeof(buffer) - 1, &dwRead, NULL) != FALSE)
        {
            recievedMessage = std::string(buffer);
            if (recievedMessage.size() > 1)
                recievedMessage.pop_back();
        }
    }
    return recievedMessage;
}

void connector::write(char* buf, int size) {
    // todo
}

void connector::clear() {
    // todo
}

#else

connector::connector(string name) : name(name) {
    umask(0);
    mkfifo(name.c_str(), S_IFIFO|0666);
}

connector::~connector() {
    unlink(name.c_str());
}

string connector::read() {
    string str;
    std::ifstream in(name, std::ios::in);
    std::getline(in, str);
    in.close();
    return str;
}

void connector::write(char* buf, int size) {
    std::ofstream out(name, std::ios::out);
    out.write(buf, size);
    out.close();
}

void connector::clear() { // maybe there is a better way
    return;
    std::ifstream in(name, std::ios::in);
    in.seekg(0, std::ifstream::end);
    long length = in.tellg();
    std::cout << length << std::endl;
    if (length > 0) {
        string str;
        std::getline(in, str);
    }
    in.seekg(0, std::ifstream::beg);
    in.close();
}

#endif