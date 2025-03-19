
# Adds an imported target "zip::zip"

include(FetchContent)

if(NOT DOWNLOADED_ZIP_SOURCE_URL)
    set(DOWNLOADED_ZIP_SOURCE_URL https://github.com/kuba--/zip/archive/refs/tags/v0.3.3.tar.gz)
endif()
if(NOT DOWNLOADED_ZIP_SOURCE_URL_HASH_SHA256)
    set(DOWNLOADED_ZIP_SOURCE_URL_HASH_SHA256 89d28f37dbb3e21c3435a2ab34aff1b5ff75dd1ec1760d28577cc766bea06719)
endif()
message(STATUS "Will build zip downloaded from ${DOWNLOADED_ZIP_SOURCE_URL} as external project")

FetchContent_Declare(
    zip_fetchcontent
    URL ${DOWNLOADED_ZIP_SOURCE_URL}
    URL_HASH SHA256=${DOWNLOADED_ZIP_SOURCE_URL_HASH_SHA256}
    TLS_VERIFY true
    NETRC OPTIONAL
)

FetchContent_MakeAvailable(zip_fetchcontent)
