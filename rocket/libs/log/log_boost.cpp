#include "log/log_boost.h"
#include <boost/foreach.hpp>
#include <map>
#include <boost/log/formatters.hpp>
#include <boost/log/filters.hpp>
//#include <boost/log/utility/init/to_file.hpp>
//#include <boost/log/utility/init/to_console.hpp>
//#include <boost/log/utility/init/common_attributes.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/attributes/timer.hpp>
#include <boost/log/attributes/constant.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_multifile_backend.hpp>
#include <boost/filesystem.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_file_backend.hpp>

using namespace std;
//using namespace boost;

namespace logging = boost::log;
namespace attrs = boost::log::attributes;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace flt = boost::log::filters;
namespace fmt = boost::log::formatters;
namespace keywords = boost::log::keywords;


namespace pylon 
{
	typedef boost::shared_ptr< logging::attribute >         attr_sptr ;
	typedef std::map<std::wstring ,  int  >      index_t ;
	typedef std::pair<std::wstring , int >       index_pair_t ;

	typedef std::map<int, log_kit::logger* >      logger_map_t ;
	typedef std::pair<int , log_kit::logger* >     logger_pair_t ;

	static logger_map_t s_loggers;
	static index_t      s_index ;

	log_kit::logger* log_kit::log_ins(const wchar_t* name) 
	{/*{{{*/

		static int index =0 ;
		if( s_index.find(name) == s_index.end())
		{
			log_kit::logger* l = new log_kit::logger ;
			s_loggers[index]  = l ;
            s_index[name]     = index;
            attr_sptr attr_id( new attrs::constant<int>  (index));
            l->add_attribute(L"id",attr_id);
			++ index ;
			return l ;
		}
		else
		{
            int i = s_index[name];
			return s_loggers[i];
		}
	}/*}}}*/

	void log_kit::clear()
	{
		BOOST_FOREACH(logger_pair_t  p, s_loggers)
		{
			delete p.second ;
		}
		s_loggers.clear();
        s_index.clear();
	}


    typedef sinks::synchronous_sink< sinks::wtext_file_backend > file_sink;
    typedef boost::shared_ptr<file_sink>                        sink_sptr; 

	sink_sptr config_sink(const std::wstring& path ,const std::wstring& name  , int auto_flush = 1)
	{/*{{{*/
		sink_sptr  sink(new file_sink(
			//                keywords::file_name = path + L"\\" + name  + L"_%Y%m%d_%H%M%S_%5N.log" , 
			keywords::file_name     = path + L"/" + name  + L".log" , 
			keywords::open_mode     = std::ios_base::app , 
			keywords::auto_flush    = auto_flush == 1,
			keywords::rotation_size = 1024 * 1024  // rotation size, in characters
			));

		sink->locked_backend()->set_file_collector(sinks::file::make_collector(
			keywords::target        = path ,
			keywords::max_size      = 16 * 1024 * 1024,              // maximum total size of the stored files, in bytes
			keywords::min_free_space = 100 * 1024 * 1024        // minimum free space on the drive, in bytes
			));

		sink->locked_backend()->scan_for_files();

		/*
		sink->locked_backend()->set_formatter(
			fmt::format("%1%: [%2%] - %3%")
			% fmt::attr< unsigned int >("Line #")
			% fmt::date_time< boost::posix_time::ptime >("TimeStamp")
			% fmt::message()
			);
		//    sink->auto_flush(false);
		*/
		return sink;

	}/*}}}*/

	void conf_filter(sink_sptr sink , std::wstring name , log_kit::level_t level)
	{/*{{{*/

        if( s_index.find(name) != s_index.end())
        {
            int index = s_index[name];

            sink->set_filter( 
                    flt::attr<log_kit::level_t >(L"Severity")  >=  level  
                    &&  flt::attr<int>(L"id")  ==  index 
                    );
        }
	}/*}}}*/


	void log_kit::conf(const wchar_t* name ,const wchar_t* path , level_t l)
	{
	    log_ins(name) ;
		sink_sptr sink = config_sink(path,name);
		conf_filter(sink, name ,  l);
		logging::wcore::get()->add_sink(sink);
	}

#if 0
#endif
}
