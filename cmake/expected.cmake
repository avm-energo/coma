include(FetchContent)

FetchContent_Declare(expected
  GIT_REPOSITORY                https://git.asu-vei.ru/avm-energo/tl-expected.git
  GIT_TAG                       master
  DOWNLOAD_EXTRACT_TIMESTAMP    TRUE
)

FetchContent_MakeAvailable(expected)
