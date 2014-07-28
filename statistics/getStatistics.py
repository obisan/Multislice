import subprocess
import datetime
import os

progpathLeft = r'D:\github\Multislice' + '\\'
progpathxRight = r'.release.common.dll' + '\\'
programExe = 'LMultiSlice.exe'
configpath = r'D:\github\Multislice\statistics\config' + '\\' 

architecture = [ "x64", "x86" ]
Fe = [ "Fe_4", "Fe_443", "Fe_69439", "Fe_1222352", "Fe_2374504",
       "Fe_3526544", "Fe_4680002", "Fe_5831178", "Fe_6984427",
       "Fe_8136541", "Fe_9289174", "Fe_10440839", "Fe_11593153",
       "Fe_12745317", "Fe_13897792", "Fe_15050030", "Fe_16203270",
       "Fe_17355647", "Fe_18508287", "Fe_19660711", "Fe_20813412",
       "Fe_21965724", "Fe_23128504" ]
pixels = [ "32x32", "64x64", "128x128", "256x256", "512x512", "1024x1024", "2048x2048", "4096x4096" ]

expantion = ".xml"

curdate = datetime.date.today()

# config path + Fe[i] + pixels[i] + expantion 

for elementArchitecture in architecture:
    for elementFe in Fe:
        for elementPixels in pixels:
            startprocess = datetime.datetime.now() 
            if not os.path.exists('result\\' + str(curdate)):
                os.makedirs('result\\' + str(curdate))
            strProgram = progpathLeft + elementArchitecture + progpathxRight + programExe
            strArgument = configpath + elementFe + "_" + elementPixels + expantion
            #subprocess.Popen([ strProgram, strArgument ], shell=True, stdout=open('result\\' + str(curdate) + '\\' + elementArchitecture + '_' + elementFe + elementPixels + '.log', 'w')).wait()
            subprocess.call([ strProgram, strArgument ], stdout=open('result\\' + str(curdate) + '\\' + elementArchitecture + '_' + elementFe + "_" + elementPixels + '.log', 'w'))
            print("Time execution process: ")
            print(datetime.datetime.now() - startprocess)
            print(elementArchitecture + '_' + elementFe + "_" + elementPixels + '.log' + '\n')          
print("Statistics collection ended")