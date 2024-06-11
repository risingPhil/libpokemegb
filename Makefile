# # Compiler flags
CXXFLAGS := -std=c++11 -fno-rtti -fno-exceptions -fno-unwind-tables -Wall -Wextra -I $(CURDIR) -I $(CURDIR)/include -g -Os

# Source files directory
SRC_DIR := src
# Build directory
BUILD_DIR := build

# Source files (add more as needed)
SRCS := $(shell find $(SRC_DIR) -type f -name '*.cpp')
# Object files
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

PNG_SUPPORT ?= 1

# Add PNG support flag if PNG_SUPPORT is set to 1
ifeq ($(PNG_SUPPORT),1)
    CXXFLAGS += -DPNG_SUPPORT
	MAKEOVERRIDES := PNG_SUPPORT=1
endif

# Ensure necessary directories exist
# This function ensures the directory for the target exists
define make_directory
	@mkdir -p $(dir $@)
endef

# Target executable
TARGET := libpokemegb.a

# Phony targets
.PHONY: all clean

# Default target
all: $(TARGET) examples

examples: $(TARGET)
	$(MAKE) -C examples $(MAKECMDGOALS) $(MAKEOVERRIDES)
	
# Rule to build the target executable
$(TARGET): $(OBJS)
	@echo "Creating static library $(TARGET)"
	ar rcs $@ $(OBJS)
	ranlib $(TARGET)

# Rule to compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR) $(C_FILE)
	$(make_directory)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create the build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Clean rule
clean:
	make -C examples clean $(MAKEOVERRIDES)
	rm -rf $(BUILD_DIR) $(TARGET)
