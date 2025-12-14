import os
DIRNOW = os.path.dirname(os.path.abspath(__file__))

if __name__ == "__main__":
    link_table = os.path.join(DIRNOW, "link_table.txt")

    with open(link_table) as fp:
        for line in fp:
            name, content = line.split(":")
            content = content.strip() + "\n"

            with open(os.path.join(DIRNOW, "pd_code", name + ".txt"), "w") as fpout:
                fpout.write(content)
