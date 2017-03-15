#include "sync_logger.h"

SyncLogger:: SyncLogger() {

}

SyncLogger::~SyncLogger() {
    if (m_ofs.is_open()) {
        m_ofs.close();
    }
}

void SyncLogger::init(const std::string& path, const std::string& prefix) {
    m_path = path;
    m_prefix = prefix;
    std::string currTime;
    getFormatTime(m_fileDate, currTime);
    m_ofs.open(getFilename(), std::ofstream::out|std::ofstream::app|std::ofstream::binary);
};

void SyncLogger::write(const std::string& msg) {
    std::string latestDate;
    std::string currTime;
    getFormatTime(latestDate, currTime);
    if (m_fileDate.compare(latestDate) != 0) {
        m_ofs.close();
        m_fileDate = latestDate;
        m_ofs.open(getFilename(), std::ofstream::out|std::ofstream::app|std::ofstream::binary);
    }
    m_ofs << "[" << currTime << "] " << msg << std::endl;
    m_ofs.flush();
};

std::string SyncLogger::getFilename() {
    return m_path + "/" + m_prefix + "_" + m_fileDate + ".log";
}

void SyncLogger::getFormatTime(std::string& dateStr, std::string& timeStr) {
    time_t timeNow;
    time(&timeNow);
    struct tm *timeInfo = localtime((const time_t*)&timeNow);

    char ds[64] = {'\0'};
    snprintf(ds, 64, "%04d%02d%02d", 
            1900 + timeInfo->tm_year,
            1 + timeInfo->tm_mon,
            timeInfo->tm_mday
            );
    dateStr = ds;
    
    char ts[64] = {'\0'};
    snprintf(ts, 64, "%04d-%02d-%02d %02d:%02d:%02d", 
            1900 + timeInfo->tm_year,
            1 + timeInfo->tm_mon,
            timeInfo->tm_mday,
            timeInfo->tm_hour,
            timeInfo->tm_min,
            timeInfo->tm_sec
            );
   timeStr = ts; 
}

