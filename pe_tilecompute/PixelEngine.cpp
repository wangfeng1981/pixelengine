//PixelEngine.cpp
#include "PixelEngine.h"

PixelEngine_GetDataFromExternal_FunctionPointer PixelEngine::GetExternalDatasetCallBack = nullptr ;
PixelEngine_GetDataFromExternal2_FunctionPointer PixelEngine::GetExternalTileDataCallBack = nullptr ;
std::unique_ptr<v8::Platform> PixelEngine::v8Platform = nullptr;

/// reverse color ramp
void PixelEngine::ColorReverse(vector<int>& colors) 
{
	int nc = colors.size()/3 ;
	int hnc = nc/2 ;
	int r ,g , b ;
	for(int i = 0 ; i<hnc ; ++ i )
	{
		r = colors[i*3] ;
		g = colors[i*3+1] ;
		b = colors[i*3+2] ;
		colors[i*3] = colors[(nc-1-i)*3] ;
		colors[i*3+1] = colors[(nc-1-i)*3+1] ;
		colors[i*3+2] = colors[(nc-1-i)*3+2] ;
		colors[(nc-1-i)*3]=r ;
		colors[(nc-1-i)*3+1]=g;
		colors[(nc-1-i)*3+2]=b ;
	}
}


/// get predefined color ramp
vector<int> PixelEngine::GetColorRamp(int colorid,int inverse) 
{
	if( colorid==1 ){
		if( inverse==0 )
		{
			return PixelEngine::ColorRainbow ;
		}else
		{
			vector<int> v = PixelEngine::ColorRainbow ;
    		PixelEngine::ColorReverse(v);
    		return v ;
		}
	}else if( colorid==2 )
	{
		if( inverse==0 )
		{
			return PixelEngine::ColorBlues ;
		}else
		{
			vector<int> v = PixelEngine::ColorBlues ;
    		PixelEngine::ColorReverse(v);
    		return v ;
		}
	}else if( colorid==3 ){
		if( inverse==0 )
		{
			return PixelEngine::ColorReds ;
		}else
		{
			vector<int> v = PixelEngine::ColorReds ;
    		PixelEngine::ColorReverse(v);
    		return v ;
		}
	}else if( colorid==4 ){
		if( inverse==0 )
		{
			return PixelEngine::ColorGreens ;
		}else
		{
			vector<int> v = PixelEngine::ColorGreens ;
    		PixelEngine::ColorReverse(v);
    		return v ;
		}
	}else
	{
		if( inverse==0 )
		{
			return PixelEngine::ColorGrays ;
		}else
		{
			vector<int> v = PixelEngine::ColorGrays ;
			PixelEngine::ColorReverse(v);
			return v ;
		}
	}
}

vector<int> PixelEngine::ColorRainbow{
	215,25,28
	,232,91,58
	,249,158,89
	,254,201,128
	,255,237,170
	,237,248,185
	,199,233,173
	,157,211,167
	,100,171,176
	,43,131,186
} ;

vector<int> PixelEngine::ColorBlues
{
	247,251,255
	,226,238,249
	,205,224,242
	,176,210,232
	,137,191,221
	,96,166,210
	,62,142,196
	,33,114,182
	,10,84,158
	,8,48,107
};

vector<int> PixelEngine::ColorReds{
	255, 245, 240
	,254, 227, 214
	,253, 198, 175
	,252, 164, 134
	,252,146,114
	,251,106,74
	,239,59,44
	,203,24,29
	,165,15,21
	,103,0,13
} ;
vector<int> PixelEngine::ColorGreens{
	247,252,245
	,232,246,227
	,208,237,202
	,178,224,171
	,142,209,140
	,102,189,111
	,61,167,90
	,35,140,69
	,3,112,46
	,0,68,27
} ;
vector<int> PixelEngine::ColorGrays{
	250,250,250
	,223,223,223
	,196,196,196
	,168,168,168
	,141,141,141
	,114,114,114
	,87,87,87
	,59,59,59
	,32,32,32
	,5,5,5

} ;

/// pe.log(...)
void PixelEngine::GlobalFunc_Log(const v8::FunctionCallbackInfo<v8::Value>& args) 
{
  if (args.Length() < 1) return;
  Isolate* isolate = args.GetIsolate();
  HandleScope scope(isolate);
  Local<Value> arg = args[0];
  String::Utf8Value value(isolate, arg);
  cout<<"log: "<< *value <<endl;
}

/// use c++ create a new empty Dataset object
Local<Object> PixelEngine::CPP_NewDataset(Isolate* isolate,Local<Context>& context
	,const int datatype 
	,const int width 
	,const int height
	,const int nband )
{
	cout<<"inside CPP_NewDataset"<<endl; 
	v8::EscapableHandleScope handle_scope(isolate);

	Local<Object> global = context->Global() ;

	Local<Value> forEachFuncInJs = global->Get(context 
    	,String::NewFromUtf8(isolate, "globalFunc_forEachPixelCallBack").ToLocalChecked() ).ToLocalChecked() ;
	Local<Object> ds = Object::New(isolate) ;
	ds->Set(context
		,String::NewFromUtf8(isolate, "forEachPixel").ToLocalChecked(),
            forEachFuncInJs );


	Local<Value> getPixelFuncInJs = global->Get(context 
    	,String::NewFromUtf8(isolate, "globalFunc_getPixelCallBack").ToLocalChecked() ).ToLocalChecked() ;	
	ds->Set(context
		,String::NewFromUtf8(isolate, "getPixel").ToLocalChecked(),
            getPixelFuncInJs );

	ds->Set(context
		,String::NewFromUtf8(isolate, "renderGray").ToLocalChecked(),
            FunctionTemplate::New(isolate, PixelEngine::GlobalFunc_RenderGrayCallBack)->GetFunction(context).ToLocalChecked() );
	ds->Set(context
		,String::NewFromUtf8(isolate, "renderPsuedColor").ToLocalChecked(),
            FunctionTemplate::New(isolate, PixelEngine::GlobalFunc_RenderPsuedColorCallBack)->GetFunction(context).ToLocalChecked() );
	ds->Set(context
		,String::NewFromUtf8(isolate, "fillRange").ToLocalChecked(),
            FunctionTemplate::New(isolate, PixelEngine::GlobalFunc_FillRangeCallBack)->GetFunction(context).ToLocalChecked() );
	ds->Set(context
		,String::NewFromUtf8(isolate, "renderRGB").ToLocalChecked(),
            FunctionTemplate::New(isolate, PixelEngine::GlobalFunc_RenderRGBCallBack)->GetFunction(context).ToLocalChecked() );
	

	ds->Set(context
		,String::NewFromUtf8(isolate, "width").ToLocalChecked()
		,Integer::New(isolate,width) ) ;
	ds->Set(context
		,String::NewFromUtf8(isolate, "height").ToLocalChecked()
		,Integer::New(isolate,height) ) ;
	ds->Set(context
		,String::NewFromUtf8(isolate, "nband").ToLocalChecked()
		,Integer::New(isolate,nband) ) ;
	ds->Set(context
		,String::NewFromUtf8(isolate, "dataType").ToLocalChecked()
		,Integer::New(isolate,datatype) ) ;

	//522
	Local<ArrayBuffer> neighborLoadedAB = ArrayBuffer::New(isolate,9) ;
	Local<Uint8Array> neighborLoadedU8 = Uint8Array::New(neighborLoadedAB,0,9) ;
	ds->Set(context
		,String::NewFromUtf8(isolate, "nbloads").ToLocalChecked()
		,neighborLoadedU8 ) ;//4 is current always 0.
	ds->Set(context
		,String::NewFromUtf8(isolate, "nbdatas").ToLocalChecked()
		,Array::New(isolate,9) ) ;// 4 is current always null.
	ds->Set(context
		,String::NewFromUtf8(isolate, "dsName").ToLocalChecked()
		,String::Empty(isolate) ) ;
	ds->Set(context
		,String::NewFromUtf8(isolate, "dsDt").ToLocalChecked()
		,String::Empty(isolate) ) ;
	ds->Set(context
		,String::NewFromUtf8(isolate, "x").ToLocalChecked()
		,Integer::New(isolate,0) ) ;
	ds->Set(context
		,String::NewFromUtf8(isolate, "y").ToLocalChecked()
		,Integer::New(isolate,0) ) ;
	ds->Set(context
		,String::NewFromUtf8(isolate, "z").ToLocalChecked()
		,Integer::New(isolate,0) ) ;

	if( datatype == 3 )
	{//short
		int bsize = width*height*nband*2 ;
		Local<ArrayBuffer> arrbuff = ArrayBuffer::New(isolate,bsize) ;
		Local<Int16Array> i16array = Int16Array::New(arrbuff,0,bsize/2) ;
		ds->Set(context
			,String::NewFromUtf8(isolate, "tiledata").ToLocalChecked()
			,i16array ) ;
	}else
	{//byte
		int bsize = width*height*nband*2 ;
		Local<ArrayBuffer> arrbuff = ArrayBuffer::New(isolate,bsize) ;
		Local<Uint8Array> u8array = Uint8Array::New(arrbuff,0,bsize) ;
		ds->Set(context
			,String::NewFromUtf8(isolate, "tiledata").ToLocalChecked()
			,u8array ) ;
	}
	return handle_scope.Escape(ds);
}

