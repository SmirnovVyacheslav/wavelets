#pragma once

#include <math.h>
#include <iostream>
#include <fstream>
#include <ctime>
#include <vector>
#include <memory>
#include <regex>
#include <ctime>

#include "sequence.h"

using namespace std;

#define DIRECT  0
#define REVERSE 2
#define FULL   true
#define N_FULL false
#define W_PACK   true
#define N_W_PACK false

#define MIN 0.00000001

extern vector<int> transform_id_map;
extern vector<wchar_t*> transform_name;
extern vector< vector<double> > sequence;

class Wavelet
{
	template <class T>
	class Data
	{
		class iterator
		{
			typename vector<T>::iterator it, it_end;

			int step, width, height, row_width, size, position;

		public:

			iterator(typename vector<T>& data, int pos, int step, int w, int h, int r_w) : step(step), width(w), height(h), row_width(r_w)
			{
				it = data.begin();
				it_end = data.end();
				advance(it, pos);
				size = data.size();
				position = pos;
			}

			typename vector<T>::iterator end()
			{
				return it_end;
			}

			T& operator()(int j, int i)
			{
				return it[(height + j) % height * row_width + (width + i) % width];
			}

			T& operator[](int i)
			{
				return it[i];
			}

			typename vector<T>::iterator& operator*()
			{
				return it;
			}

			void operator++()
			{
				position += step;
				if (position >= size) it = it_end;
				if (it != it_end) it += step;
			}

			void operator--()
			{
				it -= step;
			}
		};

		long zero_num;
		bool full, w_pack;
		double max_i, zero_prs, filter_value;
		int transform, direction, cur_layer, width, height,
			row_width, c_width, c_height, layer, square;

		vector<T> tmp;
		vector<T> data;
		vector<pair<int, int>> full_size, w_size;

		friend class Wavelet;

		iterator& get_tmp_iterator(int pos = 0, int step = 1)
		{
			return iterator(tmp, pos, step, width, height, row_width);
		}

		void reduce_width()
		{
			int t_width = width / 2;
			width = t_width % 2 ? t_width - 1 : t_width;
		}

		void reduce_height()
		{
			int t_height = height / 2;
			height = t_height % 2 ? t_height - 1 : t_height;
		}

	public:

		Data() : full(N_FULL), w_pack(N_W_PACK), transform(0), direction(REVERSE), cur_layer(0),
			zero_num(0), zero_prs(0.0), filter_value(0.0) {}

		template <class U>
		Data(const Data<U>& src) :
			full(src.full), w_pack(src.w_pack), transform(src.transform), direction(src.direction),
			cur_layer(src.cur_layer), width(src.width), height(src.height), row_width(src.row_width),
			c_width(src.c_width), c_height(src.c_height), layer(src.layer), square(src.square), max_i(src.max_i),
			zero_num(src.zero_num), zero_prs(src.zero_prs), filter_value(src.filter_value)
		{
			data = vector<T>(src.data.size());
			for (int i = 0; i < data.size(); i++)
				data[i] = static_cast<T>(src.data[i]);
			tmp = vector<T>(src.tmp.size());
			full_size = src.full_size;
			w_size = src.w_size;
		}

		bool Set_full()
		{
			if (direction == REVERSE) full = full == FULL ? N_FULL : FULL;
			return full;
		}

		bool Set_w_pack()
		{
			if (direction == REVERSE) w_pack = w_pack == W_PACK ? N_W_PACK : W_PACK;
			return w_pack;
		}

		int get_width()
		{
			return c_width;
		}

		int get_height()
		{
			return c_height;
		}

		int get_layer()
		{
			return layer;
		}

		int get_cur_layer()
		{
			return cur_layer;
		}

		double get_max_i()
		{
			return max_i;
		}

		long get_zero_num()
		{
			return zero_num;
		}

		double get_zero_prs()
		{
			return zero_prs;
		}

