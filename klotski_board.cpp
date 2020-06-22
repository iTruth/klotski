/* Copyright [2020] [iTruth]

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.  */

#include "klotski_board.h"
#include <algorithm>
#include <iostream> 
#include <fstream>
#include <random>
#include <iomanip>
#include <limits>
#include <sstream>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>

std::mt19937 klotski_board::random_engine{static_cast<std::mt19937::result_type>(time(nullptr))};

size_t klotski_board::situation_type_hash::operator()(const situation_type& situation) const noexcept{
	size_t seed = 0;
	int n = 1;
	for(const auto& i: situation){
		for(const auto& j: i){
			seed += j * n++;
		}
	}
	return seed;
}

klotski_board::klotski_board(std::string situation_string, int dx, int dy, const char split):
	dx(dx), dy(dy),
	x_distribution(std::uniform_int_distribution<>(0, dx-1)),
	y_distribution(std::uniform_int_distribution<>(0, dy-1)){
		resize_board_to_fit();
		std::replace(situation_string.begin(), situation_string.end(), split, ' ');
		std::stringstream ss(situation_string);
		for(auto& i: situation){
			for(int& n: i){
				if(ss.eof()){
					std::stringstream ss;
					ss<<"situation is not fit for "<<dx<<"x"<<dy<<" board";
					throw std::runtime_error(ss.str().c_str());
				}
				ss>>n;
				if(ss.fail()){
					throw std::runtime_error("situation is not valid");
				}

			}
		}
	}

klotski_board::klotski_board(std::fstream& file, int dx, int dy):
	dx(dx), dy(dy),
	x_distribution(std::uniform_int_distribution<>(0, dx-1)),
	y_distribution(std::uniform_int_distribution<>(0, dy-1)){
		if(!file.is_open()){
			throw std::runtime_error("file is not open");
		}
		resize_board_to_fit();
		for(auto& i:situation){
			for(int& n: i){
				if(file.eof()){
					throw std::runtime_error("file is not fit");
				}
				file>>n;
				if(file.fail()){
					throw std::runtime_error("file is not a valid board file");
				}

			}
		}
	}

klotski_board::klotski_board(int dx, int dy):
	dx(dx), dy(dy),
	x_distribution(std::uniform_int_distribution<>(0, dx-1)),
	y_distribution(std::uniform_int_distribution<>(0, dy-1)){
		reset();
	}

void klotski_board::reset() noexcept{
	resize_board_to_fit();
	int inc = 1;
	for(auto& i: situation){
		for(int& n: i){
			n = inc++;
		}
	}
	situation[dy - 1][dx - 1] = 0;
	zero_x = dx - 1;
	zero_y = dy - 1;
}

void klotski_board::upset(int depth) noexcept{
	bool is_horizontal = orientation_distribution(random_engine);
	for (int i = 0; i < depth; ++i) {
		if(is_horizontal){
			while(!move_item(x_distribution(random_engine), zero_y));
		}else{
			while(!move_item(zero_x, y_distribution(random_engine)));
		}
		is_horizontal = !is_horizontal;
	}
}

void klotski_board::print_board(bool display_board, std::ostream& os) const noexcept{
	return print_board(situation, dx, dy, display_board, os);
}

void klotski_board::print_board(const situation_type& situation, int dx, int dy, bool display_board, std::ostream& os){
	int space_n = 0;
	int max_n = dx*dy;
	while(max_n != 0){
		max_n/=10;
		++space_n;
	}
	if(space_n < 2){
		space_n = 2;
	}

	std::stringstream output_stream;
	if(display_board){
		output_stream<<std::left<<std::setw(space_n)<<"+"<<" ";
		for(int j=0; j<dx; ++j){
			output_stream<<std::setw(space_n)<<j<<" ";
		}
		output_stream<<std::endl;
		print_line(dx, space_n, output_stream);
		for(int i=0; i<dy; ++i){
			output_stream<<std::setw(space_n)<<i<<"|";
			for(int j=0; j<dx; ++j){
				if(situation[i][j] == 0){
					output_stream<<std::setw(space_n)<<" "<<"|";
				}else{
					output_stream<<std::setw(space_n)<<situation[i][j]<<"|";
				}
			}
			output_stream<<std::endl;
			print_line(dx, space_n, output_stream);
		}
	}else{
		for(int i=0; i<dy; ++i){
			for(int j=0; j<dx; ++j){
				output_stream<<std::setw(space_n)<<situation[i][j]<<" ";
			}
			output_stream<<std::endl;
		}
	}
	os<<output_stream.str();
}

bool klotski_board::move_item(int x, int y){
	auto info = get_pos_info(x, y);
	if(info->is_item_can_move){
		if(info->zero_distance != 1){
			if(info->Orientation == pos_info::Horizontal){
				move_item(x, info->is_zero_in_top_or_left?y-1:y+1);
			}else{
				move_item(info->is_zero_in_top_or_left?x-1:x+1, y);
			}
		}
		std::swap(situation[y][x], get_zero());
		zero_x = x;
		zero_y = y;
		return true;
	}
	return false;
}

bool klotski_board::is_win() const noexcept{
	return is_win(situation);
}

bool klotski_board::is_win(const situation_type& situation) noexcept{
	int curr_num=1;
	for(auto i=situation.cbegin(); i != situation.cend(); ++i){
		for(auto j = i->cbegin(); j != i->cend(); ++j){
			if(*j != curr_num++ && !(i+1 == situation.cend() && j+1 == i->cend())){
				return false;
			}
		}
	}
	return true;
}

