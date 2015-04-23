RELISH
======

Relish is a crappy attempt at making a cheap redis clone.

#### Usage

Build relish by doing the following:

```
$ mkdir build
$ cd build && cmake ..
$ make
$ ./relishd
```

Then use the CLI to send commands:

```
$ ./bin/relish-cli
> get name
(null)
> set name relish
(ok!)
> get name
relish
> del name relish
(ok!)
> get name relish
(null)
>
```

#### License

Free for everyone. You fix it, I won't.