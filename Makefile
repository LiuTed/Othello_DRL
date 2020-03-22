CC := gcc
CXX := g++
INCLUDE_PATH := include
OBJ_PATH := obj

INCLUDE_PARAM := $(addprefix -I, $(INCLUDE_PATH))
COMMON_FLAGS := -O2
CFLAGS := $(COMMON_FLAGS)
CXXFLAGS := $(COMMON_FLAGS) -std=c++11

LIBS := 

HDRS := Othello.h MCTS.h
SRC := MCTS.cpp Othello.cpp main.cpp
OBJS := $(addsuffix .o,$(basename $(SRC)))

vpath %.o $(OBJ_PATH)
vpath %.h $(INCLUDE_PATH)

%.o: %.c
	$(CC) $< -c -o $(OBJ_PATH)/$@ $(CFLAGS) $(INCLUDE_PARAM)

%.o: %.cc
	$(CXX) $< -c -o $(OBJ_PATH)/$@ $(CXXFLAGS) $(INCLUDE_PARAM)
	
%.o: %.cpp
	$(CXX) $< -c -o $(OBJ_PATH)/$@ $(CXXFLAGS) $(INCLUDE_PARAM)

.PHONY: init
.NOTPARALLEL .PHONY: all

all: init othello

init:
	@if [ ! -d $(OBJ_PATH) ]; then mkdir -p $(OBJ_PATH) || exit 1; fi

othello: $(OBJS)
	$(CXX) $(addprefix $(OBJ_PATH)/, $(OBJS)) -o $@ $(CXXFLAGS) $(INCLUDE_PARAM) $(LIBS)

bot.cpp: $(HDRS) $(SRC)
	cat $^ > bot.cpp
	for i in $(HDRS); do sed "s/#include \"$$i\"//g" -i bot.cpp; done

bot: bot.cpp
	$(CXX) $< -c -o $@ $(CXXFLAGS) $(INCLUDE_PARAM) -D_BOTZONE_ONLINE

.PHONY: clean
clean:
	-rm -rf $(OBJ_PATH) othello bot bot.cpp
