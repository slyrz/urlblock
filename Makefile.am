pkginclude_HEADERS = \
	src/urlblock.h

lib_LTLIBRARIES = \
	liburlblock.la

liburlblock_la_SOURCES = \
	data/hosts.c \
	data/paths.c \
	data/tlds.c \
	src/bitset.c \
	src/buzhash.c \
	src/file.c \
	src/hash.c \
	src/stringset.c \
	src/urlblock.c \
	src/util.c

noinst_PROGRAMS = \
	generate

generate_SOURCES = \
	src/tools/generate.c \
	src/bitset.c \
	src/buzhash.c \
	src/file.c \
	src/stringset.c \
	src/util.c \
	src/hash.c

generate_LDADD =

generate_CFLAGS = \
	$(AM_CFLAGS)

# Generated source files during build.
CLEANFILES = \
	data/hosts.c \
	data/paths.c \
	data/tlds.c

data/hosts.c: data/hosts.txt generate
	./generate -t hosts -b 0.10 -h 2.0 $< > $@

data/paths.c: data/paths.txt generate
	./generate -t paths -b 0.05 -h 2.0 $< > $@

data/tlds.c: data/tlds.txt generate
	./generate -t tlds -h 2.0 $< > $@

LDADD = liburlblock.la

check_PROGRAMS = \
	tests/benchmark \
	tests/fuzz \
	tests/hash \
	tests/url_block \
	tests/url_parse \
	tests/url_public_suffix

tests_url_block_SOURCES = \
	tests/data/hosts.c \
	tests/data/paths.c \
	tests/url_block.c

tests/data/hosts.c: tests/data/hosts.txt generate
	./generate -t hosts -n test_hosts -b 0.10 -h 2.0 $< > $@

tests/data/paths.c: tests/data/paths.txt generate
	./generate -t paths -n test_paths -b 0.05 -h 2.0 $< > $@

TESTS = $(check_PROGRAMS)
