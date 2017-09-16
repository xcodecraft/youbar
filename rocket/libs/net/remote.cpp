//

#include "net/remote.h"
#include "error/error.h"
#include "boost/regex.hpp"
//#define CURL_STATICLIB
#include "curl/curl.h"
#include "boost/scoped_array.hpp"
#include "boost/algorithm/string/replace.hpp"
#include "boost/algorithm/string/case_conv.hpp"
#include "boost/filesystem.hpp"
#include "boost/thread/mutex.hpp"
#ifndef  CURL_API
#define  CURL_API(EXP)   EXPECT(EXP,CURLE_OK,run_error());
#endif
using namespace std;
using namespace boost;
using namespace boost::filesystem;

pylon::call_result::call_result() :_result(true),_err_code(0)
{
	_reason.reset(new stringstream);
}

pylon::call_result::call_result(bool ret) :_result(ret),_err_code(0)
{
	_reason.reset(new stringstream);
}

void pylon::call_result::set_err( const std::string& reason )
{
	_result = false;
	//_reason = reason;
	*_reason << reason;
}

void pylon::call_result::set_err( int err_code,const std::string& reason )
{
	_result = false;
	_err_code = err_code;
	*_reason << reason;
}
void pylon::call_result::no_err()
{
	_result = true;
}

std::string pylon::call_result::reason()
{
	return _reason->str();
}

std::stringstream* pylon::call_result::reason_stream()
{
	return _reason.get();
}

namespace pylon
{
	long curl_writer(void *data, int size, int nmemb, ostream * content)
	{/*{{{*/
		long sizes = size * nmemb;
		content->write((char*) data,sizes);
		return sizes;
	}/*}}}*/

	typedef  std::pair<progress_report*  , ostream*>  writer_params;
	long ob_curl_writer(void *data, int size, int nmemb, writer_params* params)
	{/*{{{*/
		long sizes = size * nmemb;
		progress_report  * ob = params->first ;
		ostream*  content = params->second;
		content ->write((char*) data,sizes);
		if(ob !=NULL)
		{
			ob->add_size(sizes);
		}
		return sizes;
	}/*}}}*/

	size_t get_total_size(void *ptr, size_t size, size_t nmemb, void *stream)
	{    /*{{{*/
		int r;
		long len = 0;
		r = sscanf((char*)ptr, "Content-Length: %ld\n", &len);

		if (r)
			*((long *) stream) = len;

		return size * nmemb;
	}/*}}}*/

	//////////long curl_reader(void *data, int size, int nmemb, istream &content)
	//////////{
	//////////	long sizes = size * nmemb;
	//////////	content.read((char*) data,sizes);
	//////////	return sizes;
	//////////}

	struct curl_accessor::impl
	{
		progress_report*    _ob;
		void *      _handle;
		std::string _cookie_file;
		std::string _cookie_str;
		std::string _user_agent;
		std::string _referer;
		std::string _store_cookie;
		impl(progress_report* ob)
		{
			_ob = ob;
			_user_agent = "";
			_referer = "";
			_cookie_str="";
			_cookie_file="";
			_handle =EXPECT_NOT(curl_easy_init(),(void*)NULL,wrun_error( L"Failed to create CURL connection\n"));
		}

		//////////////////////////////////////////////////////////
		//ÉèÖÃcookieÎÄ¼þ´æ·ÅÎ»ÖÃ
		bool set_cookie(std::string cookie_file)
		{
			if (cookie_file!="")
			{
				_cookie_file=cookie_file;
				return true;
			}
			return false;
		}
		//////////////////////////////////////////////////////////
		//ÉèÖÃcookieÎÄ¼þ´æ·ÅÎ»ÖÃ
		bool set_cookiestr(std::string cookie_str)
		{
			_cookie_str=cookie_str;
			return true;
		}

