#include "sync_logger.h"

#include <sstream>

int main() {
    
    SyncLogger::instance()->init(".", "pay");
    for (int i = 0; i < 10; ++i) {
        std::stringstream ss;
        ss << "i is " << i;
        SyncLogger::instance()->write(ss.str());
    }

    return 0;
}

