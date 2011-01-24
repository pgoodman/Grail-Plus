#
# $Id$
#

ROOT_DIR = ./

DEFAULT_CXX = /Users/petergoodman/Code/llvm/Release-Asserts/bin/clang++
CXX = ${DEFAULT_CXX}
CXX_WARN_FLAGS = -pedantic -pedantic-errors -Wall -Werror -Wextra -fno-exceptions
CXX_WARN_FLAGS += -Wno-unused-function -Wno-long-long #-Winline -finline-functions
CXX_WARN_FLAGS += -fno-rtti -Wcast-qual -Wcast-align -fstrict-aliasing
CXX_FLAGS = -O2 -g -ansi -I${ROOT_DIR}
LD_FLAGS =

# are we compiling with the g++?
ifeq (${CXX}, g++)
	CXX_WARN_FLAGS += -Wshadow -Wpointer-arith \
				      -Wwrite-strings \
				      -Wfloat-equal -Wconversion -Wredundant-decls \
    				  -Wvolatile-register-var -fno-stack-protector \
    				  -Wstack-protector \
    				  -Wstrict-aliasing=2
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

all: bin/main.o

bin/%.o: fltl/%.cpp
	${CXX} ${CXX_FLAGS} -c $< -o $@
	${CXX} ${LD_FLAGS} $@ -o bin/$*

clean:
	-rm -rf bin/*.dSYM
	-rm -rf bin/*.o
