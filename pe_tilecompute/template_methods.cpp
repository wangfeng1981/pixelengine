#include "template_methods.h"


/// 2022-4-2  根据数据类型调用模版函数
void CallTemplateMethods_ForCollectionComposite(vector<void*> indataptrvec,void* outptr,
int indatatype,int outdatatype,
int elementSize,double validmin,double validmax,double filldata, TemplateMethods_CompositeMethod method)
{
    if( indatatype==1 )
    {
        vector<unsigned char*> invec ;
        for(int i = 0 ; i<indataptrvec.size() ; ++ i ) invec.push_back( (unsigned char*) indataptrvec[i]) ;
        if( outdatatype==1 )
        {
            unsigned char* out2 = (unsigned char*)outptr ;
            TemplateMethods_Composite(invec,out2,elementSize,validmin,validmax,filldata,method);
        }else if( outdatatype==2 )
        {
            unsigned short* out2 = (unsigned short*)outptr ;
            TemplateMethods_Composite(invec,out2,elementSize,validmin,validmax,filldata,method);
        }else if( outdatatype==3 )
        {
            short* out2 = (short*)outptr ;
            TemplateMethods_Composite(invec,out2,elementSize,validmin,validmax,filldata,method);

        }else if( outdatatype==4 )
        {
            unsigned int* out2 = (unsigned int*)outptr ;
            TemplateMethods_Composite(invec,out2,elementSize,validmin,validmax,filldata,method);

        }else if( outdatatype==5 )
        {
            int* out2 = (int*)outptr ;
            TemplateMethods_Composite(invec,out2,elementSize,validmin,validmax,filldata,method);
        }else if( outdatatype==6 )
        {
            float* out2 = (float*)outptr ;
            TemplateMethods_Composite(invec,out2,elementSize,validmin,validmax,filldata,method);

        }else if( outdatatype==7 )
        {
            double* out2 = (double*)outptr ;
            TemplateMethods_Composite(invec,out2,elementSize,validmin,validmax,filldata,method);

        }
    }
    else if( indatatype==2 )
    {
        vector<unsigned short*> invec ;
        for(int i = 0 ; i<indataptrvec.size() ; ++ i ) invec.push_back( (unsigned short*) indataptrvec[i]) ;
        if( outdatatype==1 )
        {
            unsigned char* out2 = (unsigned char*)outptr ;
            TemplateMethods_Composite(invec,out2,elementSize,validmin,validmax,filldata,method);
        }else if( outdatatype==2 )
        {
            unsigned short* out2 = (unsigned short*)outptr ;
            TemplateMethods_Composite(invec,out2,elementSize,validmin,validmax,filldata,method);
        }else if( outdatatype==3 )
        {
            short* out2 = (short*)outptr ;
            TemplateMethods_Composite(invec,out2,elementSize,validmin,validmax,filldata,method);

        }else if( outdatatype==4 )
        {
            unsigned int* out2 = (unsigned int*)outptr ;
            TemplateMethods_Composite(invec,out2,elementSize,validmin,validmax,filldata,method);

        }else if( outdatatype==5 )
        {
            int* out2 = (int*)outptr ;
            TemplateMethods_Composite(invec,out2,elementSize,validmin,validmax,filldata,method);
        }else if( outdatatype==6 )
        {
            float* out2 = (float*)outptr ;
            TemplateMethods_Composite(invec,out2,elementSize,validmin,validmax,filldata,method);

        }else if( outdatatype==7 )
        {
            double* out2 = (double*)outptr ;
            TemplateMethods_Composite(invec,out2,elementSize,validmin,validmax,filldata,method);

        }
    }
    else if( indatatype==3)
    {
        vector<short*> invec ;
        for(int i = 0 ; i<indataptrvec.size() ; ++ i ) invec.push_back( (short*) indataptrvec[i]) ;
        if( outdatatype==1 )
        {
            unsigned char* out2 = (unsigned char*)outptr ;
            TemplateMethods_Composite(invec,out2,elementSize,validmin,validmax,filldata,method);
        }else if( outdatatype==2 )
        {
            unsigned short* out2 = (unsigned short*)outptr ;
            TemplateMethods_Composite(invec,out2,elementSize,validmin,validmax,filldata,method);
        }else if( outdatatype==3 )
        {
            short* out2 = (short*)outptr ;
            TemplateMethods_Composite(invec,out2,elementSize,validmin,validmax,filldata,method);

        }else if( outdatatype==4 )
        {
            unsigned int* out2 = (unsigned int*)outptr ;
            TemplateMethods_Composite(invec,out2,elementSize,validmin,validmax,filldata,method);

        }else if( outdatatype==5 )
        {
            int* out2 = (int*)outptr ;
            TemplateMethods_Composite(invec,out2,elementSize,validmin,validmax,filldata,method);
        }else if( outdatatype==6 )
        {
            float* out2 = (float*)outptr ;
            TemplateMethods_Composite(invec,out2,elementSize,validmin,validmax,filldata,method);

        }else if( outdatatype==7 )
        {
            double* out2 = (double*)outptr ;
            TemplateMethods_Composite(invec,out2,elementSize,validmin,validmax,filldata,method);

        }
    }
    else if( indatatype==4 )
    {
        vector<unsigned int*> invec ;
        for(int i = 0 ; i<indataptrvec.size() ; ++ i ) invec.push_back( (unsigned int*) indataptrvec[i]) ;
        if( outdatatype==1 )
        {
            unsigned char* out2 = (unsigned char*)outptr ;
            TemplateMethods_Composite(invec,out2,elementSize,validmin,validmax,filldata,method);
        }else if( outdatatype==2 )
        {
            unsigned short* out2 = (unsigned short*)outptr ;
            TemplateMethods_Composite(invec,out2,elementSize,validmin,validmax,filldata,method);
        }else if( outdatatype==3 )
        {
            short* out2 = (short*)outptr ;
            TemplateMethods_Composite(invec,out2,elementSize,validmin,validmax,filldata,method);

        }else if( outdatatype==4 )
        {
            unsigned int* out2 = (unsigned int*)outptr ;
            TemplateMethods_Composite(invec,out2,elementSize,validmin,validmax,filldata,method);

        }else if( outdatatype==5 )
        {
            int* out2 = (int*)outptr ;
            TemplateMethods_Composite(invec,out2,elementSize,validmin,validmax,filldata,method);
        }else if( outdatatype==6 )
        {
            float* out2 = (float*)outptr ;
            TemplateMethods_Composite(invec,out2,elementSize,validmin,validmax,filldata,method);

        }else if( outdatatype==7 )
        {
            double* out2 = (double*)outptr ;
            TemplateMethods_Composite(invec,out2,elementSize,validmin,validmax,filldata,method);

        }
    }
    else if( indatatype==5 )
    {
        vector<int*> invec ;
        for(int i = 0 ; i<indataptrvec.size() ; ++ i ) invec.push_back( (int*) indataptrvec[i]) ;
        if( outdatatype==1 )
        {
            unsigned char* out2 = (unsigned char*)outptr ;
            TemplateMethods_Composite(invec,out2,elementSize,validmin,validmax,filldata,method);
        }else if( outdatatype==2 )
        {
            unsigned short* out2 = (unsigned short*)outptr ;
            TemplateMethods_Composite(invec,out2,elementSize,validmin,validmax,filldata,method);
        }else if( outdatatype==3 )
        {
            short* out2 = (short*)outptr ;
            TemplateMethods_Composite(invec,out2,elementSize,validmin,validmax,filldata,method);

        }else if( outdatatype==4 )
        {
            unsigned int* out2 = (unsigned int*)outptr ;
            TemplateMethods_Composite(invec,out2,elementSize,validmin,validmax,filldata,method);

        }else if( outdatatype==5 )
        {
            int* out2 = (int*)outptr ;
            TemplateMethods_Composite(invec,out2,elementSize,validmin,validmax,filldata,method);
        }else if( outdatatype==6 )
        {
            float* out2 = (float*)outptr ;
            TemplateMethods_Composite(invec,out2,elementSize,validmin,validmax,filldata,method);

        }else if( outdatatype==7 )
        {
            double* out2 = (double*)outptr ;
            TemplateMethods_Composite(invec,out2,elementSize,validmin,validmax,filldata,method);

        }
    }
    else if( indatatype==6 )
    {
        vector<float*> invec ;
        for(int i = 0 ; i<indataptrvec.size() ; ++ i ) invec.push_back( (float*) indataptrvec[i]) ;
        if( outdatatype==1 )
        {
            unsigned char* out2 = (unsigned char*)outptr ;
            TemplateMethods_Composite(invec,out2,elementSize,validmin,validmax,filldata,method);
        }else if( outdatatype==2 )
        {
            unsigned short* out2 = (unsigned short*)outptr ;
            TemplateMethods_Composite(invec,out2,elementSize,validmin,validmax,filldata,method);
        }else if( outdatatype==3 )
        {
            short* out2 = (short*)outptr ;
            TemplateMethods_Composite(invec,out2,elementSize,validmin,validmax,filldata,method);

        }else if( outdatatype==4 )
        {
            unsigned int* out2 = (unsigned int*)outptr ;
            TemplateMethods_Composite(invec,out2,elementSize,validmin,validmax,filldata,method);

        }else if( outdatatype==5 )
        {
            int* out2 = (int*)outptr ;
            TemplateMethods_Composite(invec,out2,elementSize,validmin,validmax,filldata,method);
        }else if( outdatatype==6 )
        {
            float* out2 = (float*)outptr ;
            TemplateMethods_Composite(invec,out2,elementSize,validmin,validmax,filldata,method);

        }else if( outdatatype==7 )
        {
            double* out2 = (double*)outptr ;
            TemplateMethods_Composite(invec,out2,elementSize,validmin,validmax,filldata,method);

        }
    }
    else if( indatatype==7 )
    {
        vector<double*> invec ;
        for(int i = 0 ; i<indataptrvec.size() ; ++ i ) invec.push_back( (double*) indataptrvec[i]) ;
        if( outdatatype==1 )
        {
            unsigned char* out2 = (unsigned char*)outptr ;
            TemplateMethods_Composite(invec,out2,elementSize,validmin,validmax,filldata,method);
        }else if( outdatatype==2 )
        {
            unsigned short* out2 = (unsigned short*)outptr ;
            TemplateMethods_Composite(invec,out2,elementSize,validmin,validmax,filldata,method);
        }else if( outdatatype==3 )
        {
            short* out2 = (short*)outptr ;
            TemplateMethods_Composite(invec,out2,elementSize,validmin,validmax,filldata,method);

        }else if( outdatatype==4 )
        {
            unsigned int* out2 = (unsigned int*)outptr ;
            TemplateMethods_Composite(invec,out2,elementSize,validmin,validmax,filldata,method);

        }else if( outdatatype==5 )
        {
            int* out2 = (int*)outptr ;
            TemplateMethods_Composite(invec,out2,elementSize,validmin,validmax,filldata,method);
        }else if( outdatatype==6 )
        {
            float* out2 = (float*)outptr ;
            TemplateMethods_Composite(invec,out2,elementSize,validmin,validmax,filldata,method);

        }else if( outdatatype==7 )
        {
            double* out2 = (double*)outptr ;
            TemplateMethods_Composite(invec,out2,elementSize,validmin,validmax,filldata,method);

        }
    }
}
