#include "bapp/timerbase.h"
#include <boost/bind.hpp>

namespace bapp
{

TimerBase::TimerBase( bnet::AsyncProcessor* processor ):
timer_(processor->Service())
{

}

TimerBase::~TimerBase()
{

}

void TimerBase::SetInterval( uint32_t msec )
{
    duration_ = boost::posix_time::microseconds(msec);
    SetTimer();
}

void TimerBase::SetTimer()
{
    timer_.expires_from_now(duration_);
    timer_.async_wait(boost::bind(&TimerBase::InternalTimer, this, _1));
}

void TimerBase::InternalTimer( const boost::system::error_code& ec )
{
    //callback
    Timer();
    //Re expiry time
    SetTimer();
}

}
