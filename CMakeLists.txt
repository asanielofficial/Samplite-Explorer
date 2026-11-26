cmake_minimum_required(VERSION 3.22)
project(SampliteExplorer VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.4
    GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(JUCE)

set(COMMON_DEFS JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0 JUCE_USE_MP3AUDIOFORMAT=1)

juce_add_gui_app(SampliteExplorer PRODUCT_NAME "Samplite Explorer")
target_sources(SampliteExplorer PRIVATE Source/Main.cpp)
target_compile_definitions(SampliteExplorer PRIVATE ${COMMON_DEFS})
target_link_libraries(SampliteExplorer
    PRIVATE juce::juce_audio_utils juce::juce_dsp
    PUBLIC juce::juce_recommended_config_flags juce::juce_recommended_warning_flags)

juce_add_console_app(ExplorerTests)
target_sources(ExplorerTests PRIVATE Tests/Tests.cpp)
target_compile_definitions(ExplorerTests PRIVATE ${COMMON_DEFS})
target_link_libraries(ExplorerTests
    PRIVATE juce::juce_audio_formats juce::juce_dsp
    PUBLIC juce::juce_recommended_config_flags)

enable_testing()
add_test(NAME ExplorerTests COMMAND ExplorerTests)
