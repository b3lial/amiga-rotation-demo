# create cosinus lookup tables
import math

STEP = 10

print("float cosLookup[] = {\\")
for i in range(0, 360, STEP):
    lookup_element = math.cos(math.radians(i))
    lookup_element = round(lookup_element, 5)

    if i + STEP >= 360:
        print("   {}\\".format(lookup_element))
    else:
        print("   {},\\".format(lookup_element))
print("};")