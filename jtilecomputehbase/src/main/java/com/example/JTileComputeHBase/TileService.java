package com.example.JTileComputeHBase;


import java.io.ByteArrayOutputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.List;

import org.springframework.core.io.ClassPathResource;
import org.springframework.stereotype.Service;

//hadoop hbase
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.hbase.*;
import org.apache.hadoop.hbase.client.*;
import org.apache.hadoop.hbase.io.compress.Compression.Algorithm;
import org.apache.hadoop.hbase.util.Bytes;
import ar.com.hjg.pngj.*;

@Service
public class TileService
{


    public static Configuration configuration;
    static {
        configuration = HBaseConfiguration.create();

        ClassPathResource hadooppath = new ClassPathResource("core-site.xml");
        ClassPathResource hbaseppath = new ClassPathResource("hbase-site.xml");

        try {

            InputStream hadoopStream = hadooppath.getInputStream();
            InputStream hbaseStream = hbaseppath.getInputStream();
            configuration.addResource(hbaseStream);
            configuration.addResource(hadoopStream);
            configuration.set("hbase.zookeeper.quorum", "hp1:2181");


        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }

    public  byte[] getTile( int pid,int col,int x,int y,int z)
    {
        try {
            String hbaseTableName = "fy3dtable" ;
            String hbaseColFamily = "tiles" ;

            Connection conn = ConnectionFactory.createConnection(configuration);
            TableName tablename = TableName.valueOf(hbaseTableName) ;//
            Table table = conn.getTable(tablename) ;

            byte[] rowkey1 = GenerateRowkey( pid , z , y ,x ) ;

            Get get = new Get(rowkey1);
            get.addColumn("tiles".getBytes() , Bytes.toBytes(col) ) ;
            Result result = table.get(get);
            if(null ==result)
            {
                System.out.println("404 no tile") ;
                return null ;
            }
            if(result.listCells()==null | result.listCells().size()==0)
            {
                System.out.println("404 no tile") ;
                return null ;
            }
            Cell cell1 = result.listCells().get(0) ;
            byte[] outByteData = CellUtil.cloneValue(cell1) ;
            byte[] pngData=  makePngByByteArray(outByteData);
            System.out.println(" bytes len  " + outByteData.length ) ;
            return pngData;

        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        return null;
    }


    public  byte[] getTileData( String tabname, String colfam, int pid, int col,int x,int y,int z)
    {
        try {
            String hbaseTableName = tabname ;
            String hbaseColFamily = colfam ;

            Connection conn = ConnectionFactory.createConnection(configuration);
            TableName tablename = TableName.valueOf(hbaseTableName) ;//
            Table table = conn.getTable(tablename) ;

            byte[] rowkey1 = GenerateRowkey( pid , z , y ,x ) ;

            Get get = new Get(rowkey1);
            get.addColumn(hbaseColFamily.getBytes() , Bytes.toBytes(col) ) ;
            Result result = table.get(get);
            if(null ==result)
            {
                System.out.println("404 no tile") ;
                return null ;
            }
            if(result.listCells()==null || result.listCells().size()==0)
            {
                System.out.println("404 no tile") ;
                return null ;
            }
            Cell cell1 = result.listCells().get(0) ;
            byte[] outByteData = CellUtil.cloneValue(cell1) ;
            System.out.println(" bytes len  " + outByteData.length ) ;
            return outByteData;

        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        return null;
    }


    public static byte ComputeBucketId( int z , int y , int x )
    {
        int numberRegion = 10 ;
        return (byte)((x+y)%numberRegion) ;
    }

    public static byte[] GenerateRowkey( int pdtid , int z , int y , int x ) throws IOException {
        byte bucketid = ComputeBucketId( z , y , x ) ;
        ByteArrayOutputStream bos = new ByteArrayOutputStream() ;
        DataOutputStream dos = new DataOutputStream(bos);
        dos.writeByte(bucketid);
        dos.writeByte((byte)pdtid) ;
        dos.writeByte((byte)z);
        dos.writeShort(y);
        dos.writeShort(x);
        dos.flush();
        return bos.toByteArray();
    }


    //根据
    public static byte[] makePngByByteArray(
            byte[] tiledata ) //0-fill , 1-land , 2-water
    {
        ImageInfo info = new ImageInfo(256,256 , 8, true ) ;
        ByteArrayOutputStream outstream = new ByteArrayOutputStream() ;
        PngWriter writer = new PngWriter(outstream ,info ) ;
        for (int row = 0; row < 256; row++) {
            ImageLineInt iline = new ImageLineInt(info);
            int theoffset = row * 256;
            for (int col = 0; col < 256 ; col++) { // this line will be written to all rows
                int it = theoffset  + col ;
                byte valx = tiledata[it] ;
                int r = 255;
                int g = 255;
                int b = 255;
                int a = 255;
                if( valx == 1 )
                {
                    r = 220; g = 221; b = 222;    a = 255;
                    // r = 255;g = 0; b = 0;  a = 255;
                }else if( valx==2 )
                {
                    r = 110; g = 207;b = 247; a = 255;
                    //	 r = 0; g = 0; b = 255;  a = 255;
                }
                ImageLineHelper.setPixelRGBA8( iline , col , r , g , b , a);
            }
            writer.writeRow(iline);
        }
        writer.end();
        return outstream.toByteArray() ;
    }
}