		iterator& get_iterator(int pos = 0, int step = 1)
		{
			return iterator(data, pos, step, width, height, row_width);
		}

		void data_dispersion(float value, vector<int>& plot_y)
		{
			for (auto i : data)
				if (abs(i) < 1.0)
					plot_y[int(abs(i) / value)]++;
		}

		void set_filter_value(double value)
		{
			filter_value = value;
		}

		double get_filter_value()
		{
			return filter_value;
		}
	};

	unsigned int time_begin, time_end;

	unique_ptr<Data<double>> image;
	unique_ptr<Data<double>> origin;

	wsmatch match;
	wstring file, result;

	void transpose(Data<double>::iterator& it)
	{
		auto tmp_it = image->get_tmp_iterator();
		int rw = image->width / 2, rh = image->height / 2;

		for (int j = 0; j < image->height; ++j)
			for (int i = 0; i < image->width; ++i)
				tmp_it(j, i) = it(j, i);

		for (int fj = 0, sj = 0; fj < image->height; fj+= 2, ++sj)
			for (int fi = 0, si = 0; fi < image->width; fi+= 2, ++si)
				if (image->direction == DIRECT)
				{
					it(sj, si)           = tmp_it(fj, fi);
					it(sj + rh, si)      = tmp_it(fj + 1, fi);
					it(sj, si + rw)      = tmp_it(fj, fi + 1);
					it(sj + rh, si + rw) = tmp_it(fj + 1, fi + 1);
				}
				else
				{
					it(fj, fi)         = tmp_it(sj, si);
					it(fj + 1, fi)     = tmp_it(sj + rh, si);
					it(fj, fi + 1)     = tmp_it(sj, si + rw);
					it(fj + 1, fi + 1) = tmp_it(sj + rh, si + rw);
				}
	}

	void trancform(Data<double>::iterator& it, vector<double>& seq_l, vector<double>& seq_h)
	{
		if (image->direction == REVERSE) transpose(it);

		int size = seq_l.size();
		int delta = image->direction == DIRECT ? 0 : size - 2;
		
		auto tmp_it = image->get_tmp_iterator();

		for (int j = 0; j < image->height; j++)
		{
			for (int i = 0; i < image->width; i+= 2)
			{
				tmp_it(j, i) = 0;
				tmp_it(j, i + 1) = 0;

				for (int seq_id = 0; seq_id < size; seq_id++)
				{
					tmp_it(j, i)     += it(j, i + seq_id - delta) * seq_l[seq_id];
					tmp_it(j, i + 1) += it(j, i + seq_id - delta) * seq_h[seq_id];
				}
			}
		};

		for (int i = 0; i < image->width; i++)
		{
			for (int j = 0; j < image->height; j+= 2)
			{
				it(j, i) = 0;
				it(j + 1, i) = 0;

				for (int seq_id = 0; seq_id < size; seq_id++)
				{
					it(j, i)     += tmp_it(j + seq_id - delta, i) * seq_l[seq_id];
					it(j + 1, i) += tmp_it(j + seq_id - delta, i) * seq_h[seq_id];
				}
			}
		};

		if (image->direction == DIRECT) transpose(it);
	}

	void transform(vector<double>& seq_l, vector<double>& seq_h, int pos, bool use_full_size)
	{
		int& width = image->width;
		int& height = image->height;
		int size_w = image->full ? seq_l.size() : width - 1;
		int size_h = image->full ? seq_l.size() : height - 1;
		vector<pair<int, int>>& full_size = use_full_size ? image->full_size : image->w_size;

		if (image->direction == DIRECT)
			if (use_full_size)
			{
				for (width, height; width > size_w && height > size_h; image->reduce_width(), image->reduce_height())
				{
					if (!full_size.empty()) image->w_size.push_back(make_pair(width, height));

					for (auto it = image->get_iterator(pos, image->square); *it != it.end(); ++it)
						trancform(it, seq_l, seq_h);

					full_size.push_back(make_pair(width, height));
				}
				if (!image->full) image->w_size.push_back(make_pair(width, height));
			}
			else
				for (auto size_it = full_size.begin(); size_it != full_size.end(); ++size_it)
				{
					width = (*size_it).first;
					height = (*size_it).second;

					for (auto it = image->get_iterator(pos, image->square); *it != it.end(); ++it)
						trancform(it, seq_l, seq_h);
				}
		else
			for (auto size_it = full_size.rbegin(); size_it != full_size.rend(); ++size_it)
			{
				width = (*size_it).first, height = (*size_it).second;
				for (auto it = image->get_iterator(pos, image->square); *it != it.end(); ++it)
					trancform(it, seq_l, seq_h);
			}
	}

