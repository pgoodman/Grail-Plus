#
# $Id$
#

ROOT_DIR = ./

DEFAULT_CXX = clang++-3.8
CXX = ${DEFAULT_CXX}
CXX_FEATURES = -fno-rtti -fno-exceptions -fstrict-aliasing -fno-stack-protector
CXX_WARN_FLAGS += -Wall -Werror -Wno-unused-function 
CXX_WARN_FLAGS += -Wcast-qual
OPTIMIZATION_LEVEL = -O0
CXX_FLAGS = ${OPTIMIZATION_LEVEL} -g -ansi -I${ROOT_DIR}
LD_FLAGS =
OUT = bin/grail
OUT2 = 
FINALIZE = echo

GNU_COMPATIBLE_FLAGS = -pedantic -pedantic-errors -Wextra -Wcast-align -Wno-long-long 
GNU_COMPATIBLE_FLAGS += -Wno-unused-local-typedefs 

# are we compiling with the g++?
ifneq (,$(findstring g++,${CXX}))
	#CXX_FEATURES += -flto
	CXX_FLAGS += -std=gnu++98
	CXX_WARN_FLAGS += -Wshadow -Wpointer-arith \
				      -Wwrite-strings \
				      -Wfloat-equal -Wconversion -Wredundant-decls \
    				  -Wvolatile-register-var \
    				  -Wstack-protector \
    				  -Wstrict-aliasing=2 \
    				  -Wold-style-cast
	
	ifneq (${OPTIMIZATION_LEVEL}, -O0)
    	CXX_WARN_FLAGS += -Wuninitialized
	endif
endif

# are we compiling with clang++?
ifneq (,$(findstring clang++,${CXX}))
	CXX_WARN_FLAGS += -Winline
endif

CXX_FLAGS += ${CXX_WARN_FLAGS} ${CXX_FEATURES} ${GNU_COMPATIBLE_FLAGS}
OBJS = bin/main.o bin/lib/io/CommandLineOptions.o 
OBJS += bin/lib/helper/CStringMap.o 
OBJS += bin/lib/io/fprint.o bin/lib/io/UTF8FileBuffer.o bin/lib/io/error.o
OBJS += bin/lib/io/fread_cfg.o bin/lib/io/fread_pda.o bin/lib/io/fread_nfa.o 
OBJS += bin/lib/io/verbose.o

all: ${OBJS}
	${CXX} ${LD_FLAGS} ${OBJS} -o ${OUT}
	${FINALIZE}

bin/%.o: grail/%.cpp
	${CXX} ${CXX_FLAGS} -c $< -o $@

bin/lib/%.o: grail/lib/%.cpp
	${CXX} ${CXX_FLAGS} -c $< -o $@

bin/lib/printer/%.o: grail/lib/io/%.cpp
	${CXX} ${CXX_FLAGS} -c $< -o $@
	
bin/test/%.o: fltl/test/%.cpp
	${CXX} ${CXX_FLAGS} -c $< -o $@

install:
	-mkdir bin
	-mkdir bin/test
	-mkdir bin/test/cfg
	-mkdir bin/lib
	-mkdir bin/lib/printer
	-mkdir bin/lib/io
	-mkdir bin/lib/helper

clean:
	-rm ${OUT} ${OUT2}
	-rm -rf bin/*.dSYM
	-rm -rf bin/*.o
	-rm -rf bin/lib/*.o
	-rm -rf bin/lib/printer/*.o
	-rm -rf bin/lib/helper/*.o
	-rm -rf bin/lib/io/*.o
	-rm -rf bin/test/*.o
	-rm -rf bin/test/cfg/*.o
