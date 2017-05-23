/***********************************************************************************
* Title      :Fast_Sieve
* Written by :Hanyu Zhang, Yunfei Lu
* Date       :03/15/2016
* Description:This program aims to count number of primes in a large domain. It can 
* exclude all primes which contain digit "7".
************************************************************************************/


#include <iostream>
#include <cmath>
#include <time.h>
#include "config.h"
#include "next_7_finder.h"

using namespace std;

extern const unsigned char mask_16[65536]; //a hash map liked data structure, map pattern to number of 1 in this pattern
extern _u8 digits[20];
extern bool consecutive;
extern _ulong low, high;

//mark_mask is used to make pattern, crossoff non-prime and test 
//reference: http://sweet.ua.pt/tos/software/prime_sieve.html
const _ulong mark_mask[64u] =                                   
{
	0x0000000000000001ull,0x0000000000000002ull,0x0000000000000004ull,0x0000000000000008ull,
	0x0000000000000010ull,0x0000000000000020ull,0x0000000000000040ull,0x0000000000000080ull,
	0x0000000000000100ull,0x0000000000000200ull,0x0000000000000400ull,0x0000000000000800ull,
	0x0000000000001000ull,0x0000000000002000ull,0x0000000000004000ull,0x0000000000008000ull,
	0x0000000000010000ull,0x0000000000020000ull,0x0000000000040000ull,0x0000000000080000ull,
	0x0000000000100000ull,0x0000000000200000ull,0x0000000000400000ull,0x0000000000800000ull,
	0x0000000001000000ull,0x0000000002000000ull,0x0000000004000000ull,0x0000000008000000ull,
	0x0000000010000000ull,0x0000000020000000ull,0x0000000040000000ull,0x0000000080000000ull,
	0x0000000100000000ull,0x0000000200000000ull,0x0000000400000000ull,0x0000000800000000ull,
	0x0000001000000000ull,0x0000002000000000ull,0x0000004000000000ull,0x0000008000000000ull,
	0x0000010000000000ull,0x0000020000000000ull,0x0000040000000000ull,0x0000080000000000ull,
	0x0000100000000000ull,0x0000200000000000ull,0x0000400000000000ull,0x0000800000000000ull,
	0x0001000000000000ull,0x0002000000000000ull,0x0004000000000000ull,0x0008000000000000ull,
	0x0010000000000000ull,0x0020000000000000ull,0x0040000000000000ull,0x0080000000000000ull,
	0x0100000000000000ull,0x0200000000000000ull,0x0400000000000000ull,0x0800000000000000ull,
	0x1000000000000000ull,0x2000000000000000ull,0x4000000000000000ull,0x8000000000000000ull
};
/*
void mark_1(pattern, i){
	pattern[i >> 6u] |= (1ull << (i & 63u));
}
*/
# define mark_1(s,o)  (s)[(o) >> 6u] |= (1ull << ((o) & 63u))

# define test_1(s,o)  (((s)[(o) >> 6u] & (1ull << ((o) & 63u))) == 0ull)

# define mark_2(s,o)  (s)[(o) >> 6u] |= mark_mask[(o) & 63u]

# define test_2(s,o)  (((s)[(o) >> 6u] & mark_mask[(o) & 63u]) == 0ull)

# define unmark(s,o)  (s)[(o) >> 6u] &= ~mark_mask[(o) & 63u]

_ulong pattern[3u * 5u * 7u * 11u * 13u];  // sieve initialization pattern 15015u

//deal with small sieve_limit
_uint smallprime[40]=
{
	2,  3,  5,  11,  13,  19,
	23, 29, 31, 41,  43,
	56, 59, 61, 83,  89,
	101,103,109,113,131, 139,
	149,151,163,181,191, 193,
	199,211,223,229,233, 239,
	241,251,263,269,281, 283
};
//initial pattern, set every multiple of prime as 0
void init_pattern() {    //initialize a new range with pre-sieved domain
	_uint i;

	for (i = 0u;i < 3u * 5u * 7u * 11u * 13u;i++)
		pattern[i] = (_ulong)0u;
	for (i = (3u >> 1u);i < 3u * 5u * 7u * 11u * 13u * 8u * _pointer_size_;i += 3u)
		mark_2(pattern, i);
	for (i = (5u >> 1u);i < 3u * 5u * 7u * 11u * 13u * 8u * _pointer_size_;i += 5u)
		mark_2(pattern, i);
	for (i = (7u >> 1u);i < 3u * 5u * 7u * 11u * 13u * 8u * _pointer_size_;i += 7u)
		mark_2(pattern, i);
	for (i = (11u >> 1u);i < 3u * 5u * 7u * 11u * 13u * 8u * _pointer_size_;i += 11u)
		mark_2(pattern, i);
	for (i = (13u >> 1u);i < 3u * 5u * 7u * 11u * 13u * 8u * _pointer_size_;i += 13u)
		mark_2(pattern, i);
}

