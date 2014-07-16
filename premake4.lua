solution "raytracer11"
  configurations {"debug", "release"}

  configuration {"debug"}
    targetdir "bin/debug"
  configuration {"release"}
    targetdir "bin/release"

project "raytracer11"
  kind "ConsoleApp"
  language "C++"
  files {"./inc/**.h", "./src/**.cpp"}
  includedirs {"./inc"}
  buildoptions {"-std=c++11"}

  configuration {"debug"}
    defines {"DEBUG"}
    flags {"Symbols"}
  configuration {"release"}
    defines {"RELEASE"}
    flags {"Optimize"}
