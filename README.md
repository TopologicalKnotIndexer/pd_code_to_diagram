## pd_code_to_diagram
given a pd_code for a topological link or knot, output a possible 2d-diagram.

## Install

```bash
pip install pd-code-to-diagram
```

## Usage
```python
import pd_code_to_diagram

# pd_code is a list of list of int
pd_code = [[6, 1, 7, 2], [14, 7, 15, 8], [4, 15, 1, 16], [10, 6, 11, 5], [8, 4, 9, 3], [18, 11, 19, 12], [20, 17, 5, 18], [12, 19, 13, 20], [16, 10, 17, 9], [2, 14, 3, 13]]

# last_socket should be int or None
#   the program will ensure in the components contains last_socket
#   there will be at least one arc on the border of 
last_socket = 1

# diagram_2d will be a matrix of int
#   its type will be list of list of int
#   get_diagram_from_pd_code will raise an RuntimeError if such diagram not exists
diagram_2d = pd_code_to_diagram.get_diagram_from_pd_code(pd_code, last_socket)
for line in diagram_2d:
    print(line)

# get string_form diagram and save to file
link = [[2, 9, 3, 10], [4, 11, 5, 12], [6, 7, 1, 8], [8, 5, 7, 6], [10, 1, 11, 2], [12, 3, 9, 4]]
with open("test.txt", "w") as fp:
    fp.write(pd_code_to_diagram.get_diagram_str_from_pd_code(link, last_socket))

diagram_now = pd_code_to_diagram.get_diagram_from_pd_code(link, last_socket)
print(pd_code_to_diagram.diagram_to_pd_code(diagram_now))
```
