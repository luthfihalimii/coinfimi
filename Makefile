CC ?= gcc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -O2
CPPFLAGS ?= -D_POSIX_C_SOURCE=200809L -Iinclude -Isrc -I$(HOME)/.local/usr/include
LDFLAGS ?= -L$(HOME)/.local/usr/lib64
LDLIBS ?= -lcurl -lcjson -lm

TARGET = cryptex

SRCS = src/main.c \
       src/api/api_client.c \
       src/converter/converter.c \
       src/market/market.c \
       src/history/history.c \
       src/ui/menu.c \
       src/utils/json_parser.c \
       src/utils/validator.c

OBJS = $(patsubst src/%.c, build/%.o, $(SRCS))

# Unit Test Sources
TEST_CONVERTER_BIN = tests/bin/test_converter
TEST_VALIDATOR_BIN = tests/bin/test_validator

.PHONY: all clean run debug test

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) $(LDLIBS) -o $@

build/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

debug: CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -g3 -O0 -DDEBUG
debug: clean $(TARGET)

test: $(TEST_CONVERTER_BIN) $(TEST_VALIDATOR_BIN)
	@echo "--- Running Unit Tests ---"
	@./$(TEST_CONVERTER_BIN)
	@./$(TEST_VALIDATOR_BIN)
	@echo "--- All Tests Passed Successfully ---"

$(TEST_CONVERTER_BIN): tests/test_converter.c src/converter/converter.c
	@mkdir -p tests/bin
	$(CC) $(CFLAGS) $(CPPFLAGS) $^ $(LDFLAGS) -lm -o $@

$(TEST_VALIDATOR_BIN): tests/test_validator.c src/utils/validator.c
	@mkdir -p tests/bin
	$(CC) $(CFLAGS) $(CPPFLAGS) $^ $(LDFLAGS) -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf build $(TARGET) tests/bin