		//////////////////////////////////////////////////////////
		//ÉèÖÃuser-agent
		bool set_user_agent(string user_agent)
		{
			_user_agent = user_agent;
			return true;
		}
		//////////////////////////////////////////////////////////
		bool set_referer(string referer)
		{
			_referer = referer;
			return true;
		}
		//////////////////////////////////////////////////////////
		void store_cookie()
		{
			CURLcode res;
			struct curl_slist *cookies;
			struct curl_slist *nc;
			int i;
			printf("Cookies, curl knows:\n");
			res = curl_easy_getinfo(_handle, CURLINFO_COOKIELIST, &cookies);
			if (res != CURLE_OK)
			{
				return;
			}
			nc = cookies, i = 1;
			_store_cookie="";
#ifndef MAX_PATH
#define MAX_PATH 1024
#endif
			while (nc)
			{
				char a[MAX_PATH]="";
				char b[MAX_PATH]="";
				char c[MAX_PATH]="";
				char d[MAX_PATH]="";
				char e[MAX_PATH]="";
				char f[MAX_PATH]="";
				sscanf(nc->data,"%s	%s	/	%s	%s	%s	%s",a,b,c,d,e,f);
				_store_cookie=_store_cookie+e+"="+f+";";
				nc = nc->next;
				i++;
			}
			if (i == 1)
			{
				_store_cookie="";
			}
			curl_slist_free_all(cookies);
		}

		//////////////////////////////////////////////////////////////////////////
		///////post(url,input_field,log,timeout)£¬postµÄÄÚÈÝÊÇÒ»¸ö×Ö¶ÎÄÚÈÝ×Ö·û´®

		call_result post(const char * url ,const char * input_field,std::ostream& outs,logger* logger/* =NULL */,int timeout/* =10 */)
		{
			call_result ret(false);
			* ret.reason_stream() << "url:" << url << std::endl;
			curl_httppost *post = NULL;
			curl_httppost *last = NULL;					///ÉèÖÃcurl_formaddµÄÇ°Á½¸ö±äÁ¿£¬³õÊ¼Îª¿Õ

			////ÉèÖÃ´«Êä²ÎÊý
			CURL_API(curl_easy_setopt(_handle,CURLOPT_URL,url));
			if (_cookie_file!="")
			{
				CURL_API(curl_easy_setopt(_handle, CURLOPT_COOKIEFILE,_cookie_file.c_str()));
				CURL_API(curl_easy_setopt(_handle, CURLOPT_COOKIEJAR,_cookie_file.c_str()));
			}
			if(_user_agent!="")
			{
				CURL_API(curl_easy_setopt(_handle,CURLOPT_USERAGENT,_user_agent.c_str()));
			}
			if (_referer!="")
			{
				CURL_API(curl_easy_setopt(_handle,CURLOPT_REFERER,_referer.c_str()));
			}
			if (_cookie_str!="")
			{
				CURL_API(curl_easy_setopt(_handle, CURLOPT_COOKIE,_cookie_str.c_str()));
			}
			//CURL_API(curl_easy_setopt(_handle,CURLOPT_,url));
			//CURL_API(curl_easy_setopt(_handle,CURLOPT_HTTPPOST,post));

			CURL_API(curl_easy_setopt(_handle,CURLOPT_FAILONERROR,1));
			CURL_API(curl_easy_setopt(_handle,CURLOPT_FOLLOWLOCATION,1));
			CURL_API(curl_easy_setopt(_handle,CURLOPT_TIMEOUT,timeout));
			CURL_API(curl_easy_setopt(_handle,CURLOPT_POST,true));
			CURL_API(curl_easy_setopt(_handle,CURLOPT_POSTFIELDS,input_field));

			////½«·´À¡½Ø»ñµ½outsÁ÷ÖÐ
			CURL_API(curl_easy_setopt(_handle,CURLOPT_WRITEFUNCTION,curl_writer));
			CURL_API(curl_easy_setopt(_handle, CURLOPT_WRITEDATA, &outs));
			CURLcode code = curl_easy_perform(_handle);
			if (CURLE_OK !=code)
			{
				ret._err_code = code;
				* ret.reason_stream() << "curl code : " << code ;
				return ret;
			}
			//»ñÈ¡cookie
			store_cookie();
			long retcode = 0;
			CURL_API(curl_easy_getinfo(_handle,CURLINFO_RESPONSE_CODE, &retcode));
			if (retcode==200)
			{
				double length = 0;
				CURL_API(curl_easy_getinfo(_handle, CURLINFO_CONTENT_LENGTH_DOWNLOAD , &length));
				ret.no_err();
				return ret;
			}
			ret._err_code = retcode;
			* ret.reason_stream()  << "net code : " <<  retcode;
			return ret;

			//CURL_API(curl_easy_setopt(_handle,CURLOPT_RETURNTRANSFER,1));
		}

