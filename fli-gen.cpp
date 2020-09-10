#include <set>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cstring>
#include <cmath>
#include <chrono>
#include <ctime>
#include <random>
#include <vector>
#include <iostream>
#include <algorithm>
#include <functional>
#include <iterator>

#define ROUND_2_INT(f) ((int)(f >= 0.0 ? (f + 0.5) : (f - 0.5)))

#define VERSION_STR "2.8.3"

using namespace std;

typedef vector<double>::iterator VI;

bool findsum(VI cur, VI end, int s, int sum)
{
	if (s == sum) return true;
	if (s > sum) return false;
	while (cur != end)
	{
		if (findsum(cur+1, end, s+*cur, sum)) return true;
		rotate(cur, cur+1, end--);
	}
	return false;
}

enum Role {
	UNDEF = 0,
	KEEPER,
	MIDFIELDER,
	DEFENDER,
	FORWARD,
	UNIVERSAL,
	NR
};

string role_to_str(int r) {
	string res;
	switch((enum Role)r) {
		case KEEPER:
			res = "вратарь";
			break;
		case DEFENDER:
			res = "Защитник";
			break;
		case MIDFIELDER:
			res = "Полузащитник";
			break;
		case FORWARD:
			res = "Нападающий";
			break;
		case UNIVERSAL:
			res = "Универсал";
			break;

		default:
			res = "!UNDEF!";
	};
	return res;
}

string roles_to_str(set<int> &roles) {
	string res;
	stringstream ss;
	ss << " | ";
	for(auto it = roles.begin(); it != roles.end(); ++it) {
		ss << role_to_str(*it) << ",";
	}
	ss << "|" ;
	res = ss.str();
	res[res.length()-2] = ' ';
	return res;
}

class Player {
	public:
		Player() : _as(0), _ds(0), _ps(0), _pc(0), _name(""), _id(-1), _rate(0.0), _first_name("") {}
		int _id;
		double _rate;
		string _name;
		string _first_name;
		int _as; // attacking skill
		int _ds; // defending skill
		int _ps; // passing skill
		int _pc; // physical condition 
		set<int> _roles;
		void self_print() {
			//cout << "id: " << _id << " rate: " << _rate << " name: " << _name << endl;
			//cout << "id: " << setw(3) << _id << " rate: " << setw(5) << _rate << " name: " << std::left << setw(15) << _name << std::right <<  setw(15) << " roles: " << roles_to_str(_roles) <<  endl;
		}
		bool is_keeper() {
			return _roles.find(KEEPER) != _roles.end();
		}
		void pretty_self_print(int i) {
			cout << i  << ". " <<  _name << " " << _first_name << " " << " (" << _rate << ")"  <<  endl;
		}
};

	class Team {
		public:
			Team(string name) : _name(name) {
			}

			void pretty_self_print() {
				int cnt = 0;
				cout << _name << " ( Рейтинг: " << _rate << ", ОФП: " << this->_sum_phys_cond << " ) "  << endl;
				for(auto it = _map.begin(); it != _map.end(); ++it) {
					(*it)->pretty_self_print(++cnt);
				}

			}

			void self_print() {
				cout << "team name: " << _name << endl;
				for(auto it = _map.begin(); it != _map.end(); ++it) {
					(*it)->self_print();
				}

				cout << "team rate: " << _rate << endl;
				cout << "possible role as:" <<
					"\n\tKeeper     : " << _role_stat[KEEPER] <<
				"\n\tDefender   : " << _role_stat[DEFENDER] <<
				"\n\tMidfielder : " << _role_stat[MIDFIELDER] <<
				"\n\tForward    : " << _role_stat[FORWARD] <<
				"\n\tUniversal  : " << _role_stat[UNIVERSAL] <<
				endl;
		}
		void insert(class Player *p) {
			_map.insert(p);
			//_rate += p->_rate;
			//_rate += p->_rate;
			//double srate = 0.0;
			//size_t cnt[NR] = {};
			for(auto it = p->_roles.begin(); it != p->_roles.end(); ++it) {
				_role_stat[*it]++;
			}
			calc_rate();
		}
		double calc_rate() {
			_rate = 0.0;
			memset(_role_stat, 0, sizeof _role_stat);
			for(auto it = _map.begin(); it != _map.end(); ++it) {
#if 0
				bool is_in = (*it)->roles.find(element) != (*it)->roles.end();
				if(is_in) {
					cout << "skip keeper" << endl;
					continue;
				}
#endif
				_rate += (*it)->_rate;
				for(auto jt = (*it)->_roles.begin(); jt != (*it)->_roles.end(); ++jt) {
					_role_stat[*jt]++;
				}
			}
			return _rate;
		}
		int calc_phys_cond() {
			_sum_phys_cond = 0;
			for(auto it = _map.begin(); it != _map.end(); ++it) {
				_sum_phys_cond += (*it)->_pc;
			}
			return _sum_phys_cond;
		}

		//map<int, class Player *> _map;
		set<class Player *> _map;
		double _rate;
		int _sum_phys_cond;
	private:
		string _name;
		size_t _role_stat[NR] = {};
};

