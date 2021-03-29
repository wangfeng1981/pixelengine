//
package com.pixelengine;
//libCppZonalStat.so in /usr/lib
//区域统计c++实现，在java中调用

public class CppZonalStat {
    static {
        System.loadLibrary("CppZonalStat") ;
    }
    public native ZonalStatResult doit(int[]segs,byte[]data,int tiley,int tilex,double reso,double valid0,double valid1,
        int iband,int datatype,int filldata) ;

    //第一次调用把二进制数据写入目标数据，有效范围内的validpxcntdata记录1，其他记录0.
    public native void assignDataBytes(byte[] sourcedata,double valid0,double valid1,int datatype,byte[] targetdata,short[] validpxcntdata) ;
    //后续合成调用把二进制数据合成到目标数据里，有效范围内的validpxcntdata记录+1，其他记录0.
    //method -1-最小合成 +1-最大合成 0-平均合成
    public native void combineDataBytes(byte[] sourcedata,double valid0,double valid1,int datatype,int method,byte[] targetdata,short[] validpxcntdata) ;
}

