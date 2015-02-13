'''
Created on 01 июля 2014 г.

@author: dubinets
'''
models = [ "Fe_4", "Fe_443", "Fe_69439", "Fe_1222352", "Fe_2374504", 
           "Fe_3526544", "Fe_4680002", "Fe_5831178", "Fe_6984427", 
           "Fe_8136541", "Fe_9289174", "Fe_10440839", "Fe_11593153", 
           "Fe_12745317", "Fe_13897792", "Fe_15050030", "Fe_16203270", 
           "Fe_17355647", "Fe_18508287", "Fe_19660711", "Fe_20813412", 
           "Fe_21965724", "Fe_23128504" ]

pixels = [ 32, 64, 128, 256, 512, 1024, 2048, 4096 ]

for elementModel in models:
    for elementPixel in pixels:
        f = open(elementModel + "_" + str(elementPixel) + "x" + str(elementPixel) + ".xml", "w")
        f.write("<?xml version=\"1.0\" ?>" + "\n")
        f.write("<action>" + "\n")
        f.write("    <io>" + "\n")
        f.write("        <fileNameInput>model/" + elementModel + ".coo" + "</fileNameInput>" + "\n")
        f.write("        <fileNameOutput>result/" + elementModel + "_" + str(elementPixel) + "x" + str(elementPixel) + "</fileNameOutput>" + "\n")
        f.write("    </io>" + "\n")
        f.write("    <image>" + "\n")
        f.write("        <nx>" + str(elementPixel) + "</nx>" + "\n")
        f.write("        <ny>" + str(elementPixel) + "</ny>" + "\n")
        f.write("       <dpa>10.0</dpa>        <!--- dots per angstrem --->" + "\n")
        f.write("    </image>" + "\n")
        f.write("    <slicing>" + "\n")
        f.write("        <radiuc>20.0</radiuc>" + "\n")
        f.write("        <numberslices>1</numberslices>" + "\n")
        f.write("    </slicing>" + "\n")
        f.write("    <microscope>" + "\n")
        f.write("        <aperture>10.37</aperture>" + "\n")
        f.write("        <cs>1.3</cs>" + "\n")
        f.write("        <defocus>-70</defocus>     <!---// nm --->" + "\n")
        f.write("        <keV>200</keV>" + "\n")
        f.write("    </microscope>" + "\n")
        f.write("</action>" + "\n")
        f.close()