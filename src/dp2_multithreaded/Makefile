CXX=g++

LTO_FLAGS=-flto -fuse-linker-plugin
CXXFLAGS=-c -Ofast -march=native ${LTO_FLAGS} -MMD -MP
LDFLAGS=-pthread ${LTO_FLAGS}

BUILD_DIR=bin
TARGET=${BUILD_DIR}/board

SRC_FILES=$(shell find -name \*.cpp)
OBJ_FILES=$(SRC_FILES:%=${BUILD_DIR}/%.o)
DEP_FILES=$(OBJ_FILES:%.o=%.d)

all: ${TARGET}
run: ${TARGET}
	${TARGET}
clean:
	rm -rf ${BUILD_DIR}

${BUILD_DIR}/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	${CXX} ${CXXFLAGS} $< -o $@

${TARGET}: ${OBJ_FILES}
	${CXX} ${LDFLAGS} $^ -o $@

-include ${DEP_FILES}

.PHONY: all, run, clean
