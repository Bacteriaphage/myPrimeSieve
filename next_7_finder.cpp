#include "next_7_finder.h"

extern _u8 digits[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};   //put each base digit into this, the last is a counter
extern bool consecutive = false;
extern _ulong low=0;
extern _ulong high=0;

_ulong init_finder(_ulong sieve_base) {
	consecutive = false;
	high = 0;
	low = 0;
	for (_u8 i = 0; i < 20; i++) {
		digits[i] = 0;
	}

	while (sieve_base) {                           // put into array
		digits[digits[19]] = (sieve_base % 10);
		sieve_base = sieve_base / 10;
		digits[19]++;
	}
	_u8 i, j, k;
	_ulong current = 0;
	for (i = digits[19] - 1; i >= 0; i--) {        //find the first 7, because this is initialize
		if (digits[i] == 7) break;
	}
	switch (i) {
	case -1: case 0: {                             //the least significant digit is 7 or no one is 7
		if (digits[0] < 7) {
			digits[0] = 7;                 //if the last one is less than 7, make it as 7
		}
		else {
			for (j = 1; ;j++) {            //consider carry, find next number which contain 7
				if (digits[j] != 9) break;
			}
			if (digits[j] == 6) {
				low = 0;
				digits[0] = 9;
				for (i = 18; i >= 0; i--) {
					low = low * 10 + digits[i];
				}
				low++;
				digits[j] = 8;
				consecutive = true;
			}
			else {
				digits[j]++;
			}
			for (k = j - 1;k > 0; k--) {
				digits[k] = 0;
			}
			if (consecutive == true) {
				high = 0;
				digits[0] = 0;
				for (_u8 h = 18; h >= 0; h--) {
					high = high * 10 + digits[h];
				}
			}
			digits[k] = 7;
		}
		break;
	}
	default: {
		low = 0;	
		for (_u8 h = 18; h >= 0; h--) {
			low = low * 10 + digits[h];
		}
		digits[i] = 8;
		consecutive = true;

		for (k = i - 1;k > 0; k--) {
			digits[k] = 0;
		}
		if (consecutive == true) {
			high = 0;
			digits[0] = 0;
			for (_u8 h = 18; h >= 0; h--) {
				high = high * 10 + digits[h];
			}
		}
		digits[k] = 7;
		break;
	}
	}
	for (_u8 h = 18; h >= 0; h--) {
		current = current * 10 + digits[h];
	}
	return current;                               //return the first 7 in this range 
}

_ulong next_7() {
	_u8 i,j,k;
	_ulong current = 0;
	if (consecutive == true) { 
		low = 0;
		high = 0;
		consecutive = false; }
	for (j = 1; ;j++) {
		if (digits[j] != 9) break;
	}
	if (digits[j] == 6) {
		low = 0;
		digits[0] = 9;
		for (i = 18; i >= 0; i--) {
			low = low * 10 + digits[i];
		}
		low++;
		digits[j] = 8;
		consecutive = true;
	}
	else {
		digits[j]++;
	}
	for (k = j - 1;k > 0; k--) {
		digits[k] = 0;
	}
	if (consecutive == true) {
		high = 0;
		digits[0] = 0;
		for (_u8 h = 18; h >= 0; h--) {
			high = high * 10 + digits[h];
		}
	}
	digits[k] = 7;

	for (_u8 h = 18; h >= 0; h--) {
		current = current * 10 + digits[h];
	}
	return current;
}
