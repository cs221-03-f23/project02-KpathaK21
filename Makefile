OBJS = passwords.o project02.o sha256.o
PROG = project02

%.o: %.c
	gcc -c -g -o $@ $< 

$(PROG): $(OBJS)
	gcc -o $@ $^

clean:
	rm -f $(PROG) $(OBJS)
