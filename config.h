#ifndef _CONFIG_HH_
#define _CONFIG_HH_

#ifndef _pointer_size_
#define _pointer_size_ 8u
#endif

#ifndef _sieve_word_            //_sieve_word_ is a parameter to manage how many long long type element in one segment
#define _sieve_word_ (1u << 12u)
#endif

typedef char			   _u8;
typedef unsigned long long _uint;
typedef unsigned long long _ulong;

#endif
