import sys
import os
import random
import string
import requests
import shutil

def retrieve_image(url, file_name):
    res = requests.get(url, stream = True)
    if res.status_code == 200:
        with open(file_name,'wb') as f:
            shutil.copyfileobj(res.raw, f)

if __name__ == "__main__":
    os.chdir("../../vectorize")
    image_source = sys.argv[1]
    random_file_name_prefix = ''.join(random.choices(string.ascii_lowercase, k=5))
    print(random_file_name_prefix)
    image_file_type = ".jpeg"
    image_file_name = random_file_name_prefix + image_file_type
    retrieve_image(image_source, image_file_name)
    input_file_path = image_file_name
    output_file_type = ".svg"
    output_file_path = random_file_name_prefix + output_file_type
    command = "python3 linedraw.py -i " + input_file_path + " -o " + output_file_path
    os.system(command)
    os.system("mv " + input_file_path + " ../interface/react-client/public/images")
    os.system("mv " + output_file_path + " ../interface/react-client/public/images")
    sys.stdout.flush()