CXX = g++
CXXFLAGS = -std=c++17 -Iinclude -O2
LDFLAGS =

SRC_DIR = src
BUILD_DIR = build
BIN = $(BUILD_DIR)/app

# Buscar todos los .cpp dentro de SRC_DIR y subdirectorios
SRCS = $(shell find $(SRC_DIR) -name '*.cpp' ! -name 'escalonar_imagen.cpp')
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

# Permitir pasar argumentos al ejecutar
ARGS ?= image.jpg

all: $(BIN)

# Crear directorio build
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Crear directorios build para cada archivo objeto
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BIN): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

run: all
	./$(BIN) $(ARGS)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all run clean
