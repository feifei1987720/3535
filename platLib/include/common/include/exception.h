#ifndef ___EXCEPTION__
#define ___EXCEPTION__

#define SHOW_NOT_IMPLE_FUNC printf("FUNC = %s not impletion!\n",  __FUNCTION__)
#include <string>
using namespace std;
class Exception
{
public:
    explicit Exception() 
    {
    }

    Exception(const char* info) 
    {
        errinfo = info;
    }
    
    ~Exception() {}
        
public:     
    const string what() const
    {   
        return errinfo;
    }

private:                       
    string errinfo;            
};
                               
#endif      
