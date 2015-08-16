# urlblock

urlblock is a C library for blocking URLs based on simple, predefined
rules. The rules will be compiled into the library, allowing you to fully
embed urlblock's functionality in your programs.

## Getting Started

Before you can build urlblock, you need to create your own set of rules:

```shell
mkdir data
touch data/hosts.txt
touch data/paths.txt
```

These rules will be used to block URLs. The `data/hosts.txt` file should
contain all blacklisted hostnames.
The `data/paths.txt` file should contain blacklisted keywords, which
will be matched anywhere inside the URL's path component.

The `utils` directory contains a Python script, which can be used to
create these rule files based on AdBlock filter lists and UNIX hosts
files.

```shell
python utils/import.py PATH...
```

Once you have defined your own set of rules, urlblock can be build by
running

```shell
make
make test
```

### License

urlblock is released under MIT license.
You can find a copy of the MIT License in the [LICENSE](./LICENSE) file.
