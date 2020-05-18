test for create PixelEngine API:

//global
PixelEngine = {} ; c++ Object::New , bind in js
RawDatasetCallBack(dt,w,h,nband); c++ FunctionTemplate , bind in js
RenderGrayCallBack() ;
var PixelEngine_Dataset_ForEachPixelFunction=function(pxfunc){
  
  var rawds = RawDatasetCallBack() ;
  rawds.renderGray = 
};

var dataset = PixelEngine.Dataset( tablename, datetime , bands) ; c++ bind in js
var dataset = PixelEngine.Dataset( dt , w , h , nband ) ; c++ bind in js
PixelEngine.Log(...) ; c++ bind in js


Dataset = {} ; 

