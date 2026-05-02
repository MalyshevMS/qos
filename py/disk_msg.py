#!/usr/bin/env python3
import argparse
import os
import sys

def main():
    parser = argparse.ArgumentParser(description="Script to prepare a disk.")

    parser.add_argument("file_path", help="Disk path")
    parser.add_argument("message", help="Message to write at disk's top")
    parser.add_argument("target_size", type=int, help="Disk size")
    padding = b""

    args = parser.parse_args()

    if args.target_size < 0:
        parser.error("Size must be greater than zero.")

    msg_bytes = args.message.encode('utf-8')

    if len(msg_bytes) >= args.target_size:
        msg_bytes = msg_bytes[:args.target_size]
        padding = b""
    else:
        for _ in range(args.target_size - len(msg_bytes)):
            padding += b"0"

    try:
        with open(args.file_path, 'wb') as f:
            f.write(msg_bytes)
            f.write(padding)
        print(f"Success: disk '{args.file_path}' has written, size = {args.target_size} bytes.")
    except PermissionError:
        print(f"Error: Insufficient permisions to write '{args.file_path}'", file=sys.stderr)
        sys.exit(1)
    except OSError as e:
        print(f"Filesystem error: {e}", file=sys.stderr)
        sys.exit(1)

if __name__ == "__main__":
    main()