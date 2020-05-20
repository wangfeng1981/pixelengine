
public class Demo {

    static {
        System.loadLibrary("TileCompute") ;
    }

    private native byte[] CallTileCompute();
    private native String Version();

    public Demo(){   //Constructor
        super();
    }
   
    public int GetInt()
    {
        return 99 ;
    }

    public byte[] GetTileData( String table, String dt, int z,int y,int x)
    {
        System.out.println("table:" + table ) ;
        System.out.println("dt:" + dt ) ;
        System.out.println("z:" + z ) ;
        System.out.println("y:" + y ) ;
        System.out.println("x:" + x ) ;
        byte[] data = new byte[256*256] ;
        for(int i = 0 ; i<256*256 ; ++ i ) data[i] = 1 ;
        return data ;
    }


    public static void main(String[] args){
        System.out.println("ok") ;

        Demo d1 = new Demo() ;
        byte[] png = d1.CallTileCompute() ;
        int p0 = png[0] ;
        System.out.println( String.format("%x",png[0]) ) ;

        //
        System.out.println("java out") ;
    }
}