package com.example.JTileComputeHBase;

import ar.com.hjg.pngj.ImageInfo;
import ar.com.hjg.pngj.ImageLineHelper;
import ar.com.hjg.pngj.ImageLineInt;
import ar.com.hjg.pngj.PngWriter;

import com.fasterxml.jackson.databind.annotation.JsonDeserialize;
import net.minidev.json.JSONValue;
import org.ahocorasick.trie.Emit;
import org.ahocorasick.trie.Trie;
import org.apache.avro.generic.GenericData;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.hbase.HBaseConfiguration;
import org.json.JSONException;
import org.json.JSONObject;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpHeaders;
import org.springframework.http.HttpStatus;
import org.springframework.http.MediaType;
import org.springframework.http.ResponseEntity;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.CrossOrigin;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.ResponseBody;

import java.io.ByteArrayOutputStream;
import java.lang.reflect.Array;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Collection;
import java.util.List;

@Controller
public class TileComputingController {

    @Autowired
    private TileService tileserver;


    @ResponseBody
    @RequestMapping(value="/tilecompute",method= RequestMethod.GET)
    @CrossOrigin(origins = "*")
    public ResponseEntity<byte[]> tileCompute(int sid, int t, int z , int y , int x){
        //sid = script-id
        // t  = datetime
        // z,y,x = tile z,y,x

        String theScript = JTileComputeHBaseApplication.scriptList.get(sid) ;
        List<PixelEngineDatasetDescriptor> desList = extractDatasetDescriptorFromScript(theScript) ;
        boolean isEveryDatasetLoaded = true ;
        String loadError = "" ;
        List<byte[]> tileDataList = new ArrayList<>() ;
        for(int ides = 0 ; ides < desList.size() ; ++ ides )
        {
            int datetime1 = desList.get(ides).datetime ;
            if( datetime1 < 0 ) datetime1 = t ;
            byte[] tileData= tileserver.getTileData(desList.get(ides).tableName,"tiles",
                    desList.get(ides).productId, datetime1 , x, y, z);
            if( tileData == null )
            {
                isEveryDatasetLoaded = false ;
                loadError = "Error 101: no tile data table,pid,z,y,x:"+desList.get(ides).tableName
                        +"," + desList.get(ides).productId + "," + z + "," + y + "," +x;
                break ;
            }else
            {
                tileDataList.add( tileData ) ;
            }
        }

        if( isEveryDatasetLoaded==false )
        {
            //System.out.println(z+" " +y+" "+x + " " + " no tile") ;
            System.out.println(loadError);
            final HttpHeaders headers = new HttpHeaders();
            headers.setContentType(MediaType.TEXT_PLAIN);
            return new ResponseEntity<byte[]>( loadError.getBytes(), headers, HttpStatus.NOT_FOUND);
        }else if( tileDataList.size()==0 )
        {
            loadError = "Error 102 : empty dataset descriptors." ;
            System.out.println(loadError);
            final HttpHeaders headers = new HttpHeaders();
            headers.setContentType(MediaType.TEXT_PLAIN);
            return new ResponseEntity<byte[]>( loadError.getBytes(), headers, HttpStatus.NOT_FOUND);
        }
        else
        {
            System.out.println(z+" " +y+" "+x + " " + " HAS tiles") ;
            //call google cpp guys with v8 staff do computing.

            byte[] pngData = makePng( tileDataList.get(0) ) ;
            final HttpHeaders headers = new HttpHeaders();
            headers.setContentType(MediaType.IMAGE_PNG);
            return new ResponseEntity<byte[]>(pngData, headers, HttpStatus.OK);
        }
    }

    @ResponseBody
    @RequestMapping(value="/script",method= RequestMethod.POST)
    @CrossOrigin(origins = "*")
    public String submitScript (String script) throws JSONException {

        int newindex = JTileComputeHBaseApplication.scriptList.size() ;
        JTileComputeHBaseApplication.scriptList.add( script) ;
        System.out.println("scriptid:"+ newindex + "; scriptext:" + script);
        return "scriptId: "+ newindex ;
    }


    public static byte[] makePng(
            byte[] tiledata ) {
        ImageInfo info = new ImageInfo(256, 256, 8, true);
        ByteArrayOutputStream outstream = new ByteArrayOutputStream();
        PngWriter writer = new PngWriter(outstream, info);
        ByteBuffer bb = ByteBuffer.allocate(2);
        for (int row = 0; row < 256; row++) {
            ImageLineInt iline = new ImageLineInt(info);
            int theoffset = row * 256;
            for (int col = 0; col < 256; col++) { // this line will be written to all rows
                int it = theoffset + col;
                bb.put(0, tiledata[it * 2 + 1]);//is working?
                bb.put(1, tiledata[it * 2 + 0]);
                short valx = bb.getShort(0);
                int r = 255;
                int g = 255;
                int b = 255;
                int a = 255;
                if (valx < 0) {
                    r = 0;
                    g = 0;
                    b = 0;
                    a = 0;
                } else {
                    int colorval = Math.min(255, (int) (valx / 9000.f * 255));
                    r = colorval;
                    g = colorval;
                    b = colorval;
                    a = 255;
                }
                ImageLineHelper.setPixelRGBA8(iline, col, r, g, b, a);
            }
            writer.writeRow(iline);
        }
        writer.end();
        return outstream.toByteArray() ;
    }

    //get dataset name (hbase table name ) from script PE.Dataset("xxx")
    public static List<PixelEngineDatasetDescriptor> extractDatasetDescriptorFromScript( String script )
    {
        List<PixelEngineDatasetDescriptor> desList = new ArrayList<>() ;
        Trie trie = Trie.builder().ignoreOverlaps()
                .addKeyword("PixelEngine.Dataset(\"")
                .addKeyword("\")")
                .build();
        Collection<Emit> emits = trie.parseText(script);
        Emit[] emitArray = emits.toArray( new Emit[emits.size()])  ;
        for( int i = 0 ; i<emitArray.length-1 ; ++ i )
        {
            Emit emit1 = emitArray[i] ;
            Emit emit2 = emitArray[i+1] ;
            if( emit1.getKeyword() == "PixelEngine.Dataset(\""
                && emit2.getKeyword()=="\")")
            {
                String name1 = script.substring( emit1.getEnd()+1 , emit2.getStart() ) ;
                if( name1.length() > 0 )
                {
                    PixelEngineDatasetDescriptor des1 = new PixelEngineDatasetDescriptor() ;
                    des1.tableName = name1 ;
                    desList.add(des1) ;
                }
            }
        }
        return desList ;
    }
}