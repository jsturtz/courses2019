# RUN WITH PYTHON3 INTERPRETER
# e.g. python3 mypython.py

import random
import string
random.seed()

filenames = ["Rando1.txt", "Rando2.txt", "Rando3.txt"]

# write 10 random chars to files
for name in filenames:
    f = open(name, "w+")
    f.write( ''.join(random.choice(string.ascii_lowercase) for x in range(10)) + '\n' )
    f.close()

# read from files and print without newlines
for name in filenames:
    with open(name) as fp:
        print(fp.readline(), end="");

# generate two random ints
int1 = random.randint(1, 42);
int2 = random.randint(1, 42);

# print last three lines
print(int1)
print(int2)
print(int1 * int2)
