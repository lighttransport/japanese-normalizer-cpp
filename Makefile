all:
	clang++ -o test_jpnormalizer -Weverything -Wall -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-padded -fsanitize=address -g -O1 test_jpnormalizer.cc
