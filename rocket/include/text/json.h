#ifndef __XJSON__HPP__ 
#define __XJSON__HPP__ 
#pragma warning( disable : 4275 )
#pragma warning( disable : 4251 )
#include <string>
#include <map>
#include <vector>
#include <list>
#include <boost/variant.hpp>
#include "port.h"
#include "pimpl.h"
#include "utls.h"
#include "text/json_core.h"


namespace  pylon 
{ 
	namespace json 
	{

		template < typename T >
		struct nodecast_
		{
			static T cast(node* n)
			{
				node_value* nv = dynamic_cast<node_value *> (n);
				return  boost::get<T>((nv->_value));
			}
			static bool can(node* n)
			{
				node_value* nv = dynamic_cast<node_value *> (n);
				if (nv == NULL) return false ;
				T tmp_v;
				node::value_type vt(tmp_v)	;
				if (vt.which()  == nv->_value.which()) 
					return true ;
				return false;
			}

		};

		template <>
		struct nodecast_<node_arr *>
		{
			static node_arr* cast(node* n)
			{
				return  dynamic_cast<node_arr*> (n);
			}
			static bool can(node* n)
			{
				return  dynamic_cast<node_arr*> (n) != NULL;
			}

		};

		template <>
		struct nodecast_<node_arr >
		{
			static node_arr cast(node* n)
			{
				return  *(dynamic_cast<node_arr*> (n));
			}
			static bool can(node* n)
			{
				return  dynamic_cast<node_arr*> (n) != NULL;
			}

		};

		template <>
		struct nodecast_<node_object *>
		{
			static node_object * cast(node* n)
			{
				return  dynamic_cast<node_object*> (n);
			}
			static bool can(node* n)
			{
				return  dynamic_cast<node_object*> (n) != NULL;
			}

		};
		template <>
		struct nodecast_<node_object >
		{
			static node_object  cast(node* n)
			{
				return  *(dynamic_cast<node_object*> (n));
			}
			static bool can(node* n)
			{
				return  dynamic_cast<node_object*> (n) != NULL;
			}

		};


		template < typename T >
		T node_cast(node::sptr n)
		{
			return nodecast_<T>::cast(n.get());
		}
		template < typename T >
		T node_cast(node* n)
		{
			return nodecast_<T>::cast(n);
		}
		template < typename T >
		bool can_cast(node* n)
		{
			return nodecast_<T>::can(n);
		}
		template < typename T >
		bool can_cast(node::sptr n)
		{
			return nodecast_<T>::can(n.get());
		}


	}
}
#endif
