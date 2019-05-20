#include <set>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cstring>


#define VERSION_STR "2.0"

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
		void print_myself() {
			//cout << "id: " << _id << " rate: " << _rate << " name: " << _name << endl;
			cout << "id: " << setw(3) << _id << " rate: " << setw(5) << _rate << " name: " << std::left << setw(15) << _name << std::right <<  setw(15) << " roles: " << roles_to_str(_roles) <<  endl;
		}
		
};

enum Role parse_role(string in) {
	enum Role ret = UNDEF;
	if(in == "вратарь") {
		ret = KEEPER;
	} else if (in == "защитник" || in == "центральныйзащитник") {
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

void print_version(char *app_0) {
	printf("\n\n\nfli-gen (Football League InfoTeCS Generator)\n"
			"\tCopyright (C) 2018-%s FLI Inc.\n"
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

void print_team(std::map<int, class Player> &t, string &name) {
	double srate = 0.0;
	size_t cnt[NR] = {};
	cout << "\n========= "<< name << " =========" << endl;
	for(auto it = t.begin(); it != t.end(); ++it) {
		it->second.print_myself();
		srate += it->second._rate;
		for(auto jt = it->second._roles.begin(); jt != it->second._roles.end(); ++jt) {
			cnt[*jt]++;
		}
	}
	cout << "total rate: " << srate << endl;
	cout << "possible role as:" <<
	        "\n\tKeeper     : " << cnt[KEEPER] <<
		"\n\tDefender   : " << cnt[DEFENDER] <<
	        "\n\tMidfielder : " << cnt[MIDFIELDER] <<
	        "\n\tForward    : " << cnt[FORWARD] <<
	        "\n\tUniversal  : " << cnt[UNIVERSAL] <<
	       	endl;
}

struct less_than_key
{
    inline bool operator() (const class Player& a, const class Player& b)
    {
        return (a._rate < b._rate);
    }
};

int main(int argc, char *argv[]) {
	ifstream in("input.txt");
	map<int, class Player> team0, team1, map;
	vector<class Player> pl_vec;
	int key = 0;
	char buf[1024];
	double rate = 0;
	double sum = 0;
	string role, name;
	bool keeper_fl = false;
	while(in >> key >> rate >> role >> name) {
		Player pl;
		cout << key << " " << rate << " " << role << " " << name << endl;
		pl._id = key;
		//name.erase(remove(name.begin(), name.end(), ' '), name.end());
		pl._name = name;
		pl._rate = rate;
		memset(buf, 0,1024);
		strncpy(buf, role.c_str(), role.length());
		char *pch = strtok(buf,",");
		while (pch != NULL) {
			enum Role r = parse_role(std::string(pch));
			if(r == KEEPER) {
				keeper_fl = true;
				if(0 == team0.size()) {
					team0.insert(make_pair(key,pl));
				} else {
					team1.insert(make_pair(key,pl));
				}
			}	
			pl._roles.insert(r);
			//cout << pch  << endl;
			pch = strtok (NULL, ",");
		}
		if(!keeper_fl) {
			pl_vec.push_back(pl);	
			//map.insert(make_pair(key,pl));
		} 
		keeper_fl = false;	
		sum += rate;
	}
	cout << "sum: " << sum << " sum/2: " << sum/2.0 <<  endl;
	size_t cnt = 0;
	
	team0.begin()->second._roles.insert(KEEPER);
	team1.begin()->second._roles.insert(KEEPER);
 
	std::sort(pl_vec.begin(), pl_vec.end(), less_than_key());
	bool fl, who_start = team0.begin()->second._rate < team1.begin()->second._rate;
	for(auto it = pl_vec.begin(); it != pl_vec.end(); ++it, cnt++) {
		fl = who_start ? cnt % 2 : !(cnt %2);
		if(fl) {
			team0.insert(make_pair(it->_id, *it));
		} else {
			team1.insert(make_pair(it->_id, *it));
		}
	}
	string t0 = "red", t1 = "blue";
	print_team(team0, t0);
	print_team(team1, t1);

	print_version(argv[0]);
}
