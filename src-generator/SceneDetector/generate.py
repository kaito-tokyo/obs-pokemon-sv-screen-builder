import cv2
from glob import glob
from jinja2 import Environment, FileSystemLoader, Template
import numpy as np
from os import path
import sys

data = []
rows = []
for file in glob('./assets/screenshots/SceneDetector/*.png'):
    name = path.basename(file)
    srcImg = cv2.imread(file)
    destImg = srcImg[894:931, 542:661]
    destImg = cv2.cvtColor(destImg, cv2.COLOR_BGR2GRAY)
    retval, destImg = cv2.threshold(destImg, 128, 255, cv2.THRESH_BINARY)
    cv2.imwrite("./assets/SceneDetector/" + name, destImg)

    data.append("{" + ",".join(str(x) for x in np.ravel(destImg).tolist()) + "}")
    rows.append(str(destImg.shape[0]))

env = Environment(
    loader=FileSystemLoader('src-generator/SceneDetector')
)
template = env.get_template('TextRecognizer.j2')
result = template.render(data=data, rows=rows)
with open('src/SceneDetectorTextRecognizerGenerated.cpp', 'w') as fp:
    fp.write(result)
