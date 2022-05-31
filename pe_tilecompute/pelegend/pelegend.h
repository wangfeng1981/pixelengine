//pelegend.h
#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include "wdirtools.h"
#include "ajson5.h"
#include "cairo.h"
#include <algorithm>

using namespace ArduinoJson;
using std::string;
using std::cout;
using std::endl;
using std::vector;


struct PeLegend 
{
	PeLegend() ;
	~PeLegend() ;

	// return 0 is ok, others are bad.
	int makePngByJsonText( string jsontext , string outpngfile , int dpi , string& error ) ;
	int makePngByJsonFile( string filepath , string outpngfile , int dpi , string& error ) ;
    inline string version() { return "v0.1.2.1"  ; } //2022-5-31 commit

    int outPngWid,outPngHei ;//这两个属性仅在生成png之后有效
} ;



struct wCairo {

public:
	inline wCairo():context(0),surface(0),width(0),height(0) {} 
	inline ~wCairo(){ if(context!=0)cairo_destroy(context); if(surface!=0)cairo_surface_destroy(surface);}
	int savePng(string filepath) ;
    int savePng2(string filepath,int& retWid,int& retHei) ;
	int create(int wid,int hei) ;
	
	inline cairo_t* getContext() { return context;} 
	void fillRect(int x,int y,int w,int h,int r,int g,int b,int a) ;
	void fillGradientRect(int x,int y,int w,int h,
						int r0,int g0,int b0,int a0, 
						int r1,int g1,int b1,int a1 ) ;
	void fillGradientRectLeft2Right(int x,int y,int w,int h,
						int r0,int g0,int b0,int a0, 
						int r1,int g1,int b1,int a1 ) ;
	
	void strokeRect(int x,int y,int w,int h,int r,int g,int b,int a) ;
	void strokeLine(int x0,int y0,int x1,int y1,int lineWid,int r,int g,int b,int a) ;

	void drawText(int x,int y,int fontSize, string text) ;

	
private:
	cairo_t * context ;
	cairo_surface_t* surface ;
	int width;
	int height;

	

	void getContentRect(int& x0,int& y0,int& wid,int& hei) ;

} ;
