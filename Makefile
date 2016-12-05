build-all: hide unhide

hide: hide.c -lxcb
	cc $(CFLAGS) $(CPPFLAGS) $^ $(LDFLAGS) $(LDLIBS) -Wall -o $@

unhide: unhide.c -lxcb
	cc $(CFLAGS) $(CPPFLAGS) $^ $(LDFLAGS) $(LDLIBS) -o $@

.PHONY: clean
clean:
	rm -f hide
	rm -f unhide
