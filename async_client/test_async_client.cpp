#include "async_client.h"
#include <iostream>
#include <string>
#include <unistd.h>

#include <signal.h>
#include "helper.h"

class TestAsyncClientCallback : public AsyncClientCallback {
public:
    virtual void onConnected(AsyncClient* client) {
        std::string auth = "auth next2015\r\n";
        client->write(auth.data(), auth.size());
    }
};

void parseResp(vcs::Buffer* buffer) {
    char buf[1024] = {0};
    size_t len = buffer->read(buf, 1024);
    std::cout << "return size=" << len << ",data=[" << buf << "]" << std::endl;
}

int main() {
    if(!vcs::Helper::Application::setSignalHandler(SIGPIPE, SIG_IGN)){
        std::cerr << "failed to set SIGPIPE to SIG_IGN" << std::endl;
        exit(-1);
    }

    TestAsyncClientCallback* callback = new TestAsyncClientCallback();
    AsyncClient client("127.0.0.1", 6379, callback);

    vcs::Buffer buffer;
    std::string setCmd = "set me pesiwang\r\n";
    std::string getCmd = "get me\r\n";

    int i = 0;
    while (true) {
        if (i % 2 == 0)
            client.write(setCmd.data(), setCmd.size());
        else 
            client.write(getCmd.data(), getCmd.size());

        client.read(&buffer);
        client.dispatch();

        parseResp(&buffer);
        sleep(1);
        i++;
    }

    delete callback;
    return 0;
}

