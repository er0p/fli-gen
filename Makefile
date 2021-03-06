#comma:=)
comma :=,
GIT_VER         :=$(patsubst refs/tags/%$(comma),%,$(filter refs/tags/%,$(shell git log --all-match --tags="v*.*" --decorate=full --no-walk -n1)))
GIT_HASH	:=$(word 2,$(shell git log --no-walk -n1))
GIT_VER_CFLAGS	:=-DGIT_VER=\"$(GIT_VER)\" -DGIT_HASH=\"$(GIT_HASH)\"

CXXFLAGS := -std=c++14 -ggdb3 $(GIT_VER_CFLAGS)
TARGET := fli-gen
SOURCES := fli-gen.cpp
BOT := fli-gen-bot

all: $(TARGET) $(BOT)

$(TARGET):
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $@
$(BOT):
	go build $@.go

parse_csv:
	./fli-gen.sh ./fli.csv


.PHONY: clean parse_csv
clean:
	rm -f $(TARGET) $(BOT)
	

