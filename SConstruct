# SCons build file for building raytracer11
from os import *
from glob import *

env = Environment()
env.Append(CPPPATH=['inc'])
env.Program(target='raytracer11', source=glob(join(getcwd(), "src", "*")))
