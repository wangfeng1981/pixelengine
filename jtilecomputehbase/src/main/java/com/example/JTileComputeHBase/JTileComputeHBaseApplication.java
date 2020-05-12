package com.example.JTileComputeHBase;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;

import java.util.ArrayList;
import java.util.List;

@SpringBootApplication
public class JTileComputeHBaseApplication {
	public static List<String> scriptList = null ;
	public static void main(String[] args) {
		scriptList = new ArrayList<String>() ;

		String demoScript1 = "function pixelfunction(data){ var ndvi = 0.0 ; if(data[0]>=0)"+
		"{  ndvi = (data[4]-data[3])/(data[4]+data[3]) ; }else {  ndvi = -1 ; }"+
		" return PE.LinearColorize( 0.0 , 1.0 , ndvi , COLOR_RAMP_GRAYS , -1 ) ;}"+
		" function datasetfunction(){ var ds = PixelEngine.Dataset(\"fy3dtable\").current() ;"+
		" var retds = ds.pixelFunction(pixelfunction) ; return retds ;} " ;
		scriptList.add( demoScript1 ) ;


		SpringApplication.run(JTileComputeHBaseApplication.class, args);
	}

}
