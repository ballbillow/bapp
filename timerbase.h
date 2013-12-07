
#ifndef BAPP_TIMERBASE_H
#define BAPP_TIMERBASE_H

#include <boost/noncopyable.hpp>
#include "bnet/asyncprocessor.h"

namespace bapp
{

class TimerBase : public boost::noncopyable
{
public:
    TimerBase(bnet::AsyncProcessor* processor);
    virtual ~TimerBase();

    void SetInterval(uint32_t msec);

    virtual void Timer() = 0;

private:
    void SetTimer();
    void InternalTimer(const boost::system::error_code& ec);

private:
    boost::asio::deadline_timer timer_;
    boost::asio::deadline_timer::duration_type duration_;
};

}

#endif // BAPP_TIMERBASE_H