struct less_than_key {
    inline bool operator() (const class Player *a, const class Player *b) {
        return (a->_rate < b->_rate);
    }
};


#include <cctype>


class FliGen {
	public:
		FliGen() : _team0 ("Красные"), _team1 ("Лазурные") {
		};
		FliGen(string t0_name, string t1_name) : _team0(t0_name), _team1(t1_name) {
		};
		virtual ~FliGen() {
			cout << __FUNCTION__ << endl;
			for (auto it : _all_players) {
				delete it;
			}
		}

		int parseFile(string &path, bool keep_excl) {
			//int count_words(const char* str) {
			auto count_words = [](const char* str) {
				if (str == NULL)
					return -1;  // let the requirements define this...
				bool inSpaces = true;
				int numWords = 0;

				while (*str != '\0') {
					if (std::isspace(*str)) {
						inSpaces = true;
					}
					else if (inSpaces) {
						numWords++;
						inSpaces = false;
					}
					++str;
				}
				return numWords;
			};

			ifstream in(path);
//			in.exceptions ( std::ifstream::failbit | std::ifstream::badbit );
			try {
				int key = 0;
				char buf[1024];
				double rate = 0;
				double sum = 0;
				string role, name, fname;
				string as , ds , ps , pc ;
				bool keeper_fl = false;
				size_t keeper_count = 0;
				size_t total_count = 0;
#if 1
				std::string line;
				while (std::getline(in, line)) {
					int wrd_cnt = count_words(line.c_str());
					//cout << "count words = " << wrd_cnt <<  endl;
					std::istringstream iss(line);
					if(0 >= wrd_cnt) {
						//cout << "empty string" << endl;
						continue;
					} else if(5 == wrd_cnt) {
						if(!(iss >> key >> rate >> role >> name >> fname)) { break; } ;
					} else if( 9 == wrd_cnt) {
						if(!(iss >> key >> rate >> role >> name >> fname >> as >> ds >> ps >> pc)) { break; } ;
					} else {
						cout << "incorrect input format" << endl; 
					}
#endif
					//while(in >> key >> rate >> role >> name >> fname >> as >> ds >> ps >> pc) 
					Player *pl = new Player();
					//cout << key << " " << rate << " " << role << " " << name << " " << fname << endl;
					pl->_id = key;
					pl->_name = name;
					pl->_first_name = fname;
					pl->_rate = rate;

					auto _stoi = [](std::string &str, int *p_value) {
						// wrapping std::stoi because it may throw an exception
						try {
							*p_value = std::stoi(str);
							return 0;
						} catch (const std::invalid_argument& ia) {
							//std::cerr << "Invalid argument: " << ia.what() << std::endl;
							return -1;
						} catch (const std::out_of_range& oor) {
							//std::cerr << "Out of Range error: " << oor.what() << std::endl;
							return -2;
						} catch (const std::exception& e) {
							//std::cerr << "Undefined error: " << e.what() << std::endl;
							return -3;
						}
					};
					_stoi(as, &pl->_as);
					_stoi(ds, &pl->_ds);
					_stoi(ps, &pl->_ps);
					_stoi(pc, &pl->_pc);
					//cout << "attacking_skill = " << as << " defending_skill = " << ds << " passing_skill = " << ps << " phys_cond = " << pc << endl;
					//cout << "as = " << pl->_as << " ds = " << pl->_ds << " ps = " << pl->_ps << " pc = " << pl->_pc << endl;
					memset(buf, 0,1024);
					strncpy(buf, role.c_str(), role.length());
					char *pch = strtok(buf,",");
					while (pch != NULL) {
						enum Role r = parse_role(std::string(pch));
						if(r == KEEPER && keeper_count < 2) {
							keeper_fl = true;
							if(keep_excl)
								pl->_rate = 0.0;
							else
								pl->_rate = rate / 2;
							pl->_roles.insert(KEEPER);
							if(0 == _team0._map.size()) {
								_team0.insert(pl);
							} else {
								_team1.insert(pl);
							}
							keeper_count++;
						}	
						pl->_roles.insert(r);
						//cout << pch  << endl;
						pch = strtok (NULL, ",");
					}
					if(!keeper_fl) {
						_all_players.push_back(pl);	
						//map.insert(make_pair(key,pl));
					} 
					keeper_fl = false;	
					sum += rate;
					total_count++;
				}
				_avg_rate = sum / total_count;
				//cout << endl << "sum: " << sum << " sum/2: " << sum/2.0 <<  " avg rate: " << _avg_rate <<  endl;
			}
			catch (std::ifstream::failure e) {
				std::cerr << "Exception opening/reading/closing file\n";
			}
			catch(std::invalid_argument &e) {
				cout << "invalid arg\n";
			}
			catch (...) {
				cout << "exception happened" << endl;
				return 1;
			}
			return 0;
			//cath()
		}
                
