G++	=	g++
CCFLAGS	=	-Wall -g -ffloat-store -ffast-math -msse -I../engine -DNDEBUG
OBJS	=	main.o
TARGET	=	puzzle

$(TARGET):	$(OBJS)
		$(G++) $(CCFLAGS) -o $(TARGET) $(OBJS)

clean:
		rm -f $(OBJS) $(TARGET) *~ core

.cc.o:
		$(G++) $(CCFLAGS) -c $<

main.o:		puzzle.h ../evaluation.h
main.o:		../engine/algorithm.h ../engine/dispatcher.h ../engine/hash.h ../engine/heuristic.h
main.o:		../engine/mcts.h ../engine/parameters.h ../engine/problem.h ../engine/policy.h
main.o:		../engine/random.h ../engine/rollout.h ../engine/mcts.h ../engine/utils.h
main.o:		../engine/aot.h

