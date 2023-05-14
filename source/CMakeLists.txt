##==============================================================================
## General.
##==============================================================================
cmake_minimum_required(VERSION 3.19)
project(unigine_project LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS FALSE)

##==============================================================================
## Environment.
##==============================================================================
set(UNIGINE_DOUBLE 0)
set(UNIGINE_SDK_PATH ${PROJECT_SOURCE_DIR}/../)
set(UNIGINE_BIN_DIR ${UNIGINE_SDK_PATH}bin)
set(UNIGINE_LIB_DIR ${UNIGINE_SDK_PATH}lib)
set(UNIGINE_INCLUDE_DIR ${UNIGINE_SDK_PATH}include)

##==============================================================================
## Dependencies.
##==============================================================================
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules)

set(target "TanksOfWorld")

# Engine.
find_package(Engine REQUIRED MODULE QUIET)

add_executable(${target}
		${PROJECT_SOURCE_DIR}/AppEditorLogic.cpp
		${PROJECT_SOURCE_DIR}/AppEditorLogic.h
		${PROJECT_SOURCE_DIR}/AppSystemLogic.cpp
		${PROJECT_SOURCE_DIR}/AppSystemLogic.h
		${PROJECT_SOURCE_DIR}/AppWorldLogic.cpp
		${PROJECT_SOURCE_DIR}/AppWorldLogic.h
		${PROJECT_SOURCE_DIR}/main.cpp
		${PROJECT_SOURCE_DIR}/Components/TankController.cpp
		${PROJECT_SOURCE_DIR}/Components/TankController.h
		${PROJECT_SOURCE_DIR}/Components/GameController.cpp
		${PROJECT_SOURCE_DIR}/Components/GameController.h
		${PROJECT_SOURCE_DIR}/Components/Bars/HealthBar.cpp
		${PROJECT_SOURCE_DIR}/Components/Bars/HealthBar.h
		${PROJECT_SOURCE_DIR}/Components/Bars/ReloadBar.cpp
		${PROJECT_SOURCE_DIR}/Components/Bars/ReloadBar.h
		${PROJECT_SOURCE_DIR}/Components/PlayerController.cpp
		${PROJECT_SOURCE_DIR}/Components/PlayerController.h
		${PROJECT_SOURCE_DIR}/Components/TankBot/TankBot.cpp
		${PROJECT_SOURCE_DIR}/Components/TankBot/TankBot.h
        ${PROJECT_SOURCE_DIR}/Components/TankBot/BotState.h
		${PROJECT_SOURCE_DIR}/Components/TankBot/BotStateFindEnemy.cpp
		${PROJECT_SOURCE_DIR}/Components/TankBot/BotStateFindEnemy.h
		${PROJECT_SOURCE_DIR}/Components/TankBot/BotStateKillEnemy.cpp
		${PROJECT_SOURCE_DIR}/Components/TankBot/BotStateKillEnemy.h
		${PROJECT_SOURCE_DIR}/Components/TankBot/BotState.cpp
		${PROJECT_SOURCE_DIR}/Components/Related/Projectile.cpp
		${PROJECT_SOURCE_DIR}/Components/Related/Projectile.h)

target_include_directories(${target}
	PRIVATE
	${UNIGINE_INCLUDE_DIR}
	${PROJECT_SOURCE_DIR}
	)

target_link_libraries(${target}
	PRIVATE
	Unigine::Engine
	)

target_compile_definitions(${target}
	PRIVATE
	$<$<BOOL:${UNIX}>:_LINUX>
	$<$<CONFIG:Debug>:DEBUG>
	$<$<NOT:$<CONFIG:Debug>>:NDEBUG>
	)

##==============================================================================
## Compiler constants.
##==============================================================================
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  set(UNIGINE_COMPILER_IS_MSVC TRUE)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(UNIGINE_COMPILER_IS_CLANG TRUE)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(UNIGINE_COMPILER_IS_GNU TRUE)
endif()

if (UNIGINE_COMPILER_IS_MSVC)
    target_compile_definitions(${target}
	PRIVATE
	_CRT_SECURE_NO_DEPRECATE
	)
    include(ProcessorCount)
    ProcessorCount(proc_count)
    target_compile_options(${target}
	PRIVATE
	/TP               # Specifies all source files are C++.
	/FS               # Forces writes to the program database (PDB) file to be serialized through MSPDBSRV.EXE.
	/fp:fast          # Specifies floating-point behavior.
	/bigobj           # Increases the number of addressable sections in an .obj file.
	/EHsc             # Specifies the kind of exception handling used by the compiler.
	/MP${proc_count}  # Build with Multiple Processes.
	)
    unset(proc_count)
    target_link_options(${target} INTERFACE "/FIXED:NO")

elseif(UNIGINE_COMPILER_IS_GNU OR UNIGINE_COMPILER_IS_CLANG)

    target_compile_options(${target}
	PRIVATE
	-m64
	-march=athlon64
	-msse4.2
	# Optimizations.
	-ffast-math
	-fno-strict-aliasing
	$<$<NOT:$<CONFIG:Debug>>:-fomit-frame-pointer>
	# Warnings.
	$<$<CONFIG:Debug>:-Wall>
	$<$<CONFIG:Debug>:-Wno-unknown-pragmas>
	$<$<CONFIG:Debug>:-Wno-unused-parameter>
	)
endif ()

set(binary_name ${target})
string(APPEND binary_name "_x64")
set_target_properties(${target} PROPERTIES DEBUG_POSTFIX "d")
set_target_properties(${target} PROPERTIES OUTPUT_NAME ${binary_name})

set_target_properties(${target}
	PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${UNIGINE_BIN_DIR}
	RUNTIME_OUTPUT_DIRECTORY_DEBUG ${UNIGINE_BIN_DIR}
	RUNTIME_OUTPUT_DIRECTORY_RELEASE ${UNIGINE_BIN_DIR}
	RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${UNIGINE_BIN_DIR}
	RUNTIME_OUTPUT_DIRECTORY_RELMINSIZE ${UNIGINE_BIN_DIR}
	)
