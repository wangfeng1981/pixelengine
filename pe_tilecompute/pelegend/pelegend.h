//pelegend.h
#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include "wdirtools.h"
#include "ajson5.h"
#include "cairo.h"
#include <algorithm>
#include "pelegendrenderproperty.h"

using namespace ArduinoJson;
using std::string;
using std::cout;
using std::endl;
using std::vector;
using std::max;
using std::min;

#define PELEGEND_TEXT_ALIGN_LEFT 0
#define PELEGEND_TEXT_ALIGN_CENTER 1
#define PELEGEND_TEXT_ALIGN_RIGHT 2

struct ColorElement {
    unsigned char r,g,b,a;
    string label;
    double val;
    inline ColorElement():r(0),g(0),b(0),a(0),val(0){}
};


struct PeLegend
{
	PeLegend() ;
	~PeLegend() ;

	// return 0 is ok, others are bad.
	int makePngByJsonText( string jsontext , string outpngfile , int dpi , string& error ) ;
	int makePngByJsonFile( string filepath , string outpngfile , int dpi , string& error ) ;
    inline string version() { return "v0.2.0.0d"  ; } //2022-5-31 commit

    /*!
     * \brief makePngByJsonText2 通过style json字符串绘制图例png图片,下面除了特殊指定都是72dpi下像素值单位，dpi不同的时候按比例计算像素值
     * \param jsontext stylejson字符串
     * \param renderPpt 渲染配置
     * \param outpngfile 输出结果png路径
     * \param dpi  输出dpi
     * \param error 错误信息
     * \return 0成功，其他不成功，错误信息查看error
     */
    int makePngByJsonText2(
            string jsontext ,
            PeLegendRenderProperty& renderPpt,
            string outpngfile ,
            int dpi ,
            string& error ) ;

    int makePngByJsonFile2(
            string filepath ,    //style.json.file
            string renderPptFile,//renderppt.json.file
            string outpngfile ,
            int dpi ,
            string& error ) ;

    int outPngWid,outPngHei ;//这两个属性仅在生成png之后有效
    inline void setTextColor(int r,int g,int b){textR=r;textG=g;textB=b;}
    inline void getTextColor(int& r,int& g,int& b){r=textR,g=textG,b=textB;}
    inline void setTextAlign(int align){textAlign=align;}
    inline int getTextAlign(){return textAlign;}
private:
    int textR,textG,textB ;//0-255
    int textAlign;// 0-left 1-center 2-right
    int drawGrayIntoPng(
            string outpngfile ,
            double minval,
            double maxval,
            int dpi ,
            int drawDirection,
            int labelFontSize,
            float labelDirectionAngle,
            int labelOffsetX,
            int labelOffsetY,
            int boxWid,
            int boxHei,
            int boxBorderWidth,
            int boxBorderColorR,
            int boxBorderColorG,
            int boxBorderColorB,
            string& error
            ) ;
    int drawRgbOrRgbaIntoPng(
            string outpngfile ,
            vector<int> bandIndexVec,//zero-based 3 or 4
            vector<double> minvalVec,//3 or 4
            vector<double> maxvalVec,//3 or 4
            int dpi ,
            int drawDirection,
            int labelFontSize,
            float labelDirectionAngle,
            int labelOffsetX,
            int labelOffsetY,
            int boxWid,
            int boxHei,
            int boxMarginX,
            int boxMarginY,
            int boxBorderWidth,
            int boxBorderColorR,
            int boxBorderColorG,
            int boxBorderColorB,
            string& error
            ) ;
    int drawLinearOrDiscreteIntoPng(
            string outpngfile ,
            bool isLinear,
            vector<ColorElement>& colorElmtVec,
            int dpi ,
            int drawDirection,
            int labelFontSize,
            float labelDirectionAngle,
            int labelOffsetX,
            int labelOffsetY,
            int boxWid,
            int boxHei,
            int boxBorderWidth,
            int boxBorderColorR,
            int boxBorderColorG,
            int boxBorderColorB,
            int tickWid,
            int tickColorR,
            int tickColorG,
            int tickColorB,
            string& error
            ) ;
    int drawExactIntoPng(
            string outpngfile ,
            vector<ColorElement> colorElmtVec,
            int dpi ,
            int drawDirection,
            int labelFontSize,
            float labelDirectionAngle,
            int labelOffsetX,
            int labelOffsetY,
            int boxWid,
            int boxHei,
            int boxMarginX,
            int boxMarginY,
            int boxBorderWidth,
            int boxBorderColorR,
            int boxBorderColorG,
            int boxBorderColorB,
            int nRows,
            int nCols,
            string& error
            ) ;
} ;


struct PeLegendRect {
   double x,y,w,h;
   inline PeLegendRect():x(0),y(0),w(0),h(0){}
} ;

struct wCairo {

public:
	inline wCairo():context(0),surface(0),width(0),height(0),textR(0),textG(0),textB(0) {}
	inline ~wCairo(){ if(context!=0)cairo_destroy(context); if(surface!=0)cairo_surface_destroy(surface);}
	int savePng(string filepath) ;
    int savePng2(string filepath,int& retWid,int& retHei) ;
	int create(int wid,int hei) ;
    void computePageSizeAndScale(const int dpi,int& pageWidth,int& pageHeight,double& scale) ;

	inline cairo_t* getContext() { return context;}
	void fillRect(int x,int y,int w,int h,int r,int g,int b,int a) ;

    //from ysmall to ysmall+height
    void fillGradientRectFromTop2Bottom(int x,int y,int w,int h,
						int r0,int g0,int b0,int a0,
						int r1,int g1,int b1,int a1 ) ;
	void fillGradientRectLeft2Right(int x,int y,int w,int h,
						int r0,int g0,int b0,int a0,
						int r1,int g1,int b1,int a1 ) ;

	void strokeRect(int x,int y,int w,int h,int r,int g,int b,int a) ;
    void strokeRect2(int lineWid,int x,int y,int w,int h,int r,int g,int b,int a) ;
	void strokeLine(int x0,int y0,int x1,int y1,int lineWid,int r,int g,int b,int a) ;

	void drawText(int x,int y,int fontSize, string text) ;
    //rotateAngle clock-wise, degree 0-90
    void drawText2(float rotateAngle,int x,int y,int fontSize, string text) ;

	inline void setTextColor(int r,int g,int b){textR=r;textG=g;textB=b;}
    inline void getTextColor(int& r,int& g,int& b){r=textR,g=textG,b=textB;}
    inline void setTextAlign(int align){textAlign=align;}
    inline int getTextAlign(){return textAlign;}
    PeLegendRect calTextDrawSize(string str,float rotAng,int fontSize);

private:
	cairo_t * context ;
	cairo_surface_t* surface ;
	int width;
	int height;
    int textR,textG,textB ;//0-255
    int textAlign;//0-left 1-center 2-right


	void getContentRect(int& x0,int& y0,int& wid,int& hei) ;

} ;
