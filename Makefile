.PHONY: build configure run clean

TARGET = JsonReader
BUILD_DIR = build

CMAKE_FLAGS = -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

build: configure
	cd $(BUILD_DIR) && cmake --build .

configure:
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake $(CMAKE_FLAGS) ..

run: build
	./$(BUILD_DIR)/$(TARGET)

clean:
	rm -rf $(BUILD_DIR)