		///////post(url,postname,filename,ostream,postµÄ¶ÔÏóÊÇ¸öÎÄ¼þ
		call_result post(const char * url ,char* postname,const char *filename,ostream &outs , logger* logger,int timeout)
		{
			call_result ret(false);
			* ret.reason_stream() << "url:" << url << std::endl;
			curl_httppost *post = NULL;
			curl_httppost *last = NULL;										/////³õÊ¼»¯Á½¸ö²ÎÊý

			std::cout<<postname<<endl;

			curl_formadd(&post,&last,
				CURLFORM_COPYNAME,postname,
				CURLFORM_FILE,filename,
				CURLFORM_END);											   //////Ìî²ÎÊý

			////ÉèÖÃ´«Êä²ÎÊý
			CURL_API(curl_easy_setopt(_handle,CURLOPT_HEADER,0));
			if (_cookie_file!="")
			{
				CURL_API(curl_easy_setopt(_handle, CURLOPT_COOKIEFILE,_cookie_file.c_str()));
				CURL_API(curl_easy_setopt(_handle, CURLOPT_COOKIEJAR,_cookie_file.c_str()));
			}
			if(_user_agent!="")
			{
				CURL_API(curl_easy_setopt(_handle,CURLOPT_USERAGENT,_user_agent.c_str()));
			}
			if (_referer!="")
			{
				CURL_API(curl_easy_setopt(_handle,CURLOPT_REFERER,_referer.c_str()));
			}
			if (_cookie_str!="")
			{
				CURL_API(curl_easy_setopt(_handle, CURLOPT_COOKIE,_cookie_str.c_str()));
			}
			CURL_API(curl_easy_setopt(_handle,CURLOPT_URL,url));
			CURL_API(curl_easy_setopt(_handle,CURLOPT_HTTPPOST,post));
			CURL_API(curl_easy_setopt(_handle,CURLOPT_TIMEOUT,timeout));
			CURL_API(curl_easy_setopt(_handle,CURLOPT_SSL_VERIFYPEER,0));
			CURL_API(curl_easy_setopt(_handle,CURLOPT_SSL_VERIFYHOST,1));

			////½«·´À¡½Ø»ñµ½outsÁ÷ÖÐ
			CURL_API(curl_easy_setopt(_handle, CURLOPT_WRITEFUNCTION, curl_writer));
			CURL_API(curl_easy_setopt(_handle, CURLOPT_WRITEDATA, &outs));

			CURLcode code = curl_easy_perform(_handle);
			if(CURLE_OK !=code)
			{
				ret._err_code = code;
				* ret.reason_stream()  << "curl code : " <<  code   ;
				return  ret;
			}
			store_cookie();
			curl_formfree(post);										///////ÊÍ·ÅÄÚ´æ

			//////·µ»Ø
			long retcode = 0;
			CURL_API(curl_easy_getinfo(_handle,CURLINFO_RESPONSE_CODE,&retcode));
			if(retcode == 200)
			{
				double length = 0;
				CURL_API(curl_easy_getinfo(_handle, CURLINFO_CONTENT_LENGTH_DOWNLOAD , &length));
				ret.no_err();
				return ret;
			}
			ret._err_code = retcode;
			* ret.reason_stream()  << "net code : " <<  retcode;
			//stringstream reson
			return  ret;
		}

