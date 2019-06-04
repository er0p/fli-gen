#include <set>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cstring>


#define VERSION_STR "2.2"

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
		int _id;
		double _rate;
		string _name;
		set<int> _roles;
		void self_print() {
			//cout << "id: " << _id << " rate: " << _rate << " name: " << _name << endl;
			cout << "id: " << setw(3) << _id << " rate: " << setw(5) << _rate << " name: " << std::left << setw(15) << _name << std::right <<  setw(15) << " roles: " << roles_to_str(_roles) <<  endl;
		}
		bool is_keeper() {
			return _roles.find(KEEPER) != _roles.end();;
		}
		
};

class Team {
	public:
		Team(string name) : _name(name) {
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
				_rate += (*it)->_rate;
				for(auto jt = (*it)->_roles.begin(); jt != (*it)->_roles.end(); ++jt) {
					_role_stat[*jt]++;
				}
			}
			return _rate;
		}

		//map<int, class Player *> _map;
		set<class Player *> _map;
		double _rate;
	private:
		string _name;
		size_t _role_stat[NR] = {};
};

struct less_than_key {
    inline bool operator() (const class Player *a, const class Player *b) {
        return (a->_rate < b->_rate);
    }
};



class FliGen {
	public:
		FliGen() : _team0 ("red"), _team1 ("blue") {
		};
		FliGen(string t0_name, string t1_name) : _team0(t0_name), _team1(t1_name) {
		};
		virtual ~FliGen() {
			cout << __FUNCTION__ << endl;
			for (auto it : _all_players) {
				delete it;
			}
		}

		void parseFile(string &path) {
			ifstream in(path);
			int key = 0;
			char buf[1024];
			double rate = 0;
			double sum = 0;
			string role, name;
			bool keeper_fl = false;
			size_t keeper_count = 0;
			size_t total_count = 0;
			while(in >> key >> rate >> role >> name) {
				Player *pl = new Player();
				cout << key << " " << rate << " " << role << " " << name << endl;
				pl->_id = key;
				//name.erase(remove(name.begin(), name.end(), ' '), name.end());
				pl->_name = name;
				pl->_rate = rate;
				memset(buf, 0,1024);
				strncpy(buf, role.c_str(), role.length());
				char *pch = strtok(buf,",");
				while (pch != NULL) {
					enum Role r = parse_role(std::string(pch));
					if(r == KEEPER) {
						keeper_fl = true;
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
			cout << endl << "sum: " << sum << " sum/2: " << sum/2.0 <<  " avg rate: " << _avg_rate <<  endl;
		}

		void splitOfferV1() {
			size_t cnt = 0;
			std::sort(_all_players.begin(), _all_players.end(), less_than_key());
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
			delta = (_team0.calc_rate() - _team1.calc_rate());
			size_t ind = 0;	
			auto tmp = _team0._map.begin();
			while(tries++ < 99 && tmp != _team0._map.end() && abs(delta) >= 0.2) {
				if((*tmp)->is_keeper()) {
					tmp++;
					continue;
				}
				auto it = std::find_if(_team1._map.begin(), _team1._map.end(), [&tmp,&delta] (auto &arg) {
						if(arg->is_keeper()) {
							return false;
						}
						double eps = ((*tmp)->_rate - arg->_rate) - delta;
						bool ret = (eps >= 0.0) && (eps < 0.2);
						return ret; 
						}
						);
				if(it != _team1._map.end()) {
					(*it)->self_print();
					Player *p1 = *tmp, *p2 = *it;
					_team0._map.erase(tmp);
					_team1._map.erase(it);
					_team0._map.insert(p2);
					_team1._map.insert(p1);
					tmp = _team0._map.begin();
				} else {
					tmp++;
				}
				delta = _team0.calc_rate() - _team1.calc_rate();
			} 
			cout << endl << "total tries (permutations) = " << tries << endl;
			if(tries < 100) 
				cout << "team ratings is equal, don't need any additional stuff" << endl;
		}
		
		void printResult() {
			cout << endl;
			_team0.self_print();
			cout << endl;
			_team1.self_print();
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
	fligen.parseFile(input_file);
	fligen.splitOfferV2();
	fligen.printResult();

	print_version(argv[0]);
}
