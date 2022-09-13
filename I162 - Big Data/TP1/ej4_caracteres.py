root_path = 'drive/My Drive/Colab Notebooks/'

from google.colab import drive
drive.mount('/content/drive', force_remount=True)

import datetime, math
import sys
sys.path.append('/content/' + root_path)

inputDir = root_path + "WordCount/input/"
outputDir = root_path + "WordCount/caracteres/"

## from MRE import Job

def fmap(key, value, context):
    words = value.split()
    for w in words:
        for car in w:
            context.write(car, 1)

        ## ej2: context.write(w, 1)
        
def fred(key, values, context):
    c=0

    ## contar cuantos valores tiene la clave
    for v in values:
        c=c+1

    ## mandar a txt (ej 2)
    context.write(key, c)



job = Job(inputDir, outputDir, fmap, fred)
success = job.waitForCompletion()
print(success)