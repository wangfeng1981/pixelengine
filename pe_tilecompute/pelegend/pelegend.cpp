//pelegend.cpp
#include "pelegend.h"


PeLegend::PeLegend() 
{
    outPngWid = 0 ;
    outPngHei = 0 ;
}



PeLegend::~PeLegend() 
{

}


// return 0 is ok, others are bad.
int PeLegend::makePngByJsonText(string jsontext , string outpngfile,int dpi , string& error ) 
{

	//A4 297x210mm  dpi72 841px x 595px
	//1mm = 2.823650px for dpi72 ;
	//
	DynamicJsonBuffer jsonBuffer;
	JsonObject& root = jsonBuffer.parseObject(jsontext);
	bool parseOk = root.success() ;//检查解析json是否成功
	if( parseOk==false )
	{
		error = string("parse json failed.") ;
	    return 11 ;
	}

	const double scaleFor72Dpi = dpi / 72.0 ;// 1.0 for 72dpi

	const int pageWidth = 200 * scaleFor72Dpi;
	const int pageHeight = 400 * scaleFor72Dpi;
	const int boxwid = 24 * scaleFor72Dpi;
	const int boxhei = 14 * scaleFor72Dpi;
	const int boxTextMargin = 5 * scaleFor72Dpi;
	const int boxYMargin = 4 * scaleFor72Dpi;
	const int fontSize = 12 * scaleFor72Dpi;
	const int lineWidth = std::max(1.0, 1.0 * scaleFor72Dpi) ;
	const int textYOffset = (boxhei - fontSize) / 2 ;

	wCairo wc ;
	int state1 = wc.create(pageWidth,pageHeight) ;
	if( state1!=0 ) {
		return state1 ;
	}

	string type = root["type"].as<char*>() ;
	JsonArray& bandsArr = root["bands"].as<JsonArray>() ;
	JsonArray& vranges = root["vranges"].as<JsonArray>() ;
	JsonArray& colors = root["colors"].as<JsonArray>() ;

	if( type== "discrete" ){
		int boundx0 = 99999 ;
		int boundx1 = 0 ;
		int boundy0 = 99999 ;
		int boundy1 = 0 ;

		int iband = bandsArr[0].as<int>() ;
		int ncolors = colors.size() ;
		for(int i = 0 ; i<ncolors;  ++ i )
		{
			JsonObject& color1 = colors[i].as<JsonObject>() ;
			int r1 = color1["r"].as<int>() ;
			int g1 = color1["g"].as<int>() ;
			int b1 = color1["b"].as<int>() ;
			int a1 = color1["a"].as<int>() ;
			string lbl1 = color1["lbl"].as<char*>() ;
			wc.fillRect( 10 , boxhei + i*boxhei , boxwid,boxhei ,
			r1,g1,b1,a1) ;
			wc.drawText( 10 + boxwid + boxTextMargin , boxhei + i*boxhei + textYOffset,fontSize , lbl1) ;

			boundx0 = std::min(boundx0 , 10 ) ;
			boundx1 = std::max(boundx1 , 10+boxwid ) ;
			boundy0 = std::min(boundy0 , boxhei + i*boxhei ) ;
			boundy1 = std::max(boundy1 , boxhei + i*boxhei+boxhei ) ;
		}
		if( boundx1 > boundx0 )
				wc.strokeRect(boundx0 , boundy0 , boundx1 - boundx0+1 , boundy1-boundy0+1,0,0,0,255) ;
	}else if( type== "linear"){

		int boundx0 = 99999 ;
		int boundx1 = 0 ;
		int boundy0 = 99999 ;
		int boundy1 = 0 ;

		int iband = bandsArr[0].as<int>() ;
		int ncolors = colors.size() ;

		for(int i = 0 ; i<ncolors-1; ++ i )
		{
			JsonObject& color0 = colors[i].as<JsonObject>() ;
			JsonObject& color1 = colors[i+1].as<JsonObject>() ;
			int r0 = color0["r"].as<int>() ;
			int g0 = color0["g"].as<int>() ;
			int b0 = color0["b"].as<int>() ;
			int a0 = color0["a"].as<int>() ;

			int r1 = color1["r"].as<int>() ;
			int g1 = color1["g"].as<int>() ;
			int b1 = color1["b"].as<int>() ;
			int a1 = color1["a"].as<int>() ;

			string lbl0 = color0["lbl"].as<char*>() ;
			string lbl1 = color1["lbl"].as<char*>() ;

			wc.fillGradientRect( 10     , boxhei + i*boxhei     , boxwid,boxhei ,
				r0,g0,b0,a0,
				r1,g1,b1,a1) ;
			wc.strokeLine(
				10 + boxwid ,
				boxhei + i*boxhei  ,
				10 + boxwid + boxTextMargin/2 ,
				boxhei + i*boxhei  ,
				lineWidth,
				0,0,0,255
				) ;

			wc.drawText( 
				10 + boxwid + boxTextMargin , 
				boxhei + i*boxhei - (fontSize/2) , 
				fontSize,
				lbl0) ;

			if( i== ncolors-2 ){
				wc.drawText( 
					10 + boxwid + boxTextMargin,
					boxhei + (i+1)*boxhei - (fontSize/2) , 
					fontSize,
					lbl1) ;
				wc.strokeLine(
					10 + boxwid ,
					boxhei + i*boxhei + boxhei  ,
					10 + boxwid + boxTextMargin/2 ,
					boxhei + i*boxhei + boxhei ,
					lineWidth,
					0,0,0,255
					) ;

			}

			boundx0 = std::min(boundx0 , 10 ) ;
			boundx1 = std::max(boundx1 , 10+boxwid ) ;
			boundy0 = std::min(boundy0 , boxhei + i*boxhei ) ;
			boundy1 = std::max(boundy1 , boxhei + i*boxhei+boxhei ) ;
		}
		if( boundx1 > boundx0 )
				wc.strokeRect(boundx0 , boundy0 , boundx1 - boundx0+1 , boundy1-boundy0+1,0,0,0,255) ;
	}else if( type== "exact" )
	{
		int ncolors = colors.size() ;
		for(int i = 0 ; i<ncolors ; ++ i )
		{
			JsonObject& color0 = colors[i].as<JsonObject>() ;
			int r0 = color0["r"].as<int>() ;
			int g0 = color0["g"].as<int>() ;
			int b0 = color0["b"].as<int>() ;
			int a0 = color0["a"].as<int>() ;
			string lbl0 = color0["lbl"].as<char*>() ;

			wc.fillRect( 
				10     , 
				boxhei + i*(boxhei+boxYMargin)     , 
				boxwid,
				boxhei ,
				r0,g0,b0,a0 ) ;
			wc.strokeRect(
				10, 
				boxhei + i*(boxhei+boxYMargin),
				boxwid,
				boxhei ,
				0,0,0,255) ;
			wc.drawText( 
				10 + boxwid + boxTextMargin , 
				boxhei + i*(boxhei+boxYMargin) ,
				fontSize,
				lbl0) ;
		}

	}
	else if(type=="gray")
	{
		if( bandsArr.size()==0 || vranges.size() == 0 ){
			error = "bands or vranges size is zero." ;
			return  21 ;
		} 
		int iband = bandsArr[0].as<int>() ;
		JsonObject& vr1 = vranges[0].as<JsonObject>() ;
		double minval = vr1["minval"].as<double>() ;
		double maxval = vr1["maxval"].as<double>() ;

		wc.fillGradientRectLeft2Right( 10 , boxhei , boxwid,boxhei ,
			0,0,0,255,
			255,255,255,255 ) ;
		wc.strokeRect( 10 , boxhei , boxwid,boxhei  ,0,0,0,255) ;
		char buffer[512] ;
		sprintf(buffer , "B%d: %g - %g", iband+1, minval, maxval) ;
		string lbl(buffer) ;
		wc.drawText(
			10 + boxwid + boxTextMargin , 
			boxhei + textYOffset ,
			fontSize,
			lbl) ;

	}else if( type=="rgb")
	{
		if( bandsArr.size()<3 || vranges.size()<3 ){
			error = "bands or vranges size lower than 3." ;
			return  21 ;
		} 
		int rarr[] = {255,0,0} ;
		int garr[] = {0,255,0} ;
		int barr[] = {0,0,255} ;
		for(int ib=0 ; ib<3;++ib )
		{
			int iband = bandsArr[ib].as<int>() ;
			JsonObject& vr1 = vranges[ib].as<JsonObject>() ;
			double minval = vr1["minval"].as<double>() ;
			double maxval = vr1["maxval"].as<double>() ;

			wc.fillRect( 10 , 
				boxhei+ib*(boxhei+boxYMargin) ,
				boxwid,
				boxhei ,
				rarr[ib], garr[ib] , barr[ib] ,255 ) ;
			wc.strokeRect( 10 , 
				boxhei+ib*(boxhei+boxYMargin) ,
				boxwid,
				boxhei  ,
				0,0,0,255) ;

			char buffer[512] ;
			sprintf(buffer , "B%d: %g - %g", iband+1, minval, maxval) ;
			string lbl(buffer) ;
			wc.drawText(
				10 + boxwid + boxTextMargin,
				boxhei+ib*(boxhei+boxYMargin) + textYOffset ,
				fontSize,
				lbl) ;
		}
		
	}else if( type=="rgba")
	{
		if( bandsArr.size()!=4 || vranges.size()!=4 ){
			error = "bands or vranges size not 4." ;
			return  21 ;
		} 
		int rarr[] = {255,0,0,0} ;
		int garr[] = {0,255,0,0} ;
		int barr[] = {0,0,255,0} ;
		int aarr[] = {255,255,255,0} ;
		for(int ib=0 ; ib<4;++ib )
		{
			int iband = bandsArr[ib].as<int>() ;
			JsonObject& vr1 = vranges[ib].as<JsonObject>() ;
			double minval = vr1["minval"].as<double>() ;
			double maxval = vr1["maxval"].as<double>() ;

			wc.fillRect( 10 , 
				boxhei+ib*(boxhei+boxYMargin) , 
				boxwid,
				boxhei ,
				rarr[ib], garr[ib] , barr[ib] ,aarr[ib] ) ;
			wc.strokeRect( 10 , 
				boxhei+ib*(boxhei+boxYMargin) , 
				boxwid,
				boxhei  ,
				0,0,0,255) ;
			char buffer[512] ;
			sprintf(buffer , "B%d: %g - %g", iband+1, minval, maxval) ;
			string lbl(buffer) ;
			wc.drawText(
				10 + boxwid + boxTextMargin, 
				boxhei+ib*(boxhei+boxYMargin) + textYOffset ,
				fontSize,
				lbl) ;
		}
	}

    wc.savePng2(outpngfile,outPngWid,outPngHei) ;

	return 0 ;
}


