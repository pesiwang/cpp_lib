#include "kv_map_client.h"
#include <sys/time.h>
#include <iostream>
#include <string>
#include <unistd.h>

#include <signal.h>
#include "helper.h"

static inline int16_t unsigned_to_signed(uint32_t i, uint32_t max_positive)
{
    if (i < max_positive) {
        return i;
    } else {
        return i - (max_positive * 2);
    }
}

static inline int64_t pythonmodulo(int64_t i, int32_t mod)
{
    if (i >= 0) {
        return i % mod;
    }
    return mod - ((-i) % mod);
}

void getIntegerAsBlock(int64_t i, int16_t& x, int16_t& y, int16_t& z)
{
    x = unsigned_to_signed(pythonmodulo(i, 4096), 2048);
    i = (i - x) / 4096;
    y = unsigned_to_signed(pythonmodulo(i, 4096), 2048);
    i = (i - y) / 4096;
    z = unsigned_to_signed(pythonmodulo(i, 4096), 2048);
}

int64_t getBlockAsInteger(int16_t x, int16_t y, int16_t z)
{
    return (int64_t) z * 0x1000000 +
        (int64_t) y * 0x1000 +
        (int64_t) x;
}

void testGet(KvMapClient* kvMapClient, int x, int y, int z) {
    static int total = 0;
    static int cnt = 0;
    ++total;
    if (++cnt % 1000 == 0) {
        std::cout << "get request, total=" << total << std::endl;
        cnt = 0;
    }

    kvMapClient->get(getBlockAsInteger(x, y, z));
}

bool testSet(KvMapClient* kvMapClient, const std::map<int64_t, std::string>& blockStrMap) {
    if (!blockStrMap.empty()) {
        static int total = 0;
        static int cnt = 0;
        total += blockStrMap.size();
        if (++cnt % 100 == 0) 
        {
            std::cout << "this time recv " << blockStrMap.size() << ",total=" << total << std::endl;
            cnt = 0;
        }
        if (total >= 10000) {
            return true;
        } else {
            return false;
        }
        /*
        for (const auto& p_v : blockStrMap) {
            int16_t x, y, z;
            getIntegerAsBlock(p_v.first, x, y, z);
            std::cout << "recv block pos=(" << x << "," << y << "," << z << ")" << std::endl; 
            kvMapClient->set(p_v.first, p_v.second);
        }*/
    }
}

int main() {
    if(!vcs::Helper::Application::setSignalHandler(SIGPIPE, SIG_IGN)){
        std::cerr << "failed to set SIGPIPE to SIG_IGN" << std::endl;
        exit(-1);
    }

    KvMapClient* kvMapClient = new KvMapClient("183.6.232.33", 3454, 1);
    while(!kvMapClient->isConnected()) {
        std::map<int64_t, std::string> blockStrMap;
        kvMapClient->dispatch(blockStrMap);
    }
    struct  timeval start;
    gettimeofday(&start,NULL);
    while (true) {
        static bool sent = false;
        if (!sent) {
            int cnt = 0;
            bool done = false;
            for (int x = 0; x < 625 && !done; x++)
                for (int z = 0; z < 625 && !done; z++)
                    for (int y = -14; y < 8 && !done; y++) {
                        testGet(kvMapClient, x, y, z);
                        if (++cnt >= 10000){
                            done = true;
                            break;
                        }
                    }
            sent = true;
        }

        std::map<int64_t, std::string> blockStrMap;
        kvMapClient->dispatch(blockStrMap);
        testSet(kvMapClient, blockStrMap);
    }
    struct  timeval end;
    unsigned  long diff = 1000 * (end.tv_sec-start.tv_sec)+ end.tv_usec-start.tv_usec;;
    std::cout << "10000 get and response use milliseconds:" << diff << std::endl;
    return 0;
}

