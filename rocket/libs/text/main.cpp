#include "text/xjson.h"
#include <vector>
using namespace pylon::json ;
using namespace pylon ;
int main(void)
{
    using namespace std;
    wcout << " Type [q or Q ] to quit \n\n ";
    wstring str; 
    parser  p ;
    while(getline(std::wcin,str))
    {
        if ( str.empty() || str[0] == 'q' || str[0] == 'Q')
            break;
        int n = 0 ;
        if (p.parse(str))
        {
            wcout << L"YES! : " <<  n  << endl;
            if(p.root()!= NULL)
                p.root()->to_json(std::wcout);
        }
        else
            wcout << L"NO" << endl;
    }

    return 0;
}
/*
#include "text/fuck.h"
int main()
{
    fuck();
}
*/
