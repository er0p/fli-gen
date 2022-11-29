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
#include <iomanip>
#include <algorithm>
#include <functional>
#include <iterator>

#include <codecvt>
#include <unicode/unistr.h>

#define ROUND_2_INT(f) ((int)(f >= 0.0 ? (f + 0.5) : (f - 0.5)))

#define VERSION_STR "2.9.3"

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
enum Indicator {
	IND_ATTACK_SKILL = 0,
	IND_DEFEND_SKILL,
	IND_PASS_SKILL,
	IND_PHYS_COND,
	IND_AGE,
	IND_OVERALL,
	IND_NR,
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

string ind_type_to_str(int type) {
	string res = "";
	switch((enum Role)type) {
		case IND_ATTACK_SKILL:
			res = "Навыки атаки";
			break;
		case IND_DEFEND_SKILL:
			res = "Навыки защиты";
			break;
		case IND_PASS_SKILL:
			res = "Навыки паса";
			break;
		case IND_PHYS_COND:
			res = "Скорость";
			break;
		case IND_AGE:
			res = "Возраст";
			break;
		case IND_OVERALL:
			res = "Интегральный показатель";
			break;

	};
	return res;
};

class Player {
	public:
		Player() : _pi{0}, _name(""), _id(-1), _rate(0.0), _first_name("") {}
		int _id;
		double _rate;
		string _name;
		string _first_name;
		int _pi[IND_NR];
		set<int> _roles;
		void self_print() {
			//cout << "id: " << _id << " rate: " << _rate << " name: " << _name << endl;
			//cout << "id: " << setw(3) << _id << " rate: " << setw(5) << _rate << " name: " << std::left << setw(15) << _name << std::right <<  setw(15) << " roles: " << roles_to_str(_roles) <<  endl;
		}
		bool is_keeper() {
			return _roles.find(KEEPER) != _roles.end();
		}
		void revoke_keeper() {
			_roles.erase(KEEPER);
		}
		void pretty_self_print(int i) {
			string player_str = ( to_string(i) + ". " +  _name + " "  +  _first_name);
			int glyphs = wstring_convert< codecvt_utf8<char32_t>, char32_t >().from_bytes(player_str).size();
			int str_size = player_str.size();
			cout << player_str;
			cout <<  setw(22 - glyphs) << cout.fill(' ');
			cout <<  " (" << fixed << setprecision(2) <<  _rate << ", " << _pi[IND_DEFEND_SKILL] << ", " <<
				_pi[IND_ATTACK_SKILL] << ", " <<
				_pi[IND_PASS_SKILL] << ", " <<
				_pi[IND_PHYS_COND] << ", "  << 
				_pi[IND_AGE] << ")"  <<  endl;
		}
};

	class Team {
		public:
			int _selected_indicator;
			Team(string name, int ind_type) : _name(name), _selected_indicator(ind_type) {
			}

			void pretty_self_print() {
				int cnt = 0;
				double mean_age = ((double)calc_by(IND_AGE)) / 7.0;
				int ds = calc_by(IND_DEFEND_SKILL);
				int as = calc_by(IND_ATTACK_SKILL);
				int ps = calc_by(IND_PASS_SKILL);
				int pc = calc_by(IND_PHYS_COND);
				cout << _name << " ( Рейтинг: " << _rate << ", " <<
				       	 " ЗАЩ: " << ds <<
				       	 " НАП: " << as <<
				       	 " ПАС: " << ps <<
				       	 " СКР: " << pc <<
					", средний возраст: " << fixed << setprecision(1) << mean_age <<  " ) "  << endl;
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
			//calc_rate();
		}
		void erase() {
			_map.clear();
			//calc_rate(_keep_excl);
		}
		double calc_rate(bool keep_excl) {
			_rate = 0.0;
			memset(_role_stat, 0, sizeof _role_stat);
			for(auto it = _map.begin(); it != _map.end(); ++it) {
				bool is_in = (*it)->_roles.find(KEEPER) == (*it)->_roles.end();
				if(is_in) {
					_rate += (*it)->_rate;
				} else {
					if(keep_excl)
						_rate += ((*it)->_rate) / 2.0;
					//cout << "skip keeper" << endl;
					//continue;
				}
				for(auto jt = (*it)->_roles.begin(); jt != (*it)->_roles.end(); ++jt) {
					_role_stat[*jt]++;
				}
			}
			//cout << "rate = " << _rate << endl;
			return _rate;
		}
		int calc_by(int type) {
			int _sum = 0;
			for(auto it = _map.begin(); it != _map.end(); ++it) {
				if((*it)->is_keeper())
					continue;
				_sum += (*it)->_pi[type];
			}
			_indicator[type] = _sum;
			return _indicator[type];
		}

