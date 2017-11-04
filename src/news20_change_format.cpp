#include <set>
#include <sstream>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <fstream>

using namespace std;

void getLabelAndSet(string s, int32_t& label, vector<uint32_t>& elements) {
	for(uint32_t i = 0;i < s.length(); ++i) {
		if(s[i] == ':') s[i] = ' ';
	}
	istringstream ss(s);
	assert(ss >> label);
	elements = vector<uint32_t>();
	uint32_t a;
	float b;
	while(ss >> a) {
		elements.push_back(a);
		assert(ss >> b);
	}
}

vector<vector<uint32_t> > sets;

void readSets() {
	ifstream input("data/news20.binary", ios::in);
	string line;
	int num = 0;
	while(getline(input,line)) {
		int32_t dummyLabel;
		sets.push_back(vector<uint32_t>());
		getLabelAndSet(line, dummyLabel, sets.back());
		sort(sets.back().begin(), sets.back().end());
		++num;
		if(num%1000 == 0) {
			cout << "num = " << num << endl;
		}
	}
	input.close();
}
void writeSets() {
	ofstream out("data/news20-fast.txt", ios::out);
	uint32_t n = sets.size();
	out << n << endl;
	for(uint32_t i = 0;i < n; ++i) {
		out << sets[i].size();
		for(uint32_t j = 0;j < sets[i].size(); ++j) {
			out << " " << sets[i][j];
		}
		out << endl;
	}
	out.close();
}

int main() {
	clock_t start = clock();
	readSets();
	clock_t end = clock();
    cout << "Reading input took " << (float)(end-start)/CLOCKS_PER_SEC << " seconds." << endl;
	cout << "size = " << sets.size() << endl;
	start = clock();
	writeSets();
	end = clock();
    cout << "Writing output took " << (float)(end-start)/CLOCKS_PER_SEC << " seconds." << endl;
	
	return 0;
}
