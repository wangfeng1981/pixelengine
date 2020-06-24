//PixelEngine.cpp
#include "PixelEngine.h"

//PixelEngine_GetDataFromExternal_FunctionPointer PixelEngine::GetExternalDatasetCallBack = nullptr ;
PixelEngine_GetDataFromExternal2_FunctionPointer PixelEngine::GetExternalTileDataCallBack = nullptr ;
PixelEngine_GetDataFromExternal2Arr_FunctionPointer PixelEngine::GetExternalTileDataArrCallBack = nullptr ;
PixelEngine_GetColorRampFromExternal_FunctionPointer PixelEngine::GetExternalColorRampCallBack = nullptr ;
std::unique_ptr<v8::Platform> PixelEngine::v8Platform = nullptr;

int PixelEngineColorRamp::upper_bound(int val) 
{
	if( this->numColors<2 )
	{
		return 0 ;
	}
	int i0 = 0 ;
	int i1 = this->numColors-1 ;
	while(i0<i1)
	{
		if(i0==i1-1)
		{
			if( val == this->ivalues[i0] ) return i0 ;
			else return i1 ;
		} 
		int ic = (i1+i0)/2 ;
		if(val==this->ivalues[ic])
		{
			return ic ;
		}else if( val>this->ivalues[ic] )
		{
			i0 = ic ;
		}else{
			i1 = ic ;
		}
	}
	if( val==this->ivalues[i0] )
	{
		return i0 ;
	}else
	{
		return i1 ;
	}
}

int PixelEngineColorRamp::binary_equal(int val) 
{
	if( this->numColors<1 )
	{
		return -1 ;
	}else if( this->numColors == 1 )
	{
		if( val == this->ivalues[0] )
		{
			return 0 ;
		}else
		{
			return -1 ;
		}
	}else
	{
		int i0 = 0 ;
		int i1 = this->numColors-1 ;
		while(i0<i1)
		{
			if(i0==i1-1)
			{
				if( val == this->ivalues[i0] ) return i0 ;
				else if( val==this->ivalues[i1] ) return i1 ;
				else return -1 ;
			} 
			int ic = (i1+i0)/2 ;
			if(val==this->ivalues[ic])
			{
				return ic ;
			}else if( val>this->ivalues[ic] )
			{
				i0 = ic ;
			}else{
				i1 = ic ;
			}
		}
		if( val==this->ivalues[i0] )
		{
			return i0 ;
		}else if( val==this->ivalues[i1] )
		{
			return i1 ;
		}else
		{
			return -1 ;
		}
	}
}

bool PixelEngineColorRamp::unwrap(Isolate* isolate , Local<v8::Value> obj)
{
	v8::HandleScope handle_scope(isolate);
	Local<Context> context(isolate->GetCurrentContext()) ;

	Local<Object> crobj = obj->ToObject(context).ToLocalChecked() ;
	this->useInteger = crobj->Get(context,
		String::NewFromUtf8(isolate,"useInteger").ToLocalChecked())
		.ToLocalChecked()->ToBoolean(isolate)->Value() ;
	this->numColors = crobj->Get(context,
		String::NewFromUtf8(isolate,"numColors").ToLocalChecked())
		.ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	this->Nodata = crobj->Get(context,
		String::NewFromUtf8(isolate,"Nodata").ToLocalChecked())
		.ToLocalChecked()->ToNumber(context).ToLocalChecked()->Value() ;

	//nodatacolor
	Uint8Array* nodatacolorArray = Uint8Array::Cast(
			* crobj->Get(context,String::NewFromUtf8(isolate,"NodataColor").ToLocalChecked()).ToLocalChecked()
			) ; 
	nodatacolorArray->CopyContents( this->NodataColor , 4) ;

	//ivalues
	Int32Array* ivaluesArray = Int32Array::Cast(
			* crobj->Get(context,String::NewFromUtf8(isolate,"ivalues").ToLocalChecked()).ToLocalChecked()
			) ; 
	ivaluesArray->CopyContents( this->ivalues , PixelEngineColorRamp::MAXNUM_COLORS*4 ) ;

	//fvalues
	Float32Array* fvaluesArray = Float32Array::Cast(
			* crobj->Get(context,String::NewFromUtf8(isolate,"fvalues").ToLocalChecked()).ToLocalChecked()
			) ; 
	fvaluesArray->CopyContents( this->fvalues , PixelEngineColorRamp::MAXNUM_COLORS*4 ) ;

	//red
	Uint8Array* rArray = Uint8Array::Cast(
			* crobj->Get(context,String::NewFromUtf8(isolate,"r").ToLocalChecked()).ToLocalChecked()
			) ; 
	rArray->CopyContents( this->r , PixelEngineColorRamp::MAXNUM_COLORS) ;

	//green
	Uint8Array* gArray = Uint8Array::Cast(
			* crobj->Get(context,String::NewFromUtf8(isolate,"g").ToLocalChecked()).ToLocalChecked()
			) ; 
	gArray->CopyContents( this->g , PixelEngineColorRamp::MAXNUM_COLORS) ;

	//blue
	Uint8Array* bArray = Uint8Array::Cast(
			* crobj->Get(context,String::NewFromUtf8(isolate,"b").ToLocalChecked()).ToLocalChecked()
			) ; 
	bArray->CopyContents( this->b , PixelEngineColorRamp::MAXNUM_COLORS) ;

	//alpha
	Uint8Array* aArray = Uint8Array::Cast(
			* crobj->Get(context,String::NewFromUtf8(isolate,"a").ToLocalChecked()).ToLocalChecked()
			) ; 
	aArray->CopyContents( this->a , PixelEngineColorRamp::MAXNUM_COLORS) ;
	return true ;
}

