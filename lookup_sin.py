# create sinus lookup tables
import math

STEP = 10

print("float sinLookup[] = {\\")
for i in range(0, 360, STEP):
    lookup_element = math.sin(math.radians(i))
    lookup_element = round(lookup_element, 5)

    if i + STEP >= 360:
        print("   {}\\".format(lookup_element))
    else:
        print("   {},\\".format(lookup_element))
print("};")