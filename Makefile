CXXOBJ = Source.o next_7_finder.o mask_16.o

testcomplex.exe:$(CXXOBJ)
	g++ -std=c++11 $(CXXOBJ) -o fast_sieve.exe

Source.o:Source.cpp
	g++ -std=c++11 Source.cpp -c

next_7_finder.o:next_7_finder.cpp
	g++ -std=c++11 next_7_finder.cpp -c

mask_16.o:mask_16.cpp
	g++ -std=c++11 mask_16.cpp -c

Source.cpp:next_7_finder.h
	touch next_7_finder.h

next_7_finder.cpp:next_7_finder.h
	touch next_7_finder.cpp

next_7_finder.h:config.h
	touch next_7_finder.h