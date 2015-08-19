#!/usr/bin/env python
import sys
import os
import re


class Rules:
    def __init__(self, path):
        self.path = path
        self.values = []
        if os.path.exists(path):
            with open(path, 'r') as stream:
                self.values = list(map(str.strip, stream))

    def append(self, value):
        for pattern in self.Pattern:
            match = re.match(pattern, value)
            if match:
                break
        else:
            return
        value = self.clean(match.group(1))
        if value:
            self.values.append(value)

    def compress(self):
        values = dict.fromkeys(self.values)
        for value in list(values.keys()):
            if any(supervalue in values
                   for supervalue in self.superset(value)):
                del values[value]
        self.values = list(values.keys())

    def save(self):
        self.compress()
        with open(self.path, 'w') as stream:
            for value in sorted(self.values):
                stream.write(value + '\n')

    def __iter__(self):
        return iter(self.values)


class Hosts(Rules):

    Pattern = [
        r'^\|\|(.*)\^(\$third-party)?$',
        r'^(?:127\.0\.0\.1|0\.0\.0\.0|::1)\s+(.*?)(\s+.*)?$',
    ]

    def clean(self, value):
        if '/' in value or '*' in value or '.' not in value:
            return
        if value.startswith('www.'):
            return value[4:]
        return value

    def superset(self, value):
        parts = value.split('.')
        for i in range(1, len(parts)):
            yield '.'.join(parts[i:])


class Paths(Rules):

    Pattern = [r'^([\-\_\&\/\.][^\$\*]*.?)$']

    def clean(self, value):
        if re.search(r'\d+([x\-\_]|h|by)+\d+', value):
            return
        if re.match(r'^\.\w+/', value):
            return
        value = value.rstrip('0123456789$*?_-/^.|')
        if len(value) <= 3:
            return
        return value

    def superset(self, value):
        for i in range(1, len(value) - 1):
            yield value[:i]


if __name__ == '__main__':
    hosts = Hosts('data/hosts.txt')
    paths = Paths('data/paths.txt')

    for arg in sys.argv[1:]:
        with open(arg, 'r') as stream:
            for line in map(str.strip, stream):
                paths.append(line)
                hosts.append(line)

    hosts.save()
    paths.save()
