#ifndef __HTTP__HPP__ 
#define __HTTP__HPP__ 
#include <string>
#include <boost/shared_ptr.hpp>
#include <pimpl.h>
#include <ostream>

namespace pylon {

    namespace http {

        /** 
         * @brief 
         */
        class buffer_t 
        {/*{{{*/
            public:
                void        reset()  { memset(_begin,0,_len); _used=0; }
                const char* const_begin()     const { return _begin; } 
                const char* const_end()       const { return _begin + _len ; } 
                char*       begin()     const { return _begin; } 
                char*       end()       const  { return _begin + _len ; } 
                char*       used_end()  const  { return _begin + _used; } 
                char*       unused_begin()  const  { return _begin + _used ; } 
                size_t      unused_size() const { return _len - _used ; }
                size_t      used_size()   const  { return _used ; }
                buffer_t    right(size_t split_len);
                void        copy(const buffer_t& other);
                void        copy_until(const buffer_t& other,const std::string& separator);
                void        copy_from(const buffer_t& other,const std::string& separator);
                void        mark_used(size_t l) {  _used += l ; }
                size_t      begin_distance(const char* point ) const { return point - _begin ; }


                buffer_t():_len(0),_used(0),_begin(NULL){}
                buffer_t(size_t l,char* p ):_len(l),_used(0),_begin(p){}

            private :
                size_t _len;
                size_t _used;
                char*  _begin;
        };/*}}}*/
        struct parser;
        /** 
         * @brief 
         */
        struct request
        {/*{{{*/
            std::string  method;
            std::string  url;
            std::string  version;
            bool         get_header(const std::string& name, std::string& value );
            void         set_header(const std::string& name, const std::string& value );
            buffer_t     alloc_buffer(size_t len=1024 );
            bool         header_check(parser* p);
            void         use_get(const char* host , const char* url);
            request();
            struct impl;  
            boost::shared_ptr< impl>  _pimpl;
        };/*}}}*/
        std::ostream& operator <<( std::ostream& out,const request& req);
        /** 
         * @brief 
         */
        struct response
        {/*{{{*/
            enum trans_encoding_t   { te_none, chunked} ;
            enum content_encoding_t { ce_none, gzip,compress,deflate,identity };

            std::string         version;
            int                 status ;
            std::string         reason;
            size_t              content_length ;
            trans_encoding_t    trans_encoding ;
            content_encoding_t  content_encoding;


            bool         get_header(const std::string& name, std::string& value );
            void         set_header(const std::string& name, const std::string& value );
            bool         header_check(parser* p);
            buffer_t     body();
            void         set_body(const buffer_t& data);
            void         append_chunk(const buffer_t& data);
            void         decode();

            int          cache_sec();
            bool         is_chunked() { return trans_encoding == chunked ; }
            buffer_t     current_chunk();
            buffer_t     alloc_buffer(size_t len=1024 );
            buffer_t     extend_buffer(buffer_t ori,size_t extend_len);
            response();
            struct impl;  
            boost::shared_ptr< impl>  _pimpl;

        };/*}}}*/
        std::ostream& operator <<( std::ostream& out,const response& resp);
        /** 
         * @brief 
         */
        class result_t
        {/*{{{*/
            public:
                enum status_t{BAD,LACK,END};
                result_t():_status(BAD),_parsed_len(0),_lack_len(0){}
                bool check_lack(size_t need , size_t have);
                bool is_end();
                bool is_bad();
                bool is_lack();
                size_t lack_length()  const { return _lack_len; }
                size_t parsed_length() const { return _parsed_len; }
                void to_end( size_t parsed) ;
                void to_lack(size_t lack)  ;
                void to_bad();
                void parsed(size_t parsed);
                status_t status(){  return _status ; }

            private:    
                status_t _status;
                size_t   _parsed_len;
                size_t   _lack_len;
        };/*}}}*/
        /** 
         * @brief 
         */
        struct parser
        {/*{{{*/
            result_t parse_header(buffer_t data , request&  requ) ;
            result_t parse_header(buffer_t data , response& reps) ;
            result_t parse_body(  buffer_t data , request&  requ) ;
            result_t parse_body(  buffer_t data , response& reps) ;
            result_t parse_chunk(buffer_t data , response& reps) ;
            bool  cache_ctrl(const std::string& str, uint& sec) ;
            parser();
            struct impl;  
            boost::shared_ptr< impl>  _pimpl;

        };/*}}}*/
    }
}
#endif 

