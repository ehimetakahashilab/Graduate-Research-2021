import json
import os
from tqdm import tqdm
import shutil

class CreateApi:
    
    
    JSON_TYPE_LIST1 = ["2013", "2014", "2015"]
    JSON_TYPE_LIST2 = ["2016", "2017"]
    TypeCounter = {}
    REMOVE_VALUE = 100
    REMOVE_FAMILY_NAME = ["trojan.win32.generic", "dangerousobject.multi.generic", "trojan.script.generic"]

    JsonDir = "./MWS2020/"
    DatasetDir = "./Dataset/"
    InfoTextDir = "./InfoText/"
    auto_remove = True
    
    
    def CheckJsonType(self, Dir):
        for item in self.JSON_TYPE_LIST1:
            if item in Dir:
                return 1
        for item in self.JSON_TYPE_LIST2:
            if item in Dir:
                return 2

        #print("This file isn't identified.")
        #print("[Year] is not included in the file name OR this [Year] Json file isn't allowed in the Class of [MakeApiclm].")
        #sys.exit(1)
    
    def CreateAllApi(self):
        os.makedirs(self.DatasetDir)

        for Dir in os.listdir(self.JsonDir):
            print("Make Dataset: " + Dir)
            json_type = self.CheckJsonType(Dir)
            print("Json Type: " + str(json_type))
            self.StoreApi(self.JsonDir + Dir + "/", json_type)
            print('')

        self.SaveInformation()

        if self.auto_remove:
            self.RemoveNonsat()
    
    
    def StoreApi(self, paths, json_type):
        for file in tqdm(os.listdir(paths)):
            try:
                with open(paths + file, 'r') as f:
                    file_json = json.load(f)

                name = file_json['virustotal']['scans']['Kaspersky']['result']

                if json_type == 1:
                    apis = [item['api'] for item in file_json['behavior']['processes'][0]['calls']]

                elif json_type == 2:
                    apis = [item['api'] for item in file_json['behavior']['processes'][1]['calls']]

            except:
                continue


            if name != None and apis != None and apis:
                if ':' in name:
                    name_split = name.rsplit(':', 1)
                    name = name_split[1]

                name_split = name.split('.')
                name_trans = ""
                join = 0
                for namestr in name_split:
                    if join == 0:
                        name_trans = namestr
                    else:
                        name_trans = name_trans + '.' + namestr
                    join += 1
                    if join >= 3:
                        break
                
                name_trans = name_trans.lower()

                if os.path.exists(self.DatasetDir + name_trans + "/"):
                    self.TypeCounter[name_trans] += 1
                else:
                    os.makedirs(self.DatasetDir + name_trans + "/")
                    self.TypeCounter[name_trans] = 1
                with open(self.DatasetDir + name_trans + "/" + str(self.TypeCounter[name_trans]) + ".txt", "w") as f:
                    counter = 0
                    for api in apis:
                        counter += 1
                        if counter == len(apis):
                            f.write(api)
                        else:
                            f.write(api + " ")
    
    
    def SaveInformation(self):
        if os.path.exists(self.InfoTextDir):
            pass
        else:
            os.makedirs(self.InfoTextDir)

        with open(self.InfoTextDir + "Info.txt", "w") as f:
            f.write("--Number of Default Files-- \n \n")
            for key in self.TypeCounter:
                f.write(key + ":" + str(self.TypeCounter[key]) + "\n")


    def RemoveNonsat(self):
        for key in list(self.TypeCounter):
            if self.TypeCounter[key] < CreateApi.REMOVE_VALUE:
                shutil.rmtree(self.DatasetDir + key)
                del self.TypeCounter[key]
            elif key in CreateApi.REMOVE_FAMILY_NAME:
                shutil.rmtree(self.DatasetDir + key)
                del self.TypeCounter[key]
    

def main():
    create_api = CreateApi()
    create_api.CreateAllApi()

if __name__ == '__main__':
    main()

    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
