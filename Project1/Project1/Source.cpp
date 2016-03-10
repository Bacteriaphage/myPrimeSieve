#include <iostream>
#include <cmath>
using namespace std;

#ifndef _pointer_size_
#define _pointer_size_ 8u
#endif

#ifndef _sieve_word_
#define _sieve_word_ (1u << 17u)
#endif

typedef unsigned int       _uint;
typedef unsigned long long _ulong;

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

# define mark_1(s,o)  (s)[(o) >> 6u] |= (1ull << ((o) & 63u))
# define test_1(s,o)  (((s)[(o) >> 6u] & (1ull << ((o) & 63u))) == 0ull)
# define mark_2(s,o)  (s)[(o) >> 6u] |= mark_mask[(o) & 63u]
# define test_2(s,o)  (((s)[(o) >> 6u] & mark_mask[(o) & 63u]) == 0ull)
# define unmark(s,o)  (s)[(o) >> 6u] &= ~mark_mask[(o) & 63u]

_ulong pattern[3u * 5u * 7u * 11u * 13u];  // sieve initialization pattern

void init_pattern() {
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

class Bucket {
public:
	_uint prime;
	_uint offset;
};

class Bucket_List {
public:
	Bucket buck[(1u << 9u) - 1u];
	_uint size;
	Bucket_List * next;

	Bucket_List() : next(nullptr), size(0) {}

	~Bucket_List() {
		delete [] buck;
	}
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

_ulong sieve_base;
_ulong sieve_limit;
_ulong sieve_span;
_ulong sieve[_sieve_word_];
void sieve_all(_ulong base, _ulong limit) {

}
_uint aux_bound;
_ulong *aux_sieve;
_uint aux_sieve_words;

inline _uint find_next_offset(_ulong this_sieve_base, _uint prime) {
	_uint o;
	_ulong t = static_cast<_ulong> (prime) * static_cast<_ulong> (prime);
	if (t > sieve_base)
		o = static_cast<_uint> (t - sieve_base) >> 1u;       //offset of t
	else
	{
		o = prime - static_cast<_uint>(sieve_base % (static_cast<_ulong> (prime)) );   //offset of prime first multiple
		if ((o & 1u) == 0u)
			o = (o >> 1u) + (prime >> 1u); // arithmetic overflow not possible
		else
			o = (o >> 1u);
	}
	return o;
}

void bucketGenerator() {
	_uint k;
	Bucket_List *b = availible_buck;
	if(b != nullptr)
		for (; b->next != nullptr; b = b->next);
	init_pattern();
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
	while (next_prime < aux_bound) {
		li = next_prime / 2u;
		if (test_2(aux_sieve, li)) {
			if (b == nullptr || b->size == ((1u << 9u) - 1u)) {
				b = create_Bucket();
			}
			_uint o = find_next_offset(sieve_base, next_prime);
			b->addBucket(next_prime, o);			
			for (_uint i = (next_prime >> 1u);i < (aux_bound >> 1u);i += next_prime)
				mark_2(aux_sieve, i);
		}
		next_prime += 2;
	}
}

void init_sieve(_ulong this_sieve_base) {
	_uint offset;
	_uint k;
	offset = this_sieve_base % (3u * 5u * 7u * 11u * 13u);
	offset = (offset + ((offset * 105u) & 127u) * 3u * 5u * 7u * 11u * 13u) >> 7u;
	if (sieve_span > _sieve_word_) {
		for (int i = 0u;i < _sieve_word_;i += k){
			int j = _sieve_word_ - i;              // remaining sieve words
			k = 3u * 5u * 7u * 11u * 13u - offset; // remaining pattern words
			if (j < k)
				k = j;
			for (j = 0u;j < k;j++)
				sieve[i + j] = pattern[offset + j];
			offset = 0u;
		}
	}
	else {
		for (int i = 0u;i < sieve_span; i += k) {
			int j = sieve_span - i;
			k = 3u * 5u * 7u * 11u * 13u - offset;
			if (j < k)
				k = j;
			for (j = 0u;j < k;j++)
				sieve[i + j] = pattern[offset + j];
			offset = 0u;
		}
	}
}

void start_sieve(_ulong this_sieve_base) {
	init_sieve(this_sieve_base);
	Bucket_List *doIt = availible_buck;
	_uint prime_index = 0;
	if (doIt == nullptr) {
		cout << "Bucket fail!";
		return;
	}
	while ((doIt->buck[prime_index].prime * doIt->buck[prime_index].prime) < (this_sieve_base + _sieve_word_ * 128u)) {
		_uint o = doIt->buck[prime_index].offset;
		_uint p = doIt->buck[prime_index].prime;
		for (; o < _sieve_word_ * 64; o += p)
			mark_2(sieve, o);
		doIt->buck[prime_index].offset = o - _sieve_word_ * 64;
		if (prime_index == doIt->size - 1u) {
			if (doIt->next == nullptr)
				break;
			doIt = doIt->next;
			prime_index = 0;
		}
		else prime_index++;
		
	}
}

int main() {
	sieve_base = 100000000000u;
	sieve_limit = 200000000000u;
	sieve_span = (sieve_limit - sieve_base) / 128u + 1u;
	aux_bound = sqrt(sieve_limit);
	aux_sieve = new _ulong[aux_bound / 128u + 1u];
	aux_sieve_words = aux_bound / 128u + 1u;
	_ulong this_sieve_base = sieve_base;
	bucketGenerator();
	for (_uint i = 0; i < ((sieve_limit - sieve_base) / 128u + 1u) / _sieve_word_ + 1u; i++) {
		start_sieve(this_sieve_base);
		this_sieve_base += _sieve_word_ * 128;
		sieve_span = (sieve_limit - this_sieve_base) / 128u + 1u;
	}
	return 0;
}



