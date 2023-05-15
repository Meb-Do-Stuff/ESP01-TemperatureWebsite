open("out", "w")
with open("out", "a") as f:
    for line in open("index.html").readlines():
        f.write(f'client.println("{line[:-1]}");\n')
