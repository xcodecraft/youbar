#pragma  once
//#include "yui/ui_need.h"
#include <atlbase.h>
#include <boost/any.hpp>
#include <boost/signals2.hpp>
#include <boost/thread.hpp>
namespace yyou
{
	typedef boost::signals2::scoped_connection   scoped_cnn;
	typedef boost::shared_ptr<boost::signals2::scoped_connection> scope_sigcnn_sptr;
	typedef std::vector<boost::shared_ptr<boost::signals2::scoped_connection> > scoped_sigcnn_parr;
	scope_sigcnn_sptr  make_sig_cnn(boost::signals2::connection& cnn);

	class  task_def
	{
	public:
		enum task_run_t{AUTO, REAL_TIME};
	};

	class  scope_status_transform
	{
		bool &_status;
	public :
		scope_status_transform(bool& status);
		~scope_status_transform();
	};


	struct service_def
	{
		enum update_level_t{ REAL_TIME,OFTEN,NORML,LOW,DAY};
	};

	class cache_stg  :	public  smart_def<cache_stg> , 
		public  sptr_map<service_def::update_level_t,cache_stg>
	{
	public:
		std::wstring		_cache_path;
		int						_cache_sec;
		static  cache_stg*  empty_stg();
	};

	typedef std::map< std::wstring , boost::any >  dyn_map ;
	typedef boost::shared_ptr< std::map< std::wstring , boost::any > > dyn_map_ptr ;





}

#define  XLOG L"xlog" 
std::wstring std_path(std::wstring& path);
std::string std_path(std::string& path);
void freemem(int mix_size,int max_size);
void optimize_mem(int restore_size);
std::wstring AddVersionBuild( std::wstring sVersion );
void split_str( std::string& ori_string,std::string split_str,std::vector<std::string> &out );
bool delete_file(std::wstring & path );
std::wstring un_std_path(std::wstring & path);
std::string un_std_path(std::string & path);
std::wstring get_exe_position();
