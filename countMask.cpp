#include <fstream>
#include <iomanip>

using namespace std;

char mask_4[16]={4, 3, 3, 2,
				 3, 2, 2, 1,
				 3, 2, 2, 1,
				 2, 1, 1, 0};

int main(){
	ofstream out("mask_16.cpp");
	for(int i = 0; i < (1 << 16); i++){
		int zero = 0;
		int now = i;
		for(int j = 0; j < 4; j++){
			zero += mask_4[now & 0xf];
			now = now >> 4u;
		}
		out << setw(2) <<zero << ',';
		if(i%256==255) out << '\n';
	}
}