// return 0 is ok, others are bad.
int PeLegend::makePngByJsonFile( string filepath , string outpngfile, int dpi , string& error ) 
{
	string text ;

	bool readok = wDirTools::readFullText( filepath , text ) ;
	if( readok==false ){
		error = "failed to read file." ;
		return 1 ;
	}

	int stat = this->makePngByJsonText(text , outpngfile, dpi , error ) ;

	if( stat != 0 ){
		cout<<"makePngByJsonText failed: "<<error<<endl ;
	}
	
	return stat ;
}




/////////////////////////////////////
///
///
///
int wCairo::savePng(string filepath) 
{
	if( context==0 ) return 1 ;
	if( surface==0 ) return 2 ;

	int x0,y0,wid,hei ;
	getContentRect(x0,y0,wid,hei) ;
    //cout<<"debug "<<x0<<","<<y0<<","<<wid<<","<<hei<<endl ;
	cairo_surface_t* surf2 = 
	cairo_surface_create_for_rectangle (surface,
                                    x0,
                                    y0,
                                    wid,
                                    hei);
	cairo_surface_write_to_png( surf2 , filepath.c_str() ) ;
	return 0 ;
}

int wCairo::savePng2(string filepath,int& retWid,int& retHei)
{
    if( context==0 ) return 1 ;
    if( surface==0 ) return 2 ;
    int x0,y0,wid,hei ;
    getContentRect(x0,y0,wid,hei) ;
    //cout<<"debug "<<x0<<","<<y0<<","<<wid<<","<<hei<<endl ;
    cairo_surface_t* surf2 =
    cairo_surface_create_for_rectangle (surface,
                                    x0,
                                    y0,
                                    wid,
                                    hei);
    cairo_surface_write_to_png( surf2 , filepath.c_str() ) ;
    retWid = wid;
    retHei = hei ;
    return 0 ;
}

