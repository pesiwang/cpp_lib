#ifndef PAY_LOGGER_H
#define PAY_LOGGER_H

#include <string>
#include <fstream>
#include <iostream>

class SyncLogger {
public:
    static SyncLogger* instance() {
        static SyncLogger logger;
        return &logger;
    }

    SyncLogger();
    ~SyncLogger(); 

    void init(const std::string& path, const std::string& prefix);
    void write(const std::string& msg);

private:
    std::string getFilename();
    void getFormatTime(std::string& dateStr, std::string& timeStr);

private:
    std::string m_path;
    std::string m_prefix;
    std::string m_fileDate;
    std::ofstream m_ofs;
};

#endif

