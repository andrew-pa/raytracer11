#include "texture.h"
#include "soil/SOIL.h"

namespace raytracer11
{
	texture2d::texture2d(const string& filename)
	{
		int width, height, chanl;
		auto data = SOIL_load_image(filename.c_str(), &width, &height, &chanl, SOIL_LOAD_AUTO);

		//if (chanl < 3) throw rexception("not enough channels");

		//unsigned char header[54];
		//unsigned int dataPos;
		//unsigned int imageSize;
		//unsigned int width, height;

		//unsigned char * data;

		//FILE * file;
		//#ifdef _MSVC_
		//fopen_s(&file, bmp_filename.c_str(), "rb");
		//#else
		//file = fopen(bmp_filename.c_str(), "rb");
		//#endif
		//if (!file) throw rexception((string("couldn't open file ") + bmp_filename));
		//if (fread(header, 1, 54, file) != 54)
		//	throw rexception("invalid BMP header");

		////// A BMP files always begins with "BM"

		//if (header[0] != 'B' || header[1] != 'M')
		//	throw rexception("invalid BMP file");

		////// Make sure this is a 24bpp file

		//if (*(int*)&(header[0x1E]) != 0)          throw rexception("Not a correct BMP file\n"); 
		//if (*(int*)&(header[0x1C]) != 24)         throw rexception("Not a correct BMP file\n");

		//dataPos = *(int*)&(header[0x0A]);
		//imageSize = *(int*)&(header[0x22]);
		//width = *(int*)&(header[0x12]);
		//height = *(int*)&(header[0x16]);
		//_size = uvec2(width, height);
		//

		//if (imageSize == 0)    imageSize = width*height * 3;
		//if (dataPos == 0)      dataPos = 54;

		//data = new unsigned char[imageSize];
		//fread(data, 1, imageSize, file);
		//fclose(file);

		_size = uvec2(width, height);

		_pixels = new vec3[(width*height)];

		int j = 0;
		for (int i = 0; i < width*height*chanl; i+=chanl)
		{
			vec3 d(0);
			d.b = (float)(data[i + 2]) / 255.f;
			d.g = (float)(data[i + 1]) / 255.f;
			d.r = (float)(data[i]) / 255.f;
			_pixels[j] = d;
			j++;
		}

	}

	void texture2d::write_bmp(const string& filename)
	{
		unsigned char* imgdata = new unsigned char[_size.x*_size.y * 4];
		for (int y = 0; y < _size.y * 4; y += 4)
		{
			for (int x = 0; x < _size.x *4; x += 4)
			{
				vec3 d = clamp(_pixels[(y*_size.x + x) / 4], vec3(0), vec3(1));
				imgdata[(y*_size.x + x)] = (unsigned char)(d.r * 255.f);
				imgdata[(y*_size.x + x) + 1] = (unsigned char)(d.g * 255.f);
				imgdata[(y*_size.x + x) + 2] = (unsigned char)(d.b * 255.f);
				imgdata[(y*_size.x + x) + 3] = 255;
			}
		}
		SOIL_save_image(filename.c_str(), SOIL_SAVE_TYPE_BMP, _size.x, _size.y, 4, imgdata);
	}

