
# Check and install PlatformIO if not present
setup: setup-clang-format
	@echo "[SETUP] Checking for PlatformIO..."
	@if ! command -v pio &> /dev/null; then \
		echo "[SETUP] PlatformIO not found. Installing..." && \
		pip install -U platformio && \
		echo "[SETUP] PlatformIO installed successfully!"; \
	else \
		echo "[SETUP] PlatformIO is already installed."; \
	fi

# Install clang-format if not present
setup-clang-format:
	@echo "[SETUP] Checking for clang-format..."
	@if ! command -v clang-format &> /dev/null; then \
		echo "[SETUP] clang-format not found. Installing..." && \
		sudo apt-get update && sudo apt-get install -y clang-format && \
		echo "[SETUP] clang-format installed successfully!"; \
	else \
		echo "[SETUP] clang-format is already installed."; \
	fi

# Find all C/C++ source files
FORMAT_FILES := $(shell find . -type f \( -name '*.c' -o -name '*.cc' -o -name '*.cpp' -o -name '*.cxx' -o -name '*.h' -o -name '*.hpp' -o -name '*.hxx' \) \
    -not -path "*/\.*" \
    -not -path "*/build/*" \
    -not -path "*/\.pio/build/*" \
    -not -path "*/\.pio/libdeps/*" \
    -not -path "*/lib/ArduinoFake/*" \
    -not -path "*/lib/Unity/*")

# Check code formatting
format-check: setup-clang-format
	@echo "[FORMAT] Checking code formatting..."
	@clang-format --version
	@for file in $(FORMAT_FILES); do \
		echo "Checking $$file"; \
		clang-format --dry-run --Werror -style=file "$$file" || exit 1; \
	done
	@echo "[FORMAT] Code formatting check passed!"

# Format all source files
format: setup-clang-format
	@echo "[FORMAT] Formatting source files..."
	@clang-format --version
	@for file in $(FORMAT_FILES); do \
		echo "Formatting $$file"; \
		clang-format -i -style=file "$$file"; \
	done
	@echo "[FORMAT] All files formatted successfully!"

# CI format check (faster, exits on first error)
format-ci: setup-clang-format
	@echo "[FORMAT] Running CI format check..."
	@clang-format --version
	@for file in $(FORMAT_FILES); do \
		echo "Checking $$file"; \
		clang-format --dry-run --Werror -style=file "$$file" || (echo "[ERROR] Formatting check failed. Run 'make format' to fix." && exit 1); \
	done
	@echo "[FORMAT] All files are properly formatted!"

# Docker-based coverage
docker-coverage:
	@echo "[DOCKER] Building Docker image..."
	docker build -t y-series-coverage -f .docker/Dockerfile .
	@echo "[DOCKER] Running coverage in container..."
	docker run --rm -v $(PWD):/workspace y-series-coverage coverage

# Clean build and coverage artifacts
clean: coverage-clean
	@echo "[CLEAN] Removing PlatformIO build artifacts..."
	pio run -t clean
	@echo "[CLEAN] Done."

coverage:
	@echo "[COVERAGE] Cleaning previous coverage data..."
	pio run -t clean -e coverage
	find .pio/build/coverage -name '*.gcda' -delete || true
	@echo "[COVERAGE] Running tests with GCC and coverage..."
	PLATFORMIO_BUILD_FLAGS="--coverage -O0 -g" pio test -e coverage
	@echo "[COVERAGE] Capturing coverage info..."
	lcov --capture --directory .pio/build/coverage/ --output-file coverage.info
	lcov --remove coverage.info '/usr/*' --output-file coverage.info
	@echo "[COVERAGE] Filtering to only include lib/ directory..."
	lcov --extract coverage.info "*/lib/*" --output-file coverage.info
	@echo "[COVERAGE] Generating HTML report in .coverage/ ..."
	genhtml coverage.info --output-directory .coverage
	@echo "[COVERAGE] Done. Open .coverage/index.html to view the report."

coverage-clean:
	rm -f coverage.info
	rm -rf .coverage
	find .pio/build/coverage -name '*.gcda' -delete || true
	@echo "[COVERAGE] Coverage files cleaned."

