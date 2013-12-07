// IEG - TSS
// Copyright 2012 Tencent Inc.  All rights reserved.
/********************************************************************
	created:	2012/09/24
	author:		billowqiu
	
	purpose:	定义下错误码，从tdr c++版本拖过来的
*********************************************************************/

#ifndef BAPP_ERROR_H
#define BAPP_ERROR_H

namespace bapp
{

class Error
{
public:
    enum ErrorType
    {
        BAPP_SUCCESS = 0,
        BAPP_PROCESS_CMD_OPTION_ERROR,
        BAPP_PROCESS_CMD_OPTION_EXIT,
        BAPP_TDRCONFIG_LOAD_ERROR,
        BAPP_TLOGCONFIG_LOAD_ERROR,
        BAPP_NET_INIT_ERROR,
        BAPP_NET_START_ERROR,
        BAPP_NET_FINI_ERROR,
        BAPP_NET_STOP_ERROR,
        BAPP_DAEMON_ERROR,
        BAPP_INIT_SIGNAL_ERROR,
        BAPP_APP_INIT_ERROR
    };
    
public:
    static const char* getErrorString(ErrorType errorCode);
};

}

#endif // BAPP_ERROR_H