	void set_zero_num()
	{
		image->zero_num = 0;
		for (auto i: image->data)
			if (abs(i) < MIN)
				++image->zero_num;
		image->zero_prs = double(image->zero_num) / double(image->square * image->layer);
	}

	void set_filter_value()
	{
		auto dispersion = vector<int>(10000);

		for (auto i : image->data)
			if (abs(i) < 1.0)
				dispersion[static_cast<int>(abs(i) * 10000)]++;

		auto next_non_zero = [&dispersion](int i) {while (dispersion[i] == 0) i++; return i;};

		int i = next_non_zero(0);
		int j = next_non_zero(i + 1);

		while (abs(1.0f - float(dispersion[j]) / float(dispersion[i])) > 0.01)
		{
			i = j;
			j = next_non_zero(i + 1);
		}
		image->filter_value = static_cast<double>(i) / 10000;
	}

public:

	Wavelet(const wstring file) : file(file)
	{
		read();
	}

	~Wavelet() {}

	Data<double>* operator->()
	{
		return &*image;
	}

	void operator++()
	{
		image->cur_layer = ++(image->cur_layer) == image->layer ? image->cur_layer - 1 : image->cur_layer;
	}

	void operator--()
	{
		image->cur_layer = --(image->cur_layer) == -1 ? 0 : image->cur_layer;
	}

	unsigned int transform(int direction, int transform_id = -1)
	{
		time_begin = clock();

		if (image->direction == direction) return clock() - time_begin;

		image->direction = direction;
		image->transform = direction == DIRECT ? transform_id : image->transform;

		vector<double>& seq_l = sequence[transform_id_map[image->transform] + direction];
		vector<double>& seq_h = sequence[transform_id_map[image->transform] + direction + 1];

		if (image->direction == DIRECT) transform(seq_l, seq_h, 0, true);
		
		if (image->w_pack == W_PACK)
		{
			transform(seq_l, seq_h, image->w_size[0].first, false);
			transform(seq_l, seq_h, image->w_size[0].second * image->c_width, false);
			transform(seq_l, seq_h, image->w_size[0].second * image->c_width + image->w_size[0].first, false);
		}
		
		if (image->direction == REVERSE)
		{
			transform(seq_l, seq_h, 0, true);
			image->full_size.clear();
			image->w_size.clear();
			image->set_filter_value(0.0);
		}

		time_end = clock();

		set_zero_num();

		if (image->direction == DIRECT) set_filter_value();
		else image->set_filter_value(0.0);

		return time_end - time_begin;
	}

	void filter()
	{
		for (auto it = image->get_iterator(); *it != it.end(); ++it)
			if (abs(**it) < image->get_filter_value())
				**it = 0.0;
		set_zero_num();
	}

	void inc_filter_value()
	{
		double step = 0.1;
		double curr_filter_value = image->get_filter_value();
		
		if (curr_filter_value != 0.0)
		{
			int i = 10;
			while (curr_filter_value * i < 1.0) i *= 10;
			step = 1.0 / i;
		}

		image->set_filter_value(curr_filter_value + step);
	}

