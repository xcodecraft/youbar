#include "xjson.h"
#include <vector>


struct ones_ : qi::symbols<char,int>
{
    ones_()
    {
        add
            ("I",1)
            ("II",2)
            ("III",3)
            ("IV",4)
            ("V",5)
            ("VI",6)
            ("VII",7)
            ("VIII",8)
            ("IX",9)
            ;
    }
} ones;

template < typename iterator>
struct roman : qi::grammar<iterator ,int(), ascii::space_type >
{
    roman() : roman::base_type(start)
    {
        start = qi::eps  [  qi::_val = 0 ]  >> 
            (
                ones [ qi::_val += qi::_1 ]  %  '+'
            )
            ;
    }

    qi::rule<iterator,int(),ascii::space_type> start;
};

template < typename iterator>
bool adder(iterator first , iterator last, int& n)
{
    using qi::double_ ; 
    using qi::phrase_parse ;
    using ascii::space ;
    using qi::int_ ;
    using qi::_1 ;
    using phoenix::ref ;
    std::vector<int > v ;
    bool  r = phrase_parse(
            first
            ,last
            ,(
                int_ % ','
//             int_[px::push_back(px::ref(v),_1) ]  >>  *('+' >> int_[ phoenix::push_back(px::ref(v), _1 ) ])
             )
//            ,'{' >> qi::int_[&print] >> '}'  | qi::int_[&print] 
//            double_ >> *(',' >> double_),
            ,space
            ,v  
            );
    if (first != last) return false ;
    n = v.size();
    return r ;
}
int main(void)
{
    using namespace std;
    cout << " Type [q or Q ] to quit \n\n ";
    string str; 
    roman<string::const_iterator>   roman_parser ;
    while(getline(std::cin,str))
    {
        if ( str.empty() || str[0] == 'q' || str[0] == 'Q')
            break;
        int n = 0 ;
//        bool r =false ;
        string::const_iterator iter = str.begin();
        string::const_iterator end  = str.end();
        bool r = phrase_parse(iter,end,roman_parser,ascii::space,n);
        if (r && iter == str.end() )
//        if (adder(str.begin(),str.end(),n))
        {
            cout << "YES! : " <<  n  << endl;
        }
        else
            cout << "NO" << endl;
    }

    return 0;
}
