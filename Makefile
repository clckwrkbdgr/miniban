BIN = miniban
TEST_BIN = $(BIN)_test
LIBS = -lSDL2 -lchthon2

SOURCES = $(wildcard src/*.cpp)
APP_SOURCES = $(wildcard *.cpp)
TEST_SOURCES = $(wildcard test/*.cpp)

OBJ = $(addprefix tmp/,$(SOURCES:.cpp=.o))
APP_OBJ = $(addprefix tmp/,$(APP_SOURCES:.cpp=.o))
TEST_OBJ = $(addprefix tmp/,$(TEST_SOURCES:.cpp=.o))
#WARNINGS = -pedantic -Werror -Wall -Wextra -Wformat=2 -Wmissing-include-dirs -Wswitch-default -Wswitch-enum -Wuninitialized -Wunused -Wfloat-equal -Wundef -Wno-endif-labels -Wshadow -Wcast-qual -Wcast-align -Wconversion -Wsign-conversion -Wlogical-op -Wmissing-declarations -Wno-multichar -Wredundant-decls -Wunreachable-code -Winline -Winvalid-pch -Wvla -Wdouble-promotion -Wzero-as-null-pointer-constant -Wuseless-cast -Wvarargs -Wsuggest-attribute=pure -Wsuggest-attribute=const -Wsuggest-attribute=noreturn -Wsuggest-attribute=format
CXXFLAGS = -MD -MP -std=c++0x $(WARNINGS)

all: $(BIN)

run: $(BIN)
	./$(BIN)

test: $(TEST_BIN)
	./$(TEST_BIN) $(TESTS)

$(BIN): $(OBJ) $(APP_OBJ)
	$(CXX) $(LIBS) -o $@ $^

$(TEST_BIN): $(OBJ) $(TEST_OBJ)
	$(CXX) $(LIBS) -o $@ $^

tmp/%.o: %.cpp
	@echo Compiling $<...
	@$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean Makefile

clean:
	$(RM) -rf tmp/* $(BIN) $(TEST_BIN)

$(shell mkdir -p tmp)
$(shell mkdir -p tmp/src)
$(shell mkdir -p tmp/test)
-include $(OBJ:%.o=%.d)
-include $(APP_OBJ:%.o=%.d)
-include $(TEST_OBJ:%.o=%.d)

