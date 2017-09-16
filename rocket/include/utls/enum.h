#pragma  once
#include <boost/foreach.hpp>
template< typename T> 
struct enum_impl
{/*{{{*/
    typedef std::map< T, std::string> impl_map_t;
    typedef std::pair< T,std::string> impl_pair_t;
    typedef std::map< T, std::wstring> impl_wmap_t;
    typedef std::pair< T,std::wstring> impl_wpair_t;

    static std::string name_of(T v)
    {/*{{{*/
        impl_map_t& map = get_ins();
        return map[v];
    }/*}}}*/
    static std::wstring wname_of(T v)
    {/*{{{*/
        impl_wmap_t& map = get_wins();
        return map[v];
    }/*}}}*/
    static bool  value_of(const std::string& name ,T& v)
    {/*{{{*/
        impl_map_t& map = get_ins();
        BOOST_FOREACH( impl_pair_t p, map)
        {
            if(algorithm::iequals(p.second,name))
            {
                v = p.first ;
                return true ;
            }
        }
        return false;
    }/*}}}*/
    static bool  value_of(const std::wstring& name ,T& v)
    {/*{{{*/
        impl_wmap_t& map = get_wins();
        BOOST_FOREACH( impl_wpair_t p, map)
        {
            if(algorithm::iequals(p.second,name))
            {
                v = p.first ;
                return true ;
            }
        }
        return false;
    }/*}}}*/

    static bool value_of(int i, T& v)
    {/*{{{*/
        impl_map_t& map = get_ins();
        if( i < 0 && i >= (int)map.size()) return false ;
        impl_map_t::iterator pos = map.find((T)i);
        if(pos == map.end())
            return false;
        v = pos->first;
        return true;
    }/*}}}*/
    static impl_map_t& get_ins()
    {/*{{{*/
        static  impl_map_t  map ;
        if(map.empty())
            init_map(map);
        return map;
    }/*}}}*/

    static impl_wmap_t& get_wins()
    {/*{{{*/
        static  impl_wmap_t  map ;
        if(map.empty())
            init_wmap(map);
        return map;
    }/*}}}*/
};/*}}}*/

