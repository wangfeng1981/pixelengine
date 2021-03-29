package com.pixelengine;

public class ZonalStatResult {
	public long count ;
	public double sum ;
    public double valmin ;
    public double valmax ;
    public double areakm2 ;//km2
    public long totalcount;
    public long fillcount;
    public void setValues(long cnt,double tsum,double vmin,double vmax,double area,long totcnt,long fcnt ) 
    {
    	count = cnt ;
    	sum = tsum ;
    	valmin = vmin;
    	valmax = vmax ;
    	areakm2 = area;
    	totalcount = totcnt;
    	fillcount = fcnt ;
    }
}
