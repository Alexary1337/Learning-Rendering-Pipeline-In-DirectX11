#ifndef _COMMON_H_
#define _COMMON_H_

#include <iostream>
using namespace std;

#define SAFE_INIT(a) (a) = nullptr;
#define SAFE_RELEASE(a) if((a)!=nullptr){(a)->Release();(a)=nullptr;}
#define SAFE_DELETE(a) if((a)!=nullptr){delete (a);(a)=nullptr;}
#define SAFE_DELETE_ARRAY(a) if((a)!=nullptr){delete[](a);(a)=nullptr;}
#define SAFE_CHECKEXIST(a) if(!(a)){return false;}

#define CONSOLE_OUT(a) cout << "SynEngine-> " <<(a)<< endl;

#endif