//Bucket data structure can store prime and offset of current prime in next sieve segment
//reference: http://sweet.ua.pt/tos/software/prime_sieve.html
//Bucket size should be designed as a cache friendly size
class Bucket {
public:
	_uint prime;
	_uint offset;
};

class Bucket_List {
public:
	//need to be cache friendly here
	Bucket buck[(1u << 9u) - 1u];
	_uint size;
	Bucket_List * next;

	Bucket_List() : next(nullptr), size(0) {}

	bool addBucket(_uint prime, _uint o) {
		buck[size].prime = prime;
		buck[size].offset = o;
		size++;
		if (size == ((1u << 9u) - 1u)) {
			//			cout << "this list is full!";
			return false;
		}
		else return true;
	}
};

Bucket_List* availible_buck = nullptr;

//generate new bucket and link it to the end of bucket linked list
Bucket_List* create_Bucket() {
	if (availible_buck == nullptr) {
		availible_buck = new Bucket_List;
		return availible_buck;
	}
	else {
		Bucket_List * process = availible_buck;
		for (;process->next != nullptr; process = process->next);
		process->next = new Bucket_List;
		return process->next;
	}
}

_ulong sieve_base;         //lower bound of range
_ulong sieve_limit;        //upper bound of range
_ulong sieve_span;         //how many long long element we need to store the whole range
_ulong sieve[_sieve_word_];//one sieve segment
_ulong prime_counter = 0;
_ulong count_7 = 0;

//use auxiliary sieve to generate Bucket_List
_uint aux_bound;
_ulong *aux_sieve;
_uint aux_sieve_words;

inline _uint find_next_offset(_ulong this_sieve_base, _uint prime) {
	_uint o;
	_ulong t = static_cast<_ulong> (prime) * static_cast<_ulong> (prime); //next multiple we need take care
	while (1) {
		if (t < this_sieve_base + _sieve_word_ * 128u) { //next multiple in this range
			if (t > this_sieve_base)
				o = static_cast<_uint> (t - this_sieve_base) >> 1u;       //offset of t based on current base
			else
			{
				o = prime - static_cast<_uint>(this_sieve_base % (static_cast<_ulong> (prime)));   //offset of prime first multiple
				if ((o & 1u) == 0u)
					o = (o >> 1u) + (prime >> 1u); // arithmetic overflow not possible
				else
					o = (o >> 1u);
			}
			return o;
		}
		this_sieve_base += _sieve_word_ * 128u; //next multiple is not in current range, find it in next range
	}
}

//use pattern and Eratosthenes sieve to generate auxiliary sieve array and Bucket
//because the size of aux_size is sqrt(sieve_limit), it can fully generate prime numbers and their offset.
void bucketGenerator() {
	_uint k;
	Bucket_List *b = availible_buck;
	if (b != nullptr)
		for (; b->next != nullptr; b = b->next);// go to the last bucket list
	init_pattern();
	//initial aux_sieve array with intialized pattern
	for (int i = 0u;i < aux_sieve_words;i += k)
	{
		int j = aux_sieve_words - i;            // remaining sieve words
		k = 3u * 5u * 7u * 11u * 13u;           // remaining pattern words
		if (j < k)
			k = j;
		for (j = 0u;j < k;j++)
			aux_sieve[i + j] = pattern[j];
	}
	_uint next_prime = 17;
	_uint li;
	//aux_sieve is a half-mature segment, it will gradually finished by this step. It is a checking array and a object array.
	while (next_prime < aux_bound) {
		li = next_prime / 2u;  //because we have already eliminate 2, the position of number we need to check should be half
		if (test_2(aux_sieve, li)) { //if the next_prime is prime number.
			//don`t have a bucket or bucket is full
			if (b == nullptr || b->size == ((1u << 9u) - 1u)) {
				b = create_Bucket();
			}
			//calculate next multiple position
			_uint o = find_next_offset(sieve_base,next_prime);
			//put the pair into bucket
			b->addBucket(next_prime, o);
			for (_uint i = (next_prime >> 1u);i < (aux_bound >> 1u);i += next_prime)
				mark_2(aux_sieve, i);  //update aux_sieve with the new next_sieve
		}
		next_prime += 2;
	}
}

