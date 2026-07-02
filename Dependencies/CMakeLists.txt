include(FetchContent)

set(FETCHCONTENT_UPDATES_DISCONNECTED ON CACHE BOOL "Do not update fetched dependencies after the first download")

FetchContent_Declare(
    glm
    URL "https://github.com/g-truc/glm/archive/refs/tags/0.9.9.8.tar.gz"
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

FetchContent_Declare(
    rapidjson
    URL "https://github.com/Tencent/rapidjson/archive/refs/tags/v1.1.0.tar.gz"
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

FetchContent_Declare(
    cereal
    URL "https://github.com/USCiLab/cereal/archive/refs/tags/v1.3.2.tar.gz"
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

FetchContent_Declare(
    tclap
    URL "https://downloads.sourceforge.net/project/tclap/tclap-1.2.5.tar.gz"
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

FetchContent_Declare(
    mustache
    GIT_REPOSITORY "https://github.com/kainjow/Mustache.git"
    GIT_TAG "506c6d3"
)

foreach(KEYENGINE_DEPENDENCY glm rapidjson cereal tclap mustache)
    FetchContent_GetProperties(${KEYENGINE_DEPENDENCY})
    if(NOT ${KEYENGINE_DEPENDENCY}_POPULATED)
        FetchContent_Populate(${KEYENGINE_DEPENDENCY})
    endif()
endforeach()

add_library(KeyThirdParty_glm INTERFACE)
add_library(KeyThirdParty::glm ALIAS KeyThirdParty_glm)
target_include_directories(KeyThirdParty_glm INTERFACE "${glm_SOURCE_DIR}")

add_library(KeyThirdParty_rapidjson INTERFACE)
add_library(KeyThirdParty::rapidjson ALIAS KeyThirdParty_rapidjson)
target_include_directories(KeyThirdParty_rapidjson INTERFACE "${rapidjson_SOURCE_DIR}/include")

add_library(KeyThirdParty_base64 INTERFACE)
add_library(KeyThirdParty::base64 ALIAS KeyThirdParty_base64)
target_include_directories(KeyThirdParty_base64 INTERFACE "${cereal_SOURCE_DIR}/include/cereal/external")

add_library(KeyThirdParty_tclap INTERFACE)
add_library(KeyThirdParty::tclap ALIAS KeyThirdParty_tclap)
target_include_directories(KeyThirdParty_tclap INTERFACE "${tclap_SOURCE_DIR}/include")

add_library(KeyThirdParty_mustache INTERFACE)
add_library(KeyThirdParty::mustache ALIAS KeyThirdParty_mustache)
target_include_directories(KeyThirdParty_mustache INTERFACE "${mustache_SOURCE_DIR}")
