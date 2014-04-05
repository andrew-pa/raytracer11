#pragma once
#include "cmmn.h"

namespace raytracer11
{
	template<typename PixelType, typename PixIndexT, typename TexIndexT>
	class texture
	{
	public:
		virtual PixelType& pixel(PixIndexT c) = 0;
		virtual PixelType& texel(TexIndexT c) = 0;
		propr(virtual PixIndexT, size, const = 0);
		virtual ~texture(){}
	};

	class texture2d : public texture<vec3, uvec2, vec2>
	{
	protected:
		vec3* _pixels;
		uvec2 _size;
	public:
		texture2d(uvec2 _s)
			: _size(_s), _pixels(new vec3[_s.x*_s.y])
		{}
		texture2d(const string& bmp_filename);

		inline vec3& pixel(uvec2 c) override
		{
			return _pixels[c.x + c.y*_size.x];
		}

		inline vec3& texel(vec2 c) override
		{
			clamp(c, vec2(0, 0), vec2(1, 1));
			uvec2 ic = floor(c*(vec2)_size);
			return pixel(ic);
		}

		propr(uvec2, size, override{ return _size;  })

		void write_bmp(const string& bmp_filename);
		void draw_text(const string& text, uvec2 pos, vec3 color);

		virtual ~texture2d();
	};
}