/// dataset.renderGray() , return a new Dataset
/// iband,vmin,vmax,nodata,nodataColor
void PixelEngine::GlobalFunc_RenderGrayCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) 
{
	cout<<"inside GlobalFunc_RenderGrayCallBack"<<endl; 
	if (args.Length() != 5 ){
		cout<<"Error: args.Length != 5 "<<endl ;
		return;
	}
	Isolate* isolate = args.GetIsolate() ;
	v8::HandleScope handle_scope(isolate);
	Local<Context> context(isolate->GetCurrentContext()) ;

	Local<Value> v8_iband = args[0];
	Local<Value> v8_vmin = args[1] ;
	Local<Value> v8_vmax = args[2] ;
	Local<Value> v8_nodata = args[3] ;
	Local<Value> v8_nodatacolor = args[4] ;

	int iband = v8_iband->ToInteger(context).ToLocalChecked()->Value() ;
	int vmin = v8_vmin->ToInteger(context).ToLocalChecked()->Value() ;
	int vmax = v8_vmax->ToInteger(context).ToLocalChecked()->Value() ;
	int nodata = v8_nodata->ToInteger(context).ToLocalChecked()->Value() ;
	Local<Object> nodataColorObj = v8_nodatacolor->ToObject(context).ToLocalChecked() ;
	int nodataColor[] = {0,0,0,0} ;
	nodataColor[0] = nodataColorObj->Get(context,0).ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	nodataColor[1] = nodataColorObj->Get(context,1).ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	nodataColor[2] = nodataColorObj->Get(context,2).ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	nodataColor[3] = nodataColorObj->Get(context,3).ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;


	Local<Object> thisobj =  args.This() ;
	int thisDataType = thisobj->Get(context,
		String::NewFromUtf8(isolate,"dataType").ToLocalChecked())
		.ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	//cout<<"thisDataType "<<thisDataType<<endl;

	int width = thisobj->Get(context,
		String::NewFromUtf8(isolate,"width").ToLocalChecked())
		.ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	//cout<<"thisDataType "<<thisDataType<<endl;

	int height = thisobj->Get(context,
		String::NewFromUtf8(isolate,"height").ToLocalChecked())
		.ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	//cout<<"thisDataType "<<thisDataType<<endl;

	int nband = thisobj->Get(context,
		String::NewFromUtf8(isolate,"nband").ToLocalChecked())
		.ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	//cout<<"thisDataType "<<thisDataType<<endl;

	//output
	Local<Object> outds = PixelEngine::CPP_NewDataset(isolate,context
		,1
		,width
		,height
		,4 );
	Local<Value> outDataValue = outds->Get(context,
		String::NewFromUtf8(isolate,"tiledata").ToLocalChecked())
		.ToLocalChecked() ;
	Uint8Array* outU8Array = Uint8Array::Cast(*outDataValue) ;
	unsigned char* outbackData = (unsigned char*) outU8Array->Buffer()->GetBackingStore()->Data() ;


	Local<Value> tiledataValue = thisobj->Get(context,
		String::NewFromUtf8(isolate,"tiledata").ToLocalChecked())
		.ToLocalChecked() ;

	int asize = width * height ;
	float theK = 255.f/(vmax-vmin) ;
	if( thisDataType==3 )
	{//short
		
		Int16Array* i16Array = Int16Array::Cast(*tiledataValue) ;
		short* backData = (short*) i16Array->Buffer()->GetBackingStore()->Data() ;
		short* backDataOffset = backData + iband * asize;
		
		for(int it = 0 ; it < asize ; ++ it )
		{
			if( backDataOffset[it] == nodata ){
				outbackData[it] = nodataColor[0] ;
				outbackData[asize+it] = nodataColor[1] ;
				outbackData[asize*2+it] = nodataColor[2] ;
				outbackData[asize*3+it] = nodataColor[3] ;
			}else
			{
				int gray = (backDataOffset[it]-vmin) * theK ;
				if( gray < 0 ) gray = 0 ;
				else if( gray > 255 ) gray = 255 ;
				outbackData[it] = gray ;
				outbackData[asize+it] = gray;
				outbackData[asize*2+it] = gray ;
				outbackData[asize*3+it] = 255;
			}
		}
	}else
	{//byte
		Uint8Array* u8Array = Uint8Array::Cast(*tiledataValue) ;
		unsigned char* backData = (unsigned char*) u8Array->Buffer()->GetBackingStore()->Data() ;
		unsigned char* backDataOffset = backData + iband * asize;
	
		for(int it = 0 ; it < asize ; ++ it )
		{
			if( backDataOffset[it] == nodata ){
				outbackData[it] = nodataColor[0] ;
				outbackData[asize+it] = nodataColor[1] ;
				outbackData[asize*2+it] = nodataColor[2] ;
				outbackData[asize*3+it] = nodataColor[3] ;
			}else
			{
				int gray = (backDataOffset[it]-vmin) * theK ;
				if( gray < 0 ) gray = 0 ;
				else if( gray > 255 ) gray = 255 ;
				outbackData[it] = gray ;
				outbackData[asize+it] = gray;
				outbackData[asize*2+it] = gray ;
				outbackData[asize*3+it] = 255;
			}
		}
	}
	
	//info.GetReturnValue().Set(i16arr);
	args.GetReturnValue().Set(outds) ;
}


