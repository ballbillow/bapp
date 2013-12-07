/// @file:   application.h
/// @brief:
/// @author: billowqiu

#ifndef BAPP_SERVERAPPLICATION_H
#define BAPP_SERVERAPPLICATION_H
#include <boost/noncopyable.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <stdint.h>
#include <string>
#include "bnet/asyncprocessor.h"
#include "bapp/error.h"

namespace bapp
{

class Application : public boost::noncopyable
{
public:
    Application();
    virtual ~Application();

    static Application* Instance();
    bnet::AsyncProcessor* GetBaseProcessor();
    Error::ErrorType AddTimer(uint16_t interval, TimerJobFunc func);
    virtual Error::ErrorType Run(int argc, const char* argv[]);

protected:
    virtual Error::ErrorType Initalize() = 0;
    virtual Error::ErrorType Finitalize() = 0;
    virtual Error::ErrorType Start() = 0;
    virtual Error::ErrorType Stop() = 0;
    virtual Error::ErrorType Reload() = 0;
	virtual Error::ErrorType PraseAppConfig() = 0;
private:
    Error::ErrorType ProcessCmdOption(int argc, const char* argv[]);
    
    Error::ErrorType LoadLogConfig();
	Error::ErrorType LoadAppConfig();
	
    void Daemon();
    void InitSignal();

    static void StopSignal(int signum);
    void StopInternal();

    static void ReloadSignal(int signum);
    void ReloadInternal();

protected:
    bool inited_;
    bool stop_;
    bool reload_;
    bool daemon_;
    std::string app_config_file_;
    boost::property_tree::ptree app_config_;
    std::string glog_config_file_;
    std::string current_path_;
    std::string app_name_;
    bnet::AsyncProcessor base_processor_;

private:
    static Application* instance_;
};

}

#endif // BAPP_SERVERAPPLICATION_H
