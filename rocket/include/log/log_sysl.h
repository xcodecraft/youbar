#ifndef __LOG_SYSLOG_H__
#define __LOG_SYSLOG_H__
#include <ostream>

class logger;
/** 
 * @brief 
 */
class log_kit
{
    public:
        /** 
         * @brief  debug,info ....
         */
        enum level_t  { debug=0, info, warn,error,undef=99};
        enum outer_t  { none =0 ,console=1 };
        static logger* log_ins(const char* name);
        static void init(const char * prjname,const char* tag ,level_t l);
        static void level(const char * name , level_t l);
        static void tag(const char * name , const char* tag );
        static void out(const char * name , outer_t extra_out); 
        static void clear();
        struct impl;
};

/** 
 * @brief 
 */
class logger
{
    public:
        bool need_log(int level);
        void level(log_kit::level_t l);
        void tag(   const char* tag );
        void out(  log_kit::outer_t extra_out); 

        std::ostream*  stream(log_kit::level_t lev);
        void log_stream();

        void debug(const char * msg );
        void info(const char * msg );
        void warn(const char * msg );
        void error(const char * msg );

        logger(const char* name);
        ~logger();
        struct impl;
    private :
        impl*  _pimpl;
};

class logger_proxy
{
    public:
        void debug(const char * msg )
        {
            _l->debug(msg);
        }
        void info(const char * msg )
        {
            _l->info(msg);
        }
        void warn(const char * msg )
        {
            _l->warn(msg);
        }
        void error(const char * msg )
        {
            _l->error(msg);
        }
        logger_proxy(const char* name):_l(log_kit::log_ins(name)){}
        logger*  _l;
};

#define LOG_WHERE  __PRETTY_FUNCTION__ << ":" << __LINE__  
#define LOG_DEBUG(x) if (x->need_log(log_kit::debug)) for(int i = 1; i > 0 ; i = 0 , x->log_stream() ) \
                                                                  *( x->stream(log_kit::debug) ) <<  LOG_WHERE  
#define LOG_DEBUG_S(x) if (x->need_log(log_kit::debug)) for(int i = 1; i > 0 ; i = 0 , x->log_stream() ) \
                                                                    *( x->stream(log_kit::debug) ) 

#define LOG_INFO(x)   if (x->need_log(log_kit::info)) for(int i = 1; i > 0 ; i = 0 , x->log_stream() ) \
                                                                  *( x->stream(log_kit::info) ) <<  LOG_WHERE  
#define LOG_INFO_S(x) if (x->need_log(log_kit::info)) for(int i = 1; i > 0 ; i = 0 , x->log_stream() ) \
                                                                  *( x->stream(log_kit::info) ) 

#define LOG_ERROR(x) if (x->need_log(log_kit::error)) for(int i = 1; i > 0 ; i = 0 , x->log_stream() ) \
                                                                  *( x->stream(log_kit::error) ) <<  LOG_WHERE  

#define LOG_ERROR_S(x) if (x->need_log(log_kit::error)) for(int i = 1; i > 0 ; i = 0 , x->log_stream() ) \
                                                                    *( x->stream(log_kit::error) ) 


#define INS_LOG(x,logname)    logger* x=log_kit::log_ins(logname);

#endif

