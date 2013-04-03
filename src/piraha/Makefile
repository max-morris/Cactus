# Build the standalone tool
#
BIN ?= .
CXX ?= c++
CXXFLAGS ?= -g -O2
SRCS = AutoGrammar.cc Bracket.cc Dot.cc End.cc \
	Generic.cc Grammar.cc Group.cc ILiteral.cc \
	Literal.cc Lookup.cc Matcher.cc Multi.cc \
	Or.cc ReParse.cc Seq.cc smart_ptr.cc Start.cc \
	Boundary.cc NegLookAhead.cc

$(BIN)/generic : $(SRCS) Piraha.hpp smart_ptr.hpp
	mkdir -p $(BIN)
	$(CXX) $(CXXFLAGS) -I . -o $(BIN)/generic $(SRCS)

clean :
	rm -f $(BIN)/generic
