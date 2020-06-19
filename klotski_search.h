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

#ifndef KLOTSKI_SEARCH_H
#define KLOTSKI_SEARCH_H

#include "klotski_board.h"
#include <unordered_map>
#include <memory>
#include <tuple>
#include <queue>

class klotski_search{
	public:
		explicit klotski_search(const klotski_board& board):
			situation(board.get_situation()),
			dx(board.get_dx() - 1), dy(board.get_dy() - 1){}
		explicit klotski_search(std::shared_ptr<klotski_board> board_ptr):
			klotski_search(*board_ptr){};

		struct record_item{
			klotski_board::situation_type situation;
			enum{
				Horizontal,
				Vertical,
				Undefined
			} Orientation;
			struct record_item* prev;
		};

		virtual bool start_search() noexcept;
		virtual bool is_situation_valid() const noexcept;
		std::tuple<int, int> get_zero_pos(const klotski_board::situation_type& situation_cur) const;
		const std::deque<klotski_board::situation_type>& get_last_route() const noexcept{
			return last_route;
		}

	private:
		void build_route(const record_item& item);

		klotski_board::situation_type situation;
		std::deque<klotski_board::situation_type> last_route;
		int dx;
		int dy;
};

#endif