	void texture2d::draw_text(const std::string& text, uvec2 pos, vec3 color)
	{
		const int char_width = 5;
		const int char_height = 7;
		static std::map<char, string>* chars = nullptr;
		if (chars == nullptr)
		{
			//bulid chars map
			chars = new std::map<char, string>;
#pragma region ABCs
			(*chars)['A'] =
				"xxxxx"
				"x...x"
				"x...x"
				"xxxxx"
				"x...x"
				"x...x"
				"x...x";
			(*chars)['B'] =
				"xxxx."
				"x...x"
				"x...x"
				"xxxx."
				"x...x"
				"x...x"
				"xxxx.";
			(*chars)['C'] =
				".xxxx"
				"x...."
				"x...."
				"x...."
				"x...."
				"x...."
				".xxxx";
			(*chars)['D'] =
				"xxxx."
				"x...x"
				"x...x"
				"x...x"
				"x...x"
				"x...x"
				"xxxx.";
			(*chars)['E'] =
				"xxxxx"
				"x...."
				"x...."
				"xxxxx"
				"x...."
				"x...."
				"xxxxx";
			(*chars)['F'] =
				"xxxxx"
				"x...."
				"x...."
				"xxxxx"
				"x...."
				"x...."
				"x....";
			(*chars)['G'] =
				"xxxxx"
				"x...."
				"x...."
				"x..xx"
				"x...x"
				"x...x"
				"xxxxx";
			(*chars)['H'] =
				"x...x"
				"x...x"
				"x...x"
				"xxxxx"
				"x...x"
				"x...x"
				"x...x";
			(*chars)['I'] =
				"xxxxx"
				"..x.."
				"..x.."
				"..x.."
				"..x.."
				"..x.."
				"xxxxx";
			(*chars)['J'] =
				"xxxxx"
				"...x."
				"...x."
				"...x."
				"...x."
				"x..x."
				".xx..";
			(*chars)['K'] =
				"x...x"
				"x..x."
				"x.x.."
				"xx..."
				"x.x.."
				"x..x."
				"x...x";
			(*chars)['L'] =
				"x...."
				"x...."
				"x...."
				"x...."
				"x...."
				"x...."
				"xxxxx";
			(*chars)['M'] =
				"xxxxx"
				"x.x.x"
				"x.x.x"
				"x.x.x"
				"x.x.x"
				"x...x"
				"x...x";
			(*chars)['N'] =
				"x...x"
				"xx..x"
				"xx..x"
				"x.x.x"
				"x..xx"
				"x..xx"
				"x...x";
			(*chars)['O'] =
				"xxxxx"
				"x...x"
				"x...x"
				"x...x"
				"x...x"
				"x...x"
				"xxxxx";
			(*chars)['P'] =
				"xxxxx"
				"x...x"
				"x...x"
				"xxxxx"
				"x...."
				"x...."
				"x....";
			(*chars)['Q'] =
				"xxxxx"
				"x...x"
				"x...x"
				"x...x"
				"x...x"
				"xxxx."
				"....x";
			(*chars)['R'] =
				"xxxxx"
				"x...x"
				"x...x"
				"xxxxx"
				"x.x.."
				"x..x."
				"x...x";
			(*chars)['S'] =
				"xxxxx"
				"x...."
				"x...."
				"xxxxx"
				"....x"
				"....x"
				"xxxxx";
			(*chars)['T'] =
				"xxxxx"
				"..x.."
				"..x.."
				"..x.."
				"..x.."
				"..x.."
				"..x..";
			(*chars)['U'] =
				"x...x"
				"x...x"
				"x...x"
				"x...x"
				"x...x"
				"x...x"
				"xxxxx";
			(*chars)['V'] =
				"x...x"
				"x...x"
				"x...x"
				"x...x"
				"x...x"
				".x.x."
				"..x..";
			(*chars)['W'] =
				"x...x"
				"x...x"
				"x...x"
				"x.x.x"
				"x.x.x"
				"x.x.x"
				"xxxxx";
			(*chars)['X'] =
				"x...x"
				".x.x."
				".x.x."
				"..x.."
				".x.x."
				".x.x."
				"x...x";
			(*chars)['Y'] =
				"x...x"
				"x...x"
				"x...x"
				"xxxxx"
				"..x.."
				"..x.."
				"..x..";
			(*chars)['Z'] =
				"xxxxx"
				"....x"
				"...x."
				"...x."
				"..x.."
				".x..."
				"xxxxx";

			(*chars)['0'] =
				"xxxxx"
				"xx..x"
				"x.x.x"
				"x.x.x"
				"x.x.x"
				"x..xx"
				"xxxxx";
			(*chars)['1'] =
				".xx.."
				"x.x.."
				"..x.."
				"..x.."
				"..x.."
				"..x.."
				"xxxxx";
			(*chars)['2'] =
				".xxx."
				"x...x"
				"....x"
				"...x."
				".xx.."
				"x...."
				"xxxxx";
			(*chars)['3'] =
				"xxxx."
				"....x"
				"....x"
				"..xx."
				"....x"
				"....x"
				"xxxx.";
			(*chars)['4'] =
				"...xx"
				".x..x"
				"x...x"
				"xxxxx"
				"....x"
				"....x"
				"....x";
			(*chars)['5'] =
				"xxxxx"
				"x...."
				"x...."
				"xxxx."
				"....x"
				"....x"
				"xxxx.";
			(*chars)['6'] =
				".xxxx"
				"x...."
				"x...."
				"xxxx."
				"x...x"
				"x...x"
				".xxx.";
			(*chars)['7'] =
				"xxxxx"
				"....x"
				"...x."
				"...x."
				"..x.."
				".x..."
				"x....";
			(*chars)['8'] =
				".xxx."
				"x...x"
				"x...x"
				".xxx."
				"x...x"
				"x...x"
				".xxx.";
			(*chars)['9'] =
				".xxx."
				"x...x"
				"x...x"
				".xxxx"
				"....x"
				"....x"
				"xxxx.";
			(*chars)[':'] =
				"....."
				"..x.."
				"..x.."
				"....."
				"..x.."
				"..x.."
				".....";
			(*chars)['.'] =
				"....."
				"....."
				"....."
				"....."
				"....."
				"xx..."
				"xx...";
			(*chars)['-'] =
				"....."
				"....."
				"....."
				"xxxxx"
				"....."
				"....."
				".....";
			(*chars)['['] =
				"xxxxx"
				"x...."
				"x...."
				"x...."
				"x...."
				"x...."
				"xxxxx";
			(*chars)[']'] =
				"xxxxx"
				"....x"
				"....x"
				"....x"
				"....x"
				"....x"
				"xxxxx";
			(*chars)['('] =
				"..xxx"
				".x..."
				"x...."
				"x...."
				"x...."
				".x..."
				"..xxx";
			(*chars)[')'] =
				"xxx.."
				"...x."
				"....x"
				"....x"
				"....x"
				"...x."
				"xxx..";
			(*chars)[','] =
				"....."
				"....."
				"....."
				"....."
				"....."
				".x..."
				"xx...";
			(*chars)['!'] =
				"..x.."
				"..x.."
				"..x.."
				"..x.."
				"..x.."
				"....."
				"..x..";
			(*chars)['_'] =
				"....."
				"....."
				"....."
				"....."
				"....."
				"....."
				"xxxxx";

			(*chars)['/'] =
				"....x"
				"....x"
				"...x."
				"..x.."
				".x..."
				"x...."
				"x....";

			(*chars)['\\'] =
				"x...."
				"x...."
				".x..."
				"..x.."
				"...x."
				"....x"
				"....x";
#pragma endregion
		}
		uvec2 texpos = pos;
		for (auto cv : text)
		{
			auto c = toupper(cv);
			if (c == ' ')
			{
				texpos.x += char_width + 2;
				continue;
			}
			if (c == '\n')
			{
				texpos.x = pos.x;
				texpos.y += char_height + 1;
				continue;
			}
			string chrm = (*chars)[c];
			uvec2 cpos = uvec2(0, 0);
			for (auto c : chrm)
			{
				if (c == 'x')
				{
					auto c = cpos + texpos;
					_pixels[c.x + c.y*_size.x] = color;
				}
				cpos.x++;
				if (cpos.x >= char_width)
				{
					cpos.x = 0;
					cpos.y++;
					if (cpos.y > char_height)
						break;
				}
			}
			texpos.x += char_width + 2;
		}

	}

	texture2d::~texture2d()
	{
		delete[] _pixels;
	}
}
