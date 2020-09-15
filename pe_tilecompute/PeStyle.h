
#ifndef PE_STYLE_H
#define PE_STYLE_H

#include <string>
#include <vector>
#include <sstream>
#include "../../sharedcodes2/ajson5.h"
using std::stringstream;

using std::vector;
using std::string;


using namespace ArduinoJson;


namespace pe {
	struct PeColorElement {
		double val;
		unsigned char r, g, b, a;
		string lbl;
		string toJson();
		inline PeColorElement() :val(0), r(0), g(0), b(0), a(0),lbl("") { };
	};

	struct PeVRangeElement {
		double minval, maxval;
		inline PeVRangeElement() :minval(0), maxval(0) {};
		string toJson();
	};

	struct PeStyle
	{
	private:
		vector<int>bands;

	public:
		string type;//discrete , linear, exact, gray, rgb , rgba
		
		vector<PeColorElement> colors;
		PeColorElement nodatacolor;
		vector<PeVRangeElement> vranges;
		
		//获取bands数组中第index个波段索引值
		int getBand(int index);

		bool loadFromJson(string jsonText);
		string toJson();

		static PeStyle emptyStyle();

		template<typename T>
		void value2rgba(T valx, unsigned char& r, unsigned char& g, unsigned char& b, unsigned char& a);

		 

		template<typename T>
		int upper_bound(T valx);

		template<typename T>
		int binary_equal(T valx);

		template<typename T>
		static inline unsigned char clamp255(T val) {
			if (val < 0) return 0;
			else if (val > 255) return 255;
			else return (unsigned char)val;
		}
		
	};




	template<typename T>
	void PeStyle::value2rgba(T valx, unsigned char& r, unsigned char& g, unsigned char& b, unsigned char& a) {

		if (valx == (T)nodatacolor.val || colors.size()==0) {
			r = nodatacolor.r;
			g = nodatacolor.g;
			b = nodatacolor.b;
			a = nodatacolor.a;
		}
		else {
			if (type == "exact"   ) {
				int ii = this->binary_equal(valx);
				if (ii < 0)
				{
					r = nodatacolor.r;
					g = nodatacolor.g;
					b = nodatacolor.b;
					a = nodatacolor.a;
				}
				else
				{
					r = colors[ii].r; g = colors[ii].g; b = colors[ii].b; a = colors[ii].a;
				}

			}
			else if (type == "linear" ) {
				if (valx <= (T)colors[0].val)
				{
					r = colors[0].r; g = colors[0].g; b = colors[0].b; a = colors[0].a;
				}
				else if (valx >= (T)(colors.end() - 1)->val)
				{
					std::vector<PeColorElement>::iterator it = colors.end() - 1;
					r = it->r; g = it->g; b = it->b; a = it->a;
				}
				else
				{
					int ii = this->upper_bound(valx);
					int ii0 = ii - 1;
					if (ii0 >= 0)
					{
						float wup = (valx - colors[ii0].val) * 1.0 / (colors[ii].val - colors[ii0].val);
						float wl = 1.f - wup;
						r = PeStyle::clamp255(colors[ii0].r * wl + colors[ii].r * wup);
						g = PeStyle::clamp255(colors[ii0].g * wl + colors[ii].g * wup);
						b = PeStyle::clamp255(colors[ii0].b * wl + colors[ii].b * wup);
						a = PeStyle::clamp255(colors[ii0].a * wl + colors[ii].a * wup);
					}
					else
					{
						r = colors[ii].r; g = colors[ii].g; b = colors[ii].b; a = colors[ii].a;
					}
				}

			}
			else if( type=="discrete" ) {
				//discrete
				if (valx <= (T)colors[0].val)
				{
					r = colors[0].r; g = colors[0].g; b = colors[0].b; a = colors[0].a;
				}
				else if (valx >= (T)(colors.end() - 1)->val)
				{
					std::vector<PeColorElement>::iterator it = colors.end() - 1;
					r = it->r; g = it->g; b = it->b; a = it->a;
				}
				else
				{
					int ii = this->upper_bound(valx);
					r = colors[ii].r; g = colors[ii].g; b = colors[ii].b; a = colors[ii].a;

				}
			}
		}
	}


	template<typename T>
	int PeStyle::upper_bound(T valx) {
		if (this->colors.size() < 2)
		{
			return 0;
		}
		int i0 = 0;
		int i1 = this->colors.size() - 1;
		while (i0 < i1)
		{
			if (i0 == i1 - 1)
			{
				if (valx == (T)this->colors[i0].val) return i0;
				else return i1;
			}
			int ic = (i1 + i0) / 2;
			if (valx == (T)this->colors[ic].val)
			{
				return ic;
			}
			else if (valx > (T)this->colors[ic].val)
			{
				i0 = ic;
			}
			else {
				i1 = ic;
			}
		}
		if (valx == (T)this->colors[i0].val)
		{
			return i0;
		}
		else
		{
			return i1;
		}
	}

	template<typename T>
	int PeStyle::binary_equal(T val) {
		if (this->colors.size() == 0)
		{
			return -1;
		}
		else if (this->colors.size() == 1)
		{
			if (val == this->colors[0].val)
			{
				return 0;
			}
			else
			{
				return -1;
			}
		}
		else
		{
			int i0 = 0;
			int i1 = this->colors.size() - 1;
			while (i0 < i1)
			{
				if (i0 == i1 - 1)
				{
					if (val == (T)this->colors[i0].val) return i0;
					else if (val == (T)this->colors[i1].val) return i1;
					else return -1;
				}
				int ic = (i1 + i0) / 2;
				if (val == (T)this->colors[ic].val)
				{
					return ic;
				}
				else if (val > (T)this->colors[ic].val)
				{
					i0 = ic;
				}
				else {
					i1 = ic;
				}
			}
			if (val == (T)this->colors[i0].val)
			{
				return i0;
			}
			else if (val == (T)this->colors[i1].val)
			{
				return i1;
			}
			else
			{
				return -1;
			}
		}
	}

}


#endif