/// dataset.renderRGB , return a new Dataset
/// ri,gi,bi,rmin,rmax,gmin,gmax,bmin,bmax
void PixelEngine::GlobalFunc_RenderRGBCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) 
{
	cout<<"inside GlobalFunc_RenderRGBCallBack"<<endl; 
	if (args.Length() != 9 ){
		cout<<"Error: args.Length != 9 "<<endl ;
		return;
	}
	Isolate* isolate = args.GetIsolate() ;
	v8::HandleScope handle_scope(isolate);
	Local<Context> context(isolate->GetCurrentContext()) ;

	int ri = args[0]->ToInteger(context).ToLocalChecked()->Value();
	int gi = args[1]->ToInteger(context).ToLocalChecked()->Value();
	int bi = args[2]->ToInteger(context).ToLocalChecked()->Value();

	int rmin = args[3]->ToInteger(context).ToLocalChecked()->Value();
	int rmax = args[4]->ToInteger(context).ToLocalChecked()->Value();

	int gmin = args[5]->ToInteger(context).ToLocalChecked()->Value();
	int gmax = args[6]->ToInteger(context).ToLocalChecked()->Value();

	int bmin = args[7]->ToInteger(context).ToLocalChecked()->Value();
	int bmax = args[8]->ToInteger(context).ToLocalChecked()->Value();

	Local<Object> thisobj =  args.This() ;
	int thisDataType = thisobj->Get(context,
		String::NewFromUtf8(isolate,"dataType").ToLocalChecked())
		.ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	//cout<<"thisDataType "<<thisDataType<<endl;

	int width = thisobj->Get(context,
		String::NewFromUtf8(isolate,"width").ToLocalChecked())
		.ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	//cout<<"thisDataType "<<thisDataType<<endl;

	int height = thisobj->Get(context,
		String::NewFromUtf8(isolate,"height").ToLocalChecked())
		.ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	//cout<<"thisDataType "<<thisDataType<<endl;

	int nband = thisobj->Get(context,
		String::NewFromUtf8(isolate,"nband").ToLocalChecked())
		.ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	//cout<<"thisDataType "<<thisDataType<<endl;

	//output
	Local<Object> outds = PixelEngine::CPP_NewDataset(isolate,context
		,1
		,width
		,height
		,4 );
	Local<Value> outDataValue = outds->Get(context,
		String::NewFromUtf8(isolate,"tiledata").ToLocalChecked())
		.ToLocalChecked() ;
	Uint8Array* outU8Array = Uint8Array::Cast(*outDataValue) ;
	unsigned char* outbackData = (unsigned char*) outU8Array->Buffer()->GetBackingStore()->Data() ;

	Local<Value> tiledataValue = thisobj->Get(context,
		String::NewFromUtf8(isolate,"tiledata").ToLocalChecked())
		.ToLocalChecked() ;

	int asize = width * height ;
	float rK = 255.f/(rmax-rmin) ;
	float gK = 255.f/(gmax-gmin) ;
	float bK = 255.f/(bmax-bmin) ;
	if( thisDataType==3 )
	{//short
		
		Int16Array* i16Array = Int16Array::Cast(*tiledataValue) ;
		short* backData = (short*) i16Array->Buffer()->GetBackingStore()->Data() ;
		short* roffset = backData + ri * asize;
		short* goffset = backData + gi * asize;
		short* boffset = backData + bi * asize;
		for(int it = 0 ; it < asize ; ++ it )
		{
			outbackData[it] = min( max( (roffset[it]-rmin) * rK,0.f) ,255.f)  ;
			outbackData[asize+it] = min( max( (goffset[it]-gmin) * gK,0.f) ,255.f) ;  
			outbackData[asize*2+it] = min( max( (boffset[it]-bmin) * bK,0.f) ,255.f) ; 
			outbackData[asize*3+it] = 255 ;
		}
	}else
	{//byte
		Uint8Array* u8Array = Uint8Array::Cast(*tiledataValue) ;
		unsigned char* backData = (unsigned char*) u8Array->Buffer()->GetBackingStore()->Data() ;
		unsigned char* roffset = backData + ri * asize;
		unsigned char* goffset = backData + gi * asize;
		unsigned char* boffset = backData + bi * asize;
		for(int it = 0 ; it < asize ; ++ it )
		{
			outbackData[it] = min( max( (roffset[it]-rmin) * rK,0.f) ,255.f)  ;
			outbackData[asize+it] = min( max( (goffset[it]-gmin) * gK,0.f) ,255.f) ;  
			outbackData[asize*2+it] = min( max( (boffset[it]-bmin) * bK,0.f) ,255.f) ; 
			outbackData[asize*3+it] = 255 ;
		}
	}
	
	//info.GetReturnValue().Set(i16arr);
	args.GetReturnValue().Set(outds) ;
}


/// fill value in the same dataset , not create a new one , no return.
/// iband,vmin,vmax
void PixelEngine::GlobalFunc_FillRangeCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) 
{
	cout<<"inside GlobalFunc_FillRangeCallBack"<<endl; 
	if (args.Length() != 3 ){
		cout<<"Error: args.Length !=2 "<<endl ;
		return;
	}
	Isolate* isolate = args.GetIsolate() ;
	v8::HandleScope handle_scope(isolate);
	Local<Context> context(isolate->GetCurrentContext()) ;

	Local<Value> v8_iband = args[0];
	Local<Value> v8_vmin = args[1] ;
	Local<Value> v8_vmax = args[2] ;

	int iband = v8_iband->ToInteger(context).ToLocalChecked()->Value() ;
	int vmin = v8_vmin->ToInteger(context).ToLocalChecked()->Value() ;
	int vmax = v8_vmax->ToInteger(context).ToLocalChecked()->Value() ;
	
	Local<Object> thisobj =  args.This() ;
	int thisDataType = thisobj->Get(context,
		String::NewFromUtf8(isolate,"dataType").ToLocalChecked())
		.ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	//cout<<"thisDataType "<<thisDataType<<endl;

	int width = thisobj->Get(context,
		String::NewFromUtf8(isolate,"width").ToLocalChecked())
		.ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	//cout<<"thisDataType "<<thisDataType<<endl;

	int height = thisobj->Get(context,
		String::NewFromUtf8(isolate,"height").ToLocalChecked())
		.ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	//cout<<"thisDataType "<<thisDataType<<endl;

	int nband = thisobj->Get(context,
		String::NewFromUtf8(isolate,"nband").ToLocalChecked())
		.ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;

	Local<Value> tiledataValue = thisobj->Get(context,
		String::NewFromUtf8(isolate,"tiledata").ToLocalChecked())
		.ToLocalChecked() ;

	int asize = width * height ;
	float theK = (vmax-vmin)*1.f/asize ;
	if( thisDataType==3 )
	{//short
		
		Int16Array* i16Array = Int16Array::Cast(*tiledataValue) ;
		short* backData = (short*) i16Array->Buffer()->GetBackingStore()->Data() ;
		short* backDataOffset = backData + iband * asize;
		for(int it = 0 ; it < asize ; ++ it )
		{
			backDataOffset[it] = it*theK + vmin ;
		}
	}else
	{//byte
		Uint8Array* u8Array = Uint8Array::Cast(*tiledataValue) ;
		unsigned char* backData = (unsigned char*) u8Array->Buffer()->GetBackingStore()->Data() ;
		unsigned char* backDataOffset = backData + iband * asize;
		for(int it = 0 ; it < asize ; ++ it )
		{
			backDataOffset[it] = it*theK + vmin ;
		}
	}
}