//pre-sieve the segment at the first time
void init_sieve(_ulong this_sieve_base) {
	_uint offset; //the offset of this_sieve_base based on pattern. it determine where we start to pre-sieve the new segment
	_uint k;
	_uint j;
	offset = this_sieve_base % (3u * 5u * 7u * 11u * 13u);
	offset = (offset + ((offset * 105u) & 127u) * 3u * 5u * 7u * 11u * 13u) >> 7u;
	if (sieve_span > _sieve_word_) {
		for (_uint i = 0u;i < _sieve_word_;i += k) {
			j = _sieve_word_ - i;                  // remaining sieve words
			k = 3u * 5u * 7u * 11u * 13u - offset; // remaining pattern words
			if (j < k)
				k = j;
			for (j = 0u;j < k;j++)
				sieve[i + j] = pattern[offset + j];
			offset = 0u;
		}
	}
	else {
		for (_uint i = 0u;i < sieve_span; i += k) {
			_uint j = sieve_span - i;
			k = 3u * 5u * 7u * 11u * 13u - offset;
			if (j < k)
				k = j;
			for (j = 0u;j < k;j++)
				sieve[i + j] = pattern[offset + j];
			offset = 0u;
		}
	}
}

//crossoff every number containing 7
void crossoff_7(_ulong this_sieve_base, _uint this_sieve_span) {
	_ulong next;
	_uint  offset;
	_ulong this_sieve_limit = this_sieve_base + this_sieve_span * 128;
	if (this_sieve_limit > sieve_limit) this_sieve_limit = sieve_limit;
	next = init_finder(this_sieve_base);
	//get a number contain 7 each time. meet a consecutive segment, directly do crossoff without call find_next function
	while (next <= this_sieve_limit || consecutive == true) {
		if (consecutive == true) {
			for (_ulong num = (low/2)*2+1; (num < high) && (num < this_sieve_limit); num += 2) {
				offset = ((num - this_sieve_base) / 2);
//				cout << num << '-' << offset << endl;
				mark_2(sieve, offset);
//				count_7++;
			}
		}
		if (next > this_sieve_limit) break;
		offset = (next - this_sieve_base) / 2;
//		cout << next << '-' << offset << endl;
		mark_2(sieve, offset);
//		count_7++;
		next = next_7();
	}
}

//count number of primes in one segment
_ulong count(_uint this_sieve_span) {
	_uint index = 0;
	_ulong now;
	_ulong zero = 0;
	_uint  j;
	if (this_sieve_span == _sieve_word_) {
		for (; index < this_sieve_span; index++) {
			now = sieve[index];
			for (j = 0; j < 4; j++) {
				zero += mask_16[now & 0xffff];
				now = now >> 16u;
			}
		}
	}
	else {
		for (; index < this_sieve_span - 1; index++) {
			now = sieve[index];
			for (j = 0; j < 4; j++) {
				zero += mask_16[now & 0xffff];
				now = now >> 16u;
			}
		}
		_uint offset = ((sieve_limit - sieve_base) / 2) % 64;
		now = sieve[this_sieve_span - 1];
		for (;offset > 0; offset--) {
			if ((now | 0xfffffffffffffffe) == 0xfffffffffffffffe)
				zero++;
			now = now >> 0x1u;
		}
	}
	return zero;
}

