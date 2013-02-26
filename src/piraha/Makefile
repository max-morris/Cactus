# Build the standalone tool
#
CXX = g++
CXXFLAGS =  -Wall
SRCS = AutoGrammar.cc Bracket.cc Dot.cc End.cc \
	Generic.cc Grammar.cc Group.cc ILiteral.cc \
	Literal.cc Lookup.cc Matcher.cc Multi.cc \
	Or.cc ReParse.cc Seq.cc smart_ptr.cc Start.cc

generic : $(SRCS) Piraha.hpp smart_ptr.hpp
	$(CXX) -I . -g -o generic $(SRCS)

clean :
	rm -f generic