/// interpolate RGB of value from ColorRamp
void PixelEngine::Value2Color(int valx,float K
	,int nodata,int* nodataColor
	,int vmin,int vmax , int interpol
	,vector<int>& colorRamp , int ncolor
	,unsigned char& rr 
	,unsigned char& rg 
	,unsigned char& rb 
	,unsigned char& ra ){
	if( valx == nodata ){
		rr = nodataColor[0] ;
		rg = nodataColor[1] ;
		rb = nodataColor[2] ;
		ra = nodataColor[3] ;
	}else
	{
		if( valx < vmin ) valx = vmin ;
		else if( valx>vmax ) valx = vmax ;
		int dn = (valx-vmin) * K ;
		float weightHigh = (valx-vmin) * K - dn ;
		if( dn < 0 )
		{
			rr = colorRamp[0] ;
			rg =  colorRamp[1] ;
			rb =  colorRamp[2]  ;
			ra = 255;
		}
		else if( dn >= ncolor-1 ) 
		{
			int off = (ncolor-1)*3;
			rr =  colorRamp[off] ; 
			rg =  colorRamp[off+1] ;
			rb =  colorRamp[off+2] ; 
			ra = 255;

		}else
		{
			int off0 = dn*3 ;
			int off1 = off0+3 ;
			if( interpol ==1 )
			{//interpol
				float wl = 1.f-weightHigh ;
				rr = colorRamp[off0]*wl+colorRamp[off1]*weightHigh ;
				rg = colorRamp[off0+1]*wl+colorRamp[off1+1]*weightHigh ;
				rb = colorRamp[off0+2]*wl+colorRamp[off1+2]*weightHigh ; 
				ra = 255;
			}else
			{//discrete
				rr = colorRamp[off0] ;
				rg = colorRamp[off0+1];
				rb = colorRamp[off0+2] ;
				ra = 255;
			}
		}
	}
}

/// dataset.renderPsuedColor(...), return a new Dataset
//iband,vmin,vmax,nodata,nodataColor,colorid,noraml/inverse(0/1),discrete/interpol(0/1)
void PixelEngine::GlobalFunc_RenderPsuedColorCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) 
{
	cout<<"inside GlobalFunc_RenderPsuedColorCallBack"<<endl; 
	if (args.Length() != 8 ){
		cout<<"Error: args.Length != 8 "<<endl ;
		return;
	}
	Isolate* isolate = args.GetIsolate() ;
	v8::HandleScope handle_scope(isolate);
	Local<Context> context(isolate->GetCurrentContext()) ;

	Local<Value> v8_iband = args[0];
	Local<Value> v8_vmin = args[1] ;
	Local<Value> v8_vmax = args[2] ;
	Local<Value> v8_nodata = args[3] ;
	Local<Value> v8_nodatacolor = args[4] ;
	Local<Value> v8_colorid = args[5] ;
	Local<Value> v8_inverse = args[6] ;
	Local<Value> v8_interpol = args[7] ;

	int iband = v8_iband->ToInteger(context).ToLocalChecked()->Value() ;
	int vmin = v8_vmin->ToInteger(context).ToLocalChecked()->Value() ;
	int vmax = v8_vmax->ToInteger(context).ToLocalChecked()->Value() ;
	int nodata = v8_nodata->ToInteger(context).ToLocalChecked()->Value() ;
	Local<Object> nodataColorObj = v8_nodatacolor->ToObject(context).ToLocalChecked() ;
	int nodataColor[] = {0,0,0,0} ;
	nodataColor[0] = nodataColorObj->Get(context,0).ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	nodataColor[1] = nodataColorObj->Get(context,1).ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	nodataColor[2] = nodataColorObj->Get(context,2).ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	nodataColor[3] = nodataColorObj->Get(context,3).ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;

	int colorid = v8_colorid->ToInteger(context).ToLocalChecked()->Value() ;
	int inverse = v8_inverse->ToInteger(context).ToLocalChecked()->Value() ;
	int interpol = v8_interpol->ToInteger(context).ToLocalChecked()->Value() ;

	vector<int> colorRamp = PixelEngine::GetColorRamp(colorid,inverse) ;
	int ncolor = colorRamp.size()/3 ;

	Local<Object> thisobj =  args.This() ;
	int thisDataType = thisobj->Get(context,
		String::NewFromUtf8(isolate,"dataType").ToLocalChecked())
		.ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	//cout<<"thisDataType "<<thisDataType<<endl;

	int width = thisobj->Get(context,
		String::NewFromUtf8(isolate,"width").ToLocalChecked())
		.ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	//cout<<"thisDataType "<<thisDataType<<endl;

	int height = thisobj->Get(context,
		String::NewFromUtf8(isolate,"height").ToLocalChecked())
		.ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	//cout<<"thisDataType "<<thisDataType<<endl;

	int nband = thisobj->Get(context,
		String::NewFromUtf8(isolate,"nband").ToLocalChecked())
		.ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	//cout<<"thisDataType "<<thisDataType<<endl;

	//output
	Local<Object> outds = PixelEngine::CPP_NewDataset(isolate,context
		,1
		,width
		,height
		,4 );
	Local<Value> outDataValue = outds->Get(context,
		String::NewFromUtf8(isolate,"tiledata").ToLocalChecked())
		.ToLocalChecked() ;
	Uint8Array* outU8Array = Uint8Array::Cast(*outDataValue) ;
	unsigned char* outbackData = (unsigned char*) outU8Array->Buffer()->GetBackingStore()->Data() ;


	Local<Value> tiledataValue = thisobj->Get(context,
		String::NewFromUtf8(isolate,"tiledata").ToLocalChecked())
		.ToLocalChecked() ;

	int asize = width * height ;
	float theK = (ncolor-1.f)/(vmax-vmin) ;
	if( thisDataType==3 )
	{//short
		
		Int16Array* i16Array = Int16Array::Cast(*tiledataValue) ;
		short* backData = (short*) i16Array->Buffer()->GetBackingStore()->Data() ;
		short* backDataOffset = backData + iband * asize;
		for(int it = 0 ; it < asize ; ++ it )
		{
			PixelEngine::Value2Color(backDataOffset[it],theK,nodata,nodataColor
				,vmin,vmax,interpol,colorRamp,ncolor,outbackData[it],outbackData[it+asize]
				,outbackData[it+asize*2],outbackData[it+asize*3]) ;

		}
	}else
	{//byte
		Uint8Array* u8Array = Uint8Array::Cast(*tiledataValue) ;
		unsigned char* backData = (unsigned char*) u8Array->Buffer()->GetBackingStore()->Data() ;
		unsigned char* backDataOffset = backData + iband * asize;
		for(int it = 0 ; it < asize ; ++ it )
		{
			PixelEngine::Value2Color(backDataOffset[it],theK,nodata,nodataColor
				,vmin,vmax,interpol,colorRamp,ncolor,outbackData[it],outbackData[it+asize]
				,outbackData[it+asize*2],outbackData[it+asize*3]) ;
		}
	}
	
	//info.GetReturnValue().Set(i16arr);
	args.GetReturnValue().Set(outds) ;
}

