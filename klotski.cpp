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

#include <iostream>
#include <fstream>
#include <limits>
#include <memory>
#include <signal.h>
#include <sstream>
#include <string>
#include <iomanip>
#include <stdexcept>
#include <getopt.h>
#include "klotski_board.h"
#include "klotski_search.h"

void print_help(){
	std::cout<<"usage:"<<std::endl
		<<"   klotski [-x N] [-y N] [-p] [-u N] [-e [situation]] [-s] [-q] [-b] [-f file] [-s [-o file]] [-r] [-h]"<<std::endl<<std::endl<<std::left
		<<std::setw(5)<<" -x"<<std::setw(20)<<" "<<"specify x size of board, default 3"<<std::endl
		<<std::setw(5)<<" -y"<<std::setw(20)<<" "<<"specify y size of board, default 3"<<std::endl
		<<std::setw(5)<<" -p,"<<std::setw(20)<<"--play"<<"play klotski, upset board 10 times if -u not specify"<<std::endl
		<<std::setw(5)<<" -u,"<<std::setw(20)<<"--upset"<<"upset board, default 10"<<std::endl
		<<std::setw(5)<<" -e,"<<std::setw(20)<<"--edit"<<"edit board with input"<<std::endl
		<<std::setw(5)<<" -s,"<<std::setw(20)<<"--search"<<"search answer"<<std::endl
		<<std::setw(5)<<" -q,"<<std::setw(20)<<"--quiet"<<"quiet mode"<<std::endl
		<<std::setw(5)<<" -b,"<<std::setw(20)<<"--board"<<"print situation with board"<<std::endl
		<<std::setw(5)<<" -o,"<<std::setw(20)<<"--output"<<"output search answer to file"<<std::endl
		<<std::setw(5)<<" -f,"<<std::setw(20)<<"--file"<<"init board from file"<<std::endl
		<<std::setw(5)<<" -r,"<<std::setw(20)<<"--research"<<"same as --play but not check win"<<std::endl
		<<std::setw(5)<<" -h,"<<std::setw(20)<<"--help"<<"display this help"<<std::endl;
}

void search_answer(std::shared_ptr<klotski_board> board, bool is_quiet, bool is_print_board, std::ostream& os = std::cout){
	if(board == nullptr){
		throw std::logic_error("board not init");
	}
	if(!is_quiet){
		board->print_board(is_print_board);
	}
	auto s = std::make_shared<klotski_search>(board);
	if(!is_quiet){
		std::cout<<"searching..."<<std::endl;
	}
	if(s->start_search()){
		const auto& route = s->get_last_route();
		if(!is_quiet){
			std::cout<<"answer found!"<<std::endl;
			os<<"steps = "<<route.size() - 1<<std::endl<<std::endl;
		}
		int step = 0;
		for(const auto& i: route){
			os<<"step "<<step++<<": "<<std::endl;
			klotski_board::print_board(i, board->get_dx(), board->get_dy(), is_print_board, os);
			os<<std::endl;
		}
	}else{
		os<<"No solution"<<std::endl;
	}
}

using namespace std;

#define KLOTSKI_OUTPUT_STREAM (is_output_to_file? situation_output_file: std::cout)

