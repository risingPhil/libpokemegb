# # Compiler flags
CXXFLAGS := -std=c++11 -fno-rtti -fno-exceptions -fno-unwind-tables -Wall -Wextra -I $(CURDIR) -I $(CURDIR)/../../include -g -Os

# Source files directory
SRC_DIR := .
# Build directory
BUILD_DIR := build

# Source files (add more as needed)
SRCS := $(shell find $(SRC_DIR) -type f -name '*.cpp')
# Object files
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

LIBS := -lpokemegb
ifeq ($(PNG_SUPPORT),1)
	LIBS += -lpng
endif

# Ensure necessary directories exist
# This function ensures the directory for the target exists
define make_directory
	@mkdir -p $(dir $@)
endef

# Target executable
TARGET := ../../do_stuff_gen1

# Phony targets
.PHONY: all clean

# Default target
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LIBS) -L ../../

# Rule to compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(make_directory)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create the build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Clean rule
clean:
	rm -rf $(BUILD_DIR) $(TARGET)