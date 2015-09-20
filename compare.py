__author__ = 'rsnair2'
import sys

fpOne = open(sys.argv[1])
fpTwo = open(sys.argv[2])

for lineOne in fpOne:
    lineTwo = fpTwo.readline()
    if lineOne.strip() != lineTwo.strip():
        print(lineOne)
        print(lineTwo)
        sys.exit(1)

print("Done")

