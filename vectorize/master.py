import sys
import os
import random
import string
import requests
import shutil

# retrieve an image hosted at url by downloading a stream of bytes
def retrieve_image(url, file_name):
    res = requests.get(url, stream = True)
    if res.status_code == 200:
        with open(file_name,'wb') as f:
            shutil.copyfileobj(res.raw, f)

if __name__ == "__main__":
    # script is called from ./interface/server/server.js so we switch file paths
    os.chdir("../../vectorize")
    # get image url from command line args
    image_source = sys.argv[1]
    # create a random 5 character string to be used as file name
    random_file_name_prefix = ''.join(random.choices(string.ascii_lowercase, k=5))
    # print random file name to stdout
    print(random_file_name_prefix)
    # images produced by DALL·E are .png files
    image_file_type = ".png"
    image_file_name = random_file_name_prefix + image_file_type
    # retrieve image from url = image_source and create image file with name = image_file_name
    retrieve_image(image_source, image_file_name)
    input_file_path = image_file_name
    # linedraw.py generates .svg files which are vectorized versions of provided input files
    output_file_type = ".svg"
    output_file_path = random_file_name_prefix + output_file_type
    # make call to linedraw.py
    command = "python3 linedraw.py -i " + input_file_path + " -o " + output_file_path
    os.system(command)
    # move files into Client's public folder to be displayed on GUI
    os.system("mv " + input_file_path + " ../interface/react-client/public/images")
    os.system("mv " + output_file_path + " ../interface/react-client/public/images")
    # flush stdout to be read in by server
    sys.stdout.flush()