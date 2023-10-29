CXXFLAGS    = -O2 -std=c++17
INCS        = -I../include
OBJSNOPATH  = DwmMcTallyUtils.o
OBJS        = $(OBJSNOPATH:%.o=../obj/%.o)

../lib/libDwmMcTally.a: ${OBJS}
	ar rv $@ $^
	-ranlib $@

../obj/%.o: %.cc
	${CXX} ${CXXFLAGS} ${INCS} -o $@ -c $<
