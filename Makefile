OBJS = passwords.o project01.o sha256.o
PROG = project01

%.o: %.c
	gcc -c -g -o $@ $< 

$(PROG): $(OBJS)
	gcc -o $@ $^

clean:
	rm -f $(PROG) $(OBJS)
