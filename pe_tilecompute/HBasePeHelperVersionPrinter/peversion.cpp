//peversion.cpp
//用于打印 libHBasePeHelperCppConnector.so 中的版本信息。
//2020-12-2 wf

#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>


using std::string;
using std::cout;
using std::endl;

//string HBasePeHelperCppConnector_GetVersion()
typedef void (*version_func_ptr)() ;

int main(){
	cout<<"A program to print libHBasePeHelperCppConnector.so version info. 2020-12-02"<<endl ;
	cout<<"libHBasePeHelperCppConnector.so should be in /usr/lib "<<endl ;
	version_func_ptr GetVersion ;
	void* handle =  dlopen ("libHBasePeHelperCppConnector.so", RTLD_LAZY);

	if (!handle) {
        cout <<"Error , dlopen failed:"<< dlerror() <<endl ;;
        exit(1);
    }
    // get the function address and make it ready for use
    char *error = nullptr ;
    GetVersion = (version_func_ptr)dlsym(handle, "HBasePeHelperCppConnector_GetVersion");
    if ((error = dlerror()) != NULL)  {
        cout <<"Error , dlsym get function address failed:"<< error <<endl ;;
        exit(2);
    }
    // call the function in *.so
    cout<<"********** VERSION INFO *************"<<endl ;
    cout<<endl;
    (*GetVersion)();
    cout<<endl ;
	cout<<"********** ************ *************"<<endl ;

    dlclose(handle);handle=nullptr;

}