int main(int argc, char *argv[])
{
	if(argc == 1){
		print_help();
		return EXIT_FAILURE;
	}

	int dx = 3;
	int dy = 3;
	bool is_play = false;
	bool is_upset = false;
	bool is_already_upset = false;
	int upset_num = 10;
	bool is_edit = false;
	std::string situation_string;
	bool is_search = false;
	bool is_quiet = false;
	bool is_print_board = false;
	bool is_output_to_file = false;
	std::fstream situation_output_file;
	bool is_read_board_from_file = false;
	std::fstream situation_input_file;
	bool is_research = false;

	const char *optstring = "x:y:pu:e::sqbo:f:rh";
	static struct option long_options[] = {
		{"play",	no_argument, NULL, 'p'},
		{"upset",	required_argument, NULL, 'u'},
		{"edit",	optional_argument, NULL, 'e'},
		{"search",	required_argument, NULL, 's'},
		{"quiet",	no_argument, NULL, 'q'},
		{"board",	no_argument, NULL, 'b'},
		{"output",	required_argument, NULL, 'o'},
		{"file",	required_argument, NULL, 'f'},
		{"research",no_argument, NULL, 'r'},
		{"help",	no_argument, NULL, 'h'},
		{0, 0, 0, 0}};

	int opt;
	int option_index = 0;
	while((opt = getopt_long(argc, argv,
					optstring, long_options, &option_index)) != -1) {
		switch(opt)
		{
			case 'x':
				try{
					dx = std::stoi(optarg);
				}catch(const std::invalid_argument&){
					cout<<"Invalid argument: x"<<endl;
					return EXIT_FAILURE;
				}
				break;

			case 'y':
				try{
					dy = std::stoi(optarg);
				}catch(const std::invalid_argument&){
					cout<<"Invalid argument: y"<<endl;
					return EXIT_FAILURE;
				}
				break;

			case 'p':
				is_play = true;
				break;

			case 'u':
				is_upset = true;
				try{
					upset_num = std::stoi(optarg);
				}catch(const std::invalid_argument&){
					cout<<"Invalid argument: upset number"<<endl;
					return EXIT_FAILURE;
				}
				break;

			case 'e':
				is_edit = true;
				if(optarg != nullptr){
					situation_string = optarg;
				}
				break;

			case 's':
				is_search = true;
				break;

			case 'q':
				is_quiet = true;
				break;

			case 'b':
				is_print_board = true;
				break;

			case 'o':
				is_output_to_file = true;
				situation_output_file.open(optarg, ios::trunc|ios::out);
				if(!situation_output_file.is_open()){
					cout<<"file not found"<<endl;
					return EXIT_FAILURE;
				}
				break;

			case 'f':
				is_read_board_from_file = true;
				situation_input_file.open(optarg, ios::in);
				if(!situation_input_file.is_open()){
					cout<<"file not found"<<endl;
					return EXIT_FAILURE;
				}
				break;

			case 'r':
				is_research = true;
				break;

			case '?':
			case 'h':
			default:
				print_help();
				return EXIT_SUCCESS;
				break;
		}
	}

	if(is_output_to_file && !is_search){
		cout<<"specifying -o must also specify -s"<<endl;
		return EXIT_FAILURE;
	}

	std::shared_ptr<klotski_board> board = nullptr;

	if(is_read_board_from_file){
		if(is_edit){
			cout<<"specifying -f can't also specify -e"<<endl;
			return EXIT_FAILURE;
		}
		try{
			board = std::make_shared<klotski_board>(situation_input_file, dx, dy);
		}catch(const std::runtime_error&){
			cout<<"file is not a valid board file"<<endl;
			return EXIT_FAILURE;
		}
	}

	if(is_edit){
		try{
			if(situation_string.empty()){
				if(!is_quiet){
					cout<<"Enter "<<dx*dy<<" numbers to construct klotski board"<<endl
						<<"Example input(3x3 board): "<<endl
						<<"1 0 6 7 3 8 2 4 5"<<endl<<endl;
					cout<<"Enter: ";
				}
				std::vector<int> situation;
				for(int i=0; i<dx*dy; ++i){
					int n;
					cin>>n;
					situation.push_back(n);
				}
				board = std::make_shared<klotski_board>(situation, dx, dy);
			}else{
				board = std::make_shared<klotski_board>(situation_string, dx, dy);
			}
		}catch(const std::runtime_error& e){
			cout<<e.what()<<endl;
			return EXIT_FAILURE;
		}
	}

	if(is_upset){
		if(board == nullptr){
			board = std::make_shared<klotski_board>(dx, dy);
		}
		board->upset(upset_num);
		is_already_upset = true;
	}

	if(is_search){
		if(board == nullptr){
			board = std::make_shared<klotski_board>(dx, dy);
		}
		search_answer(board, is_quiet, is_print_board, KLOTSKI_OUTPUT_STREAM);
	}

	if(is_play || is_research){
		int x, y;
		if(board == nullptr){
			board = std::make_shared<klotski_board>(dx, dy);
		}
		if(!is_edit && !is_already_upset){
			board->upset(upset_num);
			is_already_upset = true;
		}
		board->print_board(is_print_board);
		string line;
		string hint = "klotski>";
		while(is_research || !board->is_win()){
			cout<<hint;
			getline(cin, line);
			if(line.empty()){
				continue;
			}
			std::stringstream ss(line);
			ss>>x>>y;
			if(board->move_item(x, y)){
				board->print_board(is_print_board);
				continue;
			}
			line = line.substr(0, line.find_last_not_of(' ')+1);
			auto pos = line.find_first_of(' ');
			const std::string cmd_name = line.substr(0, pos==std::string::npos?line.size():pos);
			const std::string cmd_arg = line.substr(cmd_name.size(), line.size()-1);
			if(cmd_name == "quit" || cmd_name == "q"){
				return EXIT_SUCCESS;
			}else if(cmd_name == "reset" || cmd_name == "r"){
				board->reset();
				if(!is_quiet){
					board->print_board(is_print_board);
				}
			}else if(cmd_name == "print" || cmd_name == "p"){
				board->print_board(is_print_board);
			}else if(cmd_name == "search" || cmd_name == "s"){
				search_answer(board, is_quiet, is_print_board, KLOTSKI_OUTPUT_STREAM);
			}else if(cmd_name == "upset" || cmd_name == "u"){
				try{
					board->upset(std::stoi(cmd_arg));
					if(!is_quiet){
						board->print_board(is_print_board);
					}
				}catch(const std::invalid_argument&){
					cout<<"invalid argument"<<endl
						<<"usage: upset N"<<endl<<endl;
				}
			}else{
				system(line.c_str());
			}
		}
		cout<<"You Win!"<<endl;
	}

	return EXIT_SUCCESS;
}
