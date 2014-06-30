import subprocess
import datetime
import os

progpathLeft = r'D:\github\Multislice' + '\\'
progpathxRight = r'.release.common.dll' + '\\'
programExe = 'LMultiSlice.exe'
configpath = r'D:\github\Multislice\statistics\config' + '\\' 

architecture = [ "x64", "x86" ]
#Fe = [ "Fe_4_", "Fe_443_", "Fe_146825_", "Fe_659196_", "Fe_1093978_", "Fe_4373275_", "Fe_23128504_" ]
Fe = [ "Fe_4_", "Fe_443_", "Fe_146825_", "Fe_659196_", "Fe_1093978_", "Fe_4373275_" ]
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
            subprocess.Popen([ progpathLeft + elementArchitecture + progpathxRight + programExe, configpath + elementFe + elementPixels + expantion ], shell=True, stdout=open('result\\' + str(curdate) + '\\' + elementArchitecture + '_' + elementFe + elementPixels + '.log', 'w')).wait()
            print("Time execution process: ")
            print(datetime.datetime.now() - startprocess)
            print(elementArchitecture + '_' + elementFe + elementPixels + '.log' + '\n')          
print("Statistics collection ended")