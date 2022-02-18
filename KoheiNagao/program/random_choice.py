## random_choice ##

import glob
import random
import os 
import shutil


INPUT_DIR = '/Users/Nagao/Desktop/Feature_Dataset/trojan.win32.poweliks'
OUTPUT_DIR = '/Users/Nagao/Desktop/Features/trojan.win32.poweliks'

def random_sample_file():
    files = glob.glob(INPUT_DIR + '/*.txt')

    random_sample_file = random.sample(files, 4)
    os.makedirs(OUTPUT_DIR, exist_ok=True)

    for file in random_sample_file:
        shutil.copy2(file, OUTPUT_DIR)

if __name__ == '__main__':
    random_sample_file()