		void splitOfferV1() {
			size_t cnt = 0;
			std::sort(_all_players.begin(), _all_players.end(), less_than_key());
			//bool fl, who_start = true;
			bool fl, who_start = (*_team0._map.begin())->_rate < (*_team1._map.begin())->_rate;
			for(auto it = _all_players.begin(); it != _all_players.end(); ++it, cnt++) {
				fl = who_start ? cnt % 2 : !(cnt %2);
				if(fl) {
					_team0.insert(*it);
				} else {
					_team1.insert(*it);
				}
			}
		}

		void splitOfferV2() {
			double delta;
			size_t tries = 0;
			splitOfferV1();

			//this->printResult();
			size_t ind = 0;	
			Team *tmp_team0 = &_team0;
			Team *tmp_team1 = &_team1;

			auto r0 = tmp_team0->calc_rate(), r1= tmp_team1->calc_rate();
			if(r0 > r1) {
				tmp_team0 = &_team1;
				tmp_team1 = &_team0;
			}

			auto tmp = tmp_team0->_map.begin();

			delta = (tmp_team0->calc_rate() - tmp_team1->calc_rate());
//			cout << "delta = " << delta << endl;
			if(0 == (ROUND_2_INT(10.0*delta) % 2)) {
				//cout << "even case delta: " << delta << endl;
				while(tmp != tmp_team0->_map.end()) {
					if((*tmp)->is_keeper()) {
						tmp++;
						continue;
					}
					//cout << "team0 palyer name: "  << (*tmp)->_name << '\n';
					auto it = std::find_if(tmp_team1->_map.begin(), tmp_team1->_map.end(), [&tmp,&delta] (auto &arg) {
							if(arg->is_keeper()) {
								return false;
							}
							//cout << "team1 palyer name: "  << arg->_name << '\n';
							double cur_del = ((*tmp)->_rate - arg->_rate) - (delta/2.0);
							//cout << (*tmp)->_rate - arg->_rate   <<  " == " << (delta/2.0) << '\n';
							bool ret = (cur_del > -0.05) && (cur_del < 0.05);
							//cout << " ret: " << ret  <<  '\n';
							return ret; 
							}
							);
					if(it != tmp_team1->_map.end()) {
						(*it)->self_print();

						
						switch_players(tmp_team0, tmp_team1, *tmp, *it);
						
						tmp = tmp_team0->_map.begin();
		                                //cout << "t0: " << tmp_team0.calc_rate() << " t1: " <<  tmp_team1.calc_rate() << endl;
		                                tmp_team0->calc_rate();tmp_team1->calc_rate();
						break;
					} else {
						tmp++;
					}
				}
			} else {

				//cout << "eve " << abs(10.0*delta) <<  " "  <<((int)(10.0*delta)) << " " <<  ((10.0*delta)) % 2 << endl;
				while(tries++ < 99 && tmp != tmp_team0->_map.end() && abs(delta) >= 0.2) {
					if((*tmp)->is_keeper()) {
						tmp++;
						continue;
					}
//					cout << "team0 palyer name: "  << (*tmp)->_name << '\n';
					auto it = std::find_if(tmp_team1->_map.begin(), tmp_team1->_map.end(), [&tmp,&delta] (auto &arg) {
							if(arg->is_keeper()) {
								return false;
							}
//							cout << "team1 palyer name: "  << arg->_name << '\n';
							double eps = ((*tmp)->_rate - arg->_rate) - delta;
							bool ret = (eps >= 0.0) && (eps < 0.2);
//							cout << "eps: "  << eps << " ret: " << ret  <<  '\n';
							return ret; 
							}
							);
					if(it != tmp_team1->_map.end()) {
						(*it)->self_print();
						Player *p1 = *tmp, *p2 = *it;
						tmp_team0->_map.erase(tmp);
						tmp_team1->_map.erase(it);
						tmp_team0->_map.insert(p2);
						tmp_team1->_map.insert(p1);
						tmp = tmp_team0->_map.begin();
					} else {
						tmp++;
					}
					delta = tmp_team0->calc_rate() - tmp_team1->calc_rate();
				} 
//				cout << endl << "total tries (permutations) = " << tries << endl;
//				if(tries < 100) 
//					cout << "team ratings is equal, don't need any additional stuff" << endl;
			}
		}
		void switch_players(Team *t0, Team *t1, Player *p0, Player *p1) {
			//Player *p1 = *tmp, *p2 = *it;
			//Player *p1 = *tmp, *p2 = *it;
			t0->_map.erase(p0);
			t1->_map.erase(p1);
			t0->_map.insert(p1);
			t1->_map.insert(p0);
		}
		void shake() {
			auto *t0 = &(_team0._map);
			auto *t1 = &(_team1._map);
			auto it0 = t0->begin();
			double delta = (_team0.calc_rate() - _team1.calc_rate());
			//auto it1 = t1->begin;
			random_device rd;
			mt19937 gen(rd());
			uniform_int_distribution<> dist(0, t0->size() - 1);
			vector<int> v(8);
			generate(v.begin(), v.end(), bind(dist, gen));
			for (auto i: v) {
				std::advance(it0, i);
				if((*it0)->is_keeper())
					continue;
				//cout << i << " " << (*it0)->_name << '\n';
				for (auto it1 = t1->begin(); it1 != t1->end(); ++it1) {
					if((*it1)->is_keeper())
						continue;
					double cur_del = ((*it0)->_rate - (*it1)->_rate) - (delta/2.0);
					bool equal = (cur_del > -0.05) && (cur_del < 0.05);
					if(equal) {
						Player *p1 = *it0, *p2 = *it1;
						t0->erase(it0);
						t1->erase(it1);
						t0->insert(p2);
						t1->insert(p1);
						break;
					}
				}
				it0 = t0->begin();
				//delta = t0->calc_rate() - t1->calc_rate(); 
				//
				auto func  = [](std::set<class Player *> *set_ptr ) {
					double _ret = 0.0;
					for(auto it = set_ptr->begin(); it != set_ptr->end(); ++it) {
						_ret += (*it)->_rate;
					}
					return _ret;
				};
				delta = func(t0) - func(t1);
			}
		}