int wCairo::create(int wid,int hei) 
{
	width = wid ;
	height = hei ;
	surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, width, height);
    if( surface==0 ) return 1 ;
    context = cairo_create (surface);
    if( context==0 ) return 2 ;
    return 0 ;
}

void wCairo::fillRect(int x,int y,int w,int h,int r,int g,int b,int a) 
{
	if( context==0 ) return ;
	cairo_rectangle (context, x,y,w,h);
	cairo_set_source_rgba (context, r/255.0 , g/255.0, b/255.0, a/255.0 );
	cairo_fill (context);
}

void wCairo::drawText(int x,int y,int fontSize, string text) 
{
	if( context == 0 ) return ;
	cairo_select_font_face (context, "Noto Sans CJK SC", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL );
	cairo_set_font_size (context, fontSize);
	cairo_set_source_rgb (context, 0.0, 0.0, 0.0);
	cairo_text_extents_t extents;
	cairo_text_extents(context, text.c_str() , &extents);
	cairo_move_to (context, x, y + extents.height + ( fontSize - extents.height)/2 );
	cairo_show_text (context, text.c_str() );
}

void wCairo::fillGradientRect(int x,int y,int w,int h,
						int r0,int g0,int b0,int a0, 
						int r1,int g1,int b1,int a1 ) 
{
	if( context==0 ) return ;
	cairo_pattern_t *pat  = cairo_pattern_create_linear ( x , y,  x, y+h-1);
	cairo_pattern_add_color_stop_rgba (pat, 0, r0/255., g0/255., b0/255., a0/255.);
	cairo_pattern_add_color_stop_rgba (pat, 1, r1/255., g1/255., b1/255., a1/255.);
	cairo_rectangle (context, x ,  y, w, h );
	cairo_set_source (context, pat);
	cairo_fill (context);
	cairo_pattern_destroy (pat);
}

