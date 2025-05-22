with open("orig.txt") as f:
    lines  = f.readlines()
    nlines = len(lines)

    stalls = 0

    i = 0

    for line in lines:
#         if (i % 10000) == 0:
#             print("A")

        fields = line.split("\t")
        if fields[0] == "eustall":
            stalls += 1
        elif fields[0] == "string":
            None
        i += 1
