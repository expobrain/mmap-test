from __future__ import unicode_literals, print_function

import array


def main():
    arr = array.array('H', [i for i in range(16)])
    with open("array.data", "wb") as f:
        for _ in xrange(128 * 1024 * 1024):
            arr.tofile(f)


if __name__ == '__main__':
    main()
