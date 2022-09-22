#ifndef PELEGENDRENDERPROPERTY_H
#define PELEGENDRENDERPROPERTY_H

#include "ajson5.h"
#include <string>
#include <fstream>
#include <iostream>
using namespace ArduinoJson;
using std::ifstream;
using std::string;
using std::cout;
using std::endl;

/*!
* \param drawDirection 绘制方向0-垂直，1-水平
 * \param labelFontSize 标签字符大小
 * \param labelDirectionAngle 标签绘制角度，按顺时针计，0为水平绘制，取值0-90
 * \param labelOffsetX 标签距离box偏移值x
 * \param labelOffsetY 标签距离box偏移值y
 * \param boxWid box宽度
 * \param boxHei box高度
 * \param boxMarginX  box水平间隔仅在exact有效
 * \param boxMarginY  box垂直间隔仅在exact有效
 * \param tickWid    only use in linear and discrete >=0
 * \param tickColorR 0-255
 * \param tickColorG 0-255
 * \param tickColorB 0-255
 * \param nRows 行数最小值为1,仅在exact有效
 * \param nCols 列数最小值为1,仅在exact有效

*/
struct PeLegendRenderProperty
{
    public:
        PeLegendRenderProperty();
        virtual ~PeLegendRenderProperty();

        int drawDirection;//0-from top to bottom; 1-from left to right
        int labelFontSize;//>1
        float labelDirectionAngle;//0.0-90.0
        int labelOffsetX;//>=0
        int labelOffsetY;//>=0
        int boxWid;//>1
        int boxHei;//>1
        int boxMarginX;//>=0
        int boxMarginY;//>=0
        int boxBorderWidth;//>=0
        int boxBorderColorR;//0-255
        int boxBorderColorG;//0-255
        int boxBorderColorB;//0-255
        int tickWid;//>=0
        int tickColorR;//0-255
        int tickColorG;//0-255
        int tickColorB;//0-255
        int nRows;//>0
        int nCols;//>0
        int textAlign;//0-left 1-center 2-right
        int textColorR;//0-255
        int textColorG;//0-255
        int textColorB;//0-255



        //true-success ; false-failed
        bool loadFromJsonFile(string filename,string& error);
        bool loadFromJsonStr(string& jsontext,string& error);
        void print() ;
        string toJsonString() ;

    protected:

    private:

};

#endif // PELEGENDRENDERPROPERTY_H
