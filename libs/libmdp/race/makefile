G++	=	g++
CCFLAGS	=	-Wall -O3 -ffloat-store -ffast-math -msse -I../engine -DNDEBUG
OBJS	=	main.o parsing.o
TARGET	=	race

$(TARGET):	$(OBJS)
		$(G++) $(CCFLAGS) -o $(TARGET) $(OBJS)

clean:
		rm -f $(OBJS) $(TARGET) *~ core

.cc.o:
		$(G++) $(CCFLAGS) -c $<

main.o:		main.cc parsing.h race.h ../evaluation.h
main.o:		../engine/algorithm.h ../engine/dispatcher.h ../engine/hash.h
main.o:		../engine/heuristic.h ../engine/parameters.h ../engine/problem.h
main.o:		../engine/policy.h ../engine/random.h ../engine/utils.h
main.o:		../engine/rollout.h ../engine/uct.h ../engine/aot.h
main.o:		../engine/bdd_priority_queue.h
parsing.o:	parsing.h

