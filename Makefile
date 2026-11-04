CC ?= gcc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -O2
CPPFLAGS ?= -D_POSIX_C_SOURCE=200809L -Iinclude -Isrc

PKG_CONFIG ?= pkg-config
CURL_CFLAGS := $(shell $(PKG_CONFIG) --cflags libcurl 2>/dev/null)
CURL_LIBS := $(shell $(PKG_CONFIG) --libs libcurl 2>/dev/null)
CJSON_CFLAGS := $(shell $(PKG_CONFIG) --cflags libcjson 2>/dev/null)
CJSON_LIBS := $(shell $(PKG_CONFIG) --libs libcjson 2>/dev/null)

ifeq ($(strip $(CURL_LIBS)),)
CURL_CFLAGS :=
CURL_LIBS := -lcurl
endif

ifeq ($(strip $(CJSON_LIBS)),)
CJSON_CFLAGS := -I$(HOME)/.local/usr/include
CJSON_LIBS := -L$(HOME)/.local/usr/lib64 -lcjson
endif

CPPFLAGS += $(CURL_CFLAGS) $(CJSON_CFLAGS)
LDFLAGS ?=
LDLIBS ?= $(CURL_LIBS) $(CJSON_LIBS) -lm

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
TEST_JSON_PARSER_BIN = tests/bin/test_json_parser
TEST_HISTORY_BIN = tests/bin/test_history
TEST_API_CLIENT_BIN = tests/bin/test_api_client
TEST_UI_SCRIPT = tests/test_ui.sh

.PHONY: all clean run debug test test-api test-ui

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) $(LDLIBS) -o $@

build/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

debug: CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -g3 -O0 -DDEBUG
debug: clean $(TARGET)

test: $(TEST_CONVERTER_BIN) $(TEST_VALIDATOR_BIN) $(TEST_JSON_PARSER_BIN) $(TEST_HISTORY_BIN)
	@echo "--- Running Unit Tests ---"
	@./$(TEST_CONVERTER_BIN)
	@./$(TEST_VALIDATOR_BIN)
	@./$(TEST_JSON_PARSER_BIN)
	@./$(TEST_HISTORY_BIN)
	@echo "--- All Tests Passed Successfully ---"

$(TEST_CONVERTER_BIN): tests/test_converter.c src/converter/converter.c
	@mkdir -p tests/bin
	$(CC) $(CFLAGS) $(CPPFLAGS) $^ $(LDFLAGS) -lm -o $@

$(TEST_VALIDATOR_BIN): tests/test_validator.c src/utils/validator.c
	@mkdir -p tests/bin
	$(CC) $(CFLAGS) $(CPPFLAGS) $^ $(LDFLAGS) -o $@

$(TEST_JSON_PARSER_BIN): tests/test_json_parser.c src/utils/json_parser.c
	@mkdir -p tests/bin
	$(CC) $(CFLAGS) $(CPPFLAGS) $^ $(LDFLAGS) $(CJSON_LIBS) -lm -o $@

$(TEST_HISTORY_BIN): tests/test_history.c src/history/history.c
	@mkdir -p tests/bin
	$(CC) $(CFLAGS) $(CPPFLAGS) $^ $(LDFLAGS) -o $@

test-api: $(TEST_API_CLIENT_BIN)
	@echo "--- Running API Integration Test ---"
	@./$(TEST_API_CLIENT_BIN)

test-ui: $(TARGET)
	@echo "--- Running UI Smoke Test ---"
	@sh $(TEST_UI_SCRIPT) $(TARGET)

$(TEST_API_CLIENT_BIN): tests/test_api_client.c src/api/api_client.c src/utils/json_parser.c src/market/market.c src/converter/converter.c
	@mkdir -p tests/bin
	$(CC) $(CFLAGS) $(CPPFLAGS) $^ $(LDFLAGS) $(LDLIBS) -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf build $(TARGET) tests/bin