/// javascript callback, create a new empty Dataset
/// create an empty Dataset.
void PixelEngine::GlobalFunc_NewDatasetCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) 
{
	cout<<"inside GlobalFunc_NewDatasetCallBack"<<endl; 
	if (args.Length() != 4 ){
		cout<<"Error: args.Length != 4 "<<endl ;
		return;
	}

	Isolate* isolate = args.GetIsolate() ;
	v8::HandleScope handle_scope(isolate);
	Local<Context> context(isolate->GetCurrentContext()) ;

	Local<Value> dt0 = args[0];
	Local<Value> w0 = args[1] ;
	Local<Value> h0 = args[2] ;
	Local<Value> nb0 = args[3] ;

	int dt = dt0->ToInteger(context).ToLocalChecked()->Value() ;
	int wid = w0->ToInteger(context).ToLocalChecked()->Value() ;
	int hei = h0->ToInteger(context).ToLocalChecked()->Value() ;
	int nband = nb0->ToInteger(context).ToLocalChecked()->Value() ;

	Local<Object> ds = PixelEngine::CPP_NewDataset( isolate
		,context
		,dt
		,wid
		,hei
		,nband );

	//info.GetReturnValue().Set(i16arr);
	args.GetReturnValue().Set(ds) ;
}

/// create a Dataset from java.
/// create a dataset from name, datetime, bands
void PixelEngine::GlobalFunc_DatasetCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) 
{
	cout<<"inside GlobalFunc_DatasetCallBack"<<endl; 
	if (args.Length() != 3 ){
		cout<<"Error: args.Length != 3 "<<endl ;
		return;
	}

	Isolate* isolate = args.GetIsolate() ;
	v8::HandleScope handle_scope(isolate);
	Local<Context> context(isolate->GetCurrentContext()) ;

	Local<Value> v8name = args[0];
	Local<Value> v8datetime = args[1] ;
	Local<Value> v8bands = args[2] ;

	String::Utf8Value nameutf8( isolate , v8name) ;
	string name( *nameutf8 ) ;

	String::Utf8Value dtutf8( isolate , v8datetime) ;
	string datetime( *dtutf8 ) ;

	cout<<name<<","<<datetime<<endl ;

	if( v8bands->IsArray() )
	{
		cout<<"v8bands is array"<<endl ;
	}
	if( v8bands->IsObject() )
	{
		cout<<"v8bands is object"<<endl ;
	}
	Array* i32array = Array::Cast(*v8bands) ;
	int nband = i32array->Length() ;

	cout<<"nband "<<nband<<endl ;

	vector<int> wantBands ;
	wantBands.reserve(32) ;
	for(int ib = 0 ; ib<nband ; ++ ib )
	{
		int wantib = i32array->Get(context,ib).ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
		wantBands.push_back(wantib) ;
	}

	vector<unsigned char> externalData ;
	int dt = 0 ;
	int wid = 0 ;
	int hei = 0 ; 
	int retnbands = 0 ;

	Local<Object> global = context->Global() ;
	Local<Value> peinfo = global->Get( context
		,String::NewFromUtf8(isolate, "PixelEnginePointer").ToLocalChecked())
		.ToLocalChecked() ;
	Object* peinfoObj = Object::Cast( *peinfo ) ;
	Local<Value> thisPePtrValue = peinfoObj->GetInternalField(0) ;
	External* thisPePtrEx = External::Cast(*thisPePtrValue);
	PixelEngine* thisPePtr = static_cast<PixelEngine*>(thisPePtrEx->Value() );

	// bool externalOk = PixelEngine::GetExternalDatasetCallBack(
	// 	thisPePtrEx->Value() ,// pointer to PixelEngine Object.
	// 	name,datetime,wantBands,externalData,
	// 	dt,
	// 	wid,
	// 	hei,
	// 	retnbands) ;
	int tilez = thisPePtr->tileInfo.z  ;
	int tiley = thisPePtr->tileInfo.y  ; 
	int tilex = thisPePtr->tileInfo.x  ; 

	bool externalOk = PixelEngine::GetExternalTileDataCallBack(
		thisPePtrEx->Value() ,// pointer to PixelEngine Object.
		name,datetime,wantBands,
		tilez,
		tiley,
		tilex,
		externalData , 
		dt ,
		wid , 
		hei , 
		retnbands ) ;
	if( externalOk==false )
	{
		cout<<"Error: PixelEngine::GetExternalTileDataCallBack failed."<<endl;
		return ;//return null in javascript.
	}

	Local<Object> ds = PixelEngine::CPP_NewDataset( isolate
		,context
		,dt
		,wid
		,hei
		,retnbands );
	Local<Value> tiledataValue = ds->Get(context,
		String::NewFromUtf8(isolate,"tiledata").ToLocalChecked())
		.ToLocalChecked() ;
	Int16Array* i16Array = Int16Array::Cast(*tiledataValue) ;//here maybe byte data in future.
	short* backData = (short*) i16Array->Buffer()->GetBackingStore()->Data() ;
	memcpy(backData , externalData.data(), externalData.size() );

	ds->Set(context
		,String::NewFromUtf8(isolate, "dsName").ToLocalChecked()
		,v8name ) ;
	ds->Set(context
		,String::NewFromUtf8(isolate, "dsDt").ToLocalChecked()
		,v8datetime ) ;
	ds->Set(context
		,String::NewFromUtf8(isolate, "x").ToLocalChecked()
		,Integer::New(isolate,thisPePtr->tileInfo.x) ) ;
	ds->Set(context
		,String::NewFromUtf8(isolate, "y").ToLocalChecked()
		,Integer::New(isolate,thisPePtr->tileInfo.y) ) ;
	ds->Set(context
		,String::NewFromUtf8(isolate, "z").ToLocalChecked()
		,Integer::New(isolate,thisPePtr->tileInfo.z) ) ;

	//info.GetReturnValue().Set(i16arr);
	args.GetReturnValue().Set(ds) ;
}


