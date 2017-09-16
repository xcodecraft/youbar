#pragma once
template <typename T>
struct  smart_def
{
	typedef  boost::shared_ptr< T >  sptr;
	typedef  boost::shared_ptr< T >  shared;
	typedef  boost::scoped_ptr< T >  scoped;
};


template <typename T>
struct  vector_def								 
{
	typedef  std::vector< T >  vector;
};

template < typename T> 
struct  sptr_vector
{
	typedef  std::vector< boost::shared_ptr< T > > parr;
	typedef  boost::shared_ptr< std::vector< boost::shared_ptr< T > > > parr_sptr;
};


template < typename K, typename V > 
struct  sptr_map
{
	typedef std::pair<K,boost::shared_ptr<V> >  pair ;
	typedef std::map<K, boost::shared_ptr< V > > pmap;
};

template <typename T>
struct   interface_base : public smart_def<T> , public vector_def<T>  , public  sptr_vector< T > 
{
};

template <typename T >
struct  conf_utls : public smart_def<T> , public vector_def<T> , public  sptr_vector< T >  , public sptr_map< std::wstring , T >
{

};

