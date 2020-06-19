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

#ifndef KLOTSKI_BOARD_H
#define KLOTSKI_BOARD_H

#include <iostream> 
#include <algorithm>
#include <ctime>
#include <random>
#include <vector>
#include <memory>

class klotski_board
{
	public:
		using situation_type = std::vector<std::vector<int>>;
		template<typename ...Args>
			klotski_board(int dx, int dy, Args... args);
		template<typename T>
			klotski_board(const T& situation_array, int dx, int dy);
		klotski_board(std::string situation_string, int dx, int dy, const char split = ',');
		klotski_board(std::fstream& file, int dx, int dy);
		klotski_board(int dx=3, int dy=3);

		void reset() noexcept;
		void upset(int depth = 10) noexcept;

		virtual bool move_item(int x, int y);

		virtual void print_board(bool display_board = false, std::ostream& os = std::cout) const noexcept;
		static void print_board(const situation_type& situation, int dx, int dy, bool display_board = false, std::ostream& os = std::cout);

		bool is_win() const noexcept;
		static bool is_win(const situation_type& situation) noexcept;

		bool is_valid() const;
		static bool is_valid(const klotski_board::situation_type& situation, int dx, int dy);

		const std::vector<std::vector<int>>& get_situation() const noexcept;
		std::vector<std::vector<int>>& get_situation() noexcept;

		int get_dx() const noexcept{
			return dx;
		}

		int get_dy() const noexcept{
			return dy;
		}

		class situation_type_hash{
			public:
				size_t operator()(const situation_type& situation) const noexcept;
		};

		virtual ~klotski_board() = default;

	protected:
		struct pos_info{
			enum{
				Horizontal,
				Vertical
			} Orientation;
			bool is_zero_in_top_or_left;
			bool is_item_can_move;
			int zero_distance;
		};

		void update_zero_pos() const;
		int& get_zero();
		std::shared_ptr<pos_info> get_pos_info(int x, int y) const noexcept;

		virtual bool is_nums_linear() const noexcept;
		static bool is_nums_linear(const klotski_board::situation_type& situation) noexcept;

	private:
		template<typename... Args>
			void init_board(int n, Args... args);
		void init_board(int n);
		void resize_board_to_fit();
		static void print_line(int dx, int space_n, std::ostream& os = std::cout) noexcept;

		int dx;
		int dy;
		mutable int zero_x = 0;
		mutable int zero_y = 0;
		situation_type situation;
		std::uniform_int_distribution<> x_distribution;
		std::uniform_int_distribution<> y_distribution;
		std::bernoulli_distribution orientation_distribution{0.5};
		static std::mt19937 random_engine;
};

template<typename ...Args>
klotski_board::klotski_board(int dx, int dy, Args... args):
	dx(dx), dy(dy),
	x_distribution(std::uniform_int_distribution<>(0, dx-1)),
	y_distribution(std::uniform_int_distribution<>(0, dy-1)){
		if(sizeof...(args) != dx*dy){
			throw std::invalid_argument("klotski board init error: incorrect number of parameters");
		}
		resize_board_to_fit();
		init_board(args...);
		update_zero_pos();
	}

template<typename T>
klotski_board::klotski_board(const T& situation_array, int dx, int dy):
	dx(dx), dy(dy),
	x_distribution(std::uniform_int_distribution<>(0, dx-1)),
	y_distribution(std::uniform_int_distribution<>(0, dy-1)){
		resize_board_to_fit();
		auto const_array_iterator = situation_array.cbegin();
		std::for_each(situation.begin(), situation.end(),
				[&situation_array, &const_array_iterator](std::vector<int>& vec){
				std::for_each(vec.begin(), vec.end(), [&](int& n){
						if(const_array_iterator != situation_array.cend()){
						n = *const_array_iterator++;
						}else{
						throw std::invalid_argument("situation array is not fit");
						}
						});
				});
	}

template<typename... Args>
void klotski_board::init_board(int n, Args... args){
	int arg_num = dx * dy - 1 - sizeof...(args);
	situation[arg_num/dx][arg_num%dx] = n;
	init_board(args...);
}

#endif
