#
# $Id$
#

ROOT_DIR = ./

CXX = /Users/petergoodman/Code/llvm/Release-Asserts/bin/clang++
CXX_WARN_FLAGS = -pedantic -pedantic-errors -Wall -Werror -Wextra
CXX_WARN_FLAGS += -Wno-unused-function -Wno-long-long
CXX_FLAGS = -O0 -g -ansi -I${ROOT_DIR}
LD_FLAGS =

# are we compiling with the gcc?
ifeq (${CXX}, g++)
	CXX_WARN_FLAGS += -Wshadow -Wpointer-arith -Wcast-qual \
				      -Wwrite-strings \
				      -Wfloat-equal -Wconversion -Wredundant-decls \
    				  -Wvolatile-register-var -fno-stack-protector \
    				  -Wstack-protector
endif

# are we compiling with icc?
ifeq (${CXX}, icc)
	CXX_WARN_FLAGS =
	CXX_FLAGS += -Kc++ -Wall -Werror -wd981
	LD_FLAGS += -lstdc++
endif

CXX_FLAGS += ${CXX_WARN_FLAGS}

all: bin/main.o

bin/%.o: src/%.cpp
	${CXX} ${CXX_FLAGS} -c $< -o $@
	${CXX} ${LD_FLAGS} $@ -o bin/$*

clean:
	-rm -rf *.o
