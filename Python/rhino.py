import os
import rhinoscriptsyntax as rs
x = "D:\\HDFace_Data\\0901gx\\new_depth_pos\\"
y=2110

def value(path):
    fname=  path
    with open(fname) as f:
        content = f.readlines()
    # you may also want to remove whitespace characters like `\n` at the end of each line
    _content = [r.strip() for r in content]
    return _content
x_content=value(x+str(y)+"_x.txt")
y_content=value(x+str(y)+"_y.txt")
z_content=value(x+str(y)+"_z.txt")


for i in range(len(x_content)):
    tmp_x = x_content[i].split(" ")
    tmp_y = y_content[i].split(" ")
    tmp_z = z_content[i].split(" ")
    for j in range(640):
        rs.AddPoint((float(tmp_x[j]), float(tmp_y[j]), float(tmp_z[j])))
