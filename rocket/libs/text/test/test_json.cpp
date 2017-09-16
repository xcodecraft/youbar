#include "text/json.h"
#include "text/text.h"
#include "../utf8.h"
#include <boost/foreach.hpp>
#define BOOST_TEST_MODULE MyTest
#include <boost/test/included/unit_test.hpp>
using namespace pylon ;
using namespace pylon::json ;
using namespace std;

BOOST_AUTO_TEST_CASE( utf8_test)
{
    wstring u16 = L"PYLON可以用么" ;
    string u8;
    utf8::utf16to8(u16.begin(),u16.end() , back_inserter(u8));
    cout << u8 << endl ;
}
BOOST_AUTO_TEST_CASE( my_test )
{

    node::sptr found,root ;
    parser  p ;
    wstring str( L"{ \"link\": { \"name\": \"china\", \"type\": \"node\"  }} ");
    bool pass =  p.parse(str,root);
    BOOST_CHECK(pass) ;
    if (pass) 
    {
        root->to_json(std::wcout);
        found = root->xpath(L"/link");
        if (found)
		{
			if (can_cast<node_object*>(found))
			{
				node_object no= node_cast<node_object  >(found);
				//wstring tmpx;
				//node::value_type vt(tmpx)	;
				can_cast<wstring>(no[L"name"]);
				wcout << node_cast<wstring>(no[L"name"]) << endl ;

				for(node_object::iterator it  = no.begin(); it!=no.end(); it++)
				{
					wcout << it->first << L":" << node_cast<wstring>(it->second) <<endl;
				}
			}
			node::sptr wt = found->xpath(L"/name");
            wcout <<  node_cast<wstring>(wt) << endl ;
		}
    }

    str =  L"{ \"link\":[ 1,2 ] } ";
    pass =  p.parse(str,root);
    BOOST_CHECK(pass) ;
    if (pass) 
    {
        root->to_json(std::wcout);
        found = root->xpath(L"/link/[0]");
        if (found)
            wcout << node_cast<int>(found) << endl;
        found = root->xpath(L"/link/[3]");
        BOOST_CHECK(!found);
        found = root->xpath(L"/link");
		if(found)
		{
			if(can_cast<node_arr>(found))
			{
				BOOST_FOREACH(node::sptr x, node_cast<node_arr>(found))
				{
					wcout << node_cast<int>(x);
				}
			}
		}
    }


    str=( L"{ \"link\": { \"name\": \"中国\", \"type\": \"node\"  }} ");
    pass =  p.parse(str,root);
    BOOST_CHECK(pass) ;
    if (pass) cout << utf16to8(root->str());


    str=( L"{\"test\": \"go\\\"od\"} ");
    pass =  p.parse(str,root);
    BOOST_CHECK(pass) ;
    if (pass) cout << utf16to8(root->str());
}