_ulong start_sieve(_ulong this_sieve_base) {
	init_sieve(this_sieve_base);
	Bucket_List *doIt = availible_buck;
	_uint prime_index = 0;
	_uint this_sieve_span;
	//this segment is not the last segment
	if (sieve_span > _sieve_word_) {
		this_sieve_span = _sieve_word_;
	}
	else this_sieve_span = sieve_span;
	if (doIt == nullptr) {
		cout << "Bucket fail!";
		return -1;
	}
	//traverse bucket list to get next prime multiple number and the offset, check if it is in this span
	while ((doIt->buck[prime_index].prime * doIt->buck[prime_index].prime) < (this_sieve_base + this_sieve_span * 128u)) {
		_uint o = doIt->buck[prime_index].offset;
		_uint p = doIt->buck[prime_index].prime;
		//for this prime, sieve it in this segment
		for (; o < this_sieve_span * 64; o += p)
			mark_2(sieve, o);
		//update the offset for this prime number
		doIt->buck[prime_index].offset = o - this_sieve_span * 64;
		//go to next bucket list if we go to the end
		if (prime_index == doIt->size - 1u) {
			if (doIt->next == nullptr)
				break;
			doIt = doIt->next;
			prime_index = 0;
		}
		else prime_index++;
	}
	crossoff_7(this_sieve_base, this_sieve_span);
	return count(this_sieve_span);
}

int main() {
	clock_t start, finish;
	cout << "Fast sieve configuration: " << endl;
	cout << "Max sieve segment: " << _sieve_word_ * 128 << endl;
	cout << "number of threads: " << "1" << endl;
	cout << "estimating sieve time: " << endl;
	cout << "10^6          0.06s" << endl << "10^7          0.08s" << endl << "10^8          0.33s" << endl << "10^9          2.21s" << endl;
	cout << "10^10         20.20s" << endl;
	cout << "this test based on Microsoft Visual Studio 2015 Release x64!" << endl << endl;
	cout << "please enter base and limit of your domain! " << endl;
	cout << "base: ";
	cin >> sieve_base;
	cout << "limit: ";
	cin >> sieve_limit;
	start = clock();
	sieve_span = (sieve_limit - sieve_base) / 128u + 1u;
	aux_bound = sqrt(sieve_limit) + 1;
	aux_sieve = new _ulong[aux_bound / 128u + 1u];
	aux_sieve_words = aux_bound / 128u + 1u;
	_ulong this_sieve_base = sieve_base;
	if (sieve_limit > 288) {
		//initialize bucket with a initializing aux_sieve_array.
		bucketGenerator();
		//traverse each segment
		for (_uint i = 0; i < ((sieve_limit - sieve_base) / 128u + 1u) / _sieve_word_ + 1u; i++) {
			//cross off 7 initialize
			init_finder(this_sieve_base);
//			update: we cannot directly jump a large range which dominated by 7. because we nned make sure the bucket list
//                              can be generated continueously. One way to do optimize is not count this kind of range. However, we need
//                              traverse these range at least once.
////                    this still need improve!!!
////			if ((consecutive == 1) && (high >(this_sieve_base + _sieve_word_ * 128)) && (this_sieve_base == low)) { 
////				this_sieve_base += _sieve_word_ * 128;
////				sieve_span = (sieve_limit - this_sieve_base) / 128u + 1u;
////				continue; 
////			}
			prime_counter += start_sieve(this_sieve_base);
			this_sieve_base += _sieve_word_ * 128;
			sieve_span = (sieve_limit - this_sieve_base) / 128u + 1u;
		}
		_uint low_index = 0;
		for (int i = 4;i >= 1; i--) {
			if (sieve_base < smallprime[i]) low_index++;
		}
		prime_counter += low_index;
		cout << "number of prime excluding 7: ";
		cout << prime_counter;

	}
	else {
		_uint low_index = 0;
		_uint high_index = 0;
		for (_uint i = 0;i < 40; i++) {
			if (sieve_limit >= smallprime[i]) high_index++;
			if (sieve_base > smallprime[i]) low_index++;
		}
		prime_counter = high_index - low_index;
		cout << "number of prime excluding 7: ";
		cout << prime_counter;
	}
	finish = clock();
	cout << endl << "time: "<<(double)(finish - start) / CLOCKS_PER_SEC;
#if 0
	_ulong current = 23450000;
	cout << init_finder(current);
	while (current < 23456789) {
		if (consecutive == true) {
			cout << "[" << low << "," << high << "]" << " ";
		}
		current = next_7();
		cout << current << " ";
	}
#endif
	return 0;
}
