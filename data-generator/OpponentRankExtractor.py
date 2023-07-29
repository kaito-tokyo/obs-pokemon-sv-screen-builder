import cv2
from glob import glob
import numpy as np
from os import path
from cbor2 import dump

rect = [544, 831, 834, 36]
threshold = 200
parenDict = {}
cols = [1, 1]
data = [[0], [0]]
ratio = 0.05
for file in glob('./assets/screenshots/OpponentRankExtractor/*.png'):
    name = path.splitext(path.basename(file))[0]
    args = name.split(' ')
    x0, x1, y0, y1 = [int(x) for x in args[0:4]]
    language = args[4]
    symbolName = args[5]
    srcImg = cv2.imread(file)
    destImg = srcImg[y0:y1, x0:x1]
    destImg = cv2.cvtColor(destImg, cv2.COLOR_BGR2GRAY)
    retval, destImg = cv2.threshold(destImg, 128, 255, cv2.THRESH_BINARY)
    cv2.imwrite('./assets/OpponentRankExtractor/' + name + '.png', destImg)

    parenDict[language] = parenDict.get(language, [0, 1])
    if symbolName == 'parenStart':
        parenDict[language][0] = len(data)
    elif symbolName == 'parenEnd':
        parenDict[language][1] = len(data)

    cols.append(destImg.shape[1])
    data.append(np.ravel(destImg).tolist())

with open('data/preset/OpponentRankExtractor.cbor', 'wb') as fp:
    dump({
        "rect": rect,
        "threshold": threshold,
        "parenMap": parenDict,
        "cols": cols,
        "data": data,
        "ratio": ratio,
    }, fp)
