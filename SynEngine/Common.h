#ifndef _COMMON_H_
#define _COMMON_H_


#ifdef _DEBUG
#include <iostream>
using namespace std;
#define CONSOLE_OUT(a) cout << "SynEngine-> " <<(a)<< endl;
#else
#define CONSOLE_OUT(a);
#endif

#define SAFE_INIT(a) (a) = nullptr;
#define SAFE_RELEASE(a) if((a)!=nullptr){(a)->Release();(a)=nullptr;}
#define SAFE_DELETE(a) if((a)!=nullptr){delete (a);(a)=nullptr;}
#define SAFE_DELETE_ARRAY(a) if((a)!=nullptr){delete[](a);(a)=nullptr;}
#define SAFE_CHECKEXIST(a) if(!(a)){return false;}


#endif