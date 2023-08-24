import cv2
from glob import glob
import numpy as np
from os import path
from cbor2 import dump

rect = [542, 894, 119, 37]
threshold = 200
data = []
cols = []
for file in glob('./assets/screenshots/SceneDetector/*.png'):
    name = path.basename(file)
    srcImg = cv2.imread(file)
    destImg = srcImg[rect[1]:rect[1] + rect[3], rect[0]:rect[0] + rect[2]]
    destImg = cv2.cvtColor(destImg, cv2.COLOR_BGR2GRAY)
    retval, destImg = cv2.threshold(destImg, threshold, 255, cv2.THRESH_BINARY)
    cv2.imwrite("./assets/SceneDetector/" + name, destImg)

    cols.append(destImg.shape[1])
    data.append(np.ravel(destImg).tolist())

with open('data/preset/SceneDetector_lobbyRankShown.cbor', 'wb') as fp:
    dump({
        "rect": rect,
        "threshold": threshold,
        "ratio": 0.2,
        "cols": cols,
        "data": data,
    }, fp)
