import subprocess
import datetime
import os
import shutil

progPath    = 'D:\\github\\Multislice\\statistics\\bin\\'
programName = 'LMultiSlice.exe'
configpath  = 'D:\\github\\Multislice\\statistics\\config\\'

architectures = ["x64"]

versions = [
    "1_cpu_direct_summation_cutoff",
    "2_cpu_direct_summation_cutoff_threading",
    "3_gpu_direct_summation",
    "4_gpu_direct_summation_cutoff",
    "5_gpu_kirk_method",
    "6_gpu_rodrigues_method" ]

models = [
    "Fe_69439",
    "Fe_1222352",
    "Fe_2374504",
    "Fe_3526544",
    "Fe_4680002",
    "Fe_5831178",
    "Fe_6984427",
    "Fe_8136541",
    "Fe_9289174",
    "Fe_10440839",
    "Fe_11593153",
    "Fe_12745317",
    "Fe_13897792",
    "Fe_15050030",
    "Fe_16203270",
    "Fe_17355647",
    "Fe_18508287",
    "Fe_19660711",
    "Fe_20813412",
    "Fe_21965724",
    "Fe_23128504"]

pixels = [  "256x256",
            "512x512",
            "1024x1024",
            "2048x2048",
            "4096x4096"]

extantion = ".xml"

curdate = datetime.date.today()

for elementArchitecture in architectures:
    for elementVersions in versions[0:1]:
        for elementPixels in pixels:
            for elementModel in models:
                try:
                    startprocess = datetime.datetime.now()
                    if not os.path.exists('result\\' + str(curdate)):
                        os.makedirs('result\\' + str(curdate))
                    if not os.path.exists('result\\' + str(curdate) + '\\' + elementVersions):
                        os.makedirs('result\\' + str(curdate) + '\\' + elementVersions)

                    fullPathProgram = progPath + elementVersions + '\\' + programName
                    filenamexml = configpath + elementModel + "_" + elementPixels + extantion

                    file = open('result\\' + str(curdate) + '\\' + elementVersions + '\\' + elementModel + "_" + elementPixels + '.log', 'w')

                    process = subprocess.Popen(
                        [ fullPathProgram, filenamexml ],
                        shell=True,
                        stdout=file)\
                        .wait()

                    # process = subprocess.call(
                    #     [ fullPathProgram, filenamexml ],
                    #     stdout=file
                    # )

                    shutil.rmtree('result\\' + elementModel + "_" + elementPixels)

                    print("Process finished with exit code ", process)


                    print("Time execution process: ")
                    print(datetime.datetime.now() - startprocess)
                    print(elementArchitecture + '_' + elementModel + "_" + elementPixels + '.log' + '\n')
                except Exception as e:
                    print(str(e))




print("Statistics collection ended")

