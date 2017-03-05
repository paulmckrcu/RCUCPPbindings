# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, you can access it online at
# http://www.gnu.org/licenses/gpl-2.0.html.
#
# Copyright (c) 2016 Paul E. McKenney, IBM Corporation.

PROGS = test1 test4 test5 test6 test7 test8
#PROGS = test1 test4 test5 test6 test7 test8

CXXFLAGS = -g -std=c++1z
#CXXFLAGS = -g -std=c++11

ifeq ($(shell uname),Darwin)
  # This is where "brew install userspace-rcu" installs the headers and archives on OS X 10.11.
  CXXFLAGS += -I/usr/local/Cellar/userspace-rcu/0.9.1/include -L/usr/local/Cellar/userspace-rcu/0.9.1/lib
  # Clang on OS X doesn't yet support the "thread_local" storage-qualifier.
  CXXFLAGS += -Dthread_local=
endif

all: $(PROGS)

# NOTE:  For decent scalability on update-side tests as of early 2015,
#	 use something like jemalloc() instead of glibc malloc().
#	 If you install jemalloc at /home/paulmck/jemalloc, you will
#	 need the following additional arguments to cc:
#		-I/home/paulmck/jemalloc/include
#		-L/home/paulmck/jemalloc/lib
#		-Wl,--rpath,/home/paulmck/jemalloc/lib
#		-DUSE_JEMALLOC
#	 Verify by using the nm command and searching output for malloc.

test1: dshollman/test1.cpp
	$(CXX) $(CXXFLAGS) -I./domains -o $@ $^ -pthread -lurcu -lurcu-signal

test2: ajodwyer/test2.cpp
	$(CXX) $(CXXFLAGS) -I./domains -o $@ $^ -pthread -lurcu -lurcu-signal

test3: ajodwyer/test3.cpp
	$(CXX) $(CXXFLAGS) -I./domains -o $@ $^ -pthread -lurcu -lurcu-signal

test4: test4.cpp
	$(CXX) $(CXXFLAGS) -I./domains -o $@ $^ -pthread -lurcu -lurcu-signal

test5: domains/test5.cpp
	$(CXX) $(CXXFLAGS) -I./domains -o $@ $^ -pthread -lurcu -lurcu-bp

test6: imuerte/test6.cpp
	$(CXX) $(CXXFLAGS) -I./domains -o $@ $^ -pthread -lurcu -lurcu-signal

test7: intrusive/test7.cpp
	$(CXX) $(CXXFLAGS) -I./domains -o $@ $^ -pthread -lurcu -lurcu-signal

test8: intrusive2/test8.cpp
	$(CXX) $(CXXFLAGS) -I./domains -o $@ $^ -pthread -lurcu -lurcu-signal

clean:
	rm -rf $(PROGS) *.o *.dSYM