	void dec_filter_value()
	{
		double step = 0.1;
		double curr_filter_value = image->get_filter_value();

		if (curr_filter_value != 0.0)
		{
			int i = 10;
			while (curr_filter_value * i < 1.0) i *= 10;
			step = 1.0 / i;
		}

		image->set_filter_value(curr_filter_value - step);
	}

	void read()
	{
		fstream fp;

		origin.reset(new Data<double>);
		fp.open(file, ios::in|ios::binary);
		
		fp.read((char*)&origin->width, sizeof(int));
		fp.read((char*)&origin->height, sizeof(int));
		fp.read((char*)&origin->layer, sizeof(int));

		origin->c_height = origin->height;
		origin->row_width = origin->c_width = origin->width;
		origin->square = origin->width * origin->height;

		origin->tmp = vector<double>(origin->square);
		origin->data = vector<double>(origin->square * origin->layer);

		auto it = origin->get_iterator();

		if (regex_match(file.cbegin(), file.cend(), match, static_cast<regex>(".*\\.arch$")))
		{
			double offset, buf;
			int size, tmp_width, tmp_height;

			fp.read((char*)&size, sizeof(int));
			for (int i = 0; i < size; i++)
			{
				fp.read((char*)&tmp_width, sizeof(int));
				fp.read((char*)&tmp_height, sizeof(int));
				origin->full_size.push_back(make_pair(tmp_width, tmp_height));
			}

			fp.read((char*)&offset, sizeof(double));

			while (fp)
			{
				fp.read((char*)&buf, sizeof(double));
				if (buf < 0.0) while (buf++ < 0.0) **it = 0.0;
				else **it = buf + offset;
				if (*it != it.end()) ++it;
			}
		}
		else
		{
			short buf;

			while (fp)
			{
				fp.read((char*)&buf, sizeof(short));
				if (buf < 0) while (buf++ < 0) **it = 0.0;
				else **it = static_cast<double>(buf);
				if (*it != it.end()) ++it;
			}
		}

		origin->max_i = static_cast<double>(*max_element(origin->data.cbegin(), origin->data.cend()));

		for (it = origin->get_iterator(); *it != it.end(); ++it)
			**it /= origin->max_i;

		image.reset(new Data<double>(*origin));
		
		set_zero_num();

		fp.close();
	}

	void write()
	{
		std::fstream fp;
		
		result = file + L"_result" + (image->full ? L".arch" : L".raw");
		
		fp.open(result, std::ios::out | std::ios::binary);

		fp.write((char*)&image->width, sizeof(int));
		fp.write((char*)&image->height, sizeof(int));
		fp.write((char*)&image->layer, sizeof(int));

		if (image->full)
		{
			int size = image->full_size.size();
			fp.write((char*)&size, sizeof(int));

			for (int i = 0; i < size; i++)
			{
				fp.write((char*)&(image->full_size[i].first), sizeof(int));
				fp.write((char*)&(image->full_size[i].second), sizeof(int));
			}

			double offset = 0.0, buf;
			auto it = image->get_iterator();

			for (it; *it != it.end(); ++it)
				if (**it < offset) offset = **it;
				
			fp.write((char*)&offset, sizeof(double));

			double zero = 0;
			for (it; *it != it.end(); ++it)
				if (**it == 0.0) zero--;
				else
				{
					buf = **it - offset;
					if (zero != 0.0) fp.write((char*)&zero, sizeof(double));
					fp.write((char*)&buf, sizeof(double));
					zero = 0.0;
				}
		}
		else
		{
			short zero = 0, buf = 0;
			auto it = image->get_iterator();
			for (it; *it != it.end(); ++it)
				if (**it == 0.0) zero--;
				else
				{
					if (zero != 0) fp.write((char*)&zero, sizeof(short));
					fp.write((char*)&(**it), sizeof(short));
					zero = 0;
				}
		}

		fp.close();
	}

	void source()
	{
		image.reset(new Data<double>(*origin));
		set_zero_num();
	}
};