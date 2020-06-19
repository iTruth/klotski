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

#include "klotski_search.h"
#include <stdexcept>
#include <stack>
#include <tuple>

bool klotski_search::start_search() noexcept{
	if(!is_situation_valid()){
		return false;
	}
	if(klotski_board::is_win(situation)){
		last_route.clear();
		last_route.push_back(situation);
		return true;
	}
	std::queue<record_item> open;
	std::stack<record_item> close;
	std::unordered_map<klotski_board::situation_type, bool, klotski_board::situation_type_hash> situation_search_state;
	open.push(record_item{situation, record_item::Undefined, nullptr});
	situation_search_state[situation] = true;
	while(!open.empty()){
		auto situation_front = open.front();
		open.pop();
		close.push(situation_front);
		auto zero_pos = get_zero_pos(situation_front.situation);
		if(std::get<0>(zero_pos) != 0){
			auto situation_tmp = situation_front;
			situation_tmp.Orientation = record_item::Horizontal;
			situation_tmp.prev = &close.top();
			std::swap(situation_tmp.situation[std::get<1>(zero_pos)][std::get<0>(zero_pos)],
					situation_tmp.situation[std::get<1>(zero_pos)][std::get<0>(zero_pos) - 1]);
			if(!situation_search_state[situation_tmp.situation]){
				open.push(situation_tmp);
				situation_search_state[situation_tmp.situation] = true;
				if(klotski_board::is_win(situation_tmp.situation)){
					build_route(situation_tmp);
					return true;
				}
			}
		}
		if(std::get<0>(zero_pos) < dx){
			auto situation_tmp = situation_front;
			situation_tmp.Orientation = record_item::Horizontal;
			situation_tmp.prev = &close.top();
			std::swap(situation_tmp.situation[std::get<1>(zero_pos)][std::get<0>(zero_pos)],
					situation_tmp.situation[std::get<1>(zero_pos)][std::get<0>(zero_pos) + 1]);
			if(!situation_search_state[situation_tmp.situation]){
				open.push(situation_tmp);
				situation_search_state[situation_tmp.situation] = true;
				if(klotski_board::is_win(situation_tmp.situation)){
					build_route(situation_tmp);
					return true;
				}
			}
		}
		if(std::get<1>(zero_pos) != 0){
			auto situation_tmp = situation_front;
			situation_tmp.Orientation = record_item::Vertical;
			situation_tmp.prev = &close.top();
			std::swap(situation_tmp.situation[std::get<1>(zero_pos)][std::get<0>(zero_pos)],
					situation_tmp.situation[std::get<1>(zero_pos) - 1][std::get<0>(zero_pos)]);
			if(!situation_search_state[situation_tmp.situation]){
				open.push(situation_tmp);
				situation_search_state[situation_tmp.situation] = true;
				if(klotski_board::is_win(situation_tmp.situation)){
					build_route(situation_tmp);
					return true;
				}
			}
		}
		if(std::get<1>(zero_pos) < dy){
			auto situation_tmp = situation_front;
			situation_tmp.Orientation = record_item::Vertical;
			situation_tmp.prev = &close.top();
			std::swap(situation_tmp.situation[std::get<1>(zero_pos)][std::get<0>(zero_pos)],
					situation_tmp.situation[std::get<1>(zero_pos) + 1][std::get<0>(zero_pos)]);
			if(!situation_search_state[situation_tmp.situation]){
				open.push(situation_tmp);
				situation_search_state[situation_tmp.situation] = true;
				if(klotski_board::is_win(situation_tmp.situation)){
					build_route(situation_tmp);
					return true;
				}
			}
		}
	}
	return false;
}

bool klotski_search::is_situation_valid() const noexcept{
	return klotski_board::is_valid(situation, dx + 1, dy + 1);
}

std::tuple<int, int> klotski_search::get_zero_pos(const klotski_board::situation_type& situation_cur) const{
	int x = 0;
	int y = 0;
	for(auto i=situation_cur.cbegin(); i!=situation_cur.cend(); ++i, ++y){
		x = 0;
		for(auto j=i->cbegin(); j!=i->cend(); ++j, ++x){
			if(*j == 0){
				return std::make_tuple(x, y);
			}
		}
	}
	throw std::runtime_error("can not find zero");
}

void klotski_search::build_route(const record_item& item){
	const record_item* cur_item = &item;
	last_route.clear();
	auto last_orientation = record_item::Undefined;
	while(cur_item != nullptr){
		if(cur_item->Orientation != last_orientation){
			last_route.push_front(cur_item->situation);
			last_orientation = cur_item->Orientation;
		}
		cur_item = cur_item->prev;
	}
}

