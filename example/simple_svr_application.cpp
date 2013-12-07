#include <iostream>
#include <unistd.h>
#include <boost/bind.hpp>
#include <glog/logging.h>
#include "bapp/application.h"

struct SimpleAppConfig
{
	std::string listen_ip_;
	int16_t listen_port_;
};

class SimpleApplication : public bapp::Application
{
public:
    SimpleApplication()
    {
    }
    virtual ~SimpleApplication()
    {
    }

protected:
    virtual bapp::Error::ErrorType Initalize()
    {
        uint16_t id = 0;
        base_processor_.AddTimerJob(5000, boost::bind(&SimpleApplication::TimerFunc, this, _1), id);
        base_processor_.AddJob(boost::bind(&SimpleApplication::WorkFunc, this));

        return bapp::Error::BAPP_SUCCESS;
    }
	
	virtual bapp::Error::ErrorType PraseAppConfig()
	{				
		SimpleAppConfig config;
		config.listen_ip_ = app_config_.get("simple_app.listen_ip", "10.12.123.11");
		config.listen_port_ = app_config_.get("simple_app.listen_port", 1988);
		
		LOG(INFO) << config.listen_ip_;
		LOG(ERROR) << config.listen_port_;
		
		return bapp::Error::BAPP_SUCCESS;
	}

    void WorkFunc()
    {
        sleep(60);
    }

    void TimerFunc(const boost::system::error_code& ec)
    {
		 LOG(INFO) << __FUNCTION__;
    }

    virtual bapp::Error::ErrorType Finitalize()
    {
        return bapp::Error::BAPP_SUCCESS;
    }

    virtual bapp::Error::ErrorType Start()
    {
        while(!stop_)
        {
            base_processor_.RunOne();
        }
        return bapp::Error::BAPP_SUCCESS;
    }

    virtual bapp::Error::ErrorType Stop()
    {
		LOG(INFO) << __FUNCTION__;
		
        return bapp::Error::BAPP_SUCCESS;
    }

    virtual bapp::Error::ErrorType Reload()
    {
		LOG(INFO) << __FUNCTION__;
		
		PraseAppConfig();
		
        return bapp::Error::BAPP_SUCCESS;
    }

};

int main(int argc, const char* argv[])
{
    SimpleApplication app;    
    app.Run(argc, argv);

    return 0;
}
