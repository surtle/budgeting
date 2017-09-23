HEADERS = Category.h

default: ways 

Budget.o: Budget.c $(HEADERS) 
	gcc -c Budget.c -o Budget.o 

ways: Budget.o 
	gcc Budget.o -o ways 

clean: 
	-rm -f Budget.o
	-rm -f ways 

 
