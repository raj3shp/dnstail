## dnstail

A simple DNS proxy in C

**Usage**:

```
$ cd dnstail
$ make

$ ./dnstail 127.0.0.1 8053

dnstail: listening on udp 127.0.0.1:8053
dnstail: from:127.0.0.1:59901, query:example.com., type:A
dnstail: from:127.0.0.1:53231, query:example.com., type:CNAME
```

**Testing**:

```
dig @127.0.0.1 -p 8053 example.com
dig @127.0.0.1 -p 8053 -t CNAME example.com
```