bool klotski_board::is_valid() const{
	if(!is_nums_linear()){
		return false;
	}
	std::vector<int> linear_situation;
	for(const auto& i: situation){
		for(int j: i){
			linear_situation.push_back(j);
		}
	}

	int rn = 0;
	int situation_size = linear_situation.size();
	for (int i = 0; i < situation_size - 1; ++i) {
		if(linear_situation[i] == 0){
			continue;
		}
		for (int j = i + 1; j < situation_size; ++j) {
			if(linear_situation[j] != 0
					&&linear_situation[i] > linear_situation[j]){
				rn ^= 1;
			}
		}
	}

	if((dx & 1) == 0){
		update_zero_pos();
		rn ^= (dy - 1 - zero_y) & 1;
	}
	return rn == 0;
}

bool klotski_board::is_valid(const klotski_board::situation_type& situation, int dx, int dy){
	if(!is_nums_linear(situation)){
		return false;
	}
	std::vector<int> linear_situation;
	for(const auto& i: situation){
		for(int j: i){
			linear_situation.push_back(j);
		}
	}

	int rn = 0;
	int situation_size = linear_situation.size();
	for (int i = 0; i < situation_size - 1; ++i) {
		if(linear_situation[i] == 0){
			continue;
		}
		for (int j = i + 1; j < situation_size; ++j) {
			if(linear_situation[j] != 0
					&&linear_situation[i] > linear_situation[j]){
				rn ^= 1;
			}
		}
	}

	if((dx & 1) == 0){
		int zero_y = -1;
		for (int i = 0; i < dy; ++i) {
			for (int j = 0; j < dx; ++j) {
				if(situation[i][j] == 0){
					zero_y = i;
				}
			}
		}
		if(zero_y == -1){
			throw std::runtime_error("Can not find zero on klotski board");
		}
		rn ^= (dy - 1 - zero_y) & 1;
	}
	return rn == 0;
}

const klotski_board::situation_type& klotski_board::get_situation() const noexcept{
	return situation;
}

klotski_board::situation_type& klotski_board::get_situation() noexcept{
	return const_cast<situation_type&>(
			static_cast<const klotski_board&>(*this).get_situation()
			);
}

void klotski_board::update_zero_pos() const{
	if(situation[zero_y][zero_x] == 0){
		return;
	}
	for (int i = 0; i < dy; ++i) {
		for (int j = 0; j < dx; ++j) {
			if(situation[i][j] == 0){
				zero_x = j;
				zero_y = i;
				return;
			}
		}
	}
	throw std::runtime_error("Can not find zero on klotski board");
}

int& klotski_board::get_zero(){
	if(situation[zero_y][zero_x] != 0){
		for(int i=0; i<dy; ++i){
			for(int j=0; j<dx; ++j){
				if(situation[i][j] == 0){
					zero_x = j;
					zero_y = i;
					return situation[i][j];
				}
			}
		}
	}else{
		return situation[zero_y][zero_x];
	}
	throw std::runtime_error("Can not find zero on klotski board");
}

std::shared_ptr<klotski_board::pos_info> klotski_board::get_pos_info(int x, int y) const noexcept{
	auto info = std::make_shared<pos_info>();
	try{
		if(situation.at(y).at(x) == 0){
			info->is_item_can_move = false;
			return info;
		}
	}catch(const std::out_of_range&){
		info->is_item_can_move = false;
		return info;
	}
	for(int i=0; i<dy; ++i){
		if(situation[i][x] == 0){
			info->is_item_can_move = true;
			info->is_zero_in_top_or_left = y - i > 0;
			info->Orientation = pos_info::Horizontal;
			info->zero_distance = abs(y - i);
			zero_x = x;
			zero_y = i;
			return info;
		}
	}
	for(int i=0; i<dx; ++i){
		if(situation[y][i] == 0){
			info->is_item_can_move = true;
			info->is_zero_in_top_or_left = x - i > 0;
			info->Orientation = pos_info::Vertical;
			info->zero_distance = abs(x - i);
			zero_x = i;
			zero_y = y;
			return info;
		}
	}
	info->is_item_can_move = false;
	return info;
}

bool klotski_board::is_nums_linear() const noexcept{
	return is_nums_linear(situation);
}

bool klotski_board::is_nums_linear(const klotski_board::situation_type& situation) noexcept{
	std::vector<int> linear_situation;
	for(const auto& i: situation){
		for(int j: i){
			linear_situation.push_back(j);
		}
	}
	std::sort(linear_situation.begin(), linear_situation.end());
	int last_num = -1;
	for(const auto i: linear_situation){
		if(last_num != i-1){
			return false;
		}
		last_num = i;
	}
	return true;
}

void klotski_board::init_board(int n){
	situation[dy - 1][dx - 1] = n;
}

void klotski_board::resize_board_to_fit(){
	situation.resize(dy);
	situation.shrink_to_fit();
	std::for_each(situation.begin(), situation.end(),
			[this](std::vector<int>& vec){vec.resize(dx); vec.shrink_to_fit();});
}

void klotski_board::print_line(int dx, int space_n, std::ostream& os) noexcept{
	os<<std::setw(space_n)<<" ";
	for(int j=0; j<dx; ++j){
		os<<std::setw(space_n+1)<<std::setfill('-')<<"+";
	}
	os<<'+'<<std::endl<<std::setfill(' ');
}

