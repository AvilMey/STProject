# CMakeList.txt: proyecto de CMake para STProject, incluya el origen y defina
# la lógica específica del proyecto aquí.
#
cmake_minimum_required (VERSION 3.8)

project (STProject VERSION 0.0.1)

set (CMAKE_CXX_STANDARD 14)

# Markov Library as a separate part of the build
add_library(markov-lib "src/MarkovManager.cpp"  "src/MarkovChain.cpp")

# Agregue un origen al ejecutable de este proyecto.
add_executable (markov-ex "src/MarkovManager.cpp"  "src/Pruebas.cpp"  )

target_link_libraries(markov-ex  markov-lib)

add_subdirectory(.. /JUCE)   

juce_add_plugin(markov-plugin
    COMPANY_NAME Maria                         # Specify the name of the plugin's author
    IS_SYNTH TRUE                       # Is this a synth or an effect?
    NEEDS_MIDI_INPUT TRUE               # Does the plugin need midi input?
    NEEDS_MIDI_OUTPUT TRUE              # Does the plugin need midi output?
    IS_MIDI_EFFECT TRUE                 # Is this plugin a MIDI effect?
    # EDITOR_WANTS_KEYBOARD_FOCUS TRUE/FALSE    # Does the editor need keyboard focus?
    COPY_PLUGIN_AFTER_BUILD TRUE        # Should the plugin be installed to a default location after building?
    PLUGIN_MANUFACTURER_CODE Mery               # A four-character manufacturer id with at least one upper-case character
    PLUGIN_CODE vokR                            # A unique four-character plugin id with exactly one upper-case character
                                                # GarageBand 10.3 requires the first letter to be upper-case, and the remaining letters to be lower-case
    FORMATS AU VST3 Standalone                  # The formats to build. Other valid formats are: AAX Unity VST AU AUv3
    PRODUCT_NAME "markov-plugin")        # The name of the final executable, which can differ from the target name

    juce_generate_juce_header(markov-plugin)

    target_sources(markov-plugin
    PRIVATE
    src/PluginEditor.cpp
    src/PluginProcessor.cpp
    src/MarkovManager.cpp  
    src/MarkovChain.cpp
   )

   target_compile_definitions(markov-plugin
    PUBLIC # 
        JUCE_ALSA=1
        JUCE_DIRECTSOUND=1
        JUCE_DISABLE_CAUTIOUS_PARAMETER_ID_CHECKING=1
        #JUCE_PLUGINHOST_LADSPA=1
        #JUCE_PLUGINHOST_LV2=1
        #JUCE_PLUGINHOST_VST3=1
        JUCE_USE_OGGVORBIS=1
        #JUCE_VST3_HOST_CROSS_PLATFORM_UID=1
        # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
        JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_plugin` call
        JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_plugin` call
        JUCE_VST3_CAN_REPLACE_VST2=0
   )

   target_link_libraries(markov-plugin
    PRIVATE
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
   )



