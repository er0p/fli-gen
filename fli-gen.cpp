#include <set>
#include <vector>
#include <map>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cstring>

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
	HALFBACK,
	DEFENDER,
	FORWARD
};

class Player {
	public:
		int _id;
		double _rate;
		string _name;
		set<int> _roles;
		void print_myself() {
			cout << "id: " << _id << " rate: " << _rate << " name: " << _name << endl;
		}
		
};

enum Role parse_role(string in) {
	enum Role ret = UNDEF;
	if(in == "вратарь") {
		ret = KEEPER;
	} else if (in == "защитник") {
		ret = DEFENDER;
	} else if (in == "полузащитник") {
		ret = HALFBACK;
	} else if (in == "форвард") {
		ret = FORWARD;
	}
	return ret;
}

void print_team(std::map<int, class Player> &t, string &name) {
	double srate = 0.0;
	cout << "==== "<< name << " ====" << endl;
	for(auto it = t.begin(); it != t.end(); ++it) {
		it->second.print_myself();
		srate += it->second._rate;
	}
	cout << "total rate: " << srate << endl;
}

struct less_than_key
{
    inline bool operator() (const class Player& a, const class Player& b)
    {
        return (a._rate < b._rate);
    }
};

int main() {
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
#if 0
	for(auto it = map.begin(); it != map.end(); ++it, cnt++) {
		if(cnt % 2) {
			printf("1\n");
			team0.insert(*it);
		} else {
			printf("2\n");
			team1.insert(*it);
		}
	}
#endif
	std::sort(pl_vec.begin(), pl_vec.end(), less_than_key());
	for(auto it = pl_vec.begin(); it != pl_vec.end(); ++it, cnt++) {
		if(cnt % 2) {
			team0.insert(make_pair(it->_id, *it));
		} else {
			team1.insert(make_pair(it->_id, *it));
		}
	}
	string t0 = "red", t1 = "blue";
	print_team(team0, t0);
	print_team(team1, t1);

#if 0
	if ((vsum / 1) || !findsum(val.begin()+1, val.end(), val[0], vsum/2)) {
		out << "NO";
	} else {
		int i, sum = 0;
		for (i = 0; sum < vsum/2; sum += val[i++]) out << val[i] << " ";
		out << endl;
		while (i < val.size()) out << val[i++] << " ";
	}
#endif
}