void wCairo::fillGradientRectLeft2Right(int x,int y,int w,int h,
						int r0,int g0,int b0,int a0, 
						int r1,int g1,int b1,int a1 ) 
{
	if( context==0 ) return ;
	cairo_pattern_t *pat  = cairo_pattern_create_linear ( x , y,  x+w-1, y);
	cairo_pattern_add_color_stop_rgba (pat, 0, r0/255., g0/255., b0/255., a0/255.);
	cairo_pattern_add_color_stop_rgba (pat, 1, r1/255., g1/255., b1/255., a1/255.);
	cairo_rectangle (context, x ,  y, w, h );
	cairo_set_source (context, pat);
	cairo_fill (context);
	cairo_pattern_destroy (pat);
}

void wCairo::strokeRect(int x,int y,int w,int h,int r,int g,int b,int a) 
{
	if( context==0 ) return ;
	cairo_rectangle (context, x,y,w,h);
	cairo_set_source_rgba (context, r/255.0 , g/255.0, b/255.0, a/255.0 );
	cairo_stroke (context);
}

void wCairo::strokeLine(int x0,int y0,int x1,int y1,int lineWid,int r,int g,int b,int a) 
{
	if( context==0 ) return ;
	cairo_set_source_rgba (context , r/255. , g/255. , b/255. , a/255. );
	cairo_set_line_width ( context , lineWid);
	cairo_move_to (context, x0,y0);   
	cairo_line_to (context, x1,y1);
	cairo_stroke (context);
}

void wCairo::getContentRect(int& x0,int& y0,int& wid,int& hei) 
{
	x0 = 0 ;
	y0 = 0 ;
	wid = 1 ;
	hei = 1 ;
	if( surface==0 ) return ;

	cairo_surface_flush(surface);
	unsigned char* data = cairo_image_surface_get_data (surface);
	if( data==0 ) return ;
	x0 = 99999 ;
	int x1 = 0 ;
	y0 = 99999 ;
	int y1 = 0 ;
	for(int iy =0 ; iy < height; ++ iy )
	{
		for(int ix = 0 ; ix < width ; ++ ix )
		{
			int it = (iy * width + ix)*4 ;//rgba
			if( data[it+0]>0 || data[it+1]> 0 ||  data[it+2]> 0 ||  data[it+3]> 0 )
			{
				x0=std::min( x0 ,  ix ) ;
				x1 = std::max(x1 , ix ) ;
				y0 = std::min( y0 , iy ) ;
				y1 = std::max( y1 , iy ) ;
			}
		}
	}
	if( x1 > x0 && y1 > y0 ){
		wid = x1 - x0 + 1;
		hei = y1 - y0 + 1;
	}else{
		x0 = 0 ;
		y0 = 0 ;
		wid = 1 ;
		hei = 1 ;
	}
}