/// get external tile data only, return data array not dataset.
/// PixelEngine.GetTileData(name,datetime,z,y,x)
void PixelEngine::GlobalFunc_GetTileDataCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) 
{
	cout<<"inside GlobalFunc_GetTileDataCallBack"<<endl; 
	if (args.Length() != 5 ){
		cout<<"Error: args.Length != 5 "<<endl ;
		return;
	}

	Isolate* isolate = args.GetIsolate() ;
	v8::HandleScope handle_scope(isolate);
	Local<Context> context(isolate->GetCurrentContext()) ;

	Local<Value> v8name = args[0];
	Local<Value> v8datetime = args[1] ;

	String::Utf8Value nameutf8( isolate , v8name) ;
	string name( *nameutf8 ) ;

	String::Utf8Value dtutf8( isolate , v8datetime) ;
	string datetime( *dtutf8 ) ;

	int tilez = args[2]->ToInteger(context).ToLocalChecked()->Value() ;
	int tiley = args[3]->ToInteger(context).ToLocalChecked()->Value() ; 
	int tilex = args[4]->ToInteger(context).ToLocalChecked()->Value() ; 

	vector<int> wantBands(1,0) ;//not used yet.

	vector<unsigned char> externalData ;
	int dt = 0 ;
	int wid = 0 ;
	int hei = 0 ;
	int nband =0 ;
	
	Local<Object> global = context->Global() ;
	Local<Value> peinfo = global->Get( context
		,String::NewFromUtf8(isolate, "PixelEnginePointer").ToLocalChecked())
		.ToLocalChecked() ;
	Object* peinfoObj = Object::Cast( *peinfo ) ;
	Local<Value> thisPePtrValue = peinfoObj->GetInternalField(0) ;
	External* thisPePtrEx = External::Cast(*thisPePtrValue);


	bool externalOk = PixelEngine::GetExternalTileDataCallBack(
		thisPePtrEx->Value() ,// pointer to PixelEngine Object.
		name,datetime,wantBands,
		tilez,
		tiley,
		tilex,
		externalData , 
		dt ,
		wid , 
		hei , 
		nband ) ;
	if( externalOk==false )
	{
		cout<<"Error: PixelEngine::GlobalFunc_GetTileDataCallBack failed."<<endl;
		return ;//return null in javascript.
	}

	Local<ArrayBuffer> exArrBuff = ArrayBuffer::New(isolate,externalData.size()) ;
	unsigned char* exArrBuffDataPtr =(unsigned char*) exArrBuff->GetBackingStore()->Data() ;
	memcpy(exArrBuffDataPtr , externalData.data(), externalData.size() );
	if( dt == 3 )
	{
		Local<Int16Array> i16Array = Int16Array::New(exArrBuff, 0 , externalData.size()/2 ) ;
		args.GetReturnValue().Set(i16Array) ;
	}else
	{
		Local<Uint8Array> u8Array = Uint8Array::New(exArrBuff, 0 , externalData.size() ) ;
		args.GetReturnValue().Set(u8Array) ;
	}
}


/// create a Dataset from localfile. only for testing.
/// filepath,dt,width,hight,nband
void PixelEngine::GlobalFunc_LocalDatasetCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) 
{
	cout<<"inside GlobalFunc_LocalDatasetCallBack"<<endl; 
	if (args.Length() != 5 ){
		cout<<"Error: args.Length != 5 "<<endl ;
		return;
	}

	Isolate* isolate = args.GetIsolate() ;
	v8::HandleScope handle_scope(isolate);
	Local<Context> context(isolate->GetCurrentContext()) ;

	Local<Value> v8name = args[0];
	String::Utf8Value filename( isolate , v8name) ;

	int dt = args[1]->ToInteger(context).ToLocalChecked()->Value() ;
	int wid = args[2]->ToInteger(context).ToLocalChecked()->Value() ;
	int hei = args[3]->ToInteger(context).ToLocalChecked()->Value() ;
	int nb0 = args[4]->ToInteger(context).ToLocalChecked()->Value() ;
	vector<unsigned char> imgdata(wid*hei*nb0*2) ;
	{
		FILE* pf = fopen(*filename,"rb") ;
		fread( imgdata.data() , 1 , wid*hei*nb0*2 , pf) ;
		fclose(pf) ;
	}

	Local<Object> ds = PixelEngine::CPP_NewDataset( isolate
		,context
		,dt
		,wid
		,hei
		,nb0 );
	Local<Value> tiledataValue = ds->Get(context,
		String::NewFromUtf8(isolate,"tiledata").ToLocalChecked())
		.ToLocalChecked() ;
	Int16Array* i16Array = Int16Array::Cast(*tiledataValue) ;
	short* backData = (short*) i16Array->Buffer()->GetBackingStore()->Data() ;
	memcpy(backData , imgdata.data() , imgdata.size() );
	
	//info.GetReturnValue().Set(i16arr);
	args.GetReturnValue().Set(ds) ;
}

 
/// convert dataset to png
void PixelEngine::Dataset2Png( Isolate* isolate, Local<Context>& context, Local<Value> dsValue
	, vector<unsigned char>& retpngbinary )
{
	unsigned long now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	Object* dsObj = Object::Cast(*dsValue) ;
	int dt = dsObj->Get(context,
		String::NewFromUtf8(isolate,"dataType").ToLocalChecked())
		.ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	int width = dsObj->Get(context,
		String::NewFromUtf8(isolate,"width").ToLocalChecked())
		.ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	int height = dsObj->Get(context,
		String::NewFromUtf8(isolate,"height").ToLocalChecked())
		.ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	int nband = dsObj->Get(context,
		String::NewFromUtf8(isolate,"nband").ToLocalChecked())
		.ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	Local<Value> tiledataValue = dsObj->Get(context,
		String::NewFromUtf8(isolate,"tiledata").ToLocalChecked())
		.ToLocalChecked() ;

	if( dt == 1 )
	{
		Uint8Array* u8arr = Uint8Array::Cast(*tiledataValue) ;
		unsigned char* dataptr = (unsigned char*) u8arr->Buffer()->GetBackingStore()->Data() ;
		unsigned long now1 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	  	printf("get dataptr:%d ms \n", now1 - now);//1024*1024 use 340millisec

		const int imgsize = width * height;
		vector<unsigned char> rgbadata(imgsize * 4, 0);
		if( nband==4 )
		{
			for (int it = 0; it < imgsize; ++it)
			{
				rgbadata[it * 4 + 0] = dataptr[it];
				rgbadata[it * 4 + 1] = dataptr[it+imgsize];
				rgbadata[it * 4 + 2] = dataptr[it+2*imgsize];
				rgbadata[it * 4 + 3] = dataptr[it+3*imgsize];
			}
		}else if(nband==3 )
		{
			for (int it = 0; it < imgsize; ++it)
			{
				rgbadata[it * 4 + 0] = dataptr[it];
				rgbadata[it * 4 + 1] = dataptr[it+imgsize];
				rgbadata[it * 4 + 2] = dataptr[it+2*imgsize];
				rgbadata[it * 4 + 3] = 255;
			}
		} else
		{
			for (int it = 0; it < imgsize; ++it)
			{
				rgbadata[it * 4 + 0] = dataptr[it];
				rgbadata[it * 4 + 1] = dataptr[it];
				rgbadata[it * 4 + 2] = dataptr[it];
				rgbadata[it * 4 + 3] = 255;
			}
		}
		unsigned long now2 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	  	printf("forloop png:%d ms \n", now2 - now1);//1024*1024 use 340millisec

		retpngbinary.clear();
		retpngbinary.reserve(1024*1024*4) ;
		lodepng::State state; //optionally customize this one
		state.encoder.filter_palette_zero = 0; //
		state.encoder.add_id = false; //Don't add LodePNG version chunk to save more bytes
		state.encoder.text_compression = 1; //
		state.encoder.zlibsettings.nicematch = 258; //
		state.encoder.zlibsettings.lazymatching = 1; //
		state.encoder.zlibsettings.windowsize = 512; //32768
		state.encoder.filter_strategy = LFS_ZERO;//{ LFS_ZERO, LFS_MINSUM, LFS_ENTROPY, LFS_BRUTE_FORCE };
		state.encoder.zlibsettings.minmatch = 3;
		state.encoder.zlibsettings.btype = 2;
		state.encoder.auto_convert = 0;
		unsigned error = lodepng::encode(retpngbinary, rgbadata, width, height, state);
		
		unsigned long now3 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	  	printf("encode png:%d ms \n", now3 - now2);//1024*1024 use 340millisec

	}
}