bool PixelEngineColorRamp::unwrapWithLabels( Isolate* isolate , Local<v8::Value> obj)
{
	v8::HandleScope handle_scope(isolate);
	Local<Context> context(isolate->GetCurrentContext()) ;
	bool isok = this->unwrap(isolate , obj) ;
	if( isok )
	{
		Local<Object> crobj = obj->ToObject(context).ToLocalChecked() ;

		String::Utf8Value nodataLabelUtf8Value(isolate, crobj->Get(context,
			String::NewFromUtf8(isolate,"NodataLabel").ToLocalChecked())
			.ToLocalChecked() );
		this->NodataLabel = string(*nodataLabelUtf8Value) ;

		Array* labelarray = Array::Cast(
				*crobj->Get(context,String::NewFromUtf8(isolate,"labels").ToLocalChecked()).ToLocalChecked()
			) ;
		for(int i = 0 ; i<this->numColors ; ++ i )
		{
			String::Utf8Value utf8val(isolate, labelarray->Get(context,i).ToLocalChecked() ) ;
			this->labels[i] = string(*utf8val) ;
		}
		return true ;
	}else{
		return false ;
	}
}

void PixelEngineColorRamp::copy2v8(Isolate* isolate , Local<v8::Value> obj) 
{
	v8::HandleScope handle_scope(isolate);
	Local<Context> context(isolate->GetCurrentContext()) ;

	Local<Object> crobj = obj->ToObject(context).ToLocalChecked() ;

	//set use integer
	crobj->Set(context , 
		String::NewFromUtf8(isolate,"useInteger").ToLocalChecked() , 
		Boolean::New(isolate,true)
		) ;

	//set color number
	crobj->Set(context , 
		String::NewFromUtf8(isolate,"numColors").ToLocalChecked() , 
		Integer::New(isolate,this->numColors)
		) ;
	//set nodata
	crobj->Set(context,
		String::NewFromUtf8(isolate,"Nodata").ToLocalChecked() , 
		Number::New(isolate,this->Nodata)
		) ;
	//nodatacolor
	Uint8Array* nodatacolorArray = Uint8Array::Cast(
			* crobj->Get(context,String::NewFromUtf8(isolate,"NodataColor").ToLocalChecked()).ToLocalChecked()
			) ; 
	unsigned char* nodatacolorBackPtr = (unsigned char*)( nodatacolorArray->Buffer()->GetBackingStore().get()->Data() );
	nodatacolorBackPtr[0] = this->NodataColor[0] ;
	nodatacolorBackPtr[1] = this->NodataColor[1] ;
	nodatacolorBackPtr[2] = this->NodataColor[2] ;
	nodatacolorBackPtr[3] = this->NodataColor[3] ;

	//set ivalues
	Int32Array* ivaluesArray = Int32Array::Cast(
			* crobj->Get(context,String::NewFromUtf8(isolate,"ivalues").ToLocalChecked()).ToLocalChecked()
			) ; 
	int* ivaluesBackPtr = (int*)( ivaluesArray->Buffer()->GetBackingStore().get()->Data() );


	//red
	Uint8Array* rArray = Uint8Array::Cast(
			* crobj->Get(context,String::NewFromUtf8(isolate,"r").ToLocalChecked()).ToLocalChecked()
			) ; 
	unsigned char* rBackPtr = (unsigned char*)( rArray->Buffer()->GetBackingStore().get()->Data() );

	//green
	Uint8Array* gArray = Uint8Array::Cast(
			* crobj->Get(context,String::NewFromUtf8(isolate,"g").ToLocalChecked()).ToLocalChecked()
			) ; 
	unsigned char* gBackPtr = (unsigned char*)( gArray->Buffer()->GetBackingStore().get()->Data() );

	//blue
	Uint8Array* bArray = Uint8Array::Cast(
			* crobj->Get(context,String::NewFromUtf8(isolate,"b").ToLocalChecked()).ToLocalChecked()
			) ; 
	unsigned char* bBackPtr = (unsigned char*)( bArray->Buffer()->GetBackingStore().get()->Data() );

	//alpha
	Uint8Array* aArray = Uint8Array::Cast(
			* crobj->Get(context,String::NewFromUtf8(isolate,"a").ToLocalChecked()).ToLocalChecked()
			) ; 
	unsigned char* aBackPtr = (unsigned char*)( aArray->Buffer()->GetBackingStore().get()->Data() );

	for(int ic = 0 ; ic < this->numColors ; ++ ic )
	{
		ivaluesBackPtr[ic] = this->ivalues[ic] ;
		rBackPtr[ic] = this->r[ic] ;
		gBackPtr[ic] = this->g[ic] ;
		bBackPtr[ic] = this->b[ic] ;
		aBackPtr[ic] = this->a[ic] ;
	}

}


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

