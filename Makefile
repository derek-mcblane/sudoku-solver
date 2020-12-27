TARGET_EXEC ?= solver

BUILD_DIR ?= ./build
SRC_DIRS ?= ./source

SRCS := source/*.cpp
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

CXXFLAGS += -MMD -MP
INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

all: $(BUILD_DIR)/$(TARGET_EXEC)

debug: CXXFLAGS += -g -DDEBUG
debug: LDFLAGS += -lpthread
debug: all

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(INC_FLAGS) $(CXXFLAGS) -c $< -o $@

.PHONY: clean

clean:
	rm -r $(BUILD_DIR)

-include $(DEPS)

