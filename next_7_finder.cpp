#include "next_7_finder.h"

extern _u8 digits[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
extern bool consecutive = false;
extern _ulong low=0;
extern _ulong high=0;

_ulong init_finder(_ulong sieve_base) {
	consecutive = false;
	for (_u8 i = 0; i < 20; i++) {
		digits[i] = 0;
	}

	while (sieve_base) {
		digits[digits[19]] = (sieve_base % 10);
		sieve_base = sieve_base / 10;
		digits[19]++;
	}
	_u8 i, j, k;
	_ulong current = 0;
	for (i = digits[19] - 1; i >= 0; i--) {
		if (digits[i] == 7) break;
	}
	switch (i) {
	case -1: case 0: {
		if (digits[0] < 7) {
			digits[0] = 7;
		}
		else {
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
				for (i = 18; i >= 0; i--) {
					high = high * 10 + digits[i];
				}
			}
			digits[k] = 7;
		}
		break;
	}
	default: {
		if (digits[i] == 6) {
			low = 0;
			digits[i] = 8;
			for (i = 18; i >= 0; i--) {
				low = low * 10 + digits[i];
			}
			consecutive = true;
		}
		else {
			digits[i]++;
		}
		for (k = i - 1;k > 0; k--) {
			digits[k] = 0;
		}
		if (consecutive == true) {
			high = 0;
			digits[0] = 0;
			for (i = 18; i >= 0; i--) {
				high = high * 10 + digits[i];
			}
		}
		digits[k] = 7;
		break;
	}
	}
	for (i = 18; i >= 0; i--) {
		current = current * 10 + digits[i];
	}
	return current;
}

_ulong next_7() {
	_u8 i,j,k;
	_ulong current = 0;
	if (consecutive == true) consecutive = false;
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
		for (i = 18; i >= 0; i--) {
			high = high * 10 + digits[i];
		}
	}
	digits[k] = 7;

	for (i = 18; i >= 0; i--) {
		current = current * 10 + digits[i];
	}
	return current;
}