/// init global objects and functions 
bool PixelEngine::initTemplate( PixelEngine* thePE,Isolate* isolate, Local<Context>& context )
{
	v8::HandleScope handle_scope(isolate);

	//Add a template to hold thePE pointer
	Local<ObjectTemplate> PixelEnginePointerTemplate=ObjectTemplate::New(isolate) ;
	PixelEnginePointerTemplate->SetInternalFieldCount(1) ;
	Local<Object> peinfo = PixelEnginePointerTemplate->NewInstance(context).ToLocalChecked() ;
	Local<External> thisPePointer = External::New(isolate, thePE);
	peinfo->SetInternalField( 0,thisPePointer) ;

	Local<Object> global = context->Global() ;

	Maybe<bool> okinfo = global->Set( context
		,String::NewFromUtf8(isolate, "PixelEnginePointer").ToLocalChecked()
		,peinfo ) ;// bind PixelEngine
	if( okinfo.IsNothing() )
	{
		cout<<"Error: PixelEnginePointer is nothing."<<endl ;
	}

	//var PixelEngine = {} ;
	Local<Object> pe = Object::New(isolate) ;

	Maybe<bool> okpe = global->Set( context
		,String::NewFromUtf8(isolate, "PixelEngine").ToLocalChecked()
		,pe ) ;// bind PixelEngine
	if( okpe.IsNothing() )
	{
		cout<<"okpe is nothing."<<endl ;
	}
	pe->Set(context,String::NewFromUtf8(isolate, "ColorRampRainbow").ToLocalChecked(),
	   Integer::New(isolate,1));
	pe->Set(context,String::NewFromUtf8(isolate, "ColorRampBlues").ToLocalChecked(),
	   Integer::New(isolate,2));
	pe->Set(context,String::NewFromUtf8(isolate, "ColorRampReds").ToLocalChecked(),
	   Integer::New(isolate,3));
	pe->Set(context,String::NewFromUtf8(isolate, "ColorRampGreens").ToLocalChecked(),
	   Integer::New(isolate,4));
	pe->Set(context,String::NewFromUtf8(isolate, "ColorRampGrays").ToLocalChecked(),
	   Integer::New(isolate,0));
	pe->Set(context
		,String::NewFromUtf8(isolate, "NewDataset").ToLocalChecked(),
           FunctionTemplate::New(isolate, PixelEngine::GlobalFunc_NewDatasetCallBack)->GetFunction(context).ToLocalChecked() );

	pe->Set(context
		,String::NewFromUtf8(isolate, "Dataset").ToLocalChecked(),
           FunctionTemplate::New(isolate, PixelEngine::GlobalFunc_DatasetCallBack)->GetFunction(context).ToLocalChecked() );
	pe->Set(context
		,String::NewFromUtf8(isolate, "GetTileData").ToLocalChecked(),
           FunctionTemplate::New(isolate, PixelEngine::GlobalFunc_GetTileDataCallBack)->GetFunction(context).ToLocalChecked() );
	pe->Set(context
	,String::NewFromUtf8(isolate, "LocalDataset").ToLocalChecked(),
       FunctionTemplate::New(isolate, PixelEngine::GlobalFunc_LocalDatasetCallBack)->GetFunction(context).ToLocalChecked() );


	//normal/inverse
	pe->Set(context,String::NewFromUtf8(isolate, "ColorRampNormal").ToLocalChecked(),
	Integer::New(isolate,0));
	pe->Set(context,String::NewFromUtf8(isolate, "ColorRampInverse").ToLocalChecked(),
	Integer::New(isolate,1));
	//discrete/interpol
	pe->Set(context,String::NewFromUtf8(isolate, "ColorRampDiscrete").ToLocalChecked(),
	Integer::New(isolate,0));
	pe->Set(context,String::NewFromUtf8(isolate, "ColorRampInterpolate").ToLocalChecked(),
	Integer::New(isolate,1));


	//pe function log
	pe->Set(context
		,String::NewFromUtf8(isolate, "log").ToLocalChecked(),
           FunctionTemplate::New(isolate, PixelEngine::GlobalFunc_Log)->GetFunction(context).ToLocalChecked() );
	cout<<"debug 2001"<<endl ;

	//set globalFunc_forEachPixelCallBack in javascript
	string sourceforEachPixelFunction = R"(
		var globalFunc_forEachPixelCallBack = function(pxfunc){
			var outds = null ; 
			var outtiledata = null ;
			var width = this.width ;
			var height = this.height ;
			var asize = width*height ;
			var nband = this.nband ;
			var pxvals = new Int16Array(nband) ;
			var ib = 0 ;
			var intiledata = this.tiledata ;
			var outband = 0 ;
			var isResArray = false;
			for(var it = 0 ; it<asize ; ++ it )
			{
				for(ib=0;ib<nband;++ib)
				{
					pxvals[ib]=intiledata[ib*asize+it] ;
				}
				var res = pxfunc(pxvals,it,this) ;
				if( outtiledata==null )
				{
					if( Array.isArray(res) )
					{
						isResArray = true ;
						outband = res.length ;
						outds = globalFunc_newDatasetCallBack(3,width,height,outband) ;
						outtiledata = outds.tiledata ;
					}else
					{
						isResArray = false ;
						outband = 1 ;
						outds = globalFunc_newDatasetCallBack(3,width,height,outband) ;
						outtiledata = outds.tiledata ;
					}
				}
				if( isResArray )
				{
					for(ib=0;ib<outband;++ib)
					{
						outtiledata[ib*asize+it]=res[ib] ;
					}
				}else
				{
					outtiledata[it]=res ;				
				}

			}
			return outds ;
		} ;
		var globalFunc_getPixelCallBack=function(iband,isample,iline,nodata) {
			var gy = 0 ;
			var gx = 0 ;
			var newsample = isample ;
			var newline = iline ;
			if( isample< 0 ){
				gx -= 1 ;
				newsample += 256 ;
			}else if(isample>255 ){
				gx += 1 ;
				newsample -= 256 ;
			}
			if(iline<0 )
			{
				gy -= 1;
				newline += 256 ;
			}else if( iline>255 ){
				gy += 1 ;
				newline -= 256 ;
			}

			if( gy==0 && gx==0 )
			{
				return this.tiledata[iband*65536+newline*256+newsample] ;
			}else
			{
				var nbi = (1+gy)*3 + 1 + gx ; 
				if( this.nbloads[nbi] == 1 )
				{
					if( this.nbdatas[nbi] != null )
					{
						return this.nbdatas[nbi][iband*65536+newline*256+newsample] ;
					}else
					{
						return nodata ;
					}
				}else
				{
					this.nbloads[nbi] = 1 ;
					var newloadedData = PixelEngine.GetTileData(this.dsName,this.dsDt,this.z,this.y+gy,this.x+gx) ;
					this.nbdatas[nbi] = newloadedData ;
					if( this.nbdatas[nbi] != null )
					{
						return this.nbdatas[nbi][iband*65536+newline*256+newsample] ;
					}else
					{
						return nodata ;
					}
				}
			} 
		};
		
	)" ;
	v8::Local<v8::Script> scriptForEach =
          v8::Script::Compile(context
          	, String::NewFromUtf8(isolate,sourceforEachPixelFunction.c_str()).ToLocalChecked()
          	).ToLocalChecked();
    cout<<"debug 2002"<<endl ;
    v8::Local<v8::Value> resultForEach = scriptForEach->Run(context).ToLocalChecked();

    cout<<"debug 2003"<<endl ;
    Local<Value> forEachFuncInJs = global->Get(context 
    	,String::NewFromUtf8(isolate, "globalFunc_forEachPixelCallBack").ToLocalChecked() ).ToLocalChecked() ;
    thePE->GlobalFunc_ForEachPixelCallBack.Reset(isolate , forEachFuncInJs) ;
    cout<<"debug 2004"<<endl ;

    Local<Value> getPixelFuncInJs = global->Get(context 
    	,String::NewFromUtf8(isolate, "globalFunc_getPixelCallBack").ToLocalChecked() ).ToLocalChecked() ;
    thePE->GlobalFunc_GetPixelCallBack.Reset(isolate , getPixelFuncInJs) ;
    cout<<"debug 2005"<<endl ;


    //set globalFunc_newDatasetCallBack, this will be called in javascript ForEachPixel.
    global->Set(context
		,String::NewFromUtf8(isolate, "globalFunc_newDatasetCallBack").ToLocalChecked(),
           FunctionTemplate::New(isolate, PixelEngine::GlobalFunc_NewDatasetCallBack)->GetFunction(context).ToLocalChecked() );

	// //global function globalFunc_renderGrayCallBack
	// global->Set(context
	// 	,String::NewFromUtf8(isolate, "globalFunc_renderGrayCallBack").ToLocalChecked(),
	//           FunctionTemplate::New(isolate, PixelEngine::GlobalFunc_RenderGrayCallBack)->GetFunction(context).ToLocalChecked() );
	return true ;
}

