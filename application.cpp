#include "bapp/application.h"
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/bind.hpp>
#include <glog/logging.h>

namespace po = boost::program_options;

namespace bapp
{

Application* Application::instance_ = 0;
static std::string g_PidFile;

Application::Application():
inited_(false),
stop_(false),
reload_(false),
daemon_(false)
{
}

Application::~Application()
{
}


Error::ErrorType Application::AddTimer( uint16_t interval, TimerJobFunc func )
{
    return Error::BAPP_SUCCESS;
}

Error::ErrorType Application::Run( int argc, const char* argv[] )
{
    //设置为子类对象
    instance_ = this;
    //先获取当前路径，否则daemonize会修改之
    boost::filesystem::path current = boost::filesystem::current_path();
    current_path_ = current.c_str();

    //解析命令行
    Error::ErrorType ret = ProcessCmdOption(argc, argv);
    if (Error::BAPP_SUCCESS != ret)
    {
        return Error::BAPP_PROCESS_CMD_OPTION_ERROR;
    }
	
	google::InitGoogleLogging(app_name_.c_str());
    //初始化日志配置
    ret = LoadLogConfig();
    if (Error::BAPP_SUCCESS != ret)
    {
        return ret;
    }
    //加载应用配置文件
    ret = LoadAppConfig();
    if (Error::BAPP_SUCCESS != ret)
    {
        return ret;
    }

    //daemon
    if (daemon_)
    {
        Daemon();
    }
    //恢复当前路径
    boost::filesystem::current_path(current);
    //写入pid信息
    std::ofstream pid_file(g_PidFile.c_str());
    pid_file << ::getpid();
    pid_file.close();
    //信号处理
    InitSignal();

    //初始化
    ret = Initalize();
    if (Error::BAPP_SUCCESS != ret)
    {
        return Error::BAPP_APP_INIT_ERROR;
    }

    //启动
    Start();

    return Error::BAPP_SUCCESS;
}

Error::ErrorType Application::ProcessCmdOption( int argc, const char* argv[] )
{
    try
    {
        //如果是gdb调试传入的argv[0]参数为绝对路径,例如/data/billowqiu/xxx/yyyapp
        app_name_ = argv[0];
        std::size_t end = app_name_.find_last_of('/');
        //是gdb传入
        if (std::string::npos != end)
        {
            app_name_ = app_name_.substr(end+1);
        }
        g_PidFile = app_name_ + ".pid";

        //默认配置文件
        app_config_file_ = std::string("../config/") + app_name_ + "_conf.xml";
        glog_config_file_ = "../config/glog_conf.xml";

        po::options_description desc("Allowed options");
        desc.add_options()
            ("help", "show help message")
            ("tdrconfig", po::value<std::string>(&app_config_file_), "tdr config file, relatively path")
            ("logconfig", po::value<std::string>(&glog_config_file_), "tlog config file, relatively path")
            ("daemon", "run as a daemon")
            ("reload", "reload service")
            ("stop", "stop service")
            ("version", "version")
            ("author", "billowqiu@163.com")
            ;

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("help"))
        {
            std::clog << desc << "\n";

            return Error::BAPP_PROCESS_CMD_OPTION_EXIT;
        }
        else if (vm.count("reload"))
        {
            std::ifstream pidfile(g_PidFile.c_str());
            pid_t pid;
            if (pidfile.good())
            {
                pidfile >> pid;
                int ret = ::kill(pid, SIGRTMIN+1);

                if (-1 == ret)
                {
                    std::cerr << strerror(errno) << std::endl;
                }
            }

            return Error::BAPP_PROCESS_CMD_OPTION_EXIT;
        }
        else if (vm.count("stop"))
        {
            std::ifstream pidfile(g_PidFile.c_str());
            pid_t pid;
            if (pidfile.good())
            {
                pidfile >> pid;
                int ret = ::kill(pid, SIGUSR1);

                if (-1 == ret)
                {
                    std::cerr << strerror(errno) << std::endl;
                }
            }

            return Error::BAPP_PROCESS_CMD_OPTION_EXIT;
        }
        else if (vm.count("version"))
        {
#ifdef DEBUG
            std::clog << "debug ";
#else
            std::clog << "release ";
#endif
            std::clog << "version 0.0.1 " << __DATE__ << " " << __TIME__ << std::endl;

            return Error::BAPP_PROCESS_CMD_OPTION_EXIT;
        }

        if (vm.count("daemon"))
        {
            daemon_ = true;
        }
    }
    catch (const std::exception& e)
    {
        std::clog << e.what() << std::endl;

        return Error::BAPP_PROCESS_CMD_OPTION_ERROR;
    }

    return Error::BAPP_SUCCESS;
}

