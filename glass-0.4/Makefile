CXX=g++
CXXFLAGS=-O2 -g
LDFLAGS=

OBJS=builtins.o \
glass.o \
klassi.o \
parseq.o \
variable.o

all: glass

glass: $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(OBJS) -o $@

.PREFIXES: .cc .o

.cc.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f glass $(OBJS)
