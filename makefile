# Copyright (c) 2013, Robert Rueger <rueger@itp.uni-frankfurt.de>
#
# This file is part of YASS.
#
# YASS is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# YASS is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with YASS.  If not, see <http://www.gnu.org/licenses/>.


# ----------- BUILD SETTINGS -----------

# disable default implicit rules
MAKEFLAGS += --no-builtin-rules
.SUFFIXES:

# find the hash of the git commit we are building from
GIT_HASH = $(shell git rev-parse --short HEAD 2> /dev/null || echo *unknown*)

# compiler options
CXX      = g++
CXXFLAGS = -std=c++11 -Wall -Wextra
LDFLAGS  = -lboost_program_options -lboost_filesystem -lboost_system
DEFINES  = -DGIT_HASH=\"$(GIT_HASH)\"
ifeq ($(BUILD), RELEASE)
  CXXFLAGS += -march=native -O2 -flto
  LDFLAGS  += -fuse-linker-plugin -fwhole-program -s
  DEFINES  += -DNDEBUG
else ifeq ($(BUILD), PROFILE)
  CXXFLAGS += -march=native -O2 -g
  DEFINES  += -DNDEBUG
else
  CXXFLAGS += -g
endif


# -------------- TARGETS ---------------

# generate lists of object files (all and existing only)
OBJECTS_ALL = $(patsubst %.cpp,%.o,$(wildcard *.cpp))
OBJECTS_EXT = $(wildcard *.o)

# define implicit rule to compile a cpp file
%.o : %.cpp
	$(CXX) $(CXXFLAGS) $(DEFINES) -c $< -o $@
	$(CXX) -MM $(CXXFLAGS) $(DEFINES) $< > $*.d

# YASS executable
yass : $(OBJECTS_ALL)
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@

# pull in dependency info for existing object files
-include $(OBJECTS_EXT:.o=.d)

clean :
	rm -f yass *.o *.d
