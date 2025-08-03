all: test2a test3a test9 testrcu testrcu testrw

test2a: test2a.cpp
	c++ -g -I. -o test2a test2a.cpp -lurcu-signal

test3a: test3a.cpp
	c++ -g -I. -o test3a test3a.cpp -lurcu-signal

test9: test9.cpp
	c++ -g -I. -o test9 test9.cpp -lurcu-signal

testrcu: testrcu.cpp
	c++ -g -std=c++23 -I. -o testrcu testrcu.cpp -lurcu-signal

testrw: testrw.cpp
	c++ -g -I. -o testrw testrw.cpp -lurcu-signal
