//com_pixelengine_HBasePeHelperCppConnector
package com.pixelengine;

//Java离线计算与C++接口。

public class HBasePeHelperCppConnector {
    public native String ParseScriptForDsDt( String script ) ;
    public native TileComputeResult RunScriptForTileWithoutRender( String script, long datetime, int z , int y , int x) ;    
}

