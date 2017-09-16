#include "common.h"
#include "single_ins/log_ins.h"
#include <boost/log/common.hpp>
#include <boost/log/formatters.hpp>
#include <boost/log/filters.hpp>

#include <boost/log/utility/init/to_file.hpp>
#include <boost/log/utility/init/to_console.hpp>
#include <boost/log/utility/init/common_attributes.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_multifile_backend.hpp>
#include <boost/log/attributes/timer.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/log/common.hpp>
#include <boost/log/filters.hpp>
#include <boost/log/formatters.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_multifile_backend.hpp>
#include <boost/filesystem.hpp>
//#include "common/json_base.h"




using namespace yyou ;
using namespace std ;
using namespace boost ;



namespace logging   = boost::log;
namespace attrs = boost::log::attributes;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace fmt = boost::log::formatters;
namespace flt = boost::log::filters;
namespace keywords = boost::log::keywords;

y_logger* log_ins(log_def::aspect_t def) 
{/*{{{*/
    static    bool       init =false ;
    static    y_logger   main_logger;
    static    y_logger   auto_logger;
    static    y_logger   fps_logger;
    static    y_logger   data_logger;
    static    y_logger   rt_logger;
    if( !init ) 
    {
        boost::shared_ptr< logging::attribute > attr_main(new attrs::constant< log_def::aspect_t>(log_def::MAIN));
        boost::shared_ptr< logging::attribute > attr_auto(new attrs::constant< log_def::aspect_t>(log_def::AUTO));
        boost::shared_ptr< logging::attribute > attr_fps(new attrs::constant<  log_def::aspect_t>(log_def::FPS));
        boost::shared_ptr< logging::attribute > attr_data(new attrs::constant< log_def::aspect_t>(log_def::DATA));
        boost::shared_ptr< logging::attribute > attr_rt(new attrs::constant< log_def::aspect_t>(log_def::RT));
        main_logger.add_attribute("aspect",attr_main);
        auto_logger.add_attribute("aspect",attr_auto);
        fps_logger.add_attribute( "aspect",attr_fps);
        data_logger.add_attribute("aspect",attr_data);
        rt_logger.add_attribute(  "aspect",attr_rt);
        init = true ;
    }
    switch(def)
    {
        case log_def::MAIN :
            return &main_logger ;
        case log_def::AUTO :
            return &auto_logger ;
        case log_def::FPS  :
            return &fps_logger ;
        case log_def::DATA :
            return &data_logger ;
        case log_def::RT   :
            return &rt_logger ;
    }    
    return NULL;
}/*}}}*/

typedef sinks::synchronous_sink< sinks::text_file_backend > file_sink;
typedef boost::shared_ptr<file_sink>  sink_sptr; 
typedef std::map< const string, sink_sptr> sink_map ;
sink_sptr config_one(const std::wstring& path ,const std::wstring& name  , int auto_flush = 0)
{/*{{{*/
    sink_sptr  sink(new file_sink(
                //                keywords::file_name = path + L"\\" + name  + L"_%Y%m%d_%H%M%S_%5N.log" , 
                keywords::file_name = path + L"\\" + name  + L".log" , 
                keywords::open_mode = std::ios_base::app , 
                keywords::auto_flush  = auto_flush == 1,
                keywords::rotation_size = 1024 * 1024  // rotation size, in characters
                ));

    sink->locked_backend()->set_file_collector(sinks::file::make_collector(
                keywords::target = path ,
                keywords::max_size = 16 * 1024 * 1024,              // maximum total size of the stored files, in bytes
                keywords::min_free_space = 100 * 1024 * 1024        // minimum free space on the drive, in bytes
                ));

    sink->locked_backend()->scan_for_files();

    sink->locked_backend()->set_formatter(
            fmt::format("%1%: [%2%] - %3%")
            % fmt::attr< unsigned int >("Line #")
            % fmt::date_time< boost::posix_time::ptime >("TimeStamp")
            % fmt::message()
            );
    //    sink->auto_flush(false);
    return sink;

}/*}}}*/

void conf_filter(sink_sptr sink ,log_def::aspect_t aspect , log_def::level_t level)
{/*{{{*/
    sink->set_filter( 
            flt::attr<log_def::level_t >("Severity")  >=  level  &&
            flt::attr< log_def::aspect_t >("aspect")  ==  aspect 
            );
}/*}}}*/

