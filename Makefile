BUILDS = compile build configure rebuild test run clean distclean

BUILD_DIR ?= build
CMAKE ?= cmake
CTEST ?= ctest
CONFIG ?= Release
JOBS ?= $(shell sysctl -n hw.ncpu 2>/dev/null || nproc 2>/dev/null || echo 4)

.PHONY: $(BUILDS)
.DEFAULT_GOAL := compile

compile : build
	@echo "Compiling Project"
	$(CMAKE) --build $(BUILD_DIR) -j $(JOBS)
	@echo "Done Compiling Project"

build :
	@echo "Building CMake Project"
	$(CMAKE) -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=$(CONFIG)
	@echo "Done Building CMake Project"

configure : build

rebuild : distclean compile

test : compile
	$(CTEST) --test-dir $(BUILD_DIR) --output-on-failure

run : compile
	@if [ -x "$(BUILD_DIR)/benchmark_v1" ]; then \
		"$(BUILD_DIR)/benchmark_v1"; \
	else \
		echo "benchmark_v1 not found. Build with BUILD_BENCHMARKS=ON and check target names."; \
		exit 1; \
	fi

clean :
	@echo "Cleaning CMake Project"
	@if [ -d "$(BUILD_DIR)" ]; then \
		$(CMAKE) --build $(BUILD_DIR) --target clean; \
	fi
	@echo "Done Cleaning CMake Project"

distclean :
	@echo "Removing Build Directory"
	rm -rf $(BUILD_DIR)
	@echo "Done Removing Build Directory"