		//////////////////////////////////////////////////////////////////////////
		///////post(url,name,istream,ostream)£¬postµÄÄÚÈÝÊÇÒ»¸östream
		call_result post(const char * url ,char* postname, istream  &content ,ostream &outs , logger* logger,int timeout)
		{
			call_result ret(false);
			* ret.reason_stream() << "url:" << url;
			curl_httppost *post = NULL;
			curl_httppost *last = NULL;					///ÉèÖÃcurl_formaddµÄÇ°Á½¸ö±äÁ¿£¬³õÊ¼Îª¿Õ
			unsigned int count=0;								///Êµ¼Ê´«ÊäµÄ×Ö½ÚÊý
			unsigned int	bufsize = 0;							///Ã¿´Î¶ÁÈ¡µÄ×Ö½ÚÊý
			const int buffersize = 1024*1024*3;			///»º³åÇø×î´óÖµÎª1M×Ö½Ú
			char buf[1024];								///Ã¿´Î¶ÁÈ¡µÄ»º³åÇø
			char *buffer = new char[buffersize];		///»º³åÇø£¬streamµÄÄÚÈÝ¶ÁÈ¡³öÀ´´æ·ÅµÄÎ»ÖÃ
			memset(buffer,0,buffersize);				///³õÊ¼»¯»º³åÇø

			//////////Ñ­»·¶ÁÈ¡stream£¬½«ÆäÐ´Èëµ½buffer
			while(!content.eof()){
				memset(buf,0,1024);
				content.read(buf,1024);
				bufsize = content.gcount();
				if((count+bufsize)>buffersize)
					throw wrun_error(L"stream length is too large");
				memcpy(&buffer[count],buf,bufsize);
				count += bufsize;
			}

			/////½«bufferÄÚÈÝ´«µÝ
			curl_formadd(&post,&last,
				CURLFORM_COPYNAME,postname,
				CURLFORM_PTRCONTENTS,buffer,
				CURLFORM_CONTENTSLENGTH,count,
				CURLFORM_CONTENTTYPE,"byte",
				CURLFORM_END);

			///ÉèÖÃ´«Êä¸ñÊ½
			CURL_API(curl_easy_setopt(_handle,CURLOPT_HEADER,0));
			if (_cookie_file!="")
			{
				CURL_API(curl_easy_setopt(_handle, CURLOPT_COOKIEFILE,_cookie_file.c_str()));
				CURL_API(curl_easy_setopt(_handle, CURLOPT_COOKIEJAR,_cookie_file.c_str()));
			}
			if(_user_agent!="")
			{
				CURL_API(curl_easy_setopt(_handle,CURLOPT_USERAGENT,_user_agent.c_str()));
			}
			if (_referer!="")
			{
				CURL_API(curl_easy_setopt(_handle,CURLOPT_REFERER,_referer.c_str()));
			}
			if (_cookie_str!="")
			{
				CURL_API(curl_easy_setopt(_handle, CURLOPT_COOKIE,_cookie_str.c_str()));
			}
			CURL_API(curl_easy_setopt(_handle,CURLOPT_URL,url));
			CURL_API(curl_easy_setopt(_handle,CURLOPT_HTTPPOST,post));
			CURL_API(curl_easy_setopt(_handle,CURLOPT_TIMEOUT,timeout));
			CURL_API(curl_easy_setopt(_handle,CURLOPT_SSL_VERIFYPEER,0));
			CURL_API(curl_easy_setopt(_handle,CURLOPT_SSL_VERIFYHOST,1));

			////½«·´À¡½Ø»ñµ½outsÁ÷ÖÐ
			CURL_API(curl_easy_setopt(_handle, CURLOPT_WRITEFUNCTION, curl_writer));
			CURL_API(curl_easy_setopt(_handle, CURLOPT_WRITEDATA, &outs));

			////Ö´ÐÐ
			CURLcode code = curl_easy_perform(_handle);
			if(CURLE_OK !=code)
			{
				ret._err_code = code;
				* ret.reason_stream()  << "curl code : " <<  code;
				return  ret;
			}
			//»ñÈ¡cookie
			store_cookie();

			/////»ØÊÕÄÚ´æ£¬ÊÍ·Åpost
			delete[] buffer;
			curl_formfree(post);

			//////·µ»Ø
			long retcode = 0;
			CURL_API(curl_easy_getinfo(_handle,CURLINFO_RESPONSE_CODE,&retcode));
			if(retcode == 200)
			{
				double length = 0;
				CURL_API(curl_easy_getinfo(_handle, CURLINFO_CONTENT_LENGTH_DOWNLOAD , &length));
				ret.no_err();
				return ret;
			}
			ret._err_code = retcode;
			* ret.reason_stream()  << "net code : " <<  retcode;
			return ret;
		}

