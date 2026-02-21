import os
DIRNOW = os.path.dirname(os.path.abspath(__file__))
DIAGRAM_TEST = os.path.join(DIRNOW, "diagram_test")

def scan_all():
    wa = 0
    fail = 0
    ac = 0
    empty = 0
    total = 0
    for file_item in os.listdir(DIAGRAM_TEST):
        filepath = os.path.join(DIAGRAM_TEST, file_item)

        with open(filepath, "r") as fp:
            content = fp.read()
            if content.find("/") == -1: # 没有找到左斜线
                empty += 1
                print("empty:", file_item[:-4])
                continue
            total += 1
            lpart, rpart = content.split("/", 1)
            lpart = int(lpart)
            rpart = int(rpart)
            if rpart <= 1:
                fail += 1
            if lpart != rpart:
                print("wa:", file_item[:-4])
                wa += 1
            else:
                ac += 1

    print(f"done: ac/wa/total/empty/fail {ac}/{wa}/{total}/{empty}/{fail}")

if __name__ == "__main__":
    scan_all()
