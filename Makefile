#
# $Id$
#

ROOT_DIR = ./

DEFAULT_CXX = /Users/petergoodman/Code/llvm/Release-Asserts/bin/clang++
CXX = ${DEFAULT_CXX}
CXX_WARN_FLAGS = -pedantic -pedantic-errors 
CXX_WARN_FLAGS += -Wall -Werror -Wextra -Wno-unused-function -Wno-long-long 
CXX_WARN_FLAGS += -Wcast-qual -Wcast-align #-Winline -finline-functions
CXX_WARN_FLAGS += -fno-rtti -fstrict-aliasing -fno-exceptions
CXX_FLAGS = -O2 -g -ansi -I${ROOT_DIR}
LD_FLAGS =

# are we compiling with the g++?
ifeq (${CXX}, g++)
	CXX_WARN_FLAGS += -Wshadow -Wpointer-arith \
				      -Wwrite-strings \
				      -Wfloat-equal -Wconversion -Wredundant-decls \
    				  -Wvolatile-register-var -fno-stack-protector \
    				  -Wstack-protector \
    				  -Wstrict-aliasing=2 \
    				  -std=gnu++98 \
    				  -Wold-style-cast # -Weffc++
endif

# are we compiling with icc?
ifeq (${CXX}, icc)
	CXX_WARN_FLAGS =
	CXX_FLAGS += -Kc++ -Wall -Werror -wd981
	LD_FLAGS += -lstdc++
endif

# are we compiling with clang++?
ifeq (${CXX}, ${DEFAULT_CXX})
	CXX_FLAGS += -fcatch-undefined-behavior
endif

CXX_FLAGS += ${CXX_WARN_FLAGS}
OBJS = bin/main.o bin/test/Test.o bin/test/cfg/CFG.o
OUT = bin/main

all: ${OBJS}
	${CXX} ${LD_FLAGS} ${OBJS} -o ${OUT}

bin/%.o: fltl/%.cpp
	${CXX} ${CXX_FLAGS} -c $< -o $@
	
bin/test/%.o: fltl/test/%.cpp
	${CXX} ${CXX_FLAGS} -c $< -o $@

clean:
	-rm ${OUT}
	-rm -rf bin/*.dSYM
	-rm -rf bin/*.o
	-rm -rf bin/test/*.o
	-rm -rf bin/test/cfg/*.o