		boost::mutex curl_get;
		call_result get(const char * url ,  ostream& content, logger* logger, int timeout,int port)
		{
			LOG_DEBUG(logger) << "remote call: " << url ;
			call_result ret(false);
			* ret.reason_stream() << "url:" << url;

			if (_cookie_file!="")
			{
				CURL_API(curl_easy_setopt(_handle, CURLOPT_COOKIEFILE,_cookie_file.c_str()));
				CURL_API(curl_easy_setopt(_handle, CURLOPT_COOKIEJAR,_cookie_file.c_str()));
			}
			if(_user_agent!="")
			{
				CURL_API(curl_easy_setopt(_handle,CURLOPT_USERAGENT,_user_agent.c_str()));
			}
			if (_referer!="")
			{
				CURL_API(curl_easy_setopt(_handle,CURLOPT_REFERER,_referer.c_str()));
			}
			if (_cookie_str!="")
			{
				CURL_API(curl_easy_setopt(_handle, CURLOPT_COOKIE,_cookie_str.c_str()));
			}
			CURL_API(curl_easy_setopt(_handle, CURLOPT_URL, url));
			CURL_API(curl_easy_setopt(_handle, CURLOPT_FOLLOWLOCATION, 1));
			CURL_API(curl_easy_setopt(_handle,CURLOPT_TIMEOUT,timeout));
			CURL_API(curl_easy_setopt(_handle,CURLOPT_PORT,port));
			//get//
			CURL_API(curl_easy_setopt(_handle,CURLOPT_POST,false));
			writer_params params;
			params.first = _ob;
			params.second = &content;
			CURL_API(curl_easy_setopt(_handle, CURLOPT_WRITEFUNCTION, ob_curl_writer));
			CURL_API(curl_easy_setopt(_handle, CURLOPT_WRITEDATA, &params));
			if (_ob!=NULL)
			{
				CURL_API(curl_easy_setopt(_handle, CURLOPT_HEADERFUNCTION, get_total_size));
				CURL_API(curl_easy_setopt(_handle, CURLOPT_HEADERDATA, &_ob->_total_size));
			}
			CURLcode code = curl_easy_perform(_handle);

			if(CURLE_OK !=code)
			{
				ret._err_code = code;
				* ret.reason_stream()  << "curl code : " <<  code;
				return  ret;
			}
			store_cookie();
			long retcode = 0;
			CURL_API(curl_easy_getinfo(_handle, CURLINFO_RESPONSE_CODE , &retcode));
			LOG_DEBUG(logger) << "return code : " << retcode;
			if ( retcode == 200 )
			{
				double length = 0;
				CURL_API(curl_easy_getinfo(_handle, CURLINFO_CONTENT_LENGTH_DOWNLOAD , &length));
				ret.no_err();
				return ret;
			}
			ret._err_code = retcode;
			* ret.reason_stream()  << "net code : " <<  retcode;
			return ret;
		}

		~impl()
		{
			curl_easy_cleanup(_handle);
		}

		std::string get_store_cookie()
		{
			return _store_cookie;
		}
	};

	curl_accessor::curl_accessor(progress_report* ob):_pimpl(new impl(ob))
	{ }

