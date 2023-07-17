PCC = g++
PCFLAGS = -std=c++14 -fopenmp


ALL = kxCoreDecomp kCoreDecomp kxLocalSearch incidenthypergraph kxIndexSearch compute

all: $(ALL)
% : %.cpp
	$(PCC) $(PCFLAGS) -o $@.out $<

.PHONY : clean

clean :
	rm -f $(addsuffix .out, $(ALL))

cleansrc :
	rm -f $(addsuffix .out, $(ALL))
	rm $(COMMON)