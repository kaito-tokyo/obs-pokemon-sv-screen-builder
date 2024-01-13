import cv2
from glob import glob
import numpy as np
from os import path
from cbor2 import dump

rects = []
threshold = 200
cols = []
data = []
ratio = 0.05
matchTypes = []
for file in glob('./assets/screenshots/MyRankExtractor/*.png'):
    name = path.splitext(path.basename(file))[0]
    args = name.split(' ')
    x0, x1, y0, y1 = [int(x) for x in args[0:4]]
    rect = [int(x) for x in args[4:8]]
    matchType = args[8]
    srcImg = cv2.imread(file)
    destImg = srcImg[y0:y1, x0:x1]
    destImg = cv2.cvtColor(destImg, cv2.COLOR_BGR2GRAY)
    retval, destImg = cv2.threshold(destImg, 200, 255, cv2.THRESH_BINARY)
    cv2.imwrite('./assets/MyRankExtractor/' + name + '.png', destImg)

    rects.append(rect)
    cols.append(destImg.shape[1])
    data.append(np.ravel(destImg).tolist())
    matchTypes.append(matchType)

with open('data/preset/MyRankExtractor.cbor', 'wb') as fp:
    dump({
        "rects": rects,
        "threshold": threshold,
        "cols": cols,
        "data": data,
        "ratio": ratio,
        "matchTypes": matchTypes,
    }, fp)