void  config_logger(const std::wstring& path ,const std::wstring& prefix ,log_def::env_t  env )
{/*{{{*/



    //logging::init_log_to_console(std::clog, keywords::format = "%TimeStamp%: %_%");
    try
    {

        sink_sptr main_sink = config_one(path,prefix+ L"_main");
        sink_sptr data_sink = config_one(path,prefix+ L"_data");
        sink_sptr rt_sink   = config_one(path,prefix+ L"_rt"  );
        sink_sptr auto_sink = config_one(path,prefix+ L"_auto");
        sink_sptr fps_sink  = config_one(path,prefix+ L"_fps" );

        switch (env)
        {
            case log_def::env_debug :
                conf_filter(main_sink,  log_def::MAIN,  log_def::debug);
                conf_filter(data_sink,  log_def::DATA,  log_def::info);
                conf_filter(rt_sink,    log_def::RT,    log_def::debug);
                conf_filter(auto_sink,  log_def::AUTO,  log_def::debug);
                conf_filter(fps_sink,   log_def::FPS,   log_def::debug);

                break;
            case log_def::env_test :
                conf_filter(main_sink,  log_def::MAIN,  log_def::info);
                conf_filter(data_sink,  log_def::DATA,  log_def::info);
                conf_filter(rt_sink,    log_def::RT,    log_def::info);
                conf_filter(auto_sink,  log_def::AUTO,  log_def::info);
                conf_filter(fps_sink,   log_def::FPS,   log_def::debug);
                break;
            case log_def::env_release:
                conf_filter(main_sink,  log_def::MAIN,  log_def::error);
                conf_filter(data_sink,  log_def::DATA,  log_def::error);
                conf_filter(rt_sink,    log_def::RT,    log_def::error);
                conf_filter(auto_sink,  log_def::AUTO,  log_def::error);
                conf_filter(fps_sink,   log_def::FPS,   log_def::info);
                break;
        }
        logging::core::get()->add_sink(main_sink);
        logging::core::get()->add_sink(data_sink);
        logging::core::get()->add_sink(rt_sink);
        logging::core::get()->add_sink(auto_sink);
        logging::core::get()->add_sink(fps_sink);

        //Add some attributes too
        boost::shared_ptr< logging::attribute > attr(new attrs::local_clock);
        logging::core::get()->add_global_attribute("TimeStamp", attr);
        attr.reset(new attrs::counter< unsigned int >);
        logging::core::get()->add_global_attribute("Line #", attr);
    }


    catch (std::exception& e)
    {
        std::cout << "FAILURE: " << e.what() << std::endl;
    }


}/*}}}*/


namespace yyou
{

#define LOG_SET(MAP,ASPECT,LEVEL) MAP[log_def::name_of(ASPECT)] = log_def::name_of(LEVEL)
    class logger_conf
    {/*{{{*/

        public:
            logger_conf()
            {/*{{{*/
                _auto_flush = 0;
                LOG_SET(_setting,log_def::MAIN,  log_def::error);
                LOG_SET(_setting,log_def::DATA,  log_def::error);
                LOG_SET(_setting,log_def::RT,    log_def::error);
                LOG_SET(_setting,log_def::AUTO,  log_def::error);
                LOG_SET(_setting,log_def::FPS,   log_def::info);
            }/*}}}*/
            typedef std::pair< std::string , std::string > pair_t;
            typedef std::map< std::string , std::string > map_t;
            int _auto_flush;
            map_t  _setting ;

    };/*}}}*/



	/*
    void  operator>>( j_variant& var, logger_conf& conf )
    {
        j_object  in= boost::any_cast<j_object>(*var); 
        ass_from_any_sptr(in["auto_flush"],conf._auto_flush);
        ass_from_any_sptr(in["setting"],conf._setting);
    }
	

    void  operator<<( std::ostream& out, logger_conf& conf )
    {
        out<< "{ ";
        out<<  make_pair("auto_flush",conf._auto_flush) << ",";
        out<<  make_pair("setting",conf._setting) ;
        out<< " }" ;
    }
	*/
}
void  use_conf(const std::wstring& log_path,const std::wstring& name,logger_conf& conf )
{/*{{{*/

    boost::shared_ptr< logging::attribute > attr(new attrs::local_clock);
    logging::core::get()->add_global_attribute("TimeStamp", attr);
    attr.reset(new attrs::counter< unsigned int >);
    logging::core::get()->add_global_attribute("Line #", attr);

    BOOST_FOREACH(logger_conf::pair_t p , conf._setting)
    {
        log_def::aspect_t aspect;
        if(!log_def::value_of(p.first, aspect)) break;
        log_def::level_t  level ;
        if(!log_def::value_of(p.second, level)) break;
        wstring prefix = name +  L"_" + str2wstr(p.first);
        sink_sptr sink = config_one(log_path,prefix,conf._auto_flush);
        conf_filter(sink,  aspect,  level);
		logging::core::get()->add_sink(sink);
    }

}/*}}}*/

void  config_logger_by_file(const std::wstring& conf_pos ,const std::wstring& log_path, const std::wstring& name)
{/*{{{*/
	/*
    logger_conf conf;
    if ( filesystem::exists( conf_pos.c_str() ) )
    {
        std::ifstream ifs(conf_pos.c_str());
        js_load_conf(conf,ifs);
    }
    else
    {
        std::ofstream ofs(conf_pos.c_str());
        js_store_conf(conf,ofs);
    }
    use_conf(log_path,name,conf);
	*/
}/*}}}*/
