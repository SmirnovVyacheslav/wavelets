#pragma once

#include "wavelet.h"

using namespace std;

vector<int> transform_id_map{ 0, 4, 8, 12, 16, 20, 24, 28, 32, 36 };

vector<wchar_t*> transform_name{
	L"Daubechie 2-d wavelet" ,
	L"Daubechie 4-d wavelet" ,
	L"Daubechie 6-d wavelet" ,
	L"Daubechie 8-d wavelet" ,
	L"Daubechie 10-d wavelet" ,
	L"Daubechie 12-d wavelet" ,
	L"Daubechie 14-d wavelet" ,
	L"Daubechie 16-d wavelet" ,
	L"Daubechie 18-d wavelet" ,
	L"Daubechie 20-d wavelet" };