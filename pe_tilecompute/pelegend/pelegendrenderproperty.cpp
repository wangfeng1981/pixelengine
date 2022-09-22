#include "pelegendrenderproperty.h"

PeLegendRenderProperty::PeLegendRenderProperty()
{
    //ctor
    drawDirection= 0;//0-from top to bottom; 1-from left to right
    labelFontSize=10;//>1
    labelDirectionAngle= 0;//0.0-90.0
    labelOffsetX= 2;//>=0
    labelOffsetY= 0;//>=0
    boxWid= 20;//>1
    boxHei= 10;//>1
    boxMarginX= 40;//>=0
    boxMarginY= 4;//>=0
    boxBorderWidth=2;//>=0
    boxBorderColorR=0;//0-255
    boxBorderColorG=0;//0-255
    boxBorderColorB=0;//0-255
    tickWid=2;//>=0
    tickColorR=0;//0-255
    tickColorG=0;//0-255
    tickColorB=0;//0-255
    nRows=3;//>0
    nCols=3;//>0
    textAlign=0;//0-left 1-center 2-right
    textColorR=0;//0-255
    textColorG=0;//0-255
    textColorB=0;//0-255
}

PeLegendRenderProperty::~PeLegendRenderProperty()
{
    //dtor
}

bool PeLegendRenderProperty::loadFromJsonFile(string filename,string& error)
{
    std::ifstream ifs(filename.c_str());
    if( ifs.good()==false )
    {
        error = "ifs bad.";
        return false;
    }
    string tp,text ;
    while( getline(ifs , tp) ){
        text += tp ;
    }
    ifs.close();

    return this->loadFromJsonStr(text,error) ;
}
bool PeLegendRenderProperty::loadFromJsonStr(string& jsontext,string& error)
{
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(jsontext);
    bool parseOk = root.success() ;//检查解析json是否成功
    if( parseOk==false )
    {
        error = "bad json";
        return false;
    }
    drawDirection= root["drawDirection"].as<int>();//0-from top to bottom; 1-from left to right
    labelFontSize= root["labelFontSize"].as<int>();//>1
    labelDirectionAngle= root["labelDirectionAngle"].as<float>();//0.0-90.0
    labelOffsetX= root["labelOffsetX"].as<int>();//>=0
    labelOffsetY= root["labelOffsetY"].as<int>();//>=0
    boxWid= root["boxWid"].as<int>();//>1
    boxHei= root["boxHei"].as<int>();//>1
    boxMarginX= root["boxMarginX"].as<int>();//>=0
    boxMarginY= root["boxMarginY"].as<int>();//>=0
    boxBorderWidth= root["boxBorderWidth"].as<int>();//>=0
    boxBorderColorR=root["boxBorderColorR"].as<int>();//0-255
    boxBorderColorG=root["boxBorderColorG"].as<int>();//0-255
    boxBorderColorB=root["boxBorderColorB"].as<int>();//0-255
    tickWid=root["tickWid"].as<int>();//>=0
    tickColorR=root["tickColorR"].as<int>();//0-255
    tickColorG=root["tickColorG"].as<int>();//0-255
    tickColorB=root["tickColorB"].as<int>();//0-255
    nRows=root["nRows"].as<int>();//>0
    nCols=root["nCols"].as<int>();//>0
    textAlign=root["textAlign"].as<int>();//0-left 1-center 2-right
    textColorR=root["textColorR"].as<int>();//0-255
    textColorG=root["textColorG"].as<int>();//0-255
    textColorB=root["textColorB"].as<int>();//0-255
    return true ;
}

void PeLegendRenderProperty::print()
{
    cout<<this->toJsonString()<<endl ;
}
string PeLegendRenderProperty::toJsonString()
{
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["drawDirection"]=drawDirection ;//0-fromtoptobottom;1-fromlefttoright
    root["labelFontSize"]=labelFontSize ;//>1
    root["labelDirectionAngle"]=labelDirectionAngle ;//0.0-90.0
    root["labelOffsetX"]=labelOffsetX ;//>"]=0
    root["labelOffsetY"]=labelOffsetY ;//>"]=0
    root["boxWid"]=boxWid ;//>1
    root["boxHei"]=boxHei ;//>1
    root["boxMarginX"]=boxMarginX ;//>"]=0
    root["boxMarginY"]=boxMarginY ;//>"]=0
    root["boxBorderWidth"]=boxBorderWidth ;//>"]=0
    root["boxBorderColorR"]=boxBorderColorR ;//0-255
    root["boxBorderColorG"]=boxBorderColorG ;//0-255
    root["boxBorderColorB"]=boxBorderColorB ;//0-255
    root["tickWid"]=tickWid ;//>"]=0
    root["tickColorR"]=tickColorR ;//0-255
    root["tickColorG"]=tickColorG ;//0-255
    root["tickColorB"]=tickColorB ;//0-255
    root["nRows"]=nRows ;//>0
    root["nCols"]=nCols ;//>0
    root["textAlign"]=textAlign ;//0-left1-center2-right
    root["textColorR"]=textColorR ;//0-255
    root["textColorG"]=textColorG ;//0-255
    root["textColorB"]=textColorB ;//0-255
    string text ;
    root.prettyPrintTo(text) ;
    return text;
}
