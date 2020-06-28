package com.pixelengine;
//V8Helper这个类用于与C++进行交换数据，相互调用。
import java.util.ArrayList;

public class V8Helper {

    // this two methods used to call c++ function
    public native byte[] CallTileCompute(String script,long currentDatetime,int z,int y,int x);
    public native String Version();
    public native String CheckScriptOK(String script);
    public native V8HelperResult CallTileComputeV2(String script,long currentDatetime,int z,int y,int x) ;

    
}
