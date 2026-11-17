#pragma once
#include <cstddef>
#include <iostream>
#include <istream>
#include <ostream>
#include <memory>
#include <sched.h>
#include <string>
#include <sys/time.h>
#include <ctime>
#include <unistd.h>
#include <filesystem>
#include <fstream>
#include "Mutex.hpp"
#include <sstream>
namespace NS_LOG_MODULE {
    enum class LogLevel { INFO, WARING, ERROR, FATAL, DEBUG };
    std::string LogLevel2Message(LogLevel level) {
        switch(level)
        {
            case LogLevel::INFO:
                return "INFO";
            case LogLevel::WARING:
                return "WARING";
            case LogLevel::ERROR:
                return "ERROR";
            case LogLevel::FATAL:
                return "FATAL";
            case LogLevel::DEBUG:
                return "DEBUG";
            default:
                return "UNKNOWN";
        }
    }
    std::string GetCurrentTime() {
        struct timeval current_time;
        int n = gettimeofday(&current_time, nullptr);
        struct tm struct_time;
        localtime_r(&(current_time.tv_sec), &struct_time);
        char timestr[128];
        snprintf(timestr, sizeof(timestr), "%04d-%02d-%02d %02d:%02d:%02d",
                 struct_time.tm_year + 1900, struct_time.tm_mon + 1, struct_time.tm_mday,
                 struct_time.tm_hour, struct_time.tm_min, struct_time.tm_sec);
        return timestr;
    }
    class LogStrategy {
    public:
        virtual ~LogStrategy() = default;
        virtual void SyncLog(const std::string &message) = 0;

    private: 
    };
    class ConsoleStrategy : public LogStrategy {
    public:
        void SyncLog(const std::string &message) override {}
        ~ConsoleStrategy() {}

    private:
        Mutex _mutex;
    };
    const std::string defaultpath = "./log";
    const std::string defaultfilename = "log.txt";
    class FileLogStrategy : public LogStrategy {
    public:
        FileLogStrategy(const std::string &path = defaultpath,
                        const std::string &name = defaultfilename)
            : _logpath(path), _logfilename(name) {
            LockGuard lockguard(_mutex);
            if(std::filesystem::exists(_logpath))
                return;

            try
            { std::filesystem::create_directories(_logpath); }
            catch(const std::filesystem::filesystem_error &e)
            { std::cerr << e.what() << '\n'; }
            if(!_logpath.empty() && _logpath.back() != '/')
            {
                _logpath += "/";
            }
        }
        void SyncLog(const std::string &message) override {
            {
                LockGuard lockguard(_mutex);
                std::string targetlog = _logpath + _logfilename; // 这里为什么要放到临界区里面呢
                std::ofstream out(targetlog, std::ios::app);
                if(!out.is_open())
                {
                    std::cerr << "open" << targetlog << "failed" << std::endl;
                    return;
                }
                out << message << "\n";
                out.close(); // 这个放到析构函数那里会有什么问题?
            }
        }
        ~FileLogStrategy() {}

    private:
        std::string _logpath;
        std::string _logfilename;
        Mutex _mutex;
    };
    class Logger {
    public:
        Logger() {}
        void UseConsoleStrategy() {
            _strategy = std::make_unique<ConsoleStrategy>();
        }
        void UseFileStrategy() {
            _strategy = std::make_unique<FileLogStrategy>();
        }
        class LogMessage {
        public:
            LogMessage(LogLevel level, std::string &filename, int &line,Logger &logger)
                : _level(level), _curr_time(GetCurrentTime()), _pid(getpid()), _filename(filename),
                  _line(line),
                  _logger(logger) 
                  {
                std::stringstream ss;
                ss << "[" << _curr_time << "] "
                   << "[" << LogLevel2Message(_level) << "] "
                   << "[" << _pid << "] "
                   << "[" << _filename << "] "
                   << "[" << _line << "] "
                   << " - ";
                _loginfo = ss.str();
            }
            template <class T>
            LogMessage &operator<<(const T &info) {
                std::stringstream ss;
                ss << info;
                _loginfo += ss.str();
                return *this;
            }//
            ~LogMessage() {
                if(_logger._strategy)
                {
                    _logger._strategy->SyncLog(_loginfo);
                }
            }
        private:
            LogLevel _level;
            std::string _curr_time;
            pid_t _pid;
            std::string _filename;
            int _line;
            std::string _loginfo;
            Logger& _logger;
        };
        LogMessage operator()(LogLevel level, std::string &filename, int line) {
            return LogMessage(level, filename, line,*this);
        }
        ~Logger() {}

    private:
        std::unique_ptr<LogStrategy> _strategy;
    };
    Logger logger;
#define ENABLE_CONSOLE_LOG_STRATEGY logger.UseConsoleStrategy()
#define ENABLE_FILE_LOG_STRATEGY logger.UseFileStrategy()


} // namespace NS_LOG_MODULE