		//map<int, class Player *> _map;
		set<class Player *> _map;
		double _rate;
		int _indicator[IND_NR];
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
		int _selected_indicator;
		FliGen(int ind_type, bool keep_excl) : _team0 ("Красные", ind_type), _team1 ("Лазурные", ind_type), _selected_indicator(ind_type), _keep_excl(keep_excl) {
		};
		FliGen(string t0_name, string t1_name, int ind_type, bool keep_excl) : _team0(t0_name, ind_type), _team1(t1_name, ind_type), _keep_excl(keep_excl) {
		};
		virtual ~FliGen() {
			cout << __FUNCTION__ << endl;
			for (auto it : _all_players) {
				delete it;
			}
		}

		int parseFile(string &path) {
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
			try {
				int key = 0;
				char buf[1024];
				double rate = 0;
				double sum = 0;
				string role, name, fname, birth_date;
				string as , ds , ps , pc;
				bool keeper_fl = false;
				size_t keeper_count = 0;
				size_t total_count = 0;
#if 1
				std::string line;
				while (std::getline(in, line)) {
					int wrd_cnt = count_words(line.c_str());
					std::istringstream iss(line);
					if(0 >= wrd_cnt) {
						//cout << "empty string" << endl;
						continue;
					}
					if(wrd_cnt >= 5) {
						if(!(iss >> key >> rate >> role >> name >> fname)) { break; } ;
					}
					if(wrd_cnt >= 9) {
						if(!(iss >> as >> ds >> ps >> pc)) { break; } ;
					} 
					if(wrd_cnt >= 10) {
						if(!(iss >> birth_date)) { break; } ;
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

					_stoi(as, &pl->_pi[IND_ATTACK_SKILL]);
					_stoi(ds, &pl->_pi[IND_DEFEND_SKILL]);
					_stoi(ps, &pl->_pi[IND_PASS_SKILL]);
					_stoi(pc, &pl->_pi[IND_PHYS_COND]);
					if(!birth_date.empty()) {
						struct tm tm{0};
						int bdy = -1;
						//std::string s("32/02/2013");	
						if(birth_date.find('/') == string::npos && 0 == _stoi(birth_date, &bdy)) {
							//std::cout << "date is valid (year)" << std::endl;
							tm.tm_year = bdy - 1900;
						} else if(strptime(birth_date.c_str(), "%m/%d/%Y", &tm)) {
							//std::cout << "date is valid (m/d/Y)" << std::endl;
							bdy = tm.tm_year + 1900;
						} else {
							std::cout << "date is invalid" << std::endl;
						}
						if(bdy >= 0) {
							time_t bd_time = mktime(&tm);
							time_t curr_time = std::time(nullptr);
							double diff_sec = std::difftime(curr_time, bd_time);
							pl->_pi[IND_AGE] = floor(diff_sec/31536000.0);
							//cout << "Player " << name << " has a birthday: " << birth_date << " year of BD: " << bdy << " diff: " <<  pl->_pi[IND_AGE] <<  endl;
						}
					}

					//cout << "attacking_skill = " << as << " defending_skill = " << ds << " passing_skill = " << ps << " phys_cond = " << pc << endl;
//					cout << "as = " << pl->_pi[IND_ATTACK_SKILL] << " ds = " << pl->_pi[IND_DEFEND_SKILL] << " ps = " << pl->_pi[IND_PASS_SKILL] << " pc = " << pl->_pi[IND_PHYS_COND] << endl;
					memset(buf, 0,1024);
					strncpy(buf, role.c_str(), role.length());
					char *pch = strtok(buf,",");
					while (pch != NULL) {
						enum Role r = parse_role(std::string(pch));
						if(r == KEEPER && keeper_count < 2) {
							keeper_fl = true;
							pl->_rate = rate;
							//cout << pl->_name << endl;
							
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
				if(keeper_count < 2) {
					cout << "insufficient number of keepers" << endl;
					Player *pl = NULL;
					if(_team0._map.size()) {
						pl = *_team0._map.begin();
						pl->revoke_keeper();
						pl->_roles.insert(UNIVERSAL);
						_all_players.push_back(pl);	
						_team0.erase();
					} else {
						pl = *_team1._map.begin();
						pl->revoke_keeper();
						pl->_roles.insert(UNIVERSAL);
						_all_players.push_back(pl);
						_team1.erase();
					}
				}
				_avg_rate = sum / total_count;
				cout << endl << "sum: " << sum << " sum/2: " << sum/2.0 <<  " avg rate: " << _avg_rate <<  endl;
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
			bool fl, who_start = 0;
			double r0, r1;
			std::sort(_all_players.begin(), _all_players.end(), less_than_key());
			r0 = *_team0._map.begin() ? (*_team0._map.begin())->_rate : 0.0 ;
			r1 = *_team1._map.begin() ? (*_team1._map.begin())->_rate : 0.0 ;
			who_start = (r0 < r1);

			//cout << (*_team0._map.begin())->_name << endl;	
			//cout << (*_team1._map.begin())->_name << endl;	
			for(auto it = _all_players.begin(); it != _all_players.end(); ++it, cnt++) {
		//		cout << (*it)->_name << endl;	
				if((*it)->is_keeper()) {
					if(_keep_excl)
						(*it)->_rate = 0.0;
					else
						(*it)->_rate = (*it)->_rate / 2.0;
				}
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

			auto r0 = tmp_team0->calc_rate(_keep_excl), r1= tmp_team1->calc_rate(_keep_excl);
			//cout << "r0 = " << r0 << endl;
			//cout << "r1 = " << r1 << endl;
			if(r0 > r1) {
				tmp_team0 = &_team1;
				tmp_team1 = &_team0;
			}

			auto tmp = tmp_team0->_map.begin();

			delta = (tmp_team0->calc_rate(_keep_excl) - tmp_team1->calc_rate(_keep_excl));
			//cout << "delta = " << delta << endl;
			//if(0 == (ROUND_2_INT(10.0*delta) % 2)) {
			if((int)delta != 0) {
				//cout << "even case delta: " << delta << endl;
				while(tmp != tmp_team0->_map.end()) {
					if(_keep_excl && (*tmp)->is_keeper()) {
						tmp++;
						continue;
					}
					//cout << "team0 palyer name: "  << (*tmp)->_name << '\n';
					auto it = std::find_if(tmp_team1->_map.begin(), tmp_team1->_map.end(), [&tmp,&delta] (auto &arg) {
							if(arg->is_keeper()) {
								return false;
							}
							//cout << "team1 player name: "  << arg->_name << '\n';
							double cur_del = ((*tmp)->_rate - arg->_rate) - (delta/2.0);
							//cout << (*tmp)->_rate - arg->_rate   <<  " == " << (delta/2.0) << '\n';
							//cout << "cur_del == "   << cur_del << '\n';
							bool ret = (cur_del > -1) && (cur_del < 1);
							//cout << " ret: " << ret  <<  '\n';
							return ret; 
							}
							);
					if(it != tmp_team1->_map.end()) {
						(*it)->self_print();

						
						switch_players(tmp_team0, tmp_team1, *tmp, *it);
						
						tmp = tmp_team0->_map.begin();
		                                tmp_team0->calc_rate(_keep_excl);tmp_team1->calc_rate(_keep_excl);
		                                //cout << "t0: " << tmp_team0->calc_rate(_keep_excl) << " t1: " <<  tmp_team1->calc_rate(_keep_excl) << endl;
						break;
					} else {
						tmp++;
					}
				}
			} else {

				//cout << "eve " << abs(10.0*delta) <<  " "  <<((int)(10.0*delta)) << " " <<  ((10.0*delta)) % 2 << endl;
				while(tries++ < 99 && tmp != tmp_team0->_map.end() && abs(delta) >= 0.2) {
					if(_keep_excl && (*tmp)->is_keeper()) {
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
					delta = tmp_team0->calc_rate(_keep_excl) - tmp_team1->calc_rate(_keep_excl);
				} 
//				cout << endl << "total tries (permutations) = " << tries << endl;
//				if(tries < 100) 
//					cout << "team ratings is equal, don't need any additional stuff" << endl;
			}
		}
		void switch_players(Team *t0, Team *t1, Player *p0, Player *p1) {
		//	cout << p0->_name << " and " << p1->_name << endl;
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
			double delta = (_team0.calc_rate(_keep_excl) - _team1.calc_rate(_keep_excl));
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

		int rebalance() {
			int type = _selected_indicator;
			int red_ind = _team0.calc_by(type);
			int azure_ind = _team1.calc_by(type);
			//cout << "azure pc = " << azure_ind << endl;
			//cout << "red pc = " <<  red_ind << endl;
			int team_delta_ind = azure_ind - red_ind;
			//cout << "team delta pc = " <<  team_delta_ind << endl;
			if(std::abs(team_delta_ind) > 1) {
				double team_delta_rate = (_team0.calc_rate(_keep_excl) - _team1.calc_rate(_keep_excl));
				//try to rebalance
				for (auto it0 = _team0._map.begin(); it0 != _team0._map.end(); ++it0) {
					for (auto it1 = _team1._map.begin(); it1 != _team1._map.end(); ++it1) {
						if((*it0)->is_keeper() || (*it1)->is_keeper())
							continue;
						double cur_del = ((*it0)->_rate - (*it1)->_rate) - (team_delta_rate/2.0);
						int player_delta_ind = (*it0)->_pi[type] - (*it1)->_pi[type];
						//cout << "cur_del = " << cur_del << endl;
						int r0 = ROUND_2_INT(10.0*((*it0)->_rate));
						int r1 = ROUND_2_INT(10.0*((*it1)->_rate));
						bool equal = r0 == r1;
						//cout << "equal = " << (equal ? "true" : "false") << " player_delta_ind = " << player_delta_ind << endl;
						if(equal && player_delta_ind) {
							if((player_delta_ind < 0 && team_delta_ind > 0) || (player_delta_ind > 0 && team_delta_ind < 0)) {
								//cout << " swap players: " << (*it1)->_name << " and " << (*it0)->_name << " player delta pc = " << player_delta_ind << endl;
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
			_team0.calc_by(_selected_indicator);
			_team1.calc_by(_selected_indicator);
			std::chrono::time_point<std::chrono::system_clock> time_now = std::chrono::system_clock::now();
			std::time_t time_now_t = std::chrono::system_clock::to_time_t(time_now);
			std::tm now_tm = *std::localtime(&time_now_t);
			char buf[512];
			std::strftime(buf, 512, "%d.%m.%Y", &now_tm);
			cout << "Составы ФЛИ " << buf << endl;
			if(keep_excl)
				cout << "Без учёта";
			else 
				cout <<  "С учётом ";
			cout << " вратарей" << endl;
			if(!keep_excl)
				cout << " ( Рейтинг вратаря уполовинивается ) " << endl;
			else
				cout << " ( Рейтинг вратаря принимается равным нулю ) " << endl;
			//if ( IND_OVERALL == _selected_indicator )
			//	cout << "Балансировка выполнена по общему показателю. \n\n";
			//else
			cout << "Балансировка выполнена по показателю: " << ind_type_to_str(_selected_indicator) << "\n\n";
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
		bool _keep_excl;
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
	if(argc > 1) {
		input_file = string(argv[1]);
	} else {
		input_file = "input.txt";
	}
	int how_to_balance = IND_PHYS_COND;
	bool keep_excl = false;
	bool shake = true;
	if(argc > 2) {
		keep_excl = !(atoi(argv[2]));
	}
	if(argc > 3) {
		how_to_balance = atoi(argv[3]);
		//cout << "Chosed method: " << ind_type_to_str(how_to_balance) << "\n";
	}
	if(argc > 4) {
		shake = atoi(argv[4]);
		how_to_balance = IND_OVERALL;
	}
	FliGen fligen(how_to_balance, keep_excl);
	if(fligen.parseFile(input_file)) {
		cout << "error parsing input file\n";
		exit(13);
	}

	//fligen.splitOfferV1();
	fligen.splitOfferV2();
	if(shake) {
		fligen.shake();
	
		int num_tries = 0;
		while(0 != fligen.rebalance()) {
			if( ++num_tries > 100)
				break;
		}
	}


	fligen.prettyPrintResult(keep_excl);
	print_version(argv[0]);
	fligen.printResult();

}
