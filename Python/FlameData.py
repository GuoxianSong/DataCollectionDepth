import numpy as np
import dlib
import cv2
##Generate 3D landmark point to fit
# camera coordinate
# system is 1mm as 1 unit


data_path = "/media/guoxian/D/HDFace_Data/0901gx/"

detector = dlib.get_frontal_face_detector()
predictor = dlib.shape_predictor('shape_predictor_68_face_landmarks.dat')


def loaddepth(filepath):
    fs_read = cv2.FileStorage(filepath, cv2.FILE_STORAGE_READ)
    arr_read = fs_read.getNode('depth').mat()
    fs_read.release()
    return arr_read

def OneFace(img):
    img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    pixel_pos = []
    try:
        dets = detector(img, 1)
        shape = predictor(img, dets[0])
        for i in range(17,68): #60 and 64 is not need
            if(i==60 or i==64):
                continue;
            pixel_pos.append([shape.part(i).x,shape.part(i).y])
    except:
        return False
    return pixel_pos
    #

#Coordinate Transforma: 1920x1080 H to 640x480 L
def H2L(x,y):
    u=(x-960)*0.248069+320+8.40596
    v= (y-540)*0.248069+240-19.52178
    return u,v

def L2H(u,v):
    x=     (u-(320+8.40596))/0.248069+960
    y= (v-(240-19.52178))/0.248069+540
    return x,y

for i in range(2115,2800):
    color_img = cv2.imread(data_path+'color/0/'+str(i)+'.jpg')
#depth_img = loaddepth(data_path+'depth/2115.txt')
    pixel_pos=OneFace(color_img)
    pixel_pos = np.array(pixel_pos,dtype=int)
    np.savetxt("/media/guoxian/D/HDFace_Data/0901gx/2dlmk/"+str(i)+'.txt',pixel_pos,fmt = '%d')
#cv2.imwrite("tmp.jpg",color_img)


