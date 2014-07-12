PROBLEMS =	ctp ctp2 ctp3 puzzle race rect sailing tree wet

all:
	for p in $(PROBLEMS); do \
	    (cd $$p; make) \
        done

clean:
	for p in $(PROBLEMS); do \
	    (cd $$p; make clean) \
        done
	(cd engine; make)
	rm -f *~ core
