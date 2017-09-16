#include <boost/python.hpp>
#include "stub.h"
using namespace std;
using namespace boost::python;

BOOST_PYTHON_MODULE(py_stub)
{/*{{{*/

    def("stub_work", &stub_work) ;

}/*}}}*/
