include(FetchContent)

FetchContent_Declare(limereport
  GIT_REPOSITORY    https://git.avmenergo.ru/avm-energo/limereport.git
  GIT_TAG           master
)

set(LIMEREPORT_DEMO OFF)

FetchContent_MakeAvailable(limereport)
