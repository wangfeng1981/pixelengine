//PixelEngine.cpp

#include "PixelEngine.h"
#include <time.h>


bool PixelEngine::quietMode = false;

const double PE_CURRENTDATETIME = 0L; 

//PixelEngine_GetDataFromExternal_FunctionPointer PixelEngine::GetExternalDatasetCallBack = nullptr ;
PixelEngine_GetDataFromExternal2_FunctionPointer PixelEngine::GetExternalTileDataCallBack = nullptr ;
PixelEngine_GetDataFromExternal2Arr_FunctionPointer PixelEngine::GetExternalTileDataArrCallBack = nullptr ;
PixelEngine_GetColorRampFromExternal_FunctionPointer PixelEngine::GetExternalColorRampCallBack = nullptr ;
std::unique_ptr<v8::Platform> PixelEngine::v8Platform = nullptr;

//string PixelEngine::pejs_version = string("2.2") ;
//string PixelEngine::pejs_version = string("2.4.1.1 2020-10-11"); //2020-9-13
string PixelEngine::pejs_version = string("2.4.2.0 2020-10-15");


//// mapreduce not used yet.
bool PixelEngineMapReduce::isSame(PixelEngineMapReduce& mr)
{
	if( tabName.compare(mr.tabName)==0 )
	{
		if( datetime == mr.datetime )
		{
			if( zlevel==mr.zlevel)
			{
				if( mapTile2ObjFunction.compare(mr.mapTile2ObjFunction) ==0 )
				{
					if( reduceObj2ObjFunction.compare(mr.reduceObj2ObjFunction)==0 )
					{
						return true ;
					}
				}
			}
		}
	}
	return false ;
}

void PixelEngineMapReduceContainer::add(PixelEngineMapReduce& mr)
{
	int num = this->mapreduceVector.size() ;
	bool hasSame = false ;
	for(int i = 0 ; i<num ; ++ i )
	{
		if( this->mapreduceVector[i].isSame(mr) )
		{
			hasSame = true ;
			break ;
		}
	}
	if( hasSame==false )
	{
		this->mapreduceVector.push_back(mr) ;
		if(! PixelEngine::quietMode) cout<<"mapreduceVector add one"<<endl ;
	}
}


///////////////////////////////////////////////////////////////


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

bool PixelEngine::V8ObjectGetIntValue(Isolate* isolate,
	Local<Object>&obj,
	Local<Context>&context,
	string key,
	int& retvalue ){

	MaybeLocal<Value> maybe1 = obj->Get(context,
		String::NewFromUtf8(isolate,key.c_str()).ToLocalChecked()
		) ;
	if( maybe1.IsEmpty() )
	{
		return false ;
	}else
	{
		Local<Value> localval = maybe1.ToLocalChecked() ;
		MaybeLocal<Integer> maybe2 = localval->ToInteger(context) ;
		if( maybe2.IsEmpty() )
		{
			return false ;
		}else{
			Local<Integer> local2=maybe2.ToLocalChecked() ;
			retvalue = local2->Value() ;
			return true ;
		}
	}
}

bool PixelEngine::V8ObjectGetNumberValue(Isolate* isolate,
	Local<Object>&obj,
	Local<Context>&context,
	string key,
	double& retvalue ){

	MaybeLocal<Value> maybe1 = obj->Get(context,
		String::NewFromUtf8(isolate,key.c_str()).ToLocalChecked()
		) ;
	if( maybe1.IsEmpty() )
	{
		return false ;
	}else
	{
		Local<Value> localval = maybe1.ToLocalChecked() ;
		MaybeLocal<Number> maybe2 = localval->ToNumber(context) ;
		if( maybe2.IsEmpty() )
		{
			return false ;
		}else{
			Local<Number> local2=maybe2.ToLocalChecked() ;
			retvalue = local2->Value() ;
			return true ;
		}
	}
}

bool PixelEngine::V8ObjectGetBoolValue(Isolate* isolate,
	Local<Object>&obj,
	Local<Context>&context,
	string key,
	bool& retvalue ){

	MaybeLocal<Value> maybe1 = obj->Get(context,
		String::NewFromUtf8(isolate,key.c_str()).ToLocalChecked()
		) ;
	if( maybe1.IsEmpty() )
	{
		return false ;
	}else
	{
		Local<Value> localval = maybe1.ToLocalChecked() ;
		retvalue = localval->BooleanValue(isolate) ;
		return true ;
	}
}

bool PixelEngine::V8ObjectGetUint8Array(Isolate* isolate,
	Local<Object>&obj,
	Local<Context>&context,
	string key,
	int shouldElementNumber , 
	void* copyToPtr ){

	MaybeLocal<Value> maybe1 = obj->Get(context,
		String::NewFromUtf8(isolate,key.c_str()).ToLocalChecked()
		) ;
	if( maybe1.IsEmpty() ){
		return false ;
	}else
	{
		Local<Value> local1 = maybe1.ToLocalChecked() ;
		if( local1->IsUint8Array() )
		{
			Uint8Array* arr = Uint8Array::Cast(*local1) ;
			if( arr->Length() == shouldElementNumber )
			{
				arr->CopyContents( copyToPtr , shouldElementNumber) ;
				return true ;
			}else
			{
				return false ;
			}
		}else
		{
			return false ;
		}
	}
}


