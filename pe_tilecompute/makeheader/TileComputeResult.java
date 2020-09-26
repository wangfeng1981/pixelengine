package com.pixelengine;

public class TileComputeResult {
    int status ; // 0 is ok
    String log ; // error or log messages
    int outType; //0-dataset , 1-png
    int dataType; // 1-byte, 2-u16 , 3-i16 , 4-u32 , 5-i32 , 6-f32, 7-f64
    int width,height,nbands;
    byte[] binaryData ;
}