bool PixelEngine::RunScriptForTile(void* extra, string& jsSource,int currentdt,int z,int y,int x, vector<unsigned char>& retbinary) 
{
	cout<<"in RunScriptForTile init v8"<<endl;
	this->tileInfo.x = x ;
	this->tileInfo.y = y ;
	this->tileInfo.z = z ;
	this->extraPointer = extra ;

	bool allOk = true ;

  	// Create a new Isolate and make it the current one.
	//v8::Isolate::CreateParams create_params;
	this->create_params.array_buffer_allocator =
	v8::ArrayBuffer::Allocator::NewDefaultAllocator();
	//v8::Isolate* isolate = v8::Isolate::New(create_params);
	this->isolate = v8::Isolate::New(create_params);
	{
		cout<<"in RunScriptForTile run script"<<endl;
		v8::Isolate::Scope isolate_scope(this->isolate);
		v8::HandleScope handle_scope(this->isolate);

		// Create a new context.
		v8::Local<v8::Context> context = v8::Context::New(this->isolate );
		// Enter the context for compiling and running the hello world script.
		v8::Context::Scope context_scope(context);// enter scope
		PixelEngine::initTemplate(  this , this->isolate , context) ;
		this->m_context.Reset( this->isolate , context);
		TryCatch try_catch(this->isolate);
		string source = jsSource + "var PEMainResult=main();" ;

		// Compile the source code.
		v8::Local<v8::Script> script ;
		if (!Script::Compile(context, String::NewFromUtf8(this->isolate,
		  		source.c_str()).ToLocalChecked()).ToLocal(&script)) {
			String::Utf8Value error(this->isolate, try_catch.Exception());
			cout<<"v8 exception:"<<*error<<endl;
			// The script failed to compile; bail out.
			//return false;
			allOk = false ;
		}else
		{
			unsigned long now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			// Run the script to get the result.
			Local<v8::Value> result ;
			if (!script->Run(context).ToLocal(&result)) {
				String::Utf8Value error( this->isolate, try_catch.Exception());
				cout<<"v8 exception:"<<*error<<endl;
				// The script failed to compile; bail out.
				//return false;
				allOk = false ;
			}else
			{
				MaybeLocal<Value> peMainResult = context->Global()->Get(context 
		    		,String::NewFromUtf8(isolate, "PEMainResult").ToLocalChecked() ) ;
				if( peMainResult.IsEmpty() ) //IsNullOrUndefined() )
				{
					cout<<"PEMainResult is null or undefined."<<endl ;
					allOk = false ;
				}else
				{
					cout<<"in RunScriptForTile 4"<<endl;
					unsigned long now1 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
					printf("script run dura:%d ms \n", now1 - now);//

					//tiledata to png
					PixelEngine::Dataset2Png( isolate, context, peMainResult.ToLocalChecked()
						, retbinary );

					unsigned long now2 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
					printf("encode png:%d ms \n", now2 - now1);
				}
			}
		}
	}
	

	this->m_context.Reset() ;
	this->GlobalFunc_ForEachPixelCallBack.Reset() ;
	this->GlobalFunc_GetPixelCallBack.Reset() ;

	// Dispose the isolate and tear down V8.
	this->isolate->Dispose();
	// v8::V8::Dispose();
	// v8::V8::ShutdownPlatform();
	//delete create_params.array_buffer_allocator;

	return allOk ;
}

PixelEngine::PixelEngine() 
{
	cout<<"PixelEngine()"<<endl;
	extraPointer = 0 ;
	this->tileInfo.x = 0 ;
	this->tileInfo.y = 0 ;
	this->tileInfo.z = 0 ;
}


PixelEngine::~PixelEngine() 
{
	cout<<"~PixelEngine()"<<endl;
	
}


void PixelEngine::initV8() 
{
	// Initialize V8.
	cout<<"init v8"<<endl ;
	v8::V8::InitializeICUDefaultLocation(".");
	v8::V8::InitializeExternalStartupData(".");
	v8Platform = v8::platform::NewDefaultPlatform();
	v8::V8::InitializePlatform(v8Platform.get());
	v8::V8::Initialize();
}