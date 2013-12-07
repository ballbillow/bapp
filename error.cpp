#include "bapp/error.h"

namespace bapp
{

const char* Error::getErrorString(ErrorType errorCode)
{
    static const char* errorTab[] =
    {
        "no error",
        "process cmd option fail",
        "process cmd option exit app",
        "tdr config load fail",
        "tlog config load fail",
        "net init fail",
        "net start fail",
        "net fini fail",
        "net stop fail",
    };
    
    int errorIndex = -1 * (int)errorCode;
    if (0 > errorIndex || errorIndex >= (int)(sizeof(errorTab)/sizeof(errorTab[0])))
    {
        return errorTab[0];
    } 
    else
    {
        return errorTab[errorIndex];
    }
}

}