	//////////////////////////////////////////////////////////
	//ÉèÖÃcookieÎÄ¼þ´æ·ÅÎ»ÖÃ
	bool curl_accessor::set_cookie(std::string cookie_file)
	{
		return _pimpl->set_cookie(cookie_file);
	}
	//////////////////////////////////////////////////////////
	//ÉèÖÃcookieÎÄ¼þ´æ·ÅÎ»ÖÃ
	bool curl_accessor::set_cookiestr(std::string cookie_str)
	{
		return _pimpl->set_cookiestr(cookie_str);
	}

	//////////////////////////////////////////////////////////
	//ÉèÖÃuser-agent
	bool curl_accessor::set_user_agent(string user_agent)
	{
		return _pimpl->set_user_agent(user_agent);
	}
	//////////////////////////////////////////////////////////
	//ÉèÖÃreferer
	bool curl_accessor::set_referer(string referer)
	{
		return _pimpl->set_referer(referer);
	}
	//////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	///////post(url,input_field,log,timeout)£¬postµÄÄÚÈÝÊÇÒ»¸ö×Ö¶ÎÄÚÈÝ×Ö·û´®

	call_result curl_accessor::post(const char * url ,const char * input_field,std::ostream& outs,logger* log/* =NULL */,int timeout/* =10 */)
	{
		return _pimpl->post(url,input_field,outs,log,timeout);
	}

	///////post(url,postname,filename,ostream,postµÄ¶ÔÏóÊÇ¸öÎÄ¼þ
	call_result curl_accessor::post(const char * url ,char* postname,const char *filename,ostream &outs , logger* logger,int timeout)
	{
		return _pimpl->post(url,postname,filename,outs,logger,timeout);
	}

	//////////////////////////////////////////////////////////////////////////
	///////post(url,name,istream,ostream)£¬postµÄÄÚÈÝÊÇÒ»¸östream
	call_result curl_accessor::post(const char * url ,char* postname, istream  &content ,ostream &outs , logger* logger,int timeout)
	{
		return _pimpl->post(url,postname,content,outs,logger,timeout);
	}

	call_result curl_accessor::get(const char * url ,  ostream& content, logger* logger, int timeout,int port)
	{
		return _pimpl->get(url,content,logger,timeout,port);
	}

	std::string curl_accessor::get_store_cookie()
	{
		return _pimpl->get_store_cookie();
	}
	/*
	void local_cache_racs_proxy::get_from_cache(const wchar_t* data_path, std::ostream& out )
	{
	ifstream data_file(data_path);
	while(!data_file.eof() && data_file.good())
	{
	char buf[1025];
	memset(buf,0,1025);
	data_file.read(buf,1024);
	out << buf;
	}
	}

	std::wstring  local_cache_racs_proxy::std_cache_file_name(const char * url)
	{
	md5   url_key(url);
	return  _cache_path +  str2wstr(url_key.to_string()) + L".dat" ;
	}
	call_result local_cache_racs_proxy::get( const char* url, std::ostream& out , logger* logger, int timeout )
	{
	call_result ret(false);
	wstring  cache_file_path(std_cache_file_name(url));
	if(exists(cache_file_path))
	{
	time_t last_wtime = last_write_time(cache_file_path);
	time_t now;
	time(&now);
	if((now - last_wtime ) < _cache_sec)
	{
	if(logger)
	_DBG_LOG(logger) << "get from cache  " << url;
	get_from_cache(cache_file_path.c_str(),out);
	ret.no_err();
	return ret;
	}
	}
	stringstream sbuf;
	ret = _ori_svc->get(url,sbuf,logger,timeout);
	if(ret)
	{
	ofstream data_file(cache_file_path.c_str());
	data_file<< sbuf.str();
	out << sbuf.str();
	}
	return ret;
	}

	call_result local_cache_racs_proxy::post( const char * url ,char* postname , std::istream& content,std::ostream& outs , logger* logger ,int timeout )
	{
	return _ori_svc->post(url,postname,content,outs,logger,timeout);
	}

	call_result local_cache_racs_proxy::post( const char * url ,char* postname ,const char *filename,std::ostream& outs , logger* logger, int timeout )
	{
	return _ori_svc->post(url,postname,filename,outs,logger,timeout);
	}
	*/
}