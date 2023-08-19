import cv2
from glob import glob
import numpy as np
from os import path
from cbor2 import dump

descriptorSize = 16
height = 196
algorithm = cv2.AKAZE_create(cv2.AKAZE_DESCRIPTOR_MLDB, descriptorSize)

blueThreshold = 200
binaryThreshold = 200
indices = []
cols = []
data = []

for file in glob('./assets/MySelectionRecognizer/*.png'):
    name = path.splitext(path.basename(file))[0]
    index = int(name[1])
    srcImg = cv2.imread(file)
    grayImg = cv2.cvtColor(srcImg, cv2.COLOR_BGR2GRAY)
    _, destImg = cv2.threshold(grayImg, binaryThreshold, 255, cv2.THRESH_BINARY)

    indices.append(index)
    cols.append(destImg.shape[1])
    data.append(np.ravel(destImg).tolist())

with open('data/preset/MySelectionRecognizer.cbor', 'wb') as fp:
    dump({
        "blueThreshold": blueThreshold,
        "binaryThreshold": binaryThreshold,
        "ratio": 0.2,
        "indices": indices,
        "cols": cols,
        "data": data,
    }, fp)