bool PixelEngine::V8ObjectGetUint16Array(Isolate* isolate,
	Local<Object>& obj,
	Local<Context>& context,
	string key,
	int shouldElementNumber,
	void* copyToPtr) {

	MaybeLocal<Value> maybe1 = obj->Get(context,
		String::NewFromUtf8(isolate, key.c_str()).ToLocalChecked()
	);
	if (maybe1.IsEmpty()) {
		return false;
	}
	else
	{
		Local<Value> local1 = maybe1.ToLocalChecked();
		if (local1->IsUint16Array())
		{
			Uint16Array* arr = Uint16Array::Cast(*local1);
			if (arr->Length() == shouldElementNumber)
			{
				arr->CopyContents(copyToPtr, shouldElementNumber*2 );
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
}

bool PixelEngine::V8ObjectGetUint32Array(Isolate* isolate,
	Local<Object>& obj,
	Local<Context>& context,
	string key,
	int shouldElementNumber,
	void* copyToPtr) {

	MaybeLocal<Value> maybe1 = obj->Get(context,
		String::NewFromUtf8(isolate, key.c_str()).ToLocalChecked()
	);
	if (maybe1.IsEmpty()) {
		return false;
	}
	else
	{
		Local<Value> local1 = maybe1.ToLocalChecked();
		if (local1->IsUint32Array())
		{
			Uint32Array* arr = Uint32Array::Cast(*local1);
			if (arr->Length() == shouldElementNumber)
			{
				arr->CopyContents(copyToPtr, shouldElementNumber * 4);
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
}




bool PixelEngine::V8ObjectGetInt32Array(Isolate* isolate,
	Local<Object>&obj,
	Local<Context>&context,
	string key,
	int shouldElementNumber , 
	void* copyToPtr ){

	MaybeLocal<Value> maybe1 = obj->Get(context,
		String::NewFromUtf8(isolate,key.c_str()).ToLocalChecked()
		) ;
	if( maybe1.IsEmpty() ){
		return false ;
	}else
	{
		Local<Value> local1 = maybe1.ToLocalChecked() ;
		if( local1->IsInt32Array() )
		{
			Int32Array* arr = Int32Array::Cast(*local1) ;
			if( arr->Length() == shouldElementNumber )
			{
				arr->CopyContents( copyToPtr , shouldElementNumber*4) ;
				return true ;
			}else
			{
				return false ;
			}
		}else
		{
			return false ;
		}
	}
}

bool PixelEngine::V8ObjectGetInt16Array(Isolate* isolate,
	Local<Object>&obj,
	Local<Context>&context,
	string key,
	int shouldElementNumber , 
	void* copyToPtr ){

	MaybeLocal<Value> maybe1 = obj->Get(context,
		String::NewFromUtf8(isolate,key.c_str()).ToLocalChecked()
		) ;
	if( maybe1.IsEmpty() ){
		return false ;
	}else
	{
		Local<Value> local1 = maybe1.ToLocalChecked() ;
		if( local1->IsInt16Array() )
		{
			Int16Array* arr = Int16Array::Cast(*local1) ;
			if( arr->Length() == shouldElementNumber )
			{
				arr->CopyContents( copyToPtr , shouldElementNumber*2) ;
				return true ;
			}else
			{
				return false ;
			}
		}else
		{
			return false ;
		}
	}
}

bool PixelEngine::V8ObjectGetFloat32Array(Isolate* isolate,
	Local<Object>&obj,
	Local<Context>&context,
	string key,
	int shouldElementNumber , 
	void* copyToPtr ){

	MaybeLocal<Value> maybe1 = obj->Get(context,
		String::NewFromUtf8(isolate,key.c_str()).ToLocalChecked()
		) ;
	if( maybe1.IsEmpty() ){
		return false ;
	}else
	{
		Local<Value> local1 = maybe1.ToLocalChecked() ;
		if( local1->IsFloat32Array() )
		{
			Float32Array* arr = Float32Array::Cast(*local1) ;
			if( arr->Length() == shouldElementNumber )
			{
				arr->CopyContents( copyToPtr , shouldElementNumber*4) ;
				return true ;
			}else
			{
				return false ;
			}
		}else
		{
			return false ;
		}
	}
}


bool PixelEngine::V8ObjectGetFloat64Array(Isolate* isolate,
	Local<Object>& obj,
	Local<Context>& context,
	string key,
	int shouldElementNumber,
	void* copyToPtr) {

	MaybeLocal<Value> maybe1 = obj->Get(context,
		String::NewFromUtf8(isolate, key.c_str()).ToLocalChecked()
	);
	if (maybe1.IsEmpty()) {
		return false;
	}
	else
	{
		Local<Value> local1 = maybe1.ToLocalChecked();
		if (local1->IsFloat64Array())
		{
			Float64Array* arr = Float64Array::Cast(*local1);
			if (arr->Length() == shouldElementNumber)
			{
				arr->CopyContents(copyToPtr, shouldElementNumber * 8);
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
}


bool PixelEngineColorRamp::unwrap(Isolate* isolate , Local<v8::Value> obj)
{
	v8::HandleScope handle_scope(isolate);
	Local<Context> context(isolate->GetCurrentContext()) ;

	Local<Object> crobj  ;
	bool ok00 = obj->ToObject(context).ToLocal( &crobj ) ;
	if( ok00 == false )
	{
		if (!PixelEngine::quietMode)cout<<"PixelEngineColorRamp::unwrap ToObject failed."<<endl;
		return false ;
	}

	// this->useInteger = crobj->Get(context,
	// 	String::NewFromUtf8(isolate,"useInteger").ToLocalChecked())
	// 	.ToLocalChecked()->ToBoolean(isolate)->Value() ;
	bool ok0 = PixelEngine::V8ObjectGetBoolValue(isolate,crobj,context,"useInteger",this->useInteger) ;
	if( ok0==false ){
		if (!PixelEngine::quietMode)cout<<"PixelEngine::V8ObjectGetBoolValue useInteger failed."<<endl;
		return false ;
	}

	// this->numColors = crobj->Get(context,
	// 	String::NewFromUtf8(isolate,"numColors").ToLocalChecked())
	// 	.ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	bool ok1 = PixelEngine::V8ObjectGetIntValue(isolate,crobj,context,"numColors",this->numColors) ;
	if( ok1==false ){
		if (!PixelEngine::quietMode)cout<<"PixelEngine::V8ObjectGetIntValue numColors failed."<<endl;
		return false ;
	}

	// this->Nodata = crobj->Get(context,
	// 	String::NewFromUtf8(isolate,"Nodata").ToLocalChecked())
	// 	.ToLocalChecked()->ToNumber(context).ToLocalChecked()->Value() ;
	bool ok2 = PixelEngine::V8ObjectGetNumberValue(isolate,crobj,context,"Nodata",this->Nodata) ;
	if( ok2==false ){
		if (!PixelEngine::quietMode)cout<<"PixelEngine::V8ObjectGetNumberValue Nodata failed."<<endl;
		return false ;
	}

	//nodatacolor
	// Uint8Array* nodatacolorArray = Uint8Array::Cast(
	// 		* crobj->Get(context,String::NewFromUtf8(isolate,"NodataColor").ToLocalChecked()).ToLocalChecked()
	// 		) ; 
	// nodatacolorArray->CopyContents( this->NodataColor , 4) ;
	bool ok3 = PixelEngine::V8ObjectGetUint8Array(isolate,
			crobj,
			context,
			"NodataColor",
			4 , 
			this->NodataColor ) ;
	if( ok3==false )
	{
		if (!PixelEngine::quietMode)cout<<"PixelEngine::V8ObjectGetUint8Array failed."<<endl;
		return false ;
	}

	//ivalues
	// Int32Array* ivaluesArray = Int32Array::Cast(
	// 		* crobj->Get(context,String::NewFromUtf8(isolate,"ivalues").ToLocalChecked()).ToLocalChecked()
	// 		) ; 
	// ivaluesArray->CopyContents( this->ivalues , PixelEngineColorRamp::MAXNUM_COLORS*4 ) ;
	ok3 = PixelEngine::V8ObjectGetInt32Array(isolate,
			crobj,
			context,
			"ivalues",
			PixelEngineColorRamp::MAXNUM_COLORS , 
			this->ivalues ) ;
	if( ok3==false )
	{
		if (!PixelEngine::quietMode)cout<<"PixelEngine::V8ObjectGetInt32Array ivalues failed."<<endl;
		return false ;
	}


	// //fvalues
	// Float32Array* fvaluesArray = Float32Array::Cast(
	// 		* crobj->Get(context,String::NewFromUtf8(isolate,"fvalues").ToLocalChecked()).ToLocalChecked()
	// 		) ; 
	// fvaluesArray->CopyContents( this->fvalues , PixelEngineColorRamp::MAXNUM_COLORS*4 ) ;
	ok3 = PixelEngine::V8ObjectGetFloat32Array(isolate,
			crobj,
			context,
			"fvalues",
			PixelEngineColorRamp::MAXNUM_COLORS  , 
			this->fvalues ) ;
	if( ok3==false )
	{
		if (!PixelEngine::quietMode)cout<<"PixelEngine::V8ObjectGetFloat32Array fvalues failed."<<endl;
		return false ;
	}


	//red
	// Uint8Array* rArray = Uint8Array::Cast(
	// 		* crobj->Get(context,String::NewFromUtf8(isolate,"r").ToLocalChecked()).ToLocalChecked()
	// 		) ; 
	// rArray->CopyContents( this->r , PixelEngineColorRamp::MAXNUM_COLORS) ;
	ok3 = PixelEngine::V8ObjectGetUint8Array(isolate,
			crobj,
			context,
			"r",
			PixelEngineColorRamp::MAXNUM_COLORS , 
			this->r ) ;
	if( ok3==false )
	{
		if (!PixelEngine::quietMode)cout<<"PixelEngine::V8ObjectGetUint8Array r failed."<<endl;
		return false ;
	}

	//green
	ok3 = PixelEngine::V8ObjectGetUint8Array(isolate,
			crobj,
			context,
			"g",
			PixelEngineColorRamp::MAXNUM_COLORS , 
			this->g ) ;
	if( ok3==false )
	{
		if (!PixelEngine::quietMode)cout<<"PixelEngine::V8ObjectGetUint8Array g failed."<<endl;
		return false ;
	}

	//blue
	// Uint8Array* bArray = Uint8Array::Cast(
	// 		* crobj->Get(context,String::NewFromUtf8(isolate,"b").ToLocalChecked()).ToLocalChecked()
	// 		) ; 
	// bArray->CopyContents( this->b , PixelEngineColorRamp::MAXNUM_COLORS) ;
	ok3 = PixelEngine::V8ObjectGetUint8Array(isolate,
			crobj,
			context,
			"b",
			PixelEngineColorRamp::MAXNUM_COLORS , 
			this->b ) ;
	if( ok3==false )
	{
		if (!PixelEngine::quietMode)cout<<"PixelEngine::V8ObjectGetUint8Array b failed."<<endl;
		return false ;
	}

	//alpha
	// Uint8Array* aArray = Uint8Array::Cast(
	// 		* crobj->Get(context,String::NewFromUtf8(isolate,"a").ToLocalChecked()).ToLocalChecked()
	// 		) ; 
	// aArray->CopyContents( this->a , PixelEngineColorRamp::MAXNUM_COLORS) ;
	// return true ;
	bool oka = PixelEngine::V8ObjectGetUint8Array(isolate,
			crobj,
			context,
			"a",
			PixelEngineColorRamp::MAXNUM_COLORS , 
			this->a ) ;
	if( oka==false )
	{
		if(! PixelEngine::quietMode)cout<<"PixelEngine::V8ObjectGetUint8Array a failed."<<endl; 
		return false ;
	}
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
	Maybe<bool> ok1 = crobj->Set(context , 
		String::NewFromUtf8(isolate,"useInteger").ToLocalChecked() , 
		Boolean::New(isolate,true)
		) ;

	//set color number
	Maybe<bool> ok2 = crobj->Set(context , 
		String::NewFromUtf8(isolate,"numColors").ToLocalChecked() , 
		Integer::New(isolate,this->numColors)
		) ;
	//set nodata
	Maybe<bool> ok3 = crobj->Set(context,
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

bool PixelEngine::IsMaybeLocalOK(MaybeLocal<Value>& val) 
{
	if( val.IsEmpty() )
	{
		return false ;
	}else
	{
		Local<Value> val2 = val.ToLocalChecked() ;
		if( val2->IsNull() )
		{
			return false ;
		}else if( val2->IsUndefined() )
		{
			return false ;
		}
		return true ;
	}
}

long PixelEngine::RelativeDatetimeConvert(long currdt,long seconds) 
{
	time_t rawtime ;
	time(&rawtime) ;
	struct tm* timeinfo = localtime(&rawtime) ;

	int hms = currdt%1000000L ;
	timeinfo->tm_hour = hms / 10000;
	timeinfo->tm_min = hms/100 - timeinfo->tm_hour*100  ;
	timeinfo->tm_sec = hms%100 ;

	int ymd = currdt/1000000L ;
	timeinfo->tm_year = ymd/10000 ;
	timeinfo->tm_mon = ymd/100 - 100*timeinfo->tm_year - 1; // tm_mon is [0,11]
	timeinfo->tm_mday = ymd%100 ;

	time_t time0 = mktime(timeinfo) ;
	time_t time1 = time0 + seconds ;

	struct tm* timeinfo1 = localtime(&time1) ;

	long newdt = timeinfo1->tm_year*10000000000L
	+(timeinfo1->tm_mon+1) * 100000000 
	+timeinfo1->tm_mday * 1000000  //tm_mday must -1, i don't why.
	+timeinfo1->tm_hour * 10000 
	+timeinfo1->tm_min * 100 
	+timeinfo1->tm_sec ;
	return newdt ;

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

void PixelEngine::log(string& str) 
{
	if( this->pe_logs.length() > 1024 )
	{
		if(! PixelEngine::quietMode)cout<<"log size exceed 1024."<<endl ;
		this->pe_logs += string("...\n") ;
	}else
	{
		this->pe_logs += str+string("\n") ;
	}
}

/// pe.log(...)
void PixelEngine::GlobalFunc_Log(const v8::FunctionCallbackInfo<v8::Value>& args) 
{
	if (args.Length() < 1) return;
	Isolate* isolate = args.GetIsolate();
	HandleScope scope(isolate);
	Local<Context> context(isolate->GetCurrentContext()) ;

	Local<Value> arg = args[0];
	String::Utf8Value value(isolate, arg);
	if(! PixelEngine::quietMode)cout<<"log: "<< *value <<endl;//debug use

	Local<Object> global = context->Global() ;
	Local<Value> peinfo = global->Get( context
		,String::NewFromUtf8(isolate, "PixelEnginePointer").ToLocalChecked())
		.ToLocalChecked() ;
	Object* peinfoObj = Object::Cast( *peinfo ) ;
	Local<Value> thisPePtrValue = peinfoObj->GetInternalField(0) ;
	External* thisPePtrEx = External::Cast(*thisPePtrValue);
	PixelEngine* thisPePtr = static_cast<PixelEngine*>(thisPePtrEx->Value() );
	if( thisPePtr != nullptr )
	{
		string str1(*value) ;
		thisPePtr->log(str1) ;
	}
}

/// use c++ create a new empty Dataset object
Local<Object> PixelEngine::CPP_NewDataset(Isolate* isolate,Local<Context>& context
	,const int datatype 
	,const int width 
	,const int height
	,const int nband )
{
	if(! PixelEngine::quietMode)cout<<"inside CPP_NewDataset"<<endl; 
	v8::EscapableHandleScope handle_scope(isolate);

	Local<Object> global = context->Global() ;

	Local<Value> forEachFuncInJs = global->Get(context 
    	,String::NewFromUtf8(isolate, "globalFunc_forEachPixelCallBack").ToLocalChecked() ).ToLocalChecked() ;
	Local<Object> ds = Object::New(isolate) ;
	Maybe<bool> ok1 = ds->Set(context
		,String::NewFromUtf8(isolate, "forEachPixel").ToLocalChecked(),
            forEachFuncInJs );


	Local<Value> getPixelFuncInJs = global->Get(context 
    	,String::NewFromUtf8(isolate, "globalFunc_getPixelCallBack").ToLocalChecked() ).ToLocalChecked() ;	
	Maybe<bool> ok2 = ds->Set(context
		,String::NewFromUtf8(isolate, "getPixel").ToLocalChecked(),
            getPixelFuncInJs );

	Maybe<bool> ok3 = ds->Set(context
		,String::NewFromUtf8(isolate, "renderGray").ToLocalChecked(),
            FunctionTemplate::New(isolate, PixelEngine::GlobalFunc_RenderGrayCallBack)->GetFunction(context).ToLocalChecked() );
	Maybe<bool> ok4 = ds->Set(context
		,String::NewFromUtf8(isolate, "renderPsuedColor").ToLocalChecked(),
            FunctionTemplate::New(isolate, PixelEngine::GlobalFunc_RenderPsuedColorCallBack)->GetFunction(context).ToLocalChecked() );
	Maybe<bool> ok5 = ds->Set(context
		,String::NewFromUtf8(isolate, "fillRange").ToLocalChecked(),
            FunctionTemplate::New(isolate, PixelEngine::GlobalFunc_FillRangeCallBack)->GetFunction(context).ToLocalChecked() );
	Maybe<bool> ok6 = ds->Set(context
		,String::NewFromUtf8(isolate, "renderRGB").ToLocalChecked(),
            FunctionTemplate::New(isolate, PixelEngine::GlobalFunc_RenderRGBCallBack)->GetFunction(context).ToLocalChecked() );
	

	Maybe<bool> ok7 = ds->Set(context
		,String::NewFromUtf8(isolate, "width").ToLocalChecked()
		,Integer::New(isolate,width) ) ;
	Maybe<bool> ok8 = ds->Set(context
		,String::NewFromUtf8(isolate, "height").ToLocalChecked()
		,Integer::New(isolate,height) ) ;
	Maybe<bool> ok9 = ds->Set(context
		,String::NewFromUtf8(isolate, "nband").ToLocalChecked()
		,Integer::New(isolate,nband) ) ;
	Maybe<bool> ok10 = ds->Set(context
		,String::NewFromUtf8(isolate, "dataType").ToLocalChecked()
		,Integer::New(isolate,datatype) ) ;

	//522
	Local<ArrayBuffer> neighborLoadedAB = ArrayBuffer::New(isolate,9) ;
	Local<Uint8Array> neighborLoadedU8 = Uint8Array::New(neighborLoadedAB,0,9) ;
	Maybe<bool> ok11 = ds->Set(context
		,String::NewFromUtf8(isolate, "nbloads").ToLocalChecked()
		,neighborLoadedU8 ) ;//4 is current always 0.
	Maybe<bool> ok12 = ds->Set(context
		,String::NewFromUtf8(isolate, "nbdatas").ToLocalChecked()
		,Array::New(isolate,9) ) ;// 4 is current always null.
	Maybe<bool> ok13 = ds->Set(context
		,String::NewFromUtf8(isolate, "dsName").ToLocalChecked()
		,String::Empty(isolate) ) ;
	Maybe<bool> ok14 = ds->Set(context
		,String::NewFromUtf8(isolate, "dsDt").ToLocalChecked()
		,Number::New(isolate,0) ) ;//long
	Maybe<bool> ok15 = ds->Set(context
		,String::NewFromUtf8(isolate, "x").ToLocalChecked()
		,Integer::New(isolate,0) ) ;
	Maybe<bool> ok16 = ds->Set(context
		,String::NewFromUtf8(isolate, "y").ToLocalChecked()
		,Integer::New(isolate,0) ) ;
	Maybe<bool> ok17 = ds->Set(context
		,String::NewFromUtf8(isolate, "z").ToLocalChecked()
		,Integer::New(isolate,0) ) ;

	if( datatype == 3 )
	{//short
		int bsize = width*height*nband*2 ;
		Local<ArrayBuffer> arrbuff = ArrayBuffer::New(isolate,bsize) ;
		Local<Int16Array> i16array = Int16Array::New(arrbuff,0,bsize/2) ;
		Maybe<bool> ok18 = ds->Set(context
			,String::NewFromUtf8(isolate, "tiledata").ToLocalChecked()
			,i16array ) ;
	}else
	{//byte
		int bsize = width*height*nband ;
		Local<ArrayBuffer> arrbuff = ArrayBuffer::New(isolate,bsize) ;
		Local<Uint8Array> u8array = Uint8Array::New(arrbuff,0,bsize) ;
		Maybe<bool> ok19 = ds->Set(context
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
	if(! PixelEngine::quietMode)cout<<"inside CPP_NewDatasetArray"<<endl; 
	v8::EscapableHandleScope handle_scope(isolate);

	Local<Object> global = context->Global() ;

	Local<Value> forEachFuncInJs = global->Get(context 
    	,String::NewFromUtf8(isolate, "globalFunc_DA_forEachPixelCallBack").ToLocalChecked() ).ToLocalChecked() ;
	Local<Object> ds = Object::New(isolate) ;
	Maybe<bool> ok1 = ds->Set(context
		,String::NewFromUtf8(isolate, "forEachPixel").ToLocalChecked(),
            forEachFuncInJs );

	Maybe<bool> ok2 = ds->Set(context
		,String::NewFromUtf8(isolate, "width").ToLocalChecked()
		,Integer::New(isolate,width) ) ;
	Maybe<bool> ok3 = ds->Set(context
		,String::NewFromUtf8(isolate, "height").ToLocalChecked()
		,Integer::New(isolate,height) ) ;
	Maybe<bool> ok4 = ds->Set(context
		,String::NewFromUtf8(isolate, "nband").ToLocalChecked()
		,Integer::New(isolate,nband) ) ;
	Maybe<bool> ok5 = ds->Set(context
		,String::NewFromUtf8(isolate, "dataType").ToLocalChecked()
		,Integer::New(isolate,datatype) ) ;
	Maybe<bool> ok6 = ds->Set(context
		,String::NewFromUtf8(isolate, "numds").ToLocalChecked()
		,Integer::New(isolate,numds) ) ;
	Maybe<bool> ok7 = ds->Set(context
		,String::NewFromUtf8(isolate, "dsName").ToLocalChecked()
		,String::Empty(isolate) ) ;
	Maybe<bool> ok8 = ds->Set(context
		,String::NewFromUtf8(isolate, "fromdt").ToLocalChecked()
		,String::Empty(isolate) ) ;
	Maybe<bool> ok9 = ds->Set(context
		,String::NewFromUtf8(isolate, "todt").ToLocalChecked()
		,String::Empty(isolate) ) ;
	Maybe<bool> ok10 = ds->Set(context
		,String::NewFromUtf8(isolate, "x").ToLocalChecked()
		,Integer::New(isolate,0) ) ;
	Maybe<bool> ok11 = ds->Set(context
		,String::NewFromUtf8(isolate, "y").ToLocalChecked()
		,Integer::New(isolate,0) ) ;
	Maybe<bool> ok12 = ds->Set(context
		,String::NewFromUtf8(isolate, "z").ToLocalChecked()
		,Integer::New(isolate,0) ) ;


	Local<ArrayBuffer> timeArrayAB = ArrayBuffer::New(isolate,numds*8) ;
	Local<BigInt64Array> timeArrayInt64 = BigInt64Array::New(timeArrayAB,0,numds) ;
	Maybe<bool> ok13 = ds->Set(context
		,String::NewFromUtf8(isolate, "timeArr").ToLocalChecked()
		,timeArrayInt64 ) ;
	Maybe<bool> ok14 = ds->Set(context
		,String::NewFromUtf8(isolate, "dataArr").ToLocalChecked()
		,Array::New(isolate,numds) ) ;// 4 is current always null.
	
	return handle_scope.Escape(ds);
}



/// dataset.renderGray() , return a new Dataset
/// iband,vmin,vmax,nodata,nodataColor
void PixelEngine::GlobalFunc_RenderGrayCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) 
{
	if(! PixelEngine::quietMode)cout<<"inside GlobalFunc_RenderGrayCallBack"<<endl; 
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



PixelEngine* PixelEngine::getPixelEnginePointer(const v8::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();
	HandleScope scope(isolate);
	Local<Context> context(isolate->GetCurrentContext());
	Local<Object> global = context->Global();
	Local<Value> peinfo = global->Get(context
		, String::NewFromUtf8(isolate, "PixelEnginePointer").ToLocalChecked())
		.ToLocalChecked();
	Object* peinfoObj = Object::Cast(*peinfo);
	Local<Value> thisPePtrValue = peinfoObj->GetInternalField(0);
	External* thisPePtrEx = External::Cast(*thisPePtrValue);
	PixelEngine* thisPePtr = static_cast<PixelEngine*>(thisPePtrEx->Value());
	return thisPePtr;
}


/// convert v8 local value to cpp string
string PixelEngine::convertV8LocalValue2CppString(Isolate* isolate, Local<Value>& v8value) {
	String::Utf8Value value(isolate, v8value);
	return string(*value);
}

/// ÓÃÓÚ°ÑC++µÄPeStyle¶ÔÏó´ò°ü³Év8¶ÔÏó
Local<Value> PixelEngine::warpCppStyle2V8Object(Isolate* isolate, PeStyle& style  ) {
	EscapableHandleScope scope(isolate);
	Local<Context> context(isolate->GetCurrentContext());

	string stylejson = style.toJson();
	Local<String> v8str = String::NewFromUtf8(isolate, stylejson.c_str() ).ToLocalChecked();
	MaybeLocal<Value> v8style = JSON::Parse(context, v8str);
	Local<Value> v8stylelocal;
	if (v8style.ToLocal( &v8stylelocal)) {
		return scope.Escape(v8stylelocal);
	}
	else
	{
		if(! PixelEngine::quietMode)cout << "Error : failed to parse v8style, return null." << endl;
	}
	return scope.Escape(Null(isolate));
}

/// get render style from system by styleid
/// params styleid
void PixelEngine::GlobalFunc_GetStyleCallBack(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	cout << "inside GlobalFunc_GetStyleCallBack" << endl;
	Isolate* isolate = args.GetIsolate();
	if (args.Length() != 1) {
		if(! PixelEngine::quietMode)cout << "Error: args.Length != 1 " << endl;
		return;
	}
	v8::HandleScope handle_scope(isolate);

	Local<Value> arg0 = args[0];
	string styleid = convertV8LocalValue2CppString(isolate, arg0);
	PixelEngine* peptr = PixelEngine::getPixelEnginePointer(args);
	if (peptr) {
		if (peptr->helperPointer) {

			PeStyle retstyle;
			string errorText;
			bool styleok = peptr->helperPointer->getStyle(styleid, retstyle,errorText );
			if (styleok) {
				Local<Value> styleObj = PixelEngine::warpCppStyle2V8Object(isolate , retstyle);
				args.GetReturnValue().Set(styleObj);
			}
		}
	}


}



/// dataset.renderRGB , return a new Dataset
/// ri,gi,bi,rmin,rmax,gmin,gmax,bmin,bmax
void PixelEngine::GlobalFunc_RenderRGBCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) 
{
	if(! PixelEngine::quietMode)cout<<"inside GlobalFunc_RenderRGBCallBack"<<endl; 
	Isolate* isolate = args.GetIsolate() ;
	if (args.Length() != 9 ){
		if(! PixelEngine::quietMode)cout<<"Error: args.Length != 9 "<<endl ;
		return;
	}
	
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
	//if(! PixelEngine::quietMode)cout<<"thisDataType "<<thisDataType<<endl;

	int width = thisobj->Get(context,
		String::NewFromUtf8(isolate,"width").ToLocalChecked())
		.ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	//if(! PixelEngine::quietMode)cout<<"thisDataType "<<thisDataType<<endl;

	int height = thisobj->Get(context,
		String::NewFromUtf8(isolate,"height").ToLocalChecked())
		.ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	//if(! PixelEngine::quietMode)cout<<"thisDataType "<<thisDataType<<endl;

	int nband = thisobj->Get(context,
		String::NewFromUtf8(isolate,"nband").ToLocalChecked())
		.ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	//if(! PixelEngine::quietMode)cout<<"thisDataType "<<thisDataType<<endl;

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
	if(! PixelEngine::quietMode)cout<<"inside GlobalFunc_FillRangeCallBack"<<endl; 
	if (args.Length() != 3 ){
		if(! PixelEngine::quietMode)cout<<"Error: args.Length !=2 "<<endl ;
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
	//if(! PixelEngine::quietMode)cout<<"thisDataType "<<thisDataType<<endl;

	int width = thisobj->Get(context,
		String::NewFromUtf8(isolate,"width").ToLocalChecked())
		.ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	//if(! PixelEngine::quietMode)cout<<"thisDataType "<<thisDataType<<endl;

	int height = thisobj->Get(context,
		String::NewFromUtf8(isolate,"height").ToLocalChecked())
		.ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	//if(! PixelEngine::quietMode)cout<<"thisDataType "<<thisDataType<<endl;

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
	if(! PixelEngine::quietMode)cout<<"inside GlobalFunc_RenderPsuedColorCallBack"<<endl; 
	if (args.Length() != 8 && args.Length() != 3){
		if(! PixelEngine::quietMode)cout<<"Error: args.Length != 8 and != 3 "<<endl ;
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
		bool unwrapok = colorRamp.unwrap(isolate , v8_colorramp ) ;
		if( unwrapok==false ){
			if(! PixelEngine::quietMode)cout<<"Error: colorRamp.unwrap failed.";
			args.GetReturnValue().SetNull() ;
			return ;
		}
	}

	Local<Object> thisobj =  args.This() ;
	int thisDataType = thisobj->Get(context,
		String::NewFromUtf8(isolate,"dataType").ToLocalChecked())
		.ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	//if(! PixelEngine::quietMode)cout<<"thisDataType "<<thisDataType<<endl;

	int width = thisobj->Get(context,
		String::NewFromUtf8(isolate,"width").ToLocalChecked())
		.ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	//if(! PixelEngine::quietMode)cout<<"thisDataType "<<thisDataType<<endl;

	int height = thisobj->Get(context,
		String::NewFromUtf8(isolate,"height").ToLocalChecked())
		.ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	//if(! PixelEngine::quietMode)cout<<"thisDataType "<<thisDataType<<endl;

	int nband = thisobj->Get(context,
		String::NewFromUtf8(isolate,"nband").ToLocalChecked())
		.ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	//if(! PixelEngine::quietMode)cout<<"thisDataType "<<thisDataType<<endl;

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
	if(! PixelEngine::quietMode)cout<<"inside GlobalFunc_NewDatasetCallBack"<<endl; 
	if (args.Length() != 4 ){
		if(! PixelEngine::quietMode)cout<<"Error: args.Length != 4 "<<endl ;
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
	if(! PixelEngine::quietMode)cout<<"inside GlobalFunc_DatasetCallBack"<<endl; 
	if(args.Length() == 3 || args.Length() == 6 )
	{
		//ok
	}else
	{
		if(! PixelEngine::quietMode)cout<<"Error: args.Length != 3 or !=6 "<<endl ;
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

	int64_t datetime = (int64_t) (v8datetime->ToNumber(context).ToLocalChecked())->Value();

	if(! PixelEngine::quietMode)cout<<name<<","<<datetime<<endl ;

	if( v8bands->IsArray() )
	{
		if(! PixelEngine::quietMode)cout<<"v8bands is array"<<endl ;
	}
	if( v8bands->IsObject() )
	{
		if(! PixelEngine::quietMode)cout<<"v8bands is object"<<endl ;
	}
	Array* i32array = Array::Cast(*v8bands) ;
	int nband = i32array->Length() ;

	if(! PixelEngine::quietMode)cout<<"nband "<<nband<<endl ;

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

	int tilez = thisPePtr->tileInfo.z  ;
	int tiley = thisPePtr->tileInfo.y  ; 
	int tilex = thisPePtr->tileInfo.x  ; 
	if( args.Length()==6 )
	{
		tilez = (int) (args[3]->ToNumber(context).ToLocalChecked())->Value();
		tiley = (int) (args[4]->ToNumber(context).ToLocalChecked())->Value();
		tilex = (int) (args[5]->ToNumber(context).ToLocalChecked())->Value();
	}
	
	if( datetime == PE_CURRENTDATETIME )
	{
		//datetime = PixelEngine::long2str(thisPePtr->currentDateTime) ;
		datetime = thisPePtr->currentDateTime ;
		if(! PixelEngine::quietMode)cout<<"use current "<<datetime<<endl ;
	}else if( datetime < 0 )
	{
		datetime = PixelEngine::RelativeDatetimeConvert(thisPePtr->currentDateTime , datetime );
		if(! PixelEngine::quietMode)cout<<"use passed "<<datetime<<endl ;
	}

	string datetimestr = PixelEngine::long2str(datetime) ;

	if (PixelEngine::GetExternalTileDataCallBack != nullptr) {
		bool externalOk = PixelEngine::GetExternalTileDataCallBack(
			thisPePtrEx->Value(),// pointer to PixelEngine Object.
			name, datetimestr, wantBands,
			tilez,
			tiley,
			tilex,
			externalData,
			dt,
			wid,
			hei,
			retnbands);
		if (externalOk == false)
		{
			if(! PixelEngine::quietMode)cout << "Error: PixelEngine::GetExternalTileDataCallBack failed." << endl;
			return;//return null in javascript.
		}
	}
	else if (thisPePtr->helperPointer != nullptr) {
		string errorText;
		bool dataok = thisPePtr->helperPointer->getTileData(
			datetime,
			name,
			wantBands,
			tilez, tiley, tilex,
			externalData,
			dt,
			wid,
			hei,
			retnbands,
			errorText
		);
		if (dataok==false) {
			if(! PixelEngine::quietMode)cout << "Error: PixelEngine::helperPointer getTileData failed." << endl;
			return;//return null in javascript.
		}
	}
	else
	{
		if(! PixelEngine::quietMode)cout << "Error: PixelEngine::GetExternalTileDataCallBack is null and helper is null too." << endl;
		return;//return null in javascript.
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

	Maybe<bool> ok1 = ds->Set(context
		,String::NewFromUtf8(isolate, "dsName").ToLocalChecked()
		,v8name ) ;
	Maybe<bool> ok2 = ds->Set(context
		,String::NewFromUtf8(isolate, "dsDt").ToLocalChecked()
		,v8datetime ) ;
	Maybe<bool> ok3 = ds->Set(context
		,String::NewFromUtf8(isolate, "x").ToLocalChecked()
		,Integer::New(isolate,thisPePtr->tileInfo.x) ) ;
	Maybe<bool> ok4 = ds->Set(context
		,String::NewFromUtf8(isolate, "y").ToLocalChecked()
		,Integer::New(isolate,thisPePtr->tileInfo.y) ) ;
	Maybe<bool> ok5 = ds->Set(context
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
	if(! PixelEngine::quietMode)cout<<"inside GlobalFunc_DatasetArrayCallBack"<<endl; 
	if (args.Length() < 4 ){
		if(! PixelEngine::quietMode)cout<<"Error: args.Length < 4 "<<endl ;
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

	int64_t fromdatetime =(int64_t) (v8from->ToNumber(context).ToLocalChecked())->Value();
	int64_t todatetime = (int64_t) (v8to->ToNumber(context).ToLocalChecked())->Value();

	if(! PixelEngine::quietMode)cout<<"from to :"<<fromdatetime<<"-"<<todatetime<<endl ;

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
	if( fromdatetime == PE_CURRENTDATETIME ) 
	{
		if(! PixelEngine::quietMode)cout<<"fromdatetime use current "<<thisPePtr->currentDateTime<<endl ;
		fromdatetime = thisPePtr->currentDateTime  ;
	}else if( fromdatetime<0 )
	{
		fromdatetime =  PixelEngine::RelativeDatetimeConvert(thisPePtr->currentDateTime , fromdatetime ); 
		if(! PixelEngine::quietMode)cout<<"fromdatetime use passed "<<fromdatetime<<endl ;
	}

	if( todatetime == PE_CURRENTDATETIME )
	{
		if(! PixelEngine::quietMode)cout<<"todatetime use current "<<thisPePtr->currentDateTime<<endl ;
		todatetime =  thisPePtr->currentDateTime  ;
	}else if( todatetime < 0 )
	{
		todatetime = PixelEngine::RelativeDatetimeConvert(thisPePtr->currentDateTime , todatetime ); 
		if(! PixelEngine::quietMode)cout<<"todatetime use passed "<<todatetime<<endl ;
	}

	string fromdatetimestr = PixelEngine::long2str(fromdatetime) ;
	string todatetimestr = PixelEngine::long2str(todatetime) ;

	if (PixelEngine::GetExternalTileDataArrCallBack != nullptr ) {
		bool externalOk = PixelEngine::GetExternalTileDataArrCallBack(
			thisPePtrEx->Value(),// pointer to PixelEngine Object.
			name, fromdatetimestr, todatetimestr, wantBands,
			tilez,
			tiley,
			tilex,
			filtermon,
			filterday,
			filterhour,
			filterminu,
			filtersec,
			externalDataArr,
			externalTimeArr,
			dt,
			wid,
			hei,
			retnbands,
			retnumds);
		if (externalOk == false)
		{
			if(! PixelEngine::quietMode)cout << "Error: PixelEngine::GetExternalTileDataArrCallBack failed." << endl;
			return;//return null in javascript.
		}
	}
	else if (thisPePtr->helperPointer != nullptr ) {
		vector<int64_t> retDtVec;
		string errorText;
		bool dataok = thisPePtr->helperPointer->getTileDataArray(
			fromdatetime, todatetime,
			name,
			wantBands,
			tilez, tiley, tilex,
			filtermon, filterday, filterhour, filterminu, filtersec,
			externalDataArr,
			retDtVec,
			dt, wid, hei, retnbands, errorText);
		if (dataok==false )
		{
			if(! PixelEngine::quietMode)cout << "Error: PixelEngine::helper get tile data array failed." << endl;
			return;//return null in javascript.
		}
		retnumds = retDtVec.size();
		for (int idt = 0; idt < retDtVec.size(); ++idt) {
			externalTimeArr.push_back((long)retDtVec[idt]);
		}
	}
	else
	{
		if(! PixelEngine::quietMode)cout << "Error: PixelEngine::GetExternalTileDataArrCallBack is null and helper is null too." << endl;
		return;//return null in javascript.
	}
	

	Local<Object> ds = PixelEngine::CPP_NewDatasetArray( isolate
		,context
		,dt
		,wid
		,hei
		,retnbands
		,retnumds );
	Maybe<bool> ok1 = ds->Set(context
		,String::NewFromUtf8(isolate, "dsName").ToLocalChecked()
		,v8name ) ;
	Maybe<bool> ok2 = ds->Set(context
		,String::NewFromUtf8(isolate, "fromdt").ToLocalChecked()
		,v8from ) ;
	Maybe<bool> ok3 = ds->Set(context
		,String::NewFromUtf8(isolate, "todt").ToLocalChecked()
		,v8to ) ;

	Maybe<bool> ok4 = ds->Set(context
		,String::NewFromUtf8(isolate, "x").ToLocalChecked()
		,Integer::New(isolate,thisPePtr->tileInfo.x) ) ;
	Maybe<bool> ok5 = ds->Set(context
		,String::NewFromUtf8(isolate, "y").ToLocalChecked()
		,Integer::New(isolate,thisPePtr->tileInfo.y) ) ;
	Maybe<bool> ok6 = ds->Set(context
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
		Maybe<bool> ok5 = timeArrInt64->Set(context,ids,BigInt::New(isolate,externalTimeArr[ids]));

		//set data
		vector<unsigned char>& indatavec = externalDataArr[ids] ;
		if( dt == 1 )
		{
			Local<ArrayBuffer> arrbuf = ArrayBuffer::New(isolate,indatavec.size()) ;
			Local<Uint8Array> u8arr = Uint8Array::New(arrbuf,0,indatavec.size()) ;
			unsigned char* backData = (unsigned char*) u8arr->Buffer()->GetBackingStore()->Data() ;//here
			memcpy(backData , indatavec.data(), indatavec.size() );
			Maybe<bool> ok6 = dataArrArray->Set(context,ids,u8arr) ;
		}else if( dt==3 )
		{
			Local<ArrayBuffer> arrbuf = ArrayBuffer::New(isolate,indatavec.size()) ;
			Local<Int16Array> i16arr = Int16Array::New(arrbuf,0,indatavec.size()/2) ;
			short* backData = (short*) i16arr->Buffer()->GetBackingStore()->Data() ;//here
			memcpy(backData , indatavec.data(), indatavec.size() );
			Maybe<bool> ok7 = dataArrArray->Set(context,ids,i16arr) ;
		}else
		{
			printf("Error: unsupported data type:%d\n" , dt) ;
		}
		
	}
	//info.GetReturnValue().Set(i16arr);
	args.GetReturnValue().Set(ds) ;
}

/// ¸Ã·½·¨ÓÃÔÚ forEachPixelÀïÃæ×ö´°¿Ú¼ÆËã£¬·ÃÎÊÁÙ½üÍßÆ¬Êý¾Ý
/// get external tile data only, return data array not dataset.
/// PixelEngine.GetTileData(name,datetime,z,y,x)
void PixelEngine::GlobalFunc_GetTileDataCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) 
{
	if(! PixelEngine::quietMode)cout<<"inside GlobalFunc_GetTileDataCallBack"<<endl; 
	if (args.Length() != 5 ){
		if(! PixelEngine::quietMode)cout<<"Error: args.Length != 5 "<<endl ;
		return;
	}

	Isolate* isolate = args.GetIsolate() ;
	v8::HandleScope handle_scope(isolate);
	Local<Context> context(isolate->GetCurrentContext()) ;

	Local<Value> v8name = args[0];
	Local<Value> v8datetime = args[1] ;

	String::Utf8Value nameutf8( isolate , v8name) ;
	string name( *nameutf8 ) ;

	int64_t datetime =  (int64_t)(v8datetime->ToNumber(context).ToLocalChecked())->Value();

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

	if( datetime == PE_CURRENTDATETIME )
	{
		if(! PixelEngine::quietMode)cout<<"use current "<<thisPePtr->currentDateTime<<endl ;
		datetime =  thisPePtr->currentDateTime  ;
	}else if( datetime < 0 )
	{
		datetime = PixelEngine::RelativeDatetimeConvert(thisPePtr->currentDateTime , datetime ); 
		if(! PixelEngine::quietMode)cout<<"use passed "<<datetime<<endl ;
	}
	string datetimestr = PixelEngine::long2str(datetime) ;

	if (PixelEngine::GetExternalTileDataCallBack != nullptr)
	{
		bool externalOk = PixelEngine::GetExternalTileDataCallBack(
			thisPePtrEx->Value(),// pointer to PixelEngine Object.
			name, datetimestr, wantBands,
			tilez,
			tiley,
			tilex,
			externalData,
			dt,
			wid,
			hei,
			nband);
		if (externalOk == false)
		{
			if(! PixelEngine::quietMode)cout << "Error: PixelEngine::GlobalFunc_GetTileDataCallBack failed." << endl;
			return;//return null in javascript.
		}
	}
	else if (thisPePtr->helperPointer != nullptr) {
		string errorText;
		bool dataok = thisPePtr->helperPointer->getTileData(
			datetime,
			name,
			wantBands,
			tilez, tiley, tilex,
			externalData,
			dt,
			wid,
			hei,
			nband,
			errorText
		);
		if (dataok==false) {
			if(! PixelEngine::quietMode)cout << "Error: PixelEngine::helperPointer getTileData failed." << endl;
			return;//return null in javascript.
		}
	}
	else
	{
		if(! PixelEngine::quietMode)cout << "Error: PixelEngine::GetExternalTileDataCallBack is null and helper is null too." << endl;
		return;//return null in javascript.
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
	if(! PixelEngine::quietMode)cout<<"inside GlobalFunc_ColorRampCallBack"<<endl; 
	if (args.Length() != 2 ){
		if(! PixelEngine::quietMode)cout<<"Error: args.Length != 2 "<<endl ;
		return;
	}

	Isolate* isolate = args.GetIsolate() ;
	v8::HandleScope handle_scope(isolate);
	Local<Context> context(isolate->GetCurrentContext()) ;

	Local<Value> colorid0 = args[0];
	String::Utf8Value colorid1( isolate , colorid0) ;
	string strColorid( *colorid1 ) ;

	Local<Value> v8_crObj = args[1] ;

	Local<Object> global = context->Global();
	Local<Value> peinfo = global->Get(context
		, String::NewFromUtf8(isolate, "PixelEnginePointer").ToLocalChecked())
		.ToLocalChecked();
	Object* peinfoObj = Object::Cast(*peinfo);
	Local<Value> thisPePtrValue = peinfoObj->GetInternalField(0);
	External* thisPePtrEx = External::Cast(*thisPePtrValue);
	PixelEngine* thisPePtr = static_cast<PixelEngine*>(thisPePtrEx->Value());

	if( PixelEngine::GetExternalColorRampCallBack != nullptr )
	{
		PixelEngineColorRamp cr = PixelEngine::GetExternalColorRampCallBack(thisPePtrEx->Value(),strColorid) ;
		cr.copy2v8( isolate , v8_crObj) ;
		args.GetReturnValue().Set(v8_crObj) ;
	}
	else if (thisPePtr->helperPointer != nullptr) {
		PixelEngineColorRamp cr;
		string errorText;
		bool crok = thisPePtr->helperPointer->getColorRamp(strColorid, cr, errorText);
		if (crok == false) {
			if(! PixelEngine::quietMode)cout << "Error: PixelEngine::helperPointer get color ramp failed ." << endl;
			args.GetReturnValue().Set(v8_crObj);
		}
		else {
			cr.copy2v8(isolate, v8_crObj);
			args.GetReturnValue().Set(v8_crObj);
		}
	}
	else
	{
		if(! PixelEngine::quietMode)cout<<"Error: PixelEngine::GetExternalColorRampCallBack is nullptr and helper is null ."<<endl;
		args.GetReturnValue().Set(v8_crObj) ;
	}
}


/// create a Dataset from localfile. only for testing.
/// filepath,dt,width,hight,nband
void PixelEngine::GlobalFunc_LocalDatasetCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) 
{
	if(! PixelEngine::quietMode)cout<<"inside GlobalFunc_LocalDatasetCallBack"<<endl; 
	if (args.Length() != 5 ){
		if(! PixelEngine::quietMode)cout<<"Error: args.Length != 5 "<<endl ;
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
		size_t readbytes = fread( imgdata.data() , 1 , wid*hei*nb0*2 , pf) ;
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
	Local<Object> dsObj2 ;
	bool objok = dsValue->ToObject(context).ToLocal( &dsObj2 ) ;
	if(objok==false )
	{
		if(! PixelEngine::quietMode)cout<<"main output is not a object."<<endl ;
		return  ;
	}

	int dt = 1 ;
	int width=256;
	int height=256 ;
	int nband =0;

	bool ok1 = PixelEngine::V8ObjectGetIntValue(isolate,dsObj2,context,"dataType",dt) ;
	if( ok1==false ){
		if(! PixelEngine::quietMode)cout<<"Error : failed to get dataType of output object."<<endl; 
		return ;
	}

	ok1 = PixelEngine::V8ObjectGetIntValue(isolate,dsObj2,context,"width",width) ;
	if( ok1==false ){
		if(! PixelEngine::quietMode)cout<<"Error : failed to get width of output object."<<endl; 
		return ;
	}

	ok1 = PixelEngine::V8ObjectGetIntValue(isolate,dsObj2,context,"height",height) ;
	if( ok1==false ){
		if(! PixelEngine::quietMode)cout<<"Error : failed to get height of output object."<<endl; 
		return ;
	}

	ok1 = PixelEngine::V8ObjectGetIntValue(isolate,dsObj2,context,"nband",nband) ;
	if( ok1==false ){
		if(! PixelEngine::quietMode)cout<<"Error : failed to get nband of output object."<<endl; 
		return ;
	}

	if( nband == 0 )
	{
		if(! PixelEngine::quietMode)cout<<"failed to make png: zero nbands of output object."<<endl; 
		return ;
	}


	// dsObj->Get(context,
	// 	String::NewFromUtf8(isolate,"dataType").ToLocalChecked())
	// 	.ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	// int width = dsObj->Get(context,
	// 	String::NewFromUtf8(isolate,"width").ToLocalChecked())
	// 	.ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	// int height = dsObj->Get(context,
	// 	String::NewFromUtf8(isolate,"height").ToLocalChecked())
	// 	.ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	// int nband = dsObj->Get(context,
	// 	String::NewFromUtf8(isolate,"nband").ToLocalChecked())
	// 	.ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;


	// Local<Value> tiledataValue = dsObj->Get(context,
	// 	String::NewFromUtf8(isolate,"tiledata").ToLocalChecked())
	// 	.ToLocalChecked() ;
	unsigned long now1 = 0; 
	const int imgsize = width * height;
	vector<unsigned char> rgbadata(imgsize * 4, 0);   
	bool outimageOk = false ;
	if( dt == 1 )
	{
		int elementnumber = width*height*nband ;
		vector<unsigned char> tiledata(elementnumber) ;
		bool dataok = PixelEngine::V8ObjectGetUint8Array(isolate,
			dsObj2,
			context,
			"tiledata",
			elementnumber, 
			tiledata.data() ) ;
		if( dataok==false )
		{
			if(! PixelEngine::quietMode)cout<<"Error : failed to get tiledata."<<endl; 
			return ;
		}
		// Uint8Array* u8arr = Uint8Array::Cast(*tiledataValue) ;
		// unsigned char* dataptr = (unsigned char*) u8arr->Buffer()->GetBackingStore()->Data() ;
		now1 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	  	printf("get dataptr:%d ms \n", (int)(now1 - now) );//1024*1024 use 340millisec

		
		if( nband==4 )
		{
			for (int it = 0; it < imgsize; ++it)
			{
				rgbadata[it * 4 + 0] = tiledata[it];
				rgbadata[it * 4 + 1] = tiledata[it+imgsize];
				rgbadata[it * 4 + 2] = tiledata[it+2*imgsize];
				rgbadata[it * 4 + 3] = tiledata[it+3*imgsize];
			}
		}else if(nband==3 )
		{
			for (int it = 0; it < imgsize; ++it)
			{
				rgbadata[it * 4 + 0] = tiledata[it];
				rgbadata[it * 4 + 1] = tiledata[it+imgsize];
				rgbadata[it * 4 + 2] = tiledata[it+2*imgsize];
				rgbadata[it * 4 + 3] = 255;
			}
		} else
		{
			for (int it = 0; it < imgsize; ++it)
			{
				rgbadata[it * 4 + 0] = tiledata[it];
				rgbadata[it * 4 + 1] = tiledata[it];
				rgbadata[it * 4 + 2] = tiledata[it];
				rgbadata[it * 4 + 3] = 255;
			}
		}
		outimageOk=true ;
	}else if( dt==3 )
	{
		int elementnumber = width*height*nband ;
		vector<short> tiledata(elementnumber) ;
		bool dataok = PixelEngine::V8ObjectGetInt16Array(isolate,
			dsObj2,
			context,
			"tiledata",
			elementnumber, 
			tiledata.data() ) ;
		if( dataok==false )
		{
			if(! PixelEngine::quietMode)cout<<"Error : failed to get tiledata."<<endl; 
			return ;
		}
		now1 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	  	printf("get dataptr:%d ms \n", (int)(now1 - now) );//1024*1024 use 340millisec
		if( nband==4 )
		{
			for (int it = 0; it < imgsize; ++it)
			{
				rgbadata[it * 4 + 0] = tiledata[it];
				rgbadata[it * 4 + 1] = tiledata[it+imgsize];
				rgbadata[it * 4 + 2] = tiledata[it+2*imgsize];
				rgbadata[it * 4 + 3] = tiledata[it+3*imgsize];
			}
		}else if(nband==3 )
		{
			for (int it = 0; it < imgsize; ++it)
			{
				rgbadata[it * 4 + 0] = tiledata[it];
				rgbadata[it * 4 + 1] = tiledata[it+imgsize];
				rgbadata[it * 4 + 2] = tiledata[it+2*imgsize];
				rgbadata[it * 4 + 3] = 255;
			}
		} else
		{
			for (int it = 0; it < imgsize; ++it)
			{
				rgbadata[it * 4 + 0] = tiledata[it];
				rgbadata[it * 4 + 1] = tiledata[it];
				rgbadata[it * 4 + 2] = tiledata[it];
				rgbadata[it * 4 + 3] = 255;
			}
		}
		outimageOk=true ;
	} 

	if( outimageOk )
	{
		unsigned long now2 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	  	printf("forloop png:%d ms \n", (int)(now2 - now1) );//1024*1024 use 340millisec

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
	  	printf("encode png:%d ms \n", (int)(now3 - now2) );//1024*1024 use 340millisec
	}else
	{
		printf("Error: outimageOk == false.");
	}
}


bool PixelEngine::innerV8Dataset2TileData(Isolate* isolate, Local<Context>& context, 
	Local<Value>& v8dsValue, PeTileData& retTileData, string& error) 
{
	unsigned long now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

	Local<Object> dsObj2;
	bool objok = v8dsValue->ToObject(context).ToLocal(&dsObj2);
	if (objok == false)
	{
		if(! PixelEngine::quietMode)cout << "Error : innerV8Dataset2TileData v8dsValue is not a object." << endl;
		return false;
	}

	int dt = 1;
	int width = 256;
	int height = 256;
	int nband = 0;

	bool ok1 = PixelEngine::V8ObjectGetIntValue(isolate, dsObj2, context, "dataType", dt);
	if (ok1 == false) {
		if(! PixelEngine::quietMode)cout << "Error : failed to get dataType of output object." << endl;
		return false;
	}

	ok1 = PixelEngine::V8ObjectGetIntValue(isolate, dsObj2, context, "width", width);
	if (ok1 == false) {
		if(! PixelEngine::quietMode)cout << "Error : failed to get width of output object." << endl;
		return false;
	}

	ok1 = PixelEngine::V8ObjectGetIntValue(isolate, dsObj2, context, "height", height);
	if (ok1 == false) {
		if(! PixelEngine::quietMode)cout << "Error : failed to get height of output object." << endl;
		return false;
	}

	ok1 = PixelEngine::V8ObjectGetIntValue(isolate, dsObj2, context, "nband", nband);
	if (ok1 == false) {
		if(! PixelEngine::quietMode)cout << "Error : failed to get nband of output object." << endl;
		return false;
	}

	if (nband == 0)
	{
		if(! PixelEngine::quietMode)cout << "Error : failed to make png: zero nbands of output object." << endl;
		return false;
	}

	retTileData.dataType = dt;
	retTileData.width = width;
	retTileData.height = height;
	retTileData.nbands = nband;

	int imgsize = width * height;
	int elementnumber = imgsize * nband;

	bool copyDataOk = false;
	switch (dt) {
		case 1:
		{
			retTileData.tiledata.resize(elementnumber);
			copyDataOk = PixelEngine::V8ObjectGetUint8Array(isolate,
				dsObj2,
				context,
				"tiledata",
				elementnumber,
				retTileData.tiledata.data());

		}
		break;
		case 2:
		{
			retTileData.tiledata.resize(elementnumber*2);
			copyDataOk = PixelEngine::V8ObjectGetUint16Array(isolate,
				dsObj2,
				context,
				"tiledata",
				elementnumber,
				retTileData.tiledata.data());
		}
		break;
		case 3:
		{
			retTileData.tiledata.resize(elementnumber*2);
			copyDataOk = PixelEngine::V8ObjectGetInt16Array(isolate,
				dsObj2,
				context,
				"tiledata",
				elementnumber,
				retTileData.tiledata.data());
		}
		break;
		case 4:
		{
			retTileData.tiledata.resize(elementnumber*4);
			copyDataOk = PixelEngine::V8ObjectGetUint32Array(isolate,
				dsObj2,
				context,
				"tiledata",
				elementnumber,
				retTileData.tiledata.data());
		}
		break;
		case 5:
		{
			retTileData.tiledata.resize(elementnumber*4);
			copyDataOk = PixelEngine::V8ObjectGetInt32Array(isolate,
				dsObj2,
				context,
				"tiledata",
				elementnumber,
				retTileData.tiledata.data());
		}
		break;
		case 6:
		{
			retTileData.tiledata.resize(elementnumber*4);
			copyDataOk = PixelEngine::V8ObjectGetFloat32Array(isolate,
				dsObj2,
				context,
				"tiledata",
				elementnumber,
				retTileData.tiledata.data());
		}
		break;
		case 7:
		{
			retTileData.tiledata.resize(elementnumber*8);
			copyDataOk = PixelEngine::V8ObjectGetFloat64Array(isolate,
				dsObj2,
				context,
				"tiledata",
				elementnumber,
				retTileData.tiledata.data());
		}
		break;
	}

	return copyDataOk;
 
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
		if(! PixelEngine::quietMode)cout<<"Error: PixelEnginePointer is nothing."<<endl ;
	}

	//var PixelEngine = {} ;
	Local<Object> pe = Object::New(isolate) ;

	Maybe<bool> okpe = global->Set( context
		,String::NewFromUtf8(isolate, "PixelEngineObject").ToLocalChecked()
		,pe ) ;// bind PixelEngine
	if( okpe.IsNothing() )
	{
		if(! PixelEngine::quietMode)cout<<"okpe is nothing."<<endl ;
	}
	Maybe<bool> ok1 = pe->Set(context,String::NewFromUtf8(isolate, "ColorRampRainbow").ToLocalChecked(),
	   Integer::New(isolate,1));
	Maybe<bool> ok2 = pe->Set(context,String::NewFromUtf8(isolate, "ColorRampBlues").ToLocalChecked(),
	   Integer::New(isolate,2));
	Maybe<bool> ok3 = pe->Set(context,String::NewFromUtf8(isolate, "ColorRampReds").ToLocalChecked(),
	   Integer::New(isolate,3));
	Maybe<bool> ok4 = pe->Set(context,String::NewFromUtf8(isolate, "ColorRampGreens").ToLocalChecked(),
	   Integer::New(isolate,4));
	Maybe<bool> ok5 = pe->Set(context,String::NewFromUtf8(isolate, "ColorRampGrays").ToLocalChecked(),
	   Integer::New(isolate,0));
	Maybe<bool> ok6 = pe->Set(context,String::NewFromUtf8(isolate, "DatetimeCurrent").ToLocalChecked(),
	   Number::New(isolate,PE_CURRENTDATETIME));//modified 2020-6-20
	Maybe<bool> ok7 = pe->Set(context,String::NewFromUtf8(isolate, "Ignore").ToLocalChecked(),
	   Integer::New(isolate,-1));

	//normal/inverse
	Maybe<bool> ok8 = pe->Set(context,String::NewFromUtf8(isolate, "ColorRampNormal").ToLocalChecked(),
	Integer::New(isolate,0));
	Maybe<bool> ok9 = pe->Set(context,String::NewFromUtf8(isolate, "ColorRampInverse").ToLocalChecked(),
	Integer::New(isolate,1));
	//discrete/interpol
	Maybe<bool> ok10 = pe->Set(context,String::NewFromUtf8(isolate, "ColorRampDiscrete").ToLocalChecked(),
	Integer::New(isolate,0));
	Maybe<bool> ok11 = pe->Set(context,String::NewFromUtf8(isolate, "ColorRampInterpolate").ToLocalChecked(),
	Integer::New(isolate,1));
	Maybe<bool> ok12 = pe->Set(context,String::NewFromUtf8(isolate, "ColorRampExact").ToLocalChecked(),
	Integer::New(isolate,2));//add 2020-6-20
	Maybe<bool> ok13 = pe->Set(context,String::NewFromUtf8(isolate, "pejs_version").ToLocalChecked(),
		String::NewFromUtf8(isolate, PixelEngine::pejs_version.c_str()).ToLocalChecked()) ;


	Maybe<bool> ok14 = pe->Set(context
		,String::NewFromUtf8(isolate, "NewDataset").ToLocalChecked(),
           FunctionTemplate::New(isolate, PixelEngine::GlobalFunc_NewDatasetCallBack)->GetFunction(context).ToLocalChecked() );

	Maybe<bool> ok15 = pe->Set(context
		,String::NewFromUtf8(isolate, "Dataset").ToLocalChecked(),
           FunctionTemplate::New(isolate, PixelEngine::GlobalFunc_DatasetCallBack)->GetFunction(context).ToLocalChecked() );
	Maybe<bool> ok16 = pe->Set(context
		,String::NewFromUtf8(isolate, "GetTileData").ToLocalChecked(),
           FunctionTemplate::New(isolate, PixelEngine::GlobalFunc_GetTileDataCallBack)->GetFunction(context).ToLocalChecked() );
	Maybe<bool> ok17 = pe->Set(context
	,String::NewFromUtf8(isolate, "LocalDataset").ToLocalChecked(),
       FunctionTemplate::New(isolate, PixelEngine::GlobalFunc_LocalDatasetCallBack)->GetFunction(context).ToLocalChecked() );
	Maybe<bool> ok18 = pe->Set(context
		,String::NewFromUtf8(isolate, "DatasetArray").ToLocalChecked(),
           FunctionTemplate::New(isolate, PixelEngine::GlobalFunc_DatasetArrayCallBack)->GetFunction(context).ToLocalChecked() );
	//2020-9-13
	Maybe<bool> ok19 = pe->Set(context
		, String::NewFromUtf8(isolate, "getStyle").ToLocalChecked(),
		FunctionTemplate::New(isolate, PixelEngine::GlobalFunc_GetStyleCallBack)->GetFunction(context).ToLocalChecked());



	//pe function log
	Maybe<bool> ok20 = pe->Set(context
		,String::NewFromUtf8(isolate, "log").ToLocalChecked(),
           FunctionTemplate::New(isolate, PixelEngine::GlobalFunc_Log)->GetFunction(context).ToLocalChecked() );

	//set globalFunc_forEachPixelCallBack in javascript
	string sourceforEachPixelFunction = R"(
		const PixelEngine=PixelEngineObject;
		const pe=PixelEngineObject ;
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
					var newloadedData = PixelEngineOjbect.GetTileData(this.dsName,this.dsDt,this.z,this.y+gy,this.x+gx) ;
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
		PixelEngineObject.ColorRamp = function(colorRampId){
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
			cr.setNodata=function(nval,tr,tg,tb,ta,lb){
				this.Nodata=nval;this.NodataColor[0]=tr;this.NodataColor[1]=tg;
				this.NodataColor[2]=tb;this.NodataColor[3]=ta;this.NodataLabel=lb;
			};
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
	
	v8::Local<v8::Script> scriptForEach ;
	bool scriptOk = v8::Script::Compile(context
          	, String::NewFromUtf8(isolate,sourceforEachPixelFunction.c_str()).ToLocalChecked()
          	).ToLocal( &scriptForEach );
	if( scriptOk==false )
	{
		if(! PixelEngine::quietMode)cout<<"compile sourceforEachPixelFunction failed."<<endl ;
		return false ;
	}
    
    v8::Local<v8::Value> resultForEach ;
    bool runok = scriptForEach->Run(context).ToLocal( &resultForEach );
    if( runok==false )
	{
		if(! PixelEngine::quietMode)cout<<"run sourceforEachPixelFunction failed."<<endl ;
		return false ;
	}

    Local<Value> forEachFuncInJs = global->Get(context 
    	,String::NewFromUtf8(isolate, "globalFunc_forEachPixelCallBack").ToLocalChecked() ).ToLocalChecked() ;
    thePE->GlobalFunc_ForEachPixelCallBack.Reset(isolate , forEachFuncInJs) ;

    Local<Value> getPixelFuncInJs = global->Get(context 
    	,String::NewFromUtf8(isolate, "globalFunc_getPixelCallBack").ToLocalChecked() ).ToLocalChecked() ;
    thePE->GlobalFunc_GetPixelCallBack.Reset(isolate , getPixelFuncInJs) ;


    //set globalFunc_newDatasetCallBack, this will be called in javascript ForEachPixel.
    Maybe<bool> ok21 = global->Set(context
		,String::NewFromUtf8(isolate, "globalFunc_newDatasetCallBack").ToLocalChecked(),
           FunctionTemplate::New(isolate, PixelEngine::GlobalFunc_NewDatasetCallBack)->GetFunction(context).ToLocalChecked() );

    //set globalFunc_ColorRampCallBack, this will be c++ codes.
    Maybe<bool> ok22 = global->Set(context
		,String::NewFromUtf8(isolate, "globalFunc_ColorRampCallBack").ToLocalChecked(),
           FunctionTemplate::New(isolate, PixelEngine::GlobalFunc_ColorRampCallBack)->GetFunction(context).ToLocalChecked() );


	// //global function globalFunc_renderGrayCallBack
	// global->Set(context
	// 	,String::NewFromUtf8(isolate, "globalFunc_renderGrayCallBack").ToLocalChecked(),
	//           FunctionTemplate::New(isolate, PixelEngine::GlobalFunc_RenderGrayCallBack)->GetFunction(context).ToLocalChecked() );
	return true ;
}


//2020-9-13 get style from script
bool PixelEngine::RunToGetStyleFromScript(string& scriptContent, PeStyle& retstyle, string& retLogText)
{
	bool allOk = false;
	this->pe_logs.reserve(2048);//max 1k bytes.
	//v8::Isolate::CreateParams create_params;
	this->create_params.array_buffer_allocator =
		v8::ArrayBuffer::Allocator::NewDefaultAllocator();
	//v8::Isolate* isolate = v8::Isolate::New(create_params);
	this->isolate = v8::Isolate::New(create_params);
	{
		if(! PixelEngine::quietMode)cout << "in RunToGetStyleFromScript " << endl;
		v8::Isolate::Scope isolate_scope(this->isolate);
		v8::HandleScope handle_scope(this->isolate);

		// Create a new context.
		v8::Local<v8::Context> context = v8::Context::New(this->isolate);
		// Enter the context for compiling and running the hello world script.
		v8::Context::Scope context_scope(context);// enter scope
		PixelEngine::initTemplate(this, this->isolate, context);
		this->m_context.Reset(this->isolate, context);
		TryCatch try_catch(this->isolate);
		string source = scriptContent + "var pixelengine_run2getstyle_style=null;if(typeof setStyle=='function'){pixelengine_run2getstyle_style=setStyle();}";
		// Compile the source code.
		v8::Local<v8::Script> script;
		if (!Script::Compile(context, String::NewFromUtf8(this->isolate,
			source.c_str()).ToLocalChecked()).ToLocal(&script)) {
			String::Utf8Value error(this->isolate, try_catch.Exception());
			if(! PixelEngine::quietMode)cout << "v8 exception:" << *error << endl;
			retLogText = string() +"compile v8 exception " + (*error);
			allOk = false;
		}
		else {
			unsigned long now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			// Run the script to get the result.
			Local<v8::Value> result;
			if (!script->Run(context).ToLocal(&result)) {
				String::Utf8Value error(this->isolate, try_catch.Exception());
				string exceptionstr = string("v8 exception:") + (*error);
				if(! PixelEngine::quietMode)cout << exceptionstr << endl;
				// The script failed to compile; bail out.
				//return false;
				this->log(exceptionstr);
				retLogText = string() + "run script v8 exception " + exceptionstr;
				allOk = false;
			}
			else
			{
				MaybeLocal<Value> styleResult = context->Global()->Get(context
					, String::NewFromUtf8(isolate, "pixelengine_run2getstyle_style").ToLocalChecked());
				if (PixelEngine::IsMaybeLocalOK(styleResult) == false) //IsNullOrUndefined() )
				{
					string error1("Error: result from setStyle() is null or undefined.");
					if(! PixelEngine::quietMode)cout << error1 << endl;
					this->log(error1);
					retLogText = error1;
					allOk = false;
				}
				else
				{
					if(! PixelEngine::quietMode)cout << "in RunToGetStyleFromScript 4" << endl;

					unsigned long now1 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
					printf("script run dura:%d ms \n", (int)(now1 - now) );//

					MaybeLocal<String> outStyleJson = JSON::Stringify(context, styleResult.ToLocalChecked());
					Local<String> outStyleJson2;
					
					if (outStyleJson.ToLocal(& outStyleJson2)) {
						//SetStyle·µ»ØµÄÊÇjson¶ÔÏó
						Local<Value> styleV8Value  = outStyleJson2.As<Value>();
						string cstrStyle = PixelEngine::convertV8LocalValue2CppString(isolate , styleV8Value);
						if(! PixelEngine::quietMode)cout << "cstrStyle:" << cstrStyle << endl;//debug
						bool isjsonok = retstyle.loadFromJson(cstrStyle);
						if (isjsonok) {
							retLogText = "Info : find style by json";
							allOk =  true;
						}
						else {
							if(! PixelEngine::quietMode)cout << "Info : it seems return string from setStyle not be valid json, then try styleid..." << endl;
							//json½âÎöÊ§°Ü, ²é¿´×Ö·û´®ÊÇ·ñÊÇÒ»¸öÓÐÐ§µÄÊý×Ö£¬Èç¹ûÊÇÓÐÐ§Êý¾ÝÔòÍ¨¹ýÊý¾Ý¿â²éÕÒäÖÈ¾·½°¸
							//Local<String> localResultStr = styleResult.ToLocalChecked().As<String>() ;
							//Local<Value> localResultValue = localResultStr.As<Value>();
							Local<Value> localResultValue = styleResult.ToLocalChecked();
							string styleResultCStr = PixelEngine::convertV8LocalValue2CppString(isolate, localResultValue);
							double resultNum = 0;
							pe::wStringUtil util;
							if (util.isValidNumber(styleResultCStr, resultNum) == true )
							{
								if (this->helperPointer) {
									string errorText;
									if (this->helperPointer->getStyle(styleResultCStr, retstyle, errorText)) {
										if(! PixelEngine::quietMode)cout << "Info : getStyle ok." << endl;
										retLogText = "Info : find style by styleid";
										allOk = true;
									}
									else {
										if(! PixelEngine::quietMode)cout << "Error : helper not find style with id '" << styleResultCStr << "' " << endl;
										retLogText = string() + "Error : setStyle not return valid json and return value as styleid: "
											+ styleResultCStr+ " also not finded." ;
										allOk = false;
									}
								}
								else {
									if(! PixelEngine::quietMode)cout << "Error : helper is null " << endl;
									retLogText = "Error : helper is null";
									allOk = false;
								}
							}
							else {
								if(! PixelEngine::quietMode)cout << "Error : setStyle return value is not valid styleid '" << styleResultCStr << "' " << endl;
								retLogText = string() + "Error : setStyle return value is not valid styleid " + styleResultCStr;
								allOk = false;
							}
						}
					}
					else
					{
						if(! PixelEngine::quietMode)cout << "Error : RunToGetStyleFromScript failed in ToLocal" << endl;
						retLogText =  "Error : RunToGetStyleFromScript failed in ToLocal" ;
						allOk = false;
					}
					unsigned long now2 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
					printf("get style:%d ms \n", (int)(now2 - now1) );
				}
			}
		}

	}
	this->m_context.Reset();
	this->GlobalFunc_ForEachPixelCallBack.Reset();
	this->GlobalFunc_GetPixelCallBack.Reset();
	// Dispose the isolate and tear down V8.
	this->isolate->Dispose();

	return allOk;
}

bool PixelEngine::RunScriptForTile(void* extra, string& jsSource,long currentdt,int z,int y,int x, vector<unsigned char>& retbinary) 
{
	if(! PixelEngine::quietMode)cout<<"in RunScriptForTile init v8"<<endl;
	this->pe_logs.reserve(2048);//max 1k bytes.
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
		if(! PixelEngine::quietMode)cout<<"in RunScriptForTile run script"<<endl;
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
			if(! PixelEngine::quietMode)cout<<"v8 exception:"<<*error<<endl;
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
				string exceptionstr = string("v8 exception:")+(*error) ;
				if(! PixelEngine::quietMode)cout<<exceptionstr<<endl;
				// The script failed to compile; bail out.
				//return false;
				this->log(exceptionstr) ;
				allOk = false ;
			}else
			{
				MaybeLocal<Value> peMainResult = context->Global()->Get(context 
		    		,String::NewFromUtf8(isolate, "PEMainResult").ToLocalChecked() ) ;
				if( PixelEngine::IsMaybeLocalOK(peMainResult) == false) //IsNullOrUndefined() )
				{
					string error1("Error: the result from main() is null or undefined.") ;
					if(! PixelEngine::quietMode)cout<<error1<<endl ;
					this->log(error1) ;
					allOk = false ;
				}else
				{
					if(! PixelEngine::quietMode)cout<<"in RunScriptForTile 4"<<endl;

					unsigned long now1 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
					printf("script run dura:%d ms \n", (int)(now1 - now) );//

					//tiledata to png
					PixelEngine::Dataset2Png( isolate, context, peMainResult.ToLocalChecked()
						, retbinary );

					unsigned long now2 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
					printf("encode png:%d ms \n", (int)(now2 - now1) );
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

//Only ComputeOnce
bool PixelEngine::RunScriptForComputeOnce(void* extra, string& jsSource,long currentdt
	,int z,int y,int x, string& retJsonStr ) 
{
	if(! PixelEngine::quietMode)cout<<"in RunScriptForComputeOnce init v8"<<endl;
	this->pe_logs.reserve(2048);//max 1k bytes.
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
		if(! PixelEngine::quietMode)cout<<"in RunScriptForComputeOnce run ComputeOnce() in script"<<endl;
		v8::Isolate::Scope isolate_scope(this->isolate);
		v8::HandleScope handle_scope(this->isolate);

		// Create a new context.
		v8::Local<v8::Context> context = v8::Context::New(this->isolate );
		// Enter the context for compiling and running the hello world script.
		v8::Context::Scope context_scope(context);// enter scope
		PixelEngine::initTemplate(  this , this->isolate , context) ;
		this->m_context.Reset( this->isolate , context);
		TryCatch try_catch(this->isolate);
		string source = jsSource + "var temp_computeOnceResult=JSON.stringify(ComputeOnce());" ;

		// Compile the source code.
		v8::Local<v8::Script> script ;
		if (!Script::Compile(context, String::NewFromUtf8(this->isolate,
		  		source.c_str()).ToLocalChecked()).ToLocal(&script)) {
			String::Utf8Value error(this->isolate, try_catch.Exception());
			if(! PixelEngine::quietMode)cout<<"v8 exception:"<<*error<<endl;
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
				string exceptionstr = string("v8 exception:")+(*error) ;
				if(! PixelEngine::quietMode)cout<<exceptionstr<<endl;
				// The script failed to compile; bail out.
				//return false;
				this->log(exceptionstr) ;
				allOk = false ;
			}else
			{
				MaybeLocal<Value> coResult = context->Global()->Get(context 
		    		,String::NewFromUtf8(isolate, "temp_computeOnceResult").ToLocalChecked() ) ;
				if( PixelEngine::IsMaybeLocalOK(coResult) == false) //IsNullOrUndefined() )
				{
					string error1("Error: the result from ComputeOnce() is null or undefined.") ;
					if(! PixelEngine::quietMode)cout<<error1<<endl ;
					this->log(error1) ;
					allOk = false ;
				}else
				{
					if(! PixelEngine::quietMode)cout<<"in RunScriptForComputeOnce 4"<<endl;

					unsigned long now1 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
					printf("script run dura:%d ms \n", (int)(now1 - now) );//

					MaybeLocal<String> coStr = coResult.ToLocalChecked()->ToString(context) ;
					Local<String> coStr2 ;
					if( coStr.ToLocal(&coStr2) )
					{
						String::Utf8Value coStrUtf8( this->isolate, coStr2);
						retJsonStr = (*coStrUtf8) ;
					}else
					{
						retJsonStr = "null" ;
					}

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
	if(! PixelEngine::quietMode)cout<<"in CheckScriptOk"<<endl;

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
			if(! PixelEngine::quietMode)cout << "build source error:" << errorText << std::endl;
		}else
		{
			v8::MaybeLocal<v8::Script> compiledScript =
				v8::Script::Compile(context, source.ToLocalChecked() ) ;
			if (compiledScript.IsEmpty()) {
				//compile error
				v8::String::Utf8Value error( this->isolate, try_catch.Exception());
				errorText = std::string(*error);
				if(! PixelEngine::quietMode)cout << "compile error:" << errorText << std::endl;
			}
		}

	}
	this->isolate->Dispose();

	return errorText ;
}


//2020-9-14
//ÔËÐÐ½Å±¾±£ÁôÊý¾Ý£¬²»äÖÈ¾
bool PixelEngine::RunScriptForTileWithoutRender(void* extra, string& scriptContent, int64_t currentdt,
	int z, int y, int x, PeTileData& tileData, string& logStr) {

	if(! PixelEngine::quietMode)cout << "in RunScriptForTile init v8" << endl;
	this->pe_logs.reserve(2048);//max 1k bytes.
	this->tileInfo.x = x;
	this->tileInfo.y = y;
	this->tileInfo.z = z;
	this->extraPointer = extra;
	this->currentDateTime = currentdt;

	bool allOk = true;

	// Create a new Isolate and make it the current one.
	//v8::Isolate::CreateParams create_params;
	this->create_params.array_buffer_allocator =
		v8::ArrayBuffer::Allocator::NewDefaultAllocator();
	//v8::Isolate* isolate = v8::Isolate::New(create_params);
	this->isolate = v8::Isolate::New(create_params);
	{
		if(! PixelEngine::quietMode)cout << "in RunScriptForTileWithoutRender run script" << endl;
		v8::Isolate::Scope isolate_scope(this->isolate);
		v8::HandleScope handle_scope(this->isolate);

		// Create a new context.
		v8::Local<v8::Context> context = v8::Context::New(this->isolate);
		// Enter the context for compiling and running the hello world script.
		v8::Context::Scope context_scope(context);// enter scope
		PixelEngine::initTemplate(this, this->isolate, context);
		this->m_context.Reset(this->isolate, context);
		TryCatch try_catch(this->isolate);
		string source = scriptContent + "var PEMainResult=main();";

		// Compile the source code.
		v8::Local<v8::Script> script;
		if (!Script::Compile(context, String::NewFromUtf8(this->isolate,
			source.c_str()).ToLocalChecked()).ToLocal(&script)) {
			String::Utf8Value error(this->isolate, try_catch.Exception());
			if(! PixelEngine::quietMode)cout << "v8 exception:" << *error << endl;
			// The script failed to compile; bail out.
			//return false;

			allOk = false;
		}
		else
		{
			unsigned long now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			// Run the script to get the result.
			Local<v8::Value> result;
			if (!script->Run(context).ToLocal(&result)) {
				String::Utf8Value error(this->isolate, try_catch.Exception());
				string exceptionstr = string("v8 exception:") + (*error);
				if(! PixelEngine::quietMode)cout << exceptionstr << endl;
				// The script failed to compile; bail out.
				//return false;
				this->log(exceptionstr);
				allOk = false;
			}
			else
			{
				MaybeLocal<Value> peMainResult = context->Global()->Get(context
					, String::NewFromUtf8(isolate, "PEMainResult").ToLocalChecked());
				if (PixelEngine::IsMaybeLocalOK(peMainResult) == false) //IsNullOrUndefined() )
				{
					string error1("Error: the result from main() is null or undefined.");
					if(! PixelEngine::quietMode)cout << error1 << endl;
					this->log(error1);
					allOk = false;
				}
				else
				{
					if(! PixelEngine::quietMode)cout << "in RunScriptForTileWithoutRender step4" << endl;

					unsigned long now1 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
					if (!PixelEngine::quietMode)printf("script run dura:%d ms \n", (int)(now1 - now) );//

					// v8 dataset object 2 tileData
					string errorText;
					Local<Value> localMainResult = peMainResult.ToLocalChecked();
					bool tiledataok = this->innerV8Dataset2TileData(isolate, context, localMainResult, tileData, errorText);
					unsigned long now2 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
					if(!PixelEngine::quietMode)printf("v8 value to tiledata dura:%d ms \n", (int)(now2 - now1) );
					if(! PixelEngine::quietMode)cout << "Info : innerV8Dataset2TileData return  " << tiledataok << endl;
					allOk = tiledataok;
				}
			}
		}
	}


	this->m_context.Reset();
	this->GlobalFunc_ForEachPixelCallBack.Reset();
	this->GlobalFunc_GetPixelCallBack.Reset();

	// Dispose the isolate and tear down V8.
	this->isolate->Dispose();


	return allOk;
}
//ÔËÐÐ½Å±¾²¢äÖÈ¾pngÍ¼Æ¬
bool PixelEngine::RunScriptForTileWithRender(void* extra, string& scriptContent,PeStyle& inStyle, int64_t currentDatetime,
	int z, int y, int x, vector<unsigned char>& retPngBinary, int& pngwid,int& pnghei, string& logStr) {

	if(! PixelEngine::quietMode)cout << "in PixelEngine::RunScriptForTileWithRender" << endl;

	PeTileData retTileData0 ;
	string retLogText;
	bool tiledataok = this->RunScriptForTileWithoutRender(extra, scriptContent, currentDateTime, z, y, x,
		retTileData0, logStr);

	if (tiledataok) {
		//do render staff
		pngwid = retTileData0.width;
		pnghei = retTileData0.height;

		if (inStyle.type == "") {
			//do not use style
			if(! PixelEngine::quietMode)cout << "Info : a empty inStyle, so do not use input style." << endl;
			string renderError;
			bool renderok = innerRenderTileDataWithoutStyle(retTileData0, retPngBinary , renderError);
			logStr += renderError;
			return renderok;
		}
		else {
			//use input style
			if(! PixelEngine::quietMode)cout << "Info : use input style" << endl;
			string renderError;
			bool renderok = this->innerRenderTileDataByPeStyle(retTileData0, inStyle,  retPngBinary, renderError);
			logStr += renderError;
			return renderok;
		}
		return true;
	}
	else
	{
		return false;
	}
}



//Ê¹ÓÃesprima½âÎö½Å±¾Éú³ÉAST json¶ÔÏó 2020-9-19
bool PixelEngine::RunScriptForAST(void* extra, string& scriptContent, string& retJsonStr, string& errorText) {
	if(! PixelEngine::quietMode)cout << "in PixelEngine::RunScriptForAST" << endl;
	string esprimaMinJs = "esprima.min.js";
	if(! PixelEngine::quietMode)cout << "loading "<< esprimaMinJs << endl;
	pe::wTextfilereader reader;
	string esprimaSource = reader.readfile(esprimaMinJs);
	if (esprimaSource == "") {
		if(! PixelEngine::quietMode)cout << "Error : failed to read " << esprimaMinJs << endl;
		return false;
	}
	if(! PixelEngine::quietMode)cout << "load esprima codes with size:" << esprimaSource.size() << endl;
	bool allOk = true;

	// Create a new Isolate and make it the current one.
	//v8::Isolate::CreateParams create_params;
	this->create_params.array_buffer_allocator =
		v8::ArrayBuffer::Allocator::NewDefaultAllocator();
	//v8::Isolate* isolate = v8::Isolate::New(create_params);
	this->isolate = v8::Isolate::New(create_params);
	{
		v8::Isolate::Scope isolate_scope(this->isolate);
		v8::HandleScope handle_scope(this->isolate);
		// Create a new context.
		v8::Local<v8::Context> context = v8::Context::New(this->isolate);
		// Enter the context for compiling and running the hello world script.
		v8::Context::Scope context_scope(context);// enter scope
		PixelEngine::initTemplate(this, this->isolate, context);
		this->m_context.Reset(this->isolate, context);
		TryCatch try_catch(this->isolate);

		pe::wStringUtil strutil;
		string escapeScript = strutil.escape(scriptContent);
		if(! PixelEngine::quietMode)cout << escapeScript << endl;
 

		//add esprima codes and try catch.
		string source2 = esprimaSource + ";"
			+ "var pe_ast_parse_resultjsonstr = null ; "
			+ "var pe_ast_parse_resulterrorstr='' ; "
			+ "try{"
			+ "var pe_ast_parse_result=esprima.parseScript(\"" + escapeScript + "\"); "
			+ "pe_ast_parse_resultjsonstr=JSON.stringify(pe_ast_parse_result);"
			+ "}catch(err){"
			+ "pe_ast_parse_resulterrorstr = err;"
			+ "}";

 

		// Compile the source code.
		v8::Local<v8::Script> script;
		if (!Script::Compile(context, String::NewFromUtf8(this->isolate,
			source2.c_str()).ToLocalChecked()).ToLocal(&script)) {
			String::Utf8Value error(this->isolate, try_catch.Exception());
			if(! PixelEngine::quietMode)cout << "compile v8 exception:" << *error << endl;
			// The script failed to compile; bail out.
			//return false;
			errorText = "compile v8 exception:" + string(*error) ;
			allOk = false;
		}
		else
		{
			unsigned long now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			// Run the script to get the result.
			Local<v8::Value> result;
			if (!script->Run(context).ToLocal(&result)) {
				String::Utf8Value error(this->isolate, try_catch.Exception());
				string exceptionstr = string("run v8 exception:") + (*error);
				if(! PixelEngine::quietMode)cout << exceptionstr << endl;
				// The script failed to compile; bail out.
				//return false;
				//this->log(exceptionstr);
				errorText = "run v8 exception:" + string(*error);
				allOk = false;
			}
			else
			{
				MaybeLocal<Value> astresultjsostrObj = context->Global()->Get(context
					, String::NewFromUtf8(isolate, "pe_ast_parse_resultjsonstr").ToLocalChecked());
				if (PixelEngine::IsMaybeLocalOK(astresultjsostrObj) == false) //IsNullOrUndefined() )
				{
					allOk = false;
					string error1("Error: the ast parse result is null or undefined.");
					if(! PixelEngine::quietMode)cout << error1 << endl;

					MaybeLocal<Value> errorobj = context->Global()->Get(context
						, String::NewFromUtf8(isolate, "pe_ast_parse_resulterrorstr").ToLocalChecked());
					Local<Value> errorobj2;
					bool eok = errorobj.ToLocal(&errorobj2);
					if (eok == false) {
						errorText = error1 + "\n get AST exception string also failed.";
					}
					else {
						string estr = PixelEngine::convertV8LocalValue2CppString(isolate, errorobj2);
						errorText = error1 + "\n The AST exception:" + estr ;
					}
				}
				else
				{
					if(! PixelEngine::quietMode)cout << "ast parse result ok" << endl;
					string errorText;
					Local<Value> astresult = astresultjsostrObj.ToLocalChecked();
					retJsonStr = PixelEngine::convertV8LocalValue2CppString(isolate, astresult);
					if (retJsonStr.length() > 0) {
						allOk =true ;
					}
					else {
						if(! PixelEngine::quietMode)cout << "Error : AST result is empty string." << endl;
						errorText = "Error : AST result is empty string.";
						allOk = false;
					}
					
				}
			}
		}
	}
	this->m_context.Reset();
	this->GlobalFunc_ForEachPixelCallBack.Reset();
	this->GlobalFunc_GetPixelCallBack.Reset();
	// Dispose the isolate and tear down V8.
	this->isolate->Dispose();
	return allOk;
}


//解析Dataset-Datetime 数据集时间日期对
bool PixelEngine::RunScriptForDatasetDatetimePairs(void* extra,
	string& scriptContent,
	vector<wDatasetDatetime>& retDsDtVec,
	string& errorText)
{
	//vector<wDatasetDatetime> udsdtVec;
    string retASTLog , retASTJsonText;
    bool runASTok = this->RunScriptForAST(nullptr, scriptContent , retASTJsonText, retASTLog);
    if (runASTok) {
        wAST ast;
        printf("parsing AST...\n");
        ast.parse(retASTJsonText);
        string retVal;

        bool mainOk = ast.hasReturnableMainFunction(ast.rootNode.as<JsonObject>());
        if (mainOk) {
            printf("find dataset names ...\n");
            vector<wDatasetDatetime> dsdtVec ;
            ast.findObjectPropertyCallStatement(ast.rootNode.as<JsonObject>(), "pe", "Dataset", dsdtVec);
            ast.findObjectPropertyCallStatement(ast.rootNode.as<JsonObject>(), "PixelEngine", "Dataset", dsdtVec);
            ast.findObjectPropertyCallStatement(ast.rootNode.as<JsonObject>(), "pe", "DatasetArray", dsdtVec);
            ast.findObjectPropertyCallStatement(ast.rootNode.as<JsonObject>(), "PixelEngine", "DatasetArray", dsdtVec);

            //去掉dtds重复项
            for (vector<wDatasetDatetime>::iterator iter = dsdtVec.begin(); iter != dsdtVec.end(); ++iter) {
                bool hasone = false;
                for (vector<wDatasetDatetime>::iterator iteru = retDsDtVec.begin(); 
                	iteru != retDsDtVec.end(); ++iteru) 
                {
                    if (iteru->ds == iter->ds && iteru->dt0 == iter->dt0 && iteru->dt1 == iter->dt1) {
                        hasone = true;
                        break;
                    }
                }
                if (hasone==false) {
                    retDsDtVec.push_back(*iter);
                }
            }

            if (retDsDtVec.size() > 0) {
                //good
            }
            else {
                printf("no dataset is found\n");
            	return false;
            }
        }
        else {
            printf("script has no valid main function.\n");
            return false;
        }
    }
    else {
        printf("parse AST failed: %s \n" , retASTLog.c_str() );
        return false;
    }

	return true;
}









bool PixelEngine::innerRenderTileDataWithoutStyle(PeTileData& tileData, vector<unsigned char>& retPngBinary, string& error ) {
	if (tileData.tiledata.size() == 0) {
		if(! PixelEngine::quietMode)cout << "Error : innerRenderTileDataWithoutStyle tileData is empty." << endl;
		return false;
	}
	unsigned long  now1 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	vector<unsigned char> rgbaData;
	switch (tileData.dataType)
	{
	case 1:
	{
		unsigned char* ptr = (unsigned char*)tileData.tiledata.data() ;
		this->innerData2RGBAWithoutStyle(ptr , tileData.width, tileData.height , tileData.nbands , rgbaData);
	}
	break;
	case 2:
	{
		unsigned short* ptr = (unsigned short*)tileData.tiledata.data();
		this->innerData2RGBAWithoutStyle(ptr, tileData.width, tileData.height, tileData.nbands, rgbaData);
	}
	break;
	case 3:
	{
		short* ptr = (short*)tileData.tiledata.data();
		this->innerData2RGBAWithoutStyle(ptr, tileData.width, tileData.height, tileData.nbands, rgbaData);
	}
	break;
	case 4:
	{
		unsigned int* ptr = (unsigned int*)tileData.tiledata.data();
		this->innerData2RGBAWithoutStyle(ptr, tileData.width, tileData.height, tileData.nbands, rgbaData);
	}
	break;
	case 5:
	{
		int* ptr = (int*)tileData.tiledata.data();
		this->innerData2RGBAWithoutStyle(ptr, tileData.width, tileData.height, tileData.nbands, rgbaData);
	}
	break;
	case 6:
	{
		float* ptr = (float*)tileData.tiledata.data();
		this->innerData2RGBAWithoutStyle(ptr, tileData.width, tileData.height, tileData.nbands, rgbaData);
	}
	break;
	case 7:
	{
		double* ptr = (double*)tileData.tiledata.data();
		this->innerData2RGBAWithoutStyle(ptr, tileData.width, tileData.height, tileData.nbands, rgbaData);
	}
	break;
	default:
		break;
	}
	unsigned long  now2 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	if(! PixelEngine::quietMode)cout << "Info : make RGBA colors duration:" << now2 - now1 << " ms " << endl;

	//rgba to png
	if (rgbaData.size() == 0) {
		return false;
	}
	//rgba data to png
	this->innerRGBAData2Png(rgbaData, tileData.width, tileData.height, retPngBinary);


	return true;
}
template<typename T>
void PixelEngine::innerData2RGBAWithoutStyle(T* data, int width,int height, int nbands, vector<unsigned char>& rgbaData ) {
	rgbaData.resize( width*height*4) ;
	int dataLen = width * height;
	int dataLen2 = dataLen * 2;
	int dataLen3 = dataLen * 3;
	if (nbands == 3) {
		//rgb
		for (int it = 0; it < dataLen; ++it) {
			int it1 = it * 4;
			rgbaData[it1] = PixelEngine::clamp255(data[it]) ;
			rgbaData[it1+1] = PixelEngine::clamp255(data[dataLen + it])  ;
			rgbaData[it1+2] = PixelEngine::clamp255(data[dataLen2 + it])  ;
			rgbaData[it1+3] = 255;
		}
	}
	else if (nbands == 4) {
		//rgba
		for (int it = 0; it < dataLen; ++it) {
			int it1 = it * 4;
			rgbaData[it1] = PixelEngine::clamp255(data[it])  ;
			rgbaData[it1+1] = PixelEngine::clamp255(data[dataLen + it]) ;
			rgbaData[it1+2] = PixelEngine::clamp255(data[dataLen2 + it]) ;
			rgbaData[it1+3] = PixelEngine::clamp255(data[dataLen3 + it])  ;
		}
	}
	else {
		//grey 0-255 for band zero
		for (int it = 0; it < dataLen; ++it) {
			int it1 = it * 4;
			rgbaData[it1] = PixelEngine::clamp255(data[it]) ;
			rgbaData[it1+1] = rgbaData[it];
			rgbaData[it1+2] = rgbaData[it];
			rgbaData[it1+3] = 255;
		}
	}
}

template<typename T>
unsigned char PixelEngine::clamp255(T val) {
	if (val < 0) return 0;
	else if (val > 255) return 255;
	else return (unsigned char)val;
}


/// ¸Ã·½·¨ÖÆ×÷StyleäÖÈ¾£¬±ØÐë±£Ö¤styleÊÇÕýÈ·µÄ¡£
bool PixelEngine::innerRenderTileDataByPeStyle(PeTileData& tileData, PeStyle& style, vector<unsigned char>& retPngBinary, string& error) {
	if (tileData.tiledata.size() == 0) {
		if(! PixelEngine::quietMode)cout << "Error : innerRenderTileDataByPeStyle tileData is empty." << endl;
		return false;
	}
	if (style.type == "") {
		if(! PixelEngine::quietMode)cout << "Error : innerRenderTileDataByPeStyle style is empty." << endl;
		return false;
	}
	unsigned long  now1 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	int dataLen = tileData.width * tileData.height;
	vector<unsigned char> rgbaData;
	bool pngok = false;
	string pngLogStr;
	switch (tileData.dataType)
	{
		case 1:
		{
			unsigned char* ptr = (unsigned char*)tileData.tiledata.data();
			if (style.type == "gray" || style.type == "rgb" || style.type == "rgba") {
				pngok = this->innerData2RGBAByPeStyle2(ptr, tileData.width , tileData.height , tileData.nbands , style, rgbaData , pngLogStr );
			}
			else {
				//discrete exact linear
				pngok = this->innerData2RGBAByPeStyle(ptr, tileData.width, tileData.height, tileData.nbands, style, rgbaData, pngLogStr);
			}
		}
		break;
		case 2:
		{
			unsigned short * ptr = (unsigned short*)tileData.tiledata.data()   ;
			if (style.type == "gray" || style.type == "rgb" || style.type == "rgba") {
				pngok = this->innerData2RGBAByPeStyle2(ptr, tileData.width, tileData.height, tileData.nbands, style, rgbaData, pngLogStr);
			}
			else {
				//discrete exact linear
				pngok = this->innerData2RGBAByPeStyle(ptr, tileData.width, tileData.height, tileData.nbands, style, rgbaData, pngLogStr);
			}
		}
		break;
		case 3:
		{
			short* ptr = (short*)tileData.tiledata.data()  ;
			if (style.type == "gray" || style.type == "rgb" || style.type == "rgba") {
				pngok = this->innerData2RGBAByPeStyle2(ptr, tileData.width, tileData.height, tileData.nbands, style, rgbaData, pngLogStr);
			}
			else {
				//discrete exact linear
				pngok = this->innerData2RGBAByPeStyle(ptr, tileData.width, tileData.height, tileData.nbands, style, rgbaData, pngLogStr);
			}
		}
		break;
		case 4:
		{
			unsigned int* ptr = (unsigned int*)tileData.tiledata.data() ;
			if (style.type == "gray" || style.type == "rgb" || style.type == "rgba") {
				pngok = this->innerData2RGBAByPeStyle2(ptr, tileData.width, tileData.height, tileData.nbands, style, rgbaData, pngLogStr);
			}
			else {
				//discrete exact linear
				pngok = this->innerData2RGBAByPeStyle(ptr, tileData.width, tileData.height, tileData.nbands, style, rgbaData, pngLogStr);
			}
		}
		break;
		case 5:
		{
			int* ptr = (int*)tileData.tiledata.data()  ;
			if (style.type == "gray" || style.type == "rgb" || style.type == "rgba") {
				pngok = this->innerData2RGBAByPeStyle2(ptr, tileData.width, tileData.height, tileData.nbands, style, rgbaData, pngLogStr);
			}
			else {
				//discrete exact linear
				pngok = this->innerData2RGBAByPeStyle(ptr, tileData.width, tileData.height, tileData.nbands, style, rgbaData, pngLogStr);
			}
		}
		break;
		case 6:
		{
			float* ptr = (float*)tileData.tiledata.data()  ;
			if (style.type == "gray" || style.type == "rgb" || style.type == "rgba") {
				pngok = this->innerData2RGBAByPeStyle2(ptr, tileData.width, tileData.height, tileData.nbands, style, rgbaData, pngLogStr);
			}
			else {
				//discrete exact linear
				pngok = this->innerData2RGBAByPeStyle(ptr, tileData.width, tileData.height, tileData.nbands, style, rgbaData, pngLogStr);
			}
		}
		break;
		case 7:
		{
			double* ptr = (double*)tileData.tiledata.data() ;
			if (style.type == "gray" || style.type == "rgb" || style.type == "rgba") {
				pngok = this->innerData2RGBAByPeStyle2(ptr, tileData.width, tileData.height, tileData.nbands, style, rgbaData, pngLogStr);
			}
			else {
				//discrete exact linear
				pngok = this->innerData2RGBAByPeStyle(ptr, tileData.width, tileData.height, tileData.nbands, style, rgbaData, pngLogStr);
			}
		}
		break;
	default:
		break;
	}

	if (pngok == false) {
		if(! PixelEngine::quietMode)cout << "Error : make png failed " << pngLogStr << endl;
		error = pngLogStr;
		return false;
	}

	unsigned long  now2 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	if(! PixelEngine::quietMode)cout << "Info : make RGBA colors duration:" << now2 - now1 << " ms " << endl;

	//rgba to png
	if (rgbaData.size() == 0) {
		return false;
	}
	//rgba data to png
	this->innerRGBAData2Png(rgbaData, tileData.width, tileData.height, retPngBinary);
	return true;

}

bool PixelEngine::innerRGBAData2Png(vector<unsigned char>& rgbaData, int width, int height, vector<unsigned char>& retPngBinary) {
	unsigned long now2 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

	retPngBinary.reserve( width * height * 4 + 128);
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
	unsigned error = lodepng::encode(retPngBinary, rgbaData, width, height, state);

	unsigned long now3 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	printf("encode png:%d ms \n", (int)(now3 - now2) );//1024*1024 use 340millisec
	return true;
}

//Õë¶Ôexact£¬discrete£¬linearÈý¸öÀàÐÍ
template<typename T>
bool PixelEngine::innerData2RGBAByPeStyle(T* dataPtr, int width,int height,int nbands, PeStyle& style, vector<unsigned char>& retRGBAData, string& retLogStr) {
	int dataLen = width * height;
	int bandindex = style.getBand(0);
	if (bandindex < 0 || bandindex >= nbands) {
		stringstream ss("Error : bandindex is invalid : ");
		ss << bandindex;
		retLogStr = ss.str();
		return false;
	}
	if (style.type == "discrete" || style.type == "exact" || style.type == "linear") {
		T* newDataPtr = dataPtr + bandindex * dataLen;
		retRGBAData.resize(dataLen * 4);//RGBA
		for (int it = 0; it < dataLen; ++it)
		{
			int it1 = it * 4;
			style.value2rgba(
				newDataPtr[it],
				retRGBAData[it1],
				retRGBAData[it1 + 1],
				retRGBAData[it1 + 2],
				retRGBAData[it1 + 3]);
		}
		return true;
	}
	else {
		stringstream ss("Error : style.type=");
		ss << style.type << " not working in innerData2RGBAByPeStyle.";
		retLogStr = ss.str();
		return false;
	}
}

//Õë¶Ôgray,rgb,rgbaÈý¸öäÖÈ¾ÀàÐÍ
template<typename T>
bool PixelEngine::innerData2RGBAByPeStyle2(T* dataPtr, int width, int height, int nbands, PeStyle& style, vector<unsigned char>& retRGBAData, string& retLogStr) {
	int dataLen = width * height;
	retRGBAData.resize(dataLen * 4);//RGBA
	
	if (style.type == "gray" && style.vranges.size() > 0) {
		int bandindex = style.getBand(0);
		if (bandindex < 0 || bandindex >= nbands) {
			stringstream ss("Error : bandindex is invalid : ");
			ss << bandindex;
			retLogStr = ss.str();
			return false;
		}
		T* newDataPtr = dataPtr + bandindex * dataLen ;
		double fenmu = style.vranges[0].maxval - style.vranges[0].minval;
		for (int it = 0; it < dataLen; ++it) {
			int it1 = it * 4;
			T valx = newDataPtr[it];
			if (valx == (T)style.nodatacolor.val) {
				retRGBAData[it1] = style.nodatacolor.r; retRGBAData[it1+1] = style.nodatacolor.g;
				retRGBAData[it1+2] = style.nodatacolor.b; retRGBAData[it1+3] = style.nodatacolor.a;
			}
			else {
				if (valx <= style.vranges[0].minval) {
					retRGBAData[it1] = 0; retRGBAData[it1 + 1] = 0;
					retRGBAData[it1 + 2] = 0; retRGBAData[it1 + 3] = 255;
				}
				else if (valx >= style.vranges[0].maxval) {
					retRGBAData[it1] = 255; retRGBAData[it1 + 1] = 255;
					retRGBAData[it1 + 2] = 255; retRGBAData[it1 + 3] = 255;
				}
				else {
					if (fenmu == 0) {
						retRGBAData[it1] = 0; retRGBAData[it1 + 1] = 0;
						retRGBAData[it1 + 2] = 0; retRGBAData[it1 + 3] = 255;
					}
					else {
						int newdn = 255* (valx - style.vranges[0].minval) / fenmu;
						retRGBAData[it1] = newdn; retRGBAData[it1 + 1] = newdn;
						retRGBAData[it1 + 2] = newdn; retRGBAData[it1 + 3] = 255;
					}
				}

			}
		}
	}
	else if (style.type == "rgb" && style.vranges.size() > 2) {
		int bandindices[3];
		bandindices[0] = style.getBand(0);
		bandindices[1] = style.getBand(1);
		bandindices[2] = style.getBand(2);

		if (bandindices[0] < 0 || bandindices[0] >= nbands 
			|| bandindices[1] < 0 || bandindices[1] >= nbands
			|| bandindices[2] < 0 || bandindices[2] >= nbands) {
			stringstream ss("Error : some bandindex is invalid : ");
			ss << bandindices[0] << " " <<bandindices[1] <<" " << bandindices[2] ;
			retLogStr = ss.str();
			return false;
		}

		T* newDataPtr[3];
		newDataPtr[0] = dataPtr + bandindices[0] * dataLen;
		newDataPtr[1] = dataPtr + bandindices[1] * dataLen;
		newDataPtr[2] = dataPtr + bandindices[2] * dataLen;

		double fenmu[3];
		fenmu[0] = style.vranges[0].maxval - style.vranges[0].minval;
		fenmu[1] = style.vranges[1].maxval - style.vranges[1].minval;
		fenmu[2] = style.vranges[2].maxval - style.vranges[2].minval;
		for (int it = 0; it < dataLen; ++it) {
			int it1 = it * 4;

			if (newDataPtr[0][it] == (T)style.nodatacolor.val
				|| newDataPtr[1][it] == (T)style.nodatacolor.val
				|| newDataPtr[2][it] == (T)style.nodatacolor.val) {
				//bad pixel
				retRGBAData[it1] = style.nodatacolor.r; retRGBAData[it1 + 1] = style.nodatacolor.g;
				retRGBAData[it1 + 2] = style.nodatacolor.b; retRGBAData[it1 + 3] = style.nodatacolor.a;
			}
			else {
				//good pixel
				retRGBAData[it1 + 3] = 255;
				for (int ib = 0; ib < 3; ++ib) {
					T valx = newDataPtr[ib][it];
					if (valx <= style.vranges[ib].minval) {
						retRGBAData[it1+ib] = 0; 
					}
					else if (valx >= style.vranges[ib].maxval) {
						retRGBAData[it1 + ib] = 255;
					}
					else {
						if (fenmu[ib] == 0) {
							retRGBAData[it1 + ib] = 0;
						}
						else {
							retRGBAData[it1 + ib] = 255 * (valx - style.vranges[ib].minval) / fenmu[ib];
						}
					}
				}
			}
		}
	}
	else if (style.type == "rgba" && style.vranges.size() > 3) {
		int bandindices[4];
		bandindices[0] = style.getBand(0);
		bandindices[1] = style.getBand(1);
		bandindices[2] = style.getBand(2);
		bandindices[3] = style.getBand(3);

		if (bandindices[0] < 0 || bandindices[0] >= nbands
			|| bandindices[1] < 0 || bandindices[1] >= nbands
			|| bandindices[2] < 0 || bandindices[2] >= nbands
			|| bandindices[3] < 0 || bandindices[3] >= nbands) {
			stringstream ss("Error : some bandindex is invalid : ");
			ss << bandindices[0] << " " << bandindices[1] << " " << bandindices[2]<<" "<<bandindices[3] ;
			retLogStr = ss.str();
			return false;
		}

		T* newDataPtr[4];
		newDataPtr[0] = dataPtr + bandindices[0] * dataLen;
		newDataPtr[1] = dataPtr + bandindices[1] * dataLen;
		newDataPtr[2] = dataPtr + bandindices[2] * dataLen;
		newDataPtr[3] = dataPtr + bandindices[3] * dataLen;

		double fenmu[4];
		fenmu[0] = style.vranges[0].maxval - style.vranges[0].minval;
		fenmu[1] = style.vranges[1].maxval - style.vranges[1].minval;
		fenmu[2] = style.vranges[2].maxval - style.vranges[2].minval;
		fenmu[3] = style.vranges[3].maxval - style.vranges[3].minval;
		for (int it = 0; it < dataLen; ++it) {
			int it1 = it * 4;

			if (newDataPtr[0][it] == (T)style.nodatacolor.val
				|| newDataPtr[1][it] == (T)style.nodatacolor.val
				|| newDataPtr[2][it] == (T)style.nodatacolor.val
				|| newDataPtr[3][it] == (T)style.nodatacolor.val) {
				//bad pixel
				retRGBAData[it1] = style.nodatacolor.r; retRGBAData[it1 + 1] = style.nodatacolor.g;
				retRGBAData[it1 + 2] = style.nodatacolor.b; retRGBAData[it1 + 3] = style.nodatacolor.a;
			}
			else {
				//good pixel
				for (int ib = 0; ib < 4; ++ib) {
					T valx = newDataPtr[ib][it];
					if (valx <= style.vranges[ib].minval) {
						retRGBAData[it1 + ib] = 0;
					}
					else if (valx >= style.vranges[ib].maxval) {
						retRGBAData[it1 + ib] = 255;
					}
					else {
						if (fenmu[ib] == 0) {
							retRGBAData[it1 + ib] = 0;
						}
						else {
							retRGBAData[it1 + ib] = 255 * (valx - style.vranges[ib].minval) / fenmu[ib];
						}
					}
				}
			}
		}
		
	}
	else {
		stringstream ss("Error : style.type=");
		ss << style.type << " with vranges.size=" << style.vranges.size() << " not working in innerData2RGBAByPeStyle2.";
		retLogStr = ss.str();
		return false;
	}
	return true;
}






PixelEngine::PixelEngine() 
{
	if(! PixelEngine::quietMode)cout<<"PixelEngine()"<<endl;
	extraPointer = 0 ;
	this->tileInfo.x = 0 ;
	this->tileInfo.y = 0 ;
	this->tileInfo.z = 0 ;
	helperPointer = nullptr;
}


PixelEngine::~PixelEngine() 
{
	if(! PixelEngine::quietMode)cout<<"~PixelEngine()"<<endl;
	
}

//Ö»ÐèÒªµ÷ÓÃÒ»´Î
void PixelEngine::initV8() 
{
	// Initialize V8.
	
	if( v8Platform  ){
		if(! PixelEngine::quietMode)cout<<"v8 has inited"<<endl ;
	}else{
		if(! PixelEngine::quietMode)cout<<"init v8"<<endl ;
		v8::V8::InitializeICUDefaultLocation(".");
		v8::V8::InitializeExternalStartupData(".");
		v8Platform = v8::platform::NewDefaultPlatform();
		v8::V8::InitializePlatform(v8Platform.get());
		v8::V8::Initialize();
	}

}

//将V8 MaybeLocal<Value>转换为double
bool PixelEngine::convertV8MaybeLocalValue2Double(MaybeLocal<Value>& maybeVal,
			double& retval)
{
	if( maybeVal.IsEmpty() == false )
	{
		Local<Number> localNum ;
		if( maybeVal.ToLocal(&localNum) )
		{
			retval = localNum->Value() ;
			return true ;
		}else
		{
			return false;
		}
	}else
	{
		return false;
	}
}


/// unwrap js multipolygon object into c++ object.
bool PixelEngine::unwarpMultiPolygon(Isolate* isolate,Local<Value>& jsMulPoly,
			PeMultiPolygon& retMPoly )
{
	v8::HandleScope handle_scope(isolate);
	Local<Context> context(isolate->GetCurrentContext()) ;

	if( jsMulPoly->isArray() )
	{
		v8::Array* array0 = v8::Array::Cast( * jsMulPoly ) ;
		
		retMPoly.polys.resize( array0.Length() ) ;

		for(int i0 =0 ; i0 < array0.Length() ; ++ i0 )
		{
			MaybeLocal<Value> poly1maybe = array0->Get( context, i0) ;
			Local<Array> poly1array;
			if( poly1maybe.ToLocal(&poly1array) )
			{
				int len1 = poly1array->Length() ;
				if(! PixelEngine::quietMode)cout<<"Info : item-"<<i0<<" point count:"<<len1 <<endl;
				retMPoly.polys[i0].resize( (size_t)len1 ) ;
				for(int ipt = 0 ; ipt < len1; ++ ipt )
				{
					MaybeLocal<Value> arr2maybe = poly1array->Get(context,ipt) ;
					Local<Array> arr2local;
					if( arr2maybe.ToLocal(&arr2local) )
					{
						if( arr2local->Length() >= 2 )
						{
							MaybeLocal<Value> maybe0 = arr2local->Get(context,0);
							MaybeLocal<Value> maybe1 = arr2local->Get(context,1);
							if( convertV8MaybeLocalValue2Double( maybe0, retMPoly.polys[i0][ipt].v0 )==false )
							{
								if(! PixelEngine::quietMode)cout<<"Error : item-"<<i0<<"-"<<ipt<<" in jsMulPoly [0] is not a number"<<endl; 
								return false ;
							}
							if( convertV8MaybeLocalValue2Double( maybe1, retMPoly.polys[i0][ipt].v1 )==false )
							{
								if(! PixelEngine::quietMode)cout<<"Error : item-"<<i0<<"-"<<ipt<<" in jsMulPoly [1] is not a number"<<endl; 
								return false ;
							}
							cout<<"debug good pt:"<<retMPoly.polys[i0][ipt].v0
								<<","<<retMPoly.polys[i0][ipt].v1<<endl;
						}else{
							if(! PixelEngine::quietMode)cout<<"Error : item-"<<i0<<"-"<<ipt<<" in jsMulPoly length lower than 2"<<endl; 
							return false ;
						}
					}else
					{
						if(! PixelEngine::quietMode)cout<<"Error : item-"<<i0<<"-"<<ipt<<" in jsMulPoly is not a Array"<<endl; 
						return false ;
					}
				}
			}else
			{
				if(! PixelEngine::quietMode)cout<<"Error : item-"<<i0<<" in jsMulPoly is not a Array"<<endl; 
				return false ;
			}
		}

		return true;
	}else
	{
		if(! PixelEngine::quietMode)cout<<"Error : jsMulPoly is not Array"<<endl; 
		return false;
	}
}

//pixelengine.roi(multiPoly, zlevel, proj) 通过MultiPolygon对象生产ROI对象
void PixelEngine::GlobalFunc_RoiCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) 
{
	if(! PixelEngine::quietMode)cout<<"inside GlobalFunc_RoiCallBack"<<endl; 
	if (args.Length() < 2 ){
		if(! PixelEngine::quietMode)cout<<"Error: args.Length < 2 "<<endl ;
		return;
	}
	PixelEngine* thisPePtr = PixelEngine::getPixelEnginePointer(args);

	Isolate* isolate = args.GetIsolate() ;
	v8::HandleScope handle_scope(isolate);
	Local<Context> context(isolate->GetCurrentContext()) ;

	Local<Value> multiPoly = args[0];
	PeMultiPolygon retMPoly;
	bool polyok = unwarpMultiPolygon(isolate, multiPoly ,retMPoly );
	if( polyok==false )
	{
		if(! PixelEngine::quietMode)cout<<"Error: failed unwrap multipolygon. "<<endl ;
		return ;
	}

	int zlevel = args[1]->ToInteger(context).ToLocalChecked()->Value() ;

	string proj = "EPSG:4326";
	if( args.Length() > 2)
	{
		proj = PixelEngine::convertV8LocalValue2CppString(isolate, args[2] );
	}

	PeRoi roi ;
	roi.zlevel = zlevel;
	roi.proj = proj;
	roi.buildRoiByMulPolys( retMPoly , zlevel) ;


}


//dataset.clip() 瓦片数据集裁剪，返回新的瓦片数据集
void PixelEngine::GlobalFunc_ClipCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) 
{

}