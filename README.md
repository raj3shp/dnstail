## dnstail

A simple DNS proxy in C

Usage:

```
$ cd dnstail
$ make

$ ./dnstail 127.0.0.1 8053
```

Testing:

```
dig @127.0.0.1 -p 8053 example.com
dig @127.0.0.1 -p 8053 -t CNAME example.com
```

