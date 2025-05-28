#!/usr/bin/env python3
# We use vertical sprites which are not the standard.
# If we use someone else's data, we can re-do the file to replace 0x## with corrected data

import argparse
import re

def main():
    parser = argparse.ArgumentParser(
        description = 'Convert bytes in a horizontal sprite file to vertical sprites'
    )
    parser.add_argument('filename', help = 'Path to file to change')
    args = parser.parse_args()

    with open(args.filename, 'r') as f:
        content = f.read()
        
    pattern = re.compile(r'\{\s*((?:0x[0-9A-Fa-f]{2}\s*,\s*){7}0x[0-9A-Fa-f]{2})\s*\}')
    result = pattern.sub(replacement, content)

    with open(args.filename, 'w') as f:
        f.write(result)

def rotate_spr(spr_bytes):
    rotated = []
    for col in range(8):
        val = 0
        for row in range(8):
            bit = (spr_bytes[7 - row] >> col) & 1
            val |= bit << (7 - row)
        rotated.append(val)
    return rotated

def replacement(m):
    bytes_str = m.group(1).split(',')
    bytes_int = [ int(b.strip(), 16) for b in bytes_str ]
    rotated_bytes = rotate_spr(bytes_int)
    return '{ ' + ', '.join(f'0x{b:02X}' for b in rotated_bytes) + ' }'

if __name__ == '__main__':
    main()