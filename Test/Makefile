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

PROGS = urcu \
	rcu_head_container_of \
	rcu_head_delete \
	rcu_head_delete2 \
	rcu_head_derived \
	rcu_head_derived1 \
	rcu_head_ptr \
	rcu_scoped_reader

GCC_ARGS = -g -std=c++11

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

urcu: urcu.cpp urcu_bp.cpp urcu-mb.cpp urcu-qsbr.cpp urcu-signal.hpp rcu_domain.hpp rcu_domain_derived.hpp
	g++ $(GCC_ARGS) -o urcu urcu.cpp urcu_bp.cpp urcu-mb.cpp urcu-qsbr.cpp -lpthread -lurcu -lurcu-signal -lurcu-bp -lurcu-mb -lurcu-qsbr

rcu_head_container_of: rcu_head_container_of.cpp rcu_head_container_of.hpp
	g++ $(GCC_ARGS) -o rcu_head_container_of rcu_head_container_of.cpp -lpthread -lurcu -lurcu-signal

rcu_head_delete: rcu_head_delete.cpp rcu_head_delete.hpp
	g++ $(GCC_ARGS) -o rcu_head_delete rcu_head_delete.cpp -lpthread -lurcu -lurcu-signal

rcu_head_delete2: rcu_head_delete2.cpp rcu_head_delete2.hpp
	g++ $(GCC_ARGS) -o rcu_head_delete2 rcu_head_delete2.cpp -lpthread -lurcu -lurcu-signal

rcu_head_derived: rcu_head_derived.cpp rcu_head_delete.hpp
	g++ $(GCC_ARGS) -o rcu_head_derived rcu_head_derived.cpp -lpthread -lurcu -lurcu-signal

rcu_head_derived1: rcu_head_derived1.cpp
	g++ $(GCC_ARGS) -o rcu_head_derived1 rcu_head_derived1.cpp -lpthread -lurcu -lurcu-signal

rcu_head_ptr: rcu_head_ptr.cpp rcu_head_ptr.hpp
	g++ $(GCC_ARGS) -o rcu_head_ptr rcu_head_ptr.cpp -lpthread -lurcu -lurcu-signal

rcu_scoped_reader: rcu_scoped_reader.cpp rcu_scoped_reader.hpp
	g++ $(GCC_ARGS) -o rcu_scoped_reader rcu_scoped_reader.cpp -lpthread -lurcu -lurcu-signal

# rcu_head_lambda_capture: rcu_head_lambda_capture.cpp rcu_head_lambda_capture.hpp
#	g++ $(GCC_ARGS) -o rcu_head_lambda_capture rcu_head_lambda_capture.cpp -lpthread -lurcu -lurcu-signal

clean:
	rm -f $(PROGS)