		int rebalanceByPhysCond() {
			int red_pc = _team0.calc_phys_cond();
			int azure_pc = _team1.calc_phys_cond();
			//cout << "azure pc = " << azure_pc << endl;
			//cout << "red pc = " <<  red_pc << endl;
			int team_delta_pc = azure_pc - red_pc;
			//cout << "team delta pc = " <<  team_delta_pc << endl;
			if(std::abs(team_delta_pc) > 1) {
				double team_delta_rate = (_team0.calc_rate() - _team1.calc_rate());
				//try to rebalance
				//decltype (_team0._map) *t0;
				//decltype (_team1._map) *t1;
//				auto t0 = &(_team1._map);
//				auto t1 = &(_team0._map);
				for (auto it0 = _team0._map.begin(); it0 != _team0._map.end(); ++it0) {
					for (auto it1 = _team1._map.begin(); it1 != _team1._map.end(); ++it1) {
						if((*it0)->is_keeper() || (*it1)->is_keeper())
							continue;
						double cur_del = ((*it0)->_rate - (*it1)->_rate) - (team_delta_rate/2.0);
						int player_delta_pc = (*it0)->_pc - (*it1)->_pc;
						//cout << "cur_del = " << cur_del << endl;
						int r0 = ROUND_2_INT(10.0*((*it0)->_rate));
						int r1 = ROUND_2_INT(10.0*((*it1)->_rate));
						bool equal = r0 == r1;
						//cout << "equal = " << (equal ? "true" : "false") << " player_delta_pc = " << player_delta_pc << endl;
						if(equal && player_delta_pc) {
							if((player_delta_pc < 0 && team_delta_pc > 0) || (player_delta_pc > 0 && team_delta_pc < 0)) {
								//cout << " swap players: " << (*it1)->_name << " and " << (*it0)->_name << " player delta pc = " << player_delta_pc << endl;
								switch_players(&_team0, &_team1,  *it0, *it1);
								//cout << "rebalance pc: "<< _team1.calc_phys_cond() <<  " : " << _team0.calc_phys_cond() << endl;
								return 2;
							}
						}
					}
				}
				return 1;
			} else {
				//cout << "teams are equal by physical conditions\n";
				return 0;
			}
		}

