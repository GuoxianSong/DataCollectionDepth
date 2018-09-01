import numpy as np
import dlib
import cv2
##Generate 3D landmark point to fit
# camera coordinate

data_path = "F:/0901gx/"

detector = dlib.get_frontal_face_detector()
predictor = dlib.shape_predictor('shape_predictor_68_face_landmarks.dat')


def loaddepth(filepath):
    fs_read = cv2.FileStorage(filepath, cv2.FILE_STORAGE_READ)
    arr_read = fs_read.getNode('depth').mat()
    fs_read.release()
    return arr_read

def OneFace(img,depth):
    img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    lmk_pos = np.zeros((49,3))
    pixel_pos = []
    try:
        dets = detector(img, 1)
        shape = predictor(img, dets[0])
        for i in range((17,68)): #60 and 64 is not need
            if(i==60 or i==64):
                continue;
            pixel_pos.append([shape.part(i).x,shape.part(i).y])
    except:
        return False
    #


color_img = cv2.imread(data_path+'color/2115.jpg')
depth_img = loaddepth(data_path+'depth/2115.txt')
