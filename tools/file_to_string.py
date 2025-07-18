#!/usr/bin/env python3
import argparse


def c_escape_byte(b):
    if b == 0x09:
        return r'\t'
    elif b == 0x0A:
        return r'\n'
    elif b == 0x0D:
        return r'\r'
    elif b == 0x5C:  # \
        return r'\\'
    elif b == 0x22:  # "
        return r'\"'
    elif 0x20 <= b <= 0x7E:
        return chr(b)
    elif b == 0x25:  # %
        return '%%'
    else:
        return f'\\x{b:02x}'


def file_to_c_string(filepath):
    with open(filepath, 'rb') as f:
        content = f.read()
    escaped = ''.join(c_escape_byte(b) for b in content)
    return f'"{escaped}"'


def main():
    parser = argparse.ArgumentParser(
        description='Convert file contents to a valid C string literal.')
    parser.add_argument('input', help='Path to input file')
    args = parser.parse_args()

    c_string = file_to_c_string(args.input)
    print(c_string)


if __name__ == '__main__':
    main()