void Application::Daemon()
{
    switch(::fork())
    {
    case -1:
        ::exit(errno);

    case 0:
        break;

    default:
        ::exit(0);
    }

    if(::setsid() < 0)
    {
        exit(errno);
    }
    //adu上面说在system v系统上面才需要再次fork
    switch(::fork())
    {
    case -1:
        ::exit(errno);

    case 0:
        break;

    default:
        ::exit(0);
    }
    //不能改变当前目录，否则tlog就傻X了
    //chdir("/");
    ::close(STDIN_FILENO);
    ::close(STDOUT_FILENO);
    ::close(STDERR_FILENO);
    ::open("/dev/null", O_RDWR);
    ::dup(0);
    ::dup(0);
}

void Application::InitSignal()
{
    struct sigaction act;
    ::memset(&act, 0, sizeof(act));
    ::sigemptyset(&act.sa_mask);

    //stop
    act.sa_handler = StopSignal;
    if(::sigaction(SIGUSR1, &act, NULL) < 0)
    {
        ::exit(errno);
    }

    //reload
    act.sa_handler = ReloadSignal;
    if(::sigaction(SIGUSR2, &act, NULL) < 0||
       /*SIGUSR2被jdk里面占用了，坑爹啊*/
       ::sigaction(SIGRTMIN+1, &act, NULL) < 0)
    {
        ::exit(errno);
    }

    //act.sa_handler = HubSigHandler;
    //if(sigaction(SIGHUP, &act, NULL) < 0)
    //{
    //    std::cerr << "call sigaction fail " << strerror(errno);
    //    exit(errno);
    //}
    //忽略此信号
    act.sa_handler = SIG_IGN;
    if(::sigaction(SIGPIPE, &act, NULL) < 0)
    {
        ::exit(errno);
    }

    act.sa_handler = StopSignal;
    if(::sigaction(SIGINT, &act, NULL) < 0 ||
       ::sigaction(SIGTERM, &act, NULL) < 0 ||
       ::sigaction(SIGQUIT, &act, NULL) < 0)
    {
        ::exit(errno);
    }

    //act.sa_handler = ChildSigHandler;
    //if(sigaction(SIGCHLD, &act, NULL) < 0)
    //{
    //    std::cerr << "call sigaction fail "<< strerror(errno);
    //    exit(errno);
    //}
}

Application* Application::Instance()
{
    assert(instance_);
    return instance_;
}

bnet::AsyncProcessor* Application::GetBaseProcessor()
{
    return &base_processor_;
}

Error::ErrorType Application::LoadAppConfig()
{
	read_xml(app_config_file_, app_config_);
	
	return Error::BAPP_SUCCESS;
}

Error::ErrorType Application::LoadLogConfig()
{
    std::string logconfig(current_path_ + "/" + glog_config_file_);
	std::clog << __FUNCTION__ << logconfig << std::endl;
    using boost::property_tree::ptree;
    ptree pt;
    read_xml(logconfig, pt);
    bool logtostderr = pt.get("glog.logtostderr", false);
    FLAGS_logtostderr = logtostderr;

    bool alsologtostderr = pt.get("glog.alsologtostderr", false);
    FLAGS_alsologtostderr = alsologtostderr;

    bool log_prefix = pt.get("glog.log_prefix", true);
    FLAGS_log_prefix = log_prefix;

    int32_t minloglevel = pt.get("glog.minloglevel", 0);
    FLAGS_minloglevel = minloglevel;

    int32_t logbuflevel = pt.get("glog.logbuflevel", 0);
    FLAGS_logbuflevel = logbuflevel;

    int32_t logbufsecs = pt.get("glog.logbufsecs", 30);
    FLAGS_logbufsecs = logbufsecs;

    std::string log_dir = pt.get("glog.log_dir", "../log");
    FLAGS_log_dir = log_dir;

    std::string log_link = pt.get("glog.link", "");
    FLAGS_log_link = log_link;

    int32_t max_log_size = pt.get("glog.max_log_size", 1800);
    FLAGS_max_log_size = max_log_size;

    bool stop_logging_if_full_disk = pt.get("glog.stop_logging_if_full_disk", false);
    FLAGS_stop_logging_if_full_disk = stop_logging_if_full_disk;

    return Error::BAPP_SUCCESS;
}

void Application::StopSignal( int signum )
{
    instance_->base_processor_.AddJob(boost::bind(&Application::StopInternal, instance_));
}

void Application::StopInternal()
{
    instance_->stop_ = true;

    Stop();
}

void Application::ReloadSignal( int signum )
{
    instance_->base_processor_.AddJob(boost::bind(&Application::ReloadInternal, instance_));
}

void Application::ReloadInternal()
{
    reload_ = true;
    //load config
    Error::ErrorType ret = LoadAppConfig();
    if (Error::BAPP_SUCCESS != ret)
    {
        reload_ = false;
        return;
    }

    ret = LoadLogConfig();
    if (Error::BAPP_SUCCESS != ret)
    {
        reload_ = false;
        return;
    }
    //call back app reload
    Reload();

    reload_ = false;
}

}
