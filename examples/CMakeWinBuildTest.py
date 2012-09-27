import os

builds=\
{
    "Visual Studio 10 Win64" : "build_VS10x64",
    "Visual Studio 9 2008 Win64" : "build_VS9x64",
    "Visual Studio 8 2005 Win64" : "build_VS8x64",
    "Visual Studio 10" : "build_VS10x32",
    "Visual Studio 9 2008" : "build_VS9x32",
    "Visual Studio 8 2005" : "build_VS8x32",
}

errors=[]

def execc(cmd):
    retval=os.system(cmd)
    if retval!=0: raise Exception("%s exited with %d"%(cmd,retval))

for generator, builddir in builds.items():
    os.mkdir(builddir)
    os.chdir(builddir)
    try:
        execc("cmake -G \""+generator+"\" ../")
        execc("cmake --build ./")
        execc("ctest")
    except Exception as e:
        errors.append("%s: %s"%(generator,e))
    os.chdir("../")

if len(errors)>0:
    print "Errors:"
    for error in errors: print error

raw_input("Press enter")

