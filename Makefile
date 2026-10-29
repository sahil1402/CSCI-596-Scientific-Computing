CXX ?= c++
BUILD_DIR := build
BIN_DIR := bin
TARGET := $(BIN_DIR)/harbortune
TEST_TARGET := $(BIN_DIR)/harbortune_tests

CPPFLAGS := -Iinclude
CXXFLAGS := -std=c++17 -O3 -Wall -Wextra -Wpedantic -MMD -MP
LDFLAGS :=
LDLIBS :=

UNAME_S := $(shell uname -s)
ifeq ($(NO_OPENMP),1)
  CPPFLAGS += -DHARBORTUNE_NO_OPENMP
else ifeq ($(UNAME_S),Darwin)
  LIBOMP_PREFIX ?= $(shell brew --prefix libomp 2>/dev/null)
  ifneq ($(LIBOMP_PREFIX),)
    CPPFLAGS += -I$(LIBOMP_PREFIX)/include
    CXXFLAGS += -Xpreprocessor -fopenmp
    LDFLAGS += -L$(LIBOMP_PREFIX)/lib -Wl,-rpath,$(LIBOMP_PREFIX)/lib
    LDLIBS += -lomp
  else
    $(warning libomp was not found; building without OpenMP support)
    CPPFLAGS += -DHARBORTUNE_NO_OPENMP
  endif
else
  CXXFLAGS += -fopenmp
  LDFLAGS += -fopenmp
endif

SOURCES := $(wildcard src/*.cpp)
OBJECTS := $(patsubst src/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES))
DEPS := $(OBJECTS:.o=.d)

.PHONY: all clean test debug help

all: $(TARGET)

$(TARGET): $(OBJECTS) | $(BIN_DIR)
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(BUILD_DIR)/%.o: src/%.cpp | $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR) $(BIN_DIR):
	mkdir -p $@

debug: CXXFLAGS := -std=c++17 -O0 -g -Wall -Wextra -Wpedantic -MMD -MP
debug: clean all

test: all
	$(TARGET) --version
	$(TARGET) help

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

help:
	@echo "Targets: all, test, debug, clean"
	@echo "Set NO_OPENMP=1 to build the sequential fallback."

-include $(DEPS)

