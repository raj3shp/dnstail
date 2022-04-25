CC=gcc
objects = src/dns_protocol.o src/server.o src/dnstail.o

dnstail: src/dns_protocol.o src/server.o src/dnstail.o
	$(CC) -o dnstail src/dns_protocol.o src/server.o src/dnstail.o
clean:
	rm -f $(objects) dnstail

