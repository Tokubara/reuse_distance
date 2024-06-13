mode := Release
ifeq ($(mode),Debug)
CXXFLAGS = -fsanitize=address -fsanitize=leak -fsanitize=undefined -fno-omit-frame-pointer -O0 -g
else
CXXFLAGS = -O3
endif

analyze: analyze.c scaleTree.c counter.cxx hash.cxx
	g++ $(CXXFLAGS) analyze.c -o analyze
clean:
	rm analyze
test: analyze bats.sh
	bats bats.sh

.PHONY: clean test