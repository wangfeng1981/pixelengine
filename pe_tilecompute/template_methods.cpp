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

void CallTemplateMethods_BuildMask( void* indataptr,int indatatype,unsigned char* outptr,int elementSize,double maskval)
{
    if( indatatype==1 )
    {
        TemplateMethods_BuildMask( (unsigned char*)indataptr, outptr, elementSize, maskval );
    }else if( indatatype==2 )
    {
        TemplateMethods_BuildMask( (unsigned short*)indataptr, outptr, elementSize, maskval );
    }else if( indatatype==3 )
    {
        TemplateMethods_BuildMask( (short*)indataptr, outptr, elementSize, maskval );
    }else if( indatatype==4 )
    {
        TemplateMethods_BuildMask( (unsigned int*)indataptr, outptr, elementSize, maskval );
    }else if( indatatype==5 )
    {
        TemplateMethods_BuildMask( (int*)indataptr, outptr, elementSize, maskval );
    }else if( indatatype==6 )
    {
        TemplateMethods_BuildMask( (float*)indataptr, outptr, elementSize, maskval );
    }else if( indatatype==7 )
    {
        TemplateMethods_BuildMask( (double*)indataptr, outptr, elementSize, maskval );
    }
}
void CallTemplateMethods_BuildMask2( void* indataptr,int indatatype,unsigned char* outptr,int elementSize,double vminInc,double vmaxInc)
{
    if( indatatype==1 )
    {
        TemplateMethods_BuildMask2( (unsigned char*)indataptr,outptr,elementSize,vminInc,vmaxInc );
    }else if( indatatype==2 )
    {
        TemplateMethods_BuildMask2( (unsigned short*)indataptr,outptr,elementSize,vminInc,vmaxInc );
    }else if( indatatype==3 )
    {
        TemplateMethods_BuildMask2( (short*)indataptr,outptr,elementSize,vminInc,vmaxInc );
    }else if( indatatype==4 )
    {
        TemplateMethods_BuildMask2( (unsigned int*)indataptr,outptr,elementSize,vminInc,vmaxInc );
    }else if( indatatype==5 )
    {
        TemplateMethods_BuildMask2( (int*)indataptr,outptr,elementSize,vminInc,vmaxInc );
    }else if( indatatype==6 )
    {
        TemplateMethods_BuildMask2( (float*)indataptr,outptr,elementSize,vminInc,vmaxInc );
    }else if( indatatype==7 )
    {
        TemplateMethods_BuildMask2( (double*)indataptr,outptr,elementSize,vminInc,vmaxInc );
    }
}


void CallTemplateMethods_DsMap( void* dataPtr,int dataType,int nband,int elementSize,double oldval,double newval)
{
    if( dataType==1 )
    {
        TemplateMethods_DsMap( (unsigned char*)dataPtr,nband, elementSize, oldval, newval );
    }else if( dataType==2 )
    {
        TemplateMethods_DsMap( (unsigned short*)dataPtr,nband, elementSize, oldval, newval );
    }else if( dataType==3 )
    {
        TemplateMethods_DsMap( (short*)dataPtr,nband, elementSize, oldval, newval );
    }else if( dataType==4 )
    {
        TemplateMethods_DsMap( (unsigned int*)dataPtr,nband, elementSize, oldval, newval );
    }else if( dataType==5 )
    {
        TemplateMethods_DsMap( (int*)dataPtr,nband, elementSize, oldval, newval );
    }else if( dataType==6 )
    {
        TemplateMethods_DsMap( (float*)dataPtr,nband, elementSize, oldval, newval );
    }else if( dataType==7 )
    {
        TemplateMethods_DsMap( (double*)dataPtr,nband, elementSize, oldval, newval );
    }
}
void CallTemplateMethods_DsMap2( void* dataPtr,int dataType,int nband,int elementSize,double vminInc,double vmaxInc,double newval)
{
    if( dataType==1 )
    {
        TemplateMethods_DsMap2( (unsigned char*)dataPtr,nband, elementSize, vminInc,vmaxInc , newval );
    }else if( dataType==2 )
    {
        TemplateMethods_DsMap2( (unsigned short*)dataPtr,nband, elementSize, vminInc,vmaxInc, newval );
    }else if( dataType==3 )
    {
        TemplateMethods_DsMap2( (short*)dataPtr,nband, elementSize, vminInc,vmaxInc, newval );
    }else if( dataType==4 )
    {
        TemplateMethods_DsMap2( (unsigned int*)dataPtr,nband, elementSize, vminInc,vmaxInc, newval );
    }else if( dataType==5 )
    {
        TemplateMethods_DsMap2( (int*)dataPtr,nband, elementSize, vminInc,vmaxInc, newval );
    }else if( dataType==6 )
    {
        TemplateMethods_DsMap2( (float*)dataPtr,nband, elementSize, vminInc,vmaxInc, newval );
    }else if( dataType==7 )
    {
        TemplateMethods_DsMap2( (double*)dataPtr,nband, elementSize, vminInc,vmaxInc, newval );
    }
}

void CallTemplateMethods_TileDataMask( void* dataPtr,int dataType,int nband,int elementSize, unsigned char* maskdata,double filldata)
{
    if( dataType==1 )
    {
        TemplateMethods_TileDataMask( (unsigned char*)dataPtr,nband, elementSize, maskdata , filldata );
    }else if( dataType==2 )
    {
        TemplateMethods_TileDataMask( (unsigned short*)dataPtr,nband, elementSize, maskdata , filldata );
    }else if( dataType==3 )
    {
        TemplateMethods_TileDataMask( (short*)dataPtr,nband, elementSize, maskdata , filldata );
    }else if( dataType==4 )
    {
        TemplateMethods_TileDataMask( (unsigned int*)dataPtr,nband, elementSize, maskdata , filldata );
    }else if( dataType==5 )
    {
        TemplateMethods_TileDataMask( (int*)dataPtr,nband, elementSize,maskdata , filldata );
    }else if( dataType==6 )
    {
        TemplateMethods_TileDataMask( (float*)dataPtr,nband, elementSize, maskdata , filldata );
    }else if( dataType==7 )
    {
        TemplateMethods_TileDataMask( (double*)dataPtr,nband, elementSize, maskdata , filldata );
    }
}