string PixelEngine::long2str(long val) 
{
	char buff[32] ;
	sprintf(buff,"%ld" , val ) ;
	return string(buff) ;
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


/// use c++ create a new empty DatasetArray object
Local<Object> PixelEngine::CPP_NewDatasetArray(Isolate* isolate,Local<Context>& context
	,const int datatype 
	,const int width 
	,const int height
	,const int nband
	,const int numds )
{
	cout<<"inside CPP_NewDatasetArray"<<endl; 
	v8::EscapableHandleScope handle_scope(isolate);

	Local<Object> global = context->Global() ;

	Local<Value> forEachFuncInJs = global->Get(context 
    	,String::NewFromUtf8(isolate, "globalFunc_DA_forEachPixelCallBack").ToLocalChecked() ).ToLocalChecked() ;
	Local<Object> ds = Object::New(isolate) ;
	ds->Set(context
		,String::NewFromUtf8(isolate, "forEachPixel").ToLocalChecked(),
            forEachFuncInJs );

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
	ds->Set(context
		,String::NewFromUtf8(isolate, "numds").ToLocalChecked()
		,Integer::New(isolate,numds) ) ;
	ds->Set(context
		,String::NewFromUtf8(isolate, "dsName").ToLocalChecked()
		,String::Empty(isolate) ) ;
	ds->Set(context
		,String::NewFromUtf8(isolate, "fromdt").ToLocalChecked()
		,String::Empty(isolate) ) ;
	ds->Set(context
		,String::NewFromUtf8(isolate, "todt").ToLocalChecked()
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


	Local<ArrayBuffer> timeArrayAB = ArrayBuffer::New(isolate,numds*8) ;
	Local<BigInt64Array> timeArrayInt64 = BigInt64Array::New(timeArrayAB,0,numds) ;
	ds->Set(context
		,String::NewFromUtf8(isolate, "timeArr").ToLocalChecked()
		,timeArrayInt64 ) ;
	ds->Set(context
		,String::NewFromUtf8(isolate, "dataArr").ToLocalChecked()
		,Array::New(isolate,numds) ) ;// 4 is current always null.
	
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

/// interpolate RGB of value from predefined colors
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


//convert value to color by colorRamp
void PixelEngine::Value2Color(int valx,
	PixelEngineColorRamp& cr,
	int interpol, //0-discrete,1-linear,2-exact
	unsigned char& rr,unsigned char& rg,unsigned char& rb,unsigned char& ra )
{
	if( valx == cr.Nodata ){
		rr = cr.NodataColor[0] ;
		rg = cr.NodataColor[1] ;
		rb = cr.NodataColor[2] ;
		ra = cr.NodataColor[3] ;
	}else
	{
		if( interpol==0 )
		{//discrete
			if( cr.numColors<=0 )
			{
				rr = cr.NodataColor[0] ;
				rg = cr.NodataColor[1] ;
				rb = cr.NodataColor[2] ;
				ra = cr.NodataColor[3] ;
			}else
			{
				if(valx <= cr.ivalues[0])
				{
					rr = cr.r[0] ; rg = cr.g[0] ; rb = cr.b[0] ; ra = cr.a[0] ;
				}else if( valx >= cr.ivalues[cr.numColors-1] )
				{
					int ii = cr.numColors-1 ;
					rr = cr.r[ii] ; rg = cr.g[ii] ; rb = cr.b[ii] ; ra = cr.a[ii] ;
				}else
				{
					int ii = cr.upper_bound(valx) ;
					rr = cr.r[ii] ; rg = cr.g[ii] ; rb = cr.b[ii] ; ra = cr.a[ii] ;
				}
			}
		}else if( interpol==1 )
		{//linear
			if( cr.numColors<=0 )
			{
				rr = cr.NodataColor[0] ;
				rg = cr.NodataColor[1] ;
				rb = cr.NodataColor[2] ;
				ra = cr.NodataColor[3] ;
			}else
			{
				if(valx <= cr.ivalues[0])
				{
					rr = cr.r[0] ; rg = cr.g[0] ; rb = cr.b[0] ; ra = cr.a[0] ;
				}else if( valx >= cr.ivalues[cr.numColors-1] )
				{
					int ii = cr.numColors-1 ;
					rr = cr.r[ii] ; rg = cr.g[ii] ; rb = cr.b[ii] ; ra = cr.a[ii] ;
				}else
				{
					int ii = cr.upper_bound(valx) ;
					int ii0 = ii-1 ;
					if( ii0>=0 )
					{
						float wup = (valx-cr.ivalues[ii0])*1.f/(cr.ivalues[ii]-cr.ivalues[ii0]) ;
						float wl = 1.f - wup ;
						rr = cr.r[ii0]*wl+ cr.r[ii]*wup ;
						rg = cr.g[ii0]*wl+ cr.g[ii]*wup ;
						rb = cr.b[ii0]*wl+ cr.b[ii]*wup ; 
						ra = cr.a[ii0]*wl+ cr.a[ii]*wup ;

					}else
					{
						rr = cr.r[ii] ; rg = cr.g[ii] ; rb = cr.b[ii] ; ra = cr.a[ii] ;
					}
				}
			}
		}else
		{//exact
			if( cr.numColors<=0 )
			{
				rr = cr.NodataColor[0] ;
				rg = cr.NodataColor[1] ;
				rb = cr.NodataColor[2] ;
				ra = cr.NodataColor[3] ;
			}else
			{
				int ii = cr.binary_equal(valx) ;
				if( ii<0 )
				{
					rr = cr.NodataColor[0] ;
					rg = cr.NodataColor[1] ;
					rb = cr.NodataColor[2] ;
					ra = cr.NodataColor[3] ;
				}else
				{
					rr = cr.r[ii] ; rg = cr.g[ii] ; rb = cr.b[ii] ; ra = cr.a[ii] ;
				}
			}
		}
	}
}
	



/// dataset.renderPsuedColor(...), return a new Dataset
//args1: iband,vmin,vmax,nodata,nodataColor,colorid,noraml/inverse(0/1),discrete/interpol(0/1)
//args2: iband,colorRamp,method(discrete 0/linear 1/exact 2)
void PixelEngine::GlobalFunc_RenderPsuedColorCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) 
{
	cout<<"inside GlobalFunc_RenderPsuedColorCallBack"<<endl; 
	if (args.Length() != 8 && args.Length() != 3){
		cout<<"Error: args.Length != 8 and != 3 "<<endl ;
		return;
	}
	Isolate* isolate = args.GetIsolate() ;
	v8::HandleScope handle_scope(isolate);
	Local<Context> context(isolate->GetCurrentContext()) ;

	int iband = 0 ;
	int colormethod = 0 ;//0-discrete,1-interpol linear,2-exact.
	PixelEngineColorRamp colorRamp ;

	if( args.Length() == 8 )
	{
		Local<Value> v8_iband = args[0];
		Local<Value> v8_vmin = args[1] ;
		Local<Value> v8_vmax = args[2] ;
		Local<Value> v8_nodata = args[3] ;
		Local<Value> v8_nodatacolor = args[4] ;
		Local<Value> v8_colorid = args[5] ;
		Local<Value> v8_inverse = args[6] ;
		Local<Value> v8_interpol = args[7] ;

		iband = v8_iband->ToInteger(context).ToLocalChecked()->Value() ;
		int vmin = v8_vmin->ToInteger(context).ToLocalChecked()->Value() ;
		int vmax = v8_vmax->ToInteger(context).ToLocalChecked()->Value() ;
		colorRamp.Nodata = v8_nodata->ToInteger(context).ToLocalChecked()->Value() ;
		Local<Object> nodataColorObj = v8_nodatacolor->ToObject(context).ToLocalChecked() ;
		colorRamp.NodataColor[0] = nodataColorObj->Get(context,0).ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
		colorRamp.NodataColor[1] = nodataColorObj->Get(context,1).ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
		colorRamp.NodataColor[2] = nodataColorObj->Get(context,2).ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
		colorRamp.NodataColor[3] = nodataColorObj->Get(context,3).ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;

		int colorid = v8_colorid->ToInteger(context).ToLocalChecked()->Value() ;
		int inverse = v8_inverse->ToInteger(context).ToLocalChecked()->Value() ;
		colormethod = v8_interpol->ToInteger(context).ToLocalChecked()->Value() ;

		vector<int> rgbvec = PixelEngine::GetColorRamp(colorid,inverse) ;
		int ncolor = rgbvec.size()/3 ;
		float stepf = (vmax-vmin) / (ncolor-1.f) ;
		colorRamp.useInteger=true ;
		for( int i = 0 ; i<ncolor ; ++ i )
		{
			colorRamp.ivalues[i] = vmin + i*stepf ;
			colorRamp.r[i] = rgbvec[i*3+0] ;
			colorRamp.g[i] = rgbvec[i*3+1] ;
			colorRamp.b[i] = rgbvec[i*3+2] ;
			colorRamp.a[i] = 255 ;
			++ colorRamp.numColors ;
		}
	}else if( args.Length() == 3 )
	{//use colorramp

		Local<Value> v8_iband = args[0];
		Local<Value> v8_colorramp = args[1] ;
		Local<Value> v8_interpol = args[2] ;
		iband = v8_iband->ToInteger(context).ToLocalChecked()->Value() ;
		colormethod = v8_interpol->ToInteger(context).ToLocalChecked()->Value() ;
		colorRamp.unwrap(isolate , v8_colorramp ) ;
	}

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
	int asize2 = asize*2 ;
	int asize3 = asize*3 ;
	if( thisDataType==3 )
	{//short
		Int16Array* i16Array = Int16Array::Cast(*tiledataValue) ;
		short* backData = (short*) i16Array->Buffer()->GetBackingStore()->Data() ;
		short* backDataOffset = backData + iband * asize;
		for(int it = 0 ; it < asize ; ++ it )
		{
			PixelEngine::Value2Color(backDataOffset[it], 
				colorRamp , 
				colormethod , 
				outbackData[it],
				outbackData[it+asize],
				outbackData[it+asize2],
				outbackData[it+asize3]) ;
		}
	}else
	{//byte
		Uint8Array* u8Array = Uint8Array::Cast(*tiledataValue) ;
		unsigned char* backData = (unsigned char*) u8Array->Buffer()->GetBackingStore()->Data() ;
		unsigned char* backDataOffset = backData + iband * asize;
		for(int it = 0 ; it < asize ; ++ it )
		{
			PixelEngine::Value2Color(backDataOffset[it], 
				colorRamp , 
				colormethod , 
				outbackData[it],
				outbackData[it+asize],
				outbackData[it+asize2],
				outbackData[it+asize3]) ;
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

	if( datetime == "-1" )
	{
		cout<<"use current "<<thisPePtr->currentDateTime<<endl ;
		datetime = PixelEngine::long2str(thisPePtr->currentDateTime) ;
	}

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





/// create a DatasetArray from external.
/// @params name
/// @params fromDatetime included
/// @params toDatetime not included
/// @params bands [0,1,2,3] not used yet
/// @params filterMon optional
/// @params filterDay optional
/// @params filterHour optional
/// @params filterMinu optional
/// @params filterSec optional    PixelEngine.Ignore
void PixelEngine::GlobalFunc_DatasetArrayCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) 
{
	cout<<"inside GlobalFunc_DatasetArrayCallBack"<<endl; 
	if (args.Length() < 4 ){
		cout<<"Error: args.Length < 4 "<<endl ;
		return;
	}

	Isolate* isolate = args.GetIsolate() ;
	v8::HandleScope handle_scope(isolate);
	Local<Context> context(isolate->GetCurrentContext()) ;

	Local<Value> v8name = args[0];
	Local<Value> v8from = args[1] ;
	Local<Value> v8to = args[2] ;// not include
	Local<Value> v8bands = args[3] ;// not used yet

	int filtermon = -1 ;
	int filterday = -1 ;
	int filterhour = -1 ;
	int filterminu = -1 ;
	int filtersec = -1 ;
	if( args.Length() > 4 )
	{
		filtermon = Integer::Cast(*args[4])->Value() ;
	}
	if( args.Length()> 5)
	{
		filterday = Integer::Cast(*args[5])->Value() ;
	}
	if( args.Length()> 6)
	{
		filterhour = Integer::Cast(*args[6])->Value() ;
	}
	if( args.Length()> 7)
	{
		filterminu = Integer::Cast(*args[7])->Value() ;
	}
	if( args.Length()> 8)
	{
		filtersec = Integer::Cast(*args[8])->Value() ;
	}


	String::Utf8Value nameutf8( isolate , v8name) ;
	string name( *nameutf8 ) ;

	String::Utf8Value dt0utf8( isolate , v8from) ;
	string fromdatetime( *dt0utf8 ) ;

	String::Utf8Value dt1utf8( isolate , v8to) ;
	string todatetime( *dt1utf8 ) ;

	cout<<"from to :"<<fromdatetime<<"-"<<todatetime<<endl ;

	Array* i32array = Array::Cast(*v8bands) ;
	int nband = i32array->Length() ;
	vector<int> wantBands ;
	wantBands.reserve(32) ;
	for(int ib = 0 ; ib<nband ; ++ ib )
	{
		int wantib = i32array->Get(context,ib).ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
		wantBands.push_back(wantib) ;
	}

	vector<vector<unsigned char> > externalDataArr ;
	vector<long> externalTimeArr ;
	int dt = 0 ;
	int wid = 0 ;
	int hei = 0 ; 
	int retnbands = 0 ;// return number of bands in one dataset
	int retnumds = 0 ;// return number of datasets

	Local<Object> global = context->Global() ;
	Local<Value> peinfo = global->Get( context
		,String::NewFromUtf8(isolate, "PixelEnginePointer").ToLocalChecked())
		.ToLocalChecked() ;
	Object* peinfoObj = Object::Cast( *peinfo ) ;
	Local<Value> thisPePtrValue = peinfoObj->GetInternalField(0) ;
	External* thisPePtrEx = External::Cast(*thisPePtrValue);
	PixelEngine* thisPePtr = static_cast<PixelEngine*>(thisPePtrEx->Value() );

	int tilez = thisPePtr->tileInfo.z  ;
	int tiley = thisPePtr->tileInfo.y  ; 
	int tilex = thisPePtr->tileInfo.x  ; 
	if( fromdatetime == "-1" )
	{
		cout<<"fromdatetime use current "<<thisPePtr->currentDateTime<<endl ;
		fromdatetime = PixelEngine::long2str(thisPePtr->currentDateTime) ;
	}

	if( todatetime == "-1" )
	{
		cout<<"todatetime use current "<<thisPePtr->currentDateTime<<endl ;
		todatetime = PixelEngine::long2str(thisPePtr->currentDateTime) ;
	}


	bool externalOk = PixelEngine::GetExternalTileDataArrCallBack(
		thisPePtrEx->Value() ,// pointer to PixelEngine Object.
		name,fromdatetime,todatetime,wantBands,
		tilez,
		tiley,
		tilex,
		filtermon,
		filterday,
		filterhour,
		filterminu,
		filtersec ,
		externalDataArr , 
		externalTimeArr , 
		dt ,
		wid , 
		hei , 
		retnbands ,
		retnumds ) ;
	if( externalOk==false )
	{
		cout<<"Error: PixelEngine::GetExternalTileDataArrCallBack failed."<<endl;
		return ;//return null in javascript.
	}

	Local<Object> ds = PixelEngine::CPP_NewDatasetArray( isolate
		,context
		,dt
		,wid
		,hei
		,retnbands
		,retnumds );
	ds->Set(context
		,String::NewFromUtf8(isolate, "dsName").ToLocalChecked()
		,v8name ) ;
	ds->Set(context
		,String::NewFromUtf8(isolate, "fromdt").ToLocalChecked()
		,v8from ) ;
	ds->Set(context
		,String::NewFromUtf8(isolate, "todt").ToLocalChecked()
		,v8to ) ;

	ds->Set(context
		,String::NewFromUtf8(isolate, "x").ToLocalChecked()
		,Integer::New(isolate,thisPePtr->tileInfo.x) ) ;
	ds->Set(context
		,String::NewFromUtf8(isolate, "y").ToLocalChecked()
		,Integer::New(isolate,thisPePtr->tileInfo.y) ) ;
	ds->Set(context
		,String::NewFromUtf8(isolate, "z").ToLocalChecked()
		,Integer::New(isolate,thisPePtr->tileInfo.z) ) ;


	Local<Value> dataArrValue = ds->Get( context
		,String::NewFromUtf8(isolate, "dataArr").ToLocalChecked())
		.ToLocalChecked() ;
	Array* dataArrArray = Array::Cast(*dataArrValue) ;

	Local<Value> timeArrValue = ds->Get( context
		,String::NewFromUtf8(isolate, "timeArr").ToLocalChecked())
		.ToLocalChecked() ;
	BigInt64Array* timeArrInt64 = BigInt64Array::Cast(*timeArrValue) ;


	for(int ids = 0 ; ids < retnumds ; ++ ids )
	{
		//set time
		timeArrInt64->Set(context,ids,BigInt::New(isolate,externalTimeArr[ids]));

		//set data
		vector<unsigned char>& indatavec = externalDataArr[ids] ;
		Local<ArrayBuffer> arrbuf = ArrayBuffer::New(isolate,indatavec.size()) ;
		Local<Int16Array> i16arr = Int16Array::New(arrbuf,0,indatavec.size()/2) ;
		short* backData = (short*) i16arr->Buffer()->GetBackingStore()->Data() ;
		memcpy(backData , indatavec.data(), indatavec.size() );
		dataArrArray->Set(context,ids,i16arr) ;
	}
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
	PixelEngine* thisPePtr = (PixelEngine*) thisPePtrEx->Value() ;

	if( datetime == "-1" )
	{
		cout<<"use current "<<thisPePtr->currentDateTime<<endl ;
		datetime = PixelEngine::long2str(thisPePtr->currentDateTime) ;
	}


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

/// get external colorramp.
/// PixelEngine.ColorRamp(colorid,colorRampObj)
void PixelEngine::GlobalFunc_ColorRampCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) 
{
	cout<<"inside GlobalFunc_ColorRampCallBack"<<endl; 
	if (args.Length() != 2 ){
		cout<<"Error: args.Length != 2 "<<endl ;
		return;
	}

	Isolate* isolate = args.GetIsolate() ;
	v8::HandleScope handle_scope(isolate);
	Local<Context> context(isolate->GetCurrentContext()) ;

	Local<Value> colorid0 = args[0];
	String::Utf8Value colorid1( isolate , colorid0) ;
	string strColorid( *colorid1 ) ;

	Local<Value> v8_crObj = args[1] ;

	if( PixelEngine::GetExternalColorRampCallBack != nullptr )
	{
		Local<Object> global = context->Global() ;
		Local<Value> peinfo = global->Get( context
			,String::NewFromUtf8(isolate, "PixelEnginePointer").ToLocalChecked())
			.ToLocalChecked() ;
		Object* peinfoObj = Object::Cast( *peinfo ) ;
		Local<Value> thisPePtrValue = peinfoObj->GetInternalField(0) ;
		External* thisPePtrEx = External::Cast(*thisPePtrValue);
		PixelEngine* thisPePtr = static_cast<PixelEngine*>(thisPePtrEx->Value() );

		PixelEngineColorRamp cr = PixelEngine::GetExternalColorRampCallBack(thisPePtrEx->Value(),strColorid) ;
		cr.copy2v8( isolate , v8_crObj) ;
		args.GetReturnValue().Set(v8_crObj) ;
	}else
	{
		cout<<"Error: PixelEngine::GetExternalColorRampCallBack is nullptr."<<endl;
		args.GetReturnValue().Set(v8_crObj) ;
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
	pe->Set(context,String::NewFromUtf8(isolate, "DatetimeCurrent").ToLocalChecked(),
	   Integer::New(isolate,0));//modified 2020-6-20
	pe->Set(context,String::NewFromUtf8(isolate, "Ignore").ToLocalChecked(),
	   Integer::New(isolate,-1));

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
	pe->Set(context,String::NewFromUtf8(isolate, "ColorRampExact").ToLocalChecked(),
	Integer::New(isolate,2));//add 2020-6-20


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
	pe->Set(context
		,String::NewFromUtf8(isolate, "DatasetArray").ToLocalChecked(),
           FunctionTemplate::New(isolate, PixelEngine::GlobalFunc_DatasetArrayCallBack)->GetFunction(context).ToLocalChecked() );



	//pe function log
	pe->Set(context
		,String::NewFromUtf8(isolate, "log").ToLocalChecked(),
           FunctionTemplate::New(isolate, PixelEngine::GlobalFunc_Log)->GetFunction(context).ToLocalChecked() );

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
		var globalFunc_DA_forEachPixelCallBack = function(pxfunc){
			var outds = null ; 
			var outtiledata = null ;
			var width = this.width ;
			var height = this.height ;
			var asize = width*height ;
			var nband = this.nband ;
			var nds = this.numds ;
			var tbvals = new Int16Array(nband*nds) ;
			var ib = 0 ;var ids=0; var itb=0;var bit=0;
			var tdarr = this.dataArr ;
			var outband = 0 ;
			var isResArray = false;
			for(var it = 0 ; it<asize ; ++ it )
			{
				itb=0;
				for(ib=0;ib<nband;++ib){    
					bit=ib*asize+it;
					for(ids=0;ids<nds;++ids)
					{
						tbvals[itb++]=tdarr[ids][bit] ;
					}
				}
				var res = pxfunc(tbvals,it,nds,this) ;
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
			if(this.dsName==null || this.dsName=="" ) return nodata ;
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
		PixelEngine.ColorRamp = function(colorRampId){
			var cr = new Object() ;
			cr.useInteger = true ;
			cr.numColors = 0 ;
			cr.ivalues = new Int32Array(50) ;
			cr.fvalues = new Float32Array(50) ;
			cr.r = new Uint8Array(50) ;
			cr.g = new Uint8Array(50) ;
			cr.b = new Uint8Array(50) ;
			cr.a = new Uint8Array(50) ;
			cr.labels = new Array(50) ;
			cr.Nodata = 0 ;
			cr.NodataColor = new Uint8Array(4) ;
			cr.NodataLabel = "Nan" ;
			cr.add = function(v,r,g,b,a,l){
				if(this.numColors>50) return ;
				let i = this.numColors ;
				this.ivalues[i] = v ;
				this.fvalues[i] = v ;
				this.r[i] = r ;
				this.g[i] = g ;
				this.b[i] = b ;
				this.a[i] = a ;
				this.labels[i] = l ;
				++this.numColors ;
				if( this.useInteger==true )
				{
					if( Number.isInteger(v) == false ) this.useInteger=false ;
				}
				return this.numColors ;
			} ;
			if(colorRampId!==undefined){
				globalFunc_ColorRampCallBack(colorRampId,cr) ;
			}
			return cr ;
		};	
	)" ;
	v8::Local<v8::Script> scriptForEach =
          v8::Script::Compile(context
          	, String::NewFromUtf8(isolate,sourceforEachPixelFunction.c_str()).ToLocalChecked()
          	).ToLocalChecked();
    
    v8::Local<v8::Value> resultForEach = scriptForEach->Run(context).ToLocalChecked();

    Local<Value> forEachFuncInJs = global->Get(context 
    	,String::NewFromUtf8(isolate, "globalFunc_forEachPixelCallBack").ToLocalChecked() ).ToLocalChecked() ;
    thePE->GlobalFunc_ForEachPixelCallBack.Reset(isolate , forEachFuncInJs) ;

    Local<Value> getPixelFuncInJs = global->Get(context 
    	,String::NewFromUtf8(isolate, "globalFunc_getPixelCallBack").ToLocalChecked() ).ToLocalChecked() ;
    thePE->GlobalFunc_GetPixelCallBack.Reset(isolate , getPixelFuncInJs) ;


    //set globalFunc_newDatasetCallBack, this will be called in javascript ForEachPixel.
    global->Set(context
		,String::NewFromUtf8(isolate, "globalFunc_newDatasetCallBack").ToLocalChecked(),
           FunctionTemplate::New(isolate, PixelEngine::GlobalFunc_NewDatasetCallBack)->GetFunction(context).ToLocalChecked() );

    //set globalFunc_ColorRampCallBack, this will be c++ codes.
    global->Set(context
		,String::NewFromUtf8(isolate, "globalFunc_ColorRampCallBack").ToLocalChecked(),
           FunctionTemplate::New(isolate, PixelEngine::GlobalFunc_ColorRampCallBack)->GetFunction(context).ToLocalChecked() );


	// //global function globalFunc_renderGrayCallBack
	// global->Set(context
	// 	,String::NewFromUtf8(isolate, "globalFunc_renderGrayCallBack").ToLocalChecked(),
	//           FunctionTemplate::New(isolate, PixelEngine::GlobalFunc_RenderGrayCallBack)->GetFunction(context).ToLocalChecked() );
	return true ;
}

bool PixelEngine::RunScriptForTile(void* extra, string& jsSource,long currentdt,int z,int y,int x, vector<unsigned char>& retbinary) 
{
	cout<<"in RunScriptForTile init v8"<<endl;
	this->tileInfo.x = x ;
	this->tileInfo.y = y ;
	this->tileInfo.z = z ;
	this->extraPointer = extra ;
	this->currentDateTime = currentdt ;

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

string PixelEngine::CheckScriptOk(string& scriptSource) 
{
	cout<<"in CheckScriptOk"<<endl;

	string errorText = "" ;
	this->create_params.array_buffer_allocator =
	v8::ArrayBuffer::Allocator::NewDefaultAllocator();
	this->isolate = v8::Isolate::New(create_params);
	{
		v8::Isolate::Scope isolate_scope(this->isolate);
		v8::HandleScope handle_scope(this->isolate);
		v8::Local<v8::Context> context = v8::Context::New(this->isolate );
		v8::Context::Scope context_scope(context);// enter scope
		
		v8::TryCatch try_catch(this->isolate);
		v8::MaybeLocal<v8::String> source =
			v8::String::NewFromUtf8( this->isolate, scriptSource.c_str() ,
				v8::NewStringType::kNormal) ;
		if (source.IsEmpty()) {
			//compile error
			v8::String::Utf8Value error( this->isolate, try_catch.Exception());
			errorText = std::string(*error);
			std::cout << "build source error:" << errorText << std::endl;
		}else
		{
			v8::MaybeLocal<v8::Script> compiledScript =
				v8::Script::Compile(context, source.ToLocalChecked() ) ;
			if (compiledScript.IsEmpty()) {
				//compile error
				v8::String::Utf8Value error( this->isolate, try_catch.Exception());
				errorText = std::string(*error);
				std::cout << "compile error:" << errorText << std::endl;
			}
		}

	}
	this->isolate->Dispose();

	return errorText ;
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