		void prettyPrintResult(bool keep_excl) {
			_team0.calc_phys_cond();
			_team1.calc_phys_cond();
			std::chrono::time_point<std::chrono::system_clock> time_now = std::chrono::system_clock::now();
			std::time_t time_now_t = std::chrono::system_clock::to_time_t(time_now);
			std::tm now_tm = *std::localtime(&time_now_t);
			char buf[512];
			std::strftime(buf, 512, "%d.%m.%Y", &now_tm);
			cout << "Составы ФЛИ " << buf << endl;
			if(keep_excl)
				cout << "Без учёта";
			else 
				cout <<  "С учётом";
			cout << " вратарей" << endl << endl;
			_team1.pretty_self_print();
			cout << endl;
			_team0.pretty_self_print();
			cout << endl;
		}

		void printResult() {
			cout << endl;
			_team1.self_print();
			cout << endl;
			_team0.self_print();
			cout << endl;
		}
	private:
		enum Role parse_role(string in) {
			enum Role ret = UNDEF;
			if(in == "вратарь") {
				ret = KEEPER;
			} else if (in == "защитник"  || in == "крайнийзащитник" || in == "центральныйзащитник") {
				ret = DEFENDER;
			} else if (in == "полузащитник" || in == "крайнийполузащитник") {
				ret = MIDFIELDER;
			} else if (in == "форвард") {
				ret = FORWARD;
			} else if (in == "универсал") {
				ret = UNIVERSAL;
			}
			return ret;
		}
		double _avg_rate;
		//map<int, class Player> _team0, _team1;
		Team _team0, _team1;
		vector<class Player *> _all_players;
};



void print_version(char *app_0) {
	printf("\n\n\nfli-gen (Football League InfoTeCS Generator)\n"
			"\tCopyright (C) 2019-%s FLI Inc.\n"
			"\tCompiled on %s at %s\n",
			&__DATE__[7],
			__DATE__, __TIME__
	      );
	//printf("\tversion (tag) = \"%s\"\n", GIT_VER);
	printf("\tversion (str) = \"%s\"\n", VERSION_STR);
	printf("\tgit hash = \"%s\"\n", GIT_HASH);
	printf("\n");

	cout << "" << basename(app_0) << "\t"  << VERSION_STR << endl;
}

int main(int argc, char *argv[]) {
	string input_file;
	FliGen fligen;
	if(argc > 1) {
		input_file = string(argv[1]);
	} else {
		input_file = "input.txt";
	}
	bool keep_excl = argc > 2;
	if(fligen.parseFile(input_file, keep_excl)) {
		cout << "error parsing input file\n";
		exit(13);
	}

	fligen.splitOfferV2();
	fligen.shake();
	
	int num_tries = 0;
	while(0 != fligen.rebalanceByPhysCond()) {
		if( ++num_tries > 100)
			break;
	}
	fligen.printResult();

	print_version(argv[0]);

	fligen.prettyPrintResult(keep_excl);
}
