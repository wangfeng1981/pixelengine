package com.pixelengine;

public class V8Helper {
    static {
        System.loadLibrary("TileCompute") ;
    }

    // this two methods used to call c++ function
    public native byte[] CallTileCompute(String script,int currentDatetime,int z,int y,int x);
    public native String Version();

    //this method will be called by c++
    public byte[] GetTileData( String table , String dateTime , int z , int y , int x)
    {
        byte[] tiledata= new byte[256*256] ;
        return tiledata ;
    }
}
