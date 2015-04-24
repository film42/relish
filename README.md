Relish
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
> key name
(null)
> set name relish
(ok!)
> key name
relish
> del name relish
(ok!)
> key name
(null)
>
```

#### License

Free for everyone. You fix it, I won't.