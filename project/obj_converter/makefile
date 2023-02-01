APP_NAME := obj_converter
CC := g++
CPPFLAGS := -std=c++20 -Werror -Wall -Wpedantic -Wextra -Wfloat-equal -Wfloat-conversion -Wvla -O3
DEBUGFLAGS := -g3
LINKFLAGS := 
INC_DIRS := inc ../3d_engine/inc
INC_DIRS := $(INC_DIRS:%=-I%)
SRC_DIR := src
OUT_DIR := out
SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(SRCS:$(SRC_DIR)/%.cpp=$(OUT_DIR)/%.o)
GCOV_MODULES := $(OBJS:%.o=%.gcno)
DEPENDS := $(SRCS:$(SRC_DIR)/%.cpp=$(OUT_DIR)/%.d)

_MAKE_OUT_DIR_ := $(shell mkdir -p $(OUT_DIR))

.PHONY: debug release convert memcheck clean
.NOTPARALLEL: debug release

debug: CPPFLAGS += $(DEBUGFLAGS)
debug: LINKFLAGS += $(DEBUGFLAGS)
debug: debug.lastbuildstate $(APP_NAME).exe

release: release.lastbuildstate $(APP_NAME).exe

debug.lastbuildstate:
	@touch debug.lastbuildstate
	@rm -f $(OBJS) $(OUT_DIR)/main.o $(OUT_DIR)/*.g* *.exe
	@rm -f release.lastbuildstate

release.lastbuildstate:
	@touch release.lastbuildstate
	@rm -f $(OBJS) $(OUT_DIR)/main.o $(OUT_DIR)/*.g* *.exe
	@rm -f debug.lastbuildstate

$(APP_NAME).exe: $(OBJS)
	@echo "Link: $(notdir $@)"
	@$(CC) $^ -o $@ $(LINKFLAGS)

convert: release
	@./$(APP_NAME).exe $(file_obj) $(file_mtl) $(file_lgt) $(file_inc) $(file_dst)

memcheck: debug
	valgrind --leak-check=full -s ./$(APP_NAME).exe $(file_obj) $(file_mtl) $(file_lgt) $(file_inc) $(file_dst)

$(OUT_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo "Compile: $(notdir $<)"
	@$(CC) $(CPPFLAGS) $(INC_DIRS) -MMD -o $@ -c $<

-include $(DEPENDS)

clean:
	@rm -rf $(OUT_DIR) *.exe *.lastbuildstate
	