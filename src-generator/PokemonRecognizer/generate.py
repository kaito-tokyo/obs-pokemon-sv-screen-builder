import cv2
from glob import glob
from jinja2 import Environment, FileSystemLoader, Template
import numpy as np
from os import path
import sys

descriptorSize = 16
height = 196
algorithm = cv2.AKAZE_create(cv2.AKAZE_DESCRIPTOR_MLDB, descriptorSize)

pokemonNames = []
data = []
for file in glob('./assets/PokemonRecognizer/*/*.png'):
    name = path.splitext(path.basename(file))[0]
    srcBGRA = cv2.imread(file, cv2.IMREAD_UNCHANGED)
    scaledBGRA = cv2.resize(srcBGRA, (srcBGRA.shape[1] * height // srcBGRA.shape[0], height))
    targetBGR = cv2.cvtColor(scaledBGRA, cv2.COLOR_BGRA2BGR)
    mask = scaledBGRA[:, :, 3]

    descriptors = algorithm.detectAndCompute(targetBGR, mask)[1]
    data.append('{' + ','.join(str(x) for x in np.ravel(descriptors).tolist()) + '}')
    _, pokemonName = name.split(' ')
    pokemonNames.append(pokemonName)

env = Environment(
    loader=FileSystemLoader('src-generator/PokemonRecognizer')
)
template = env.get_template('PokemonRecognizer.j2')
result = template.render(data=data, pokemonNames=pokemonNames, height=height, descriptorSize=descriptorSize)
with open('src/modules/PokemonRecognizerGenerated.cpp', 'w') as fp:
   fp.write(result)
