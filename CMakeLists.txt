# CMakeList.txt: proyecto de CMake para STProject, incluya el origen y defina
# la lógica específica del proyecto aquí.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("STProject")

set (CMAKE_CXX_STANDARD 14)

# Agregue un origen al ejecutable de este proyecto.
add_executable (STProject "src/MarkovManager.cpp" "src/MarkovManager.h" "src/MarkovChain.cpp" "src/MarkovChain.h" "src/PluginEditor.cpp" "src/PluginEditor.h" "src/PluginProcessor.cpp" "src/PluginProcessor.h" "src/Pruebas.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET STProject PROPERTY CXX_STANDARD 20)
endif()

# TODO: Agregue pruebas y destinos de instalación si es necesario.

add_subdirectory(.. /JUCE)   
