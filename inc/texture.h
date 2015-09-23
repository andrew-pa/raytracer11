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
		propr(virtual PixIndexT, size, = 0);
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
			c = clamp(c, vec2(0, 0), vec2(1, 1));
			uvec2 ic = floor(c*(vec2)_size);
			if(ic.x == _size.x) ic.x--;
			if(ic.y == _size.y) ic.y--;
			//cout << "read texel @ " << c.x << ", " << c.y << " === " << ic.x << ", " << ic.y << endl;
			return pixel(ic);
		}

		propr(uvec2, size, override{ return _size;  })

		void write_bmp(const string& bmp_filename);
		void draw_text(const string& text, uvec2 pos, vec3 color);

		virtual ~texture2d();
	};

	class checker_texture : public texture<vec3, uvec2, vec2>
	{
		float _checker_size;
		vec3 colors[2];
	public:
		proprw(vec3, color0, { return colors[0]; });
		proprw(vec3, color1, { return colors[1]; });
		proprw(float, checker_size, { return _checker_size; });

		checker_texture(vec3 c0 = vec3(1,1,0), vec3 c1 = vec3(0,1,0), float cs = 8.f)
			: _checker_size(cs)
		{
			colors[0] = c0;
			colors[1] = c1;
		}

		//don't use this!
		inline vec3& pixel(uvec2 xy)
		{
			return texel((vec2)xy / vec2(64, 64));
		}

		inline vec3& texel(vec2 uv)
		{
			int ch = fabsf((int)(floor(uv.x*_checker_size)
				+ floor(uv.y*_checker_size)));
			return colors[ch%2];
		}

		propr(uvec2, size, override{ return uvec2(64, 64); });
	};

	/*class noise_texture : public texture<vec3, uvec2, vec2>
	{
		vec2 _ns;
		vec3 _c;
		bool _absv;
	public:
		proprw(vec2, noise_scale, { return _ns; });
		proprw(vec3, color, { return _c; });
		proprw(bool, absolute_noise, { return _absv; });

		noise_texture(vec3 c, vec2 ns = vec2(10), bool ab = false)
			: _ns(ns), _c(c), _absv(ab)
		{
		}

		//don't use this!
		inline vec3& pixel(uvec2 xy)
		{
			return texel((vec2)xy / vec2(64, 64));
		}

		inline vec3& texel(vec2 uv)
		{
			float p = perlin(uv*_ns);
			if (_absv)
				p = fabsf(p);
			else
				p = .8f * (p - 1);
			return p*_c;

		}

		propr(uvec2, size, override{ return uvec2(64, 64); });
	};*/
}
