#!/usr/bin/env python3
import sys

def solve_chal1(username):
    ascii_sum = sum(ord(c) for c in username)
    expected = ((ascii_sum*5000) ^ 0x55) * 1337
    serial = f"s3cRet_aGENt-{expected}"

    return serial

if __name__ == "__main__":
    if len(sys.argv) > 1:
        username = sys.argv[1]
    else:
        username = input("Enter username: ")
        
    serial = solve_chal1(username)
    if serial:
        print(f"Flag : Cybersphere{{{username}_{serial}}}")
