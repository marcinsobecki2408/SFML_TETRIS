#include "connector.h"

#ifdef _WIN32

connector::connector(string name) {
    // TODO: Fix connectors
    for (int i = 0; i < name.length(); i++)
    {
        if (name[i] == '/')
            name[i] = '\\';
    }
    name.insert(0, "\\\\.");
    std::wstring nameW = std::wstring(begin(name), end(name));
    LPCWSTR wideName = nameW.c_str();
    // "\\\\.\\pipe\\TetrisPipe"
    // "\\\\.\\tmp\\biai_input"

    hPipe = CreateNamedPipe(wideName,
                            PIPE_ACCESS_DUPLEX,
                            PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,   // FILE_FLAG_FIRST_PIPE_INSTANCE is not needed but forces CreateNamedPipe(..) to fail if the pipe already exists...
                            2,
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
    //if (FALSE == PeekNamedPipe(hPipe,
    //                           0,
    //                           0,
    //                           0,
    //                           &totalAvailableBytes,
    //                           0))
    //{
    //    // Handle failure. (Me)
    //}
    //    // If there is some data, read it
    //else if (totalAvailableBytes > 0)
    //{
        if (ReadFile(hPipe, buffer, sizeof(buffer) - 1, &dwRead, NULL) != FALSE)
        {
            recievedMessage = std::string(buffer);
            if (recievedMessage.size() > 1)
                recievedMessage.pop_back();
        }
    //}
    return recievedMessage;
}

void connector::write(char* buf, int size) {
    // todo
    WriteFile(hPipe,
        buf,
        size + 1,   // = length of string + terminating '\0' !!!
        &dwWritten,
        NULL);
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

string innector::read() {
    string str;
    std::getline(in, str);
    return str;
}

innector::~innector() {
    in.close();
}

void outnector::write(char* buf, int size) {
    out.write(buf, size);
    out << std::endl;
//    out.flush();
}

outnector::~outnector() {
    out.close();
}

#endif