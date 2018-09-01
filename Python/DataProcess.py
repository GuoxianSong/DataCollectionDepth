import cv2
import numpy as np
import os
import math
os.chdir("C:/Users/IMI-GuoXian/Desktop/Work/MultiCam/")
path = "Data/"
num  = 10
def LoadDepth(filepath):
    fs_read = cv2.FileStorage(filepath, cv2.FILE_STORAGE_READ)
    arr_read = fs_read.getNode('depth').mat()
    fs_read.release()
    return arr_read

def run():
    for i in range(2):
        for j in range(4085,4648):
            tmp = LoadDepth("Data/depth/"+str(i)+"/"+str(j)+'.txt')
            np.savetxt("Data/newDepth/" +str(i)+"/"+str(j)+'.txt', tmp, fmt="%1.4f")


def isRotationMatrix(R):
    Rt = np.transpose(R)
    shouldBeIdentity = np.dot(Rt, R)
    I = np.identity(3, dtype=R.dtype)
    n = np.linalg.norm(I - shouldBeIdentity)
    return n < 1e-6


def rotationMatrixToEulerAngles(R):
    assert (isRotationMatrix(R))

    sy = math.sqrt(R[0, 0] * R[0, 0] + R[1, 0] * R[1, 0])

    singular = sy < 1e-6

    if not singular:
        x = math.atan2(R[2, 1], R[2, 2])
        y = math.atan2(-R[2, 0], sy)
        z = math.atan2(R[1, 0], R[0, 0])
    else:
        x = math.atan2(-R[1, 2], R[1, 1])
        y = math.atan2(-R[2, 0], sy)
        z = 0

    return np.array([x, y, z])

#run()
tmp = np.loadtxt("Data/Calibration/MutliV/matrix.txt")
r = tmp[:3,:]
t = tmp[3,:]
z = np.array([0,0,1])

print(np.matmul(z,r)+t)
print(rotationMatrixToEulerAngles(r.transpose()))