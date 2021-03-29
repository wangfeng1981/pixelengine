package com.pixelengine;

import java.io.*;

public class Main {

    public static void main(String[] args) throws IOException {
	// write your code here
        System.out.println(System.getProperty("java.library.path"));
        System.loadLibrary("CppZonalStat");


        int[] segs = new int[128*3] ;
        for(int y=0; y<128;++y ){
            segs[y*3+0] = 0 ;
            segs[y*3+1] = y ;
            segs[y*3+2] = 128 ;
        }

        BufferedReader consoleReader = new BufferedReader(new InputStreamReader(System.in));

        while(true){
            // Reading data using readLine
            System.out.println("==== please input filename ====") ;
            String filename = consoleReader.readLine();

            System.out.println("==== please input datatype:") ;
            int datatype = Integer.parseInt(consoleReader.readLine());

            System.out.println("==== please input method:") ;
            int method = Integer.parseInt(consoleReader.readLine());

            File thefile = new File(filename) ;
            if( thefile.exists()==false ){
                System.out.println("file not ok");
                continue;
            }
            InputStream inputStream = new FileInputStream(filename);
            long fileSize = thefile.length();
            byte[] allBytes = new byte[(int) fileSize];
            inputStream.read(allBytes);
            inputStream.close();

            System.out.println("filename:"+filename);
            System.out.println("fileSize:"+fileSize ) ;
            System.out.println("size/256/256:"+fileSize/256/256);

            CppZonalStat zs = new CppZonalStat() ;

            int datalen = convertdatalen(datatype) ;

            short [] validpxcntdata = new short[256*256] ;
            byte[] targetdata = new byte[256*256*datalen] ;

            byte[] banddata0 = new byte[256*256*datalen];
            System.arraycopy( allBytes, 0, banddata0, 0 , 256*256*datalen );
            byte[] banddata1 = new byte[256*256*datalen] ;
            System.arraycopy( allBytes, 1*256*256*datalen, banddata1, 0 , 256*256*datalen );

            zs.assignDataBytes(banddata0, 0, 99999, datatype, targetdata, validpxcntdata );
            zs.combineDataBytes(banddata1, 0, 99999, datatype, method, targetdata, validpxcntdata );



            ZonalStatResult result = zs.doit(segs,targetdata,0,0,360/256.0, 0, 99999,
                    0, datatype,0) ;
            if( result != null){
                System.out.println("count:"+result.count);
                System.out.println("areakm2:"+result.areakm2);
                System.out.println("fillcount:"+result.fillcount);
                System.out.println("sum:"+result.sum);
                System.out.println("totalcount:"+result.totalcount);
                System.out.println("valmax:"+result.valmax);
                System.out.println("valmin:"+result.valmin);
            }

        }

        
    }
    public static int convertdatalen( int datatype){
        if( datatype==1 ) return 1;
        else if( datatype<=3 ) return 2;
        else if( datatype<=6 ) return 4;
        else if( datatype==7 ) return 8;
        else return 0 ;
    }
}
