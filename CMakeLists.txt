cmake_minimum_required(VERSION 3.16)

project(brepvm)

################################################################################
# Source groups
################################################################################
set(no_group_source_files
    "include/brepvm/VM.h"
    "source/VM.cpp"
)
source_group("" FILES ${no_group_source_files})

set(bytecodes
    "include/brepvm/Bytecodes.h"
    "include/brepvm/CodesBuilder.h"
    "source/Bytecodes.cpp"
    "source/CodesBuilder.cpp"
)
source_group("bytecodes" FILES ${bytecodes})

set(dataset
    "include/brepvm/CodeBlock.h"
    "include/brepvm/OpFieldMap.h"
    "include/brepvm/OpFieldType.h"
    "source/OpFieldMap.cpp"
)
source_group("dataset" FILES ${dataset})

set(nodes
    "include/brepvm/Compiler.h"
    "include/brepvm/Decompiler.h"
    "include/brepvm/Optimizer.h"
    "include/brepvm/Profiler.h"
    "source/Compiler.cpp"
    "source/Decompiler.cpp"
    "source/Optimizer.cpp"
    "source/Profiler.cpp"
)
source_group("nodes" FILES ${nodes})

set(opcodes
    "include/brepvm/geo_opcodes.h"
    "include/brepvm/math_opcodes.h"
    "include/brepvm/stl_opcodes.h"
    "source/geo_opcodes.cpp"
    "source/math_opcodes.cpp"
    "source/stl_opcodes.cpp"
)
source_group("opcodes" FILES ${opcodes})

set(optimize
    "include/brepvm/DelComments.h"
    "include/brepvm/FlowGraph.h"
    "source/DelComments.cpp"
    "source/FlowGraph.cpp"
)
source_group("optimize" FILES ${optimize})

set(value
    "include/brepvm/ValueCache.h"
    "include/brepvm/ValueType.h"
    "include/brepvm/VMHelper.h"
    "source/ValueCache.cpp"
    "source/VMHelper.cpp"
)
source_group("value" FILES ${value})

set(ALL_FILES
    ${no_group_source_files}
    ${bytecodes}
    ${dataset}
    ${nodes}
    ${opcodes}
    ${optimize}
    ${value}
)

################################################################################
# Target
################################################################################

add_library(${PROJECT_NAME} STATIC ${ALL_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC include)
if(TARGET easyvm)
    target_link_libraries(${PROJECT_NAME} PRIVATE easyvm)
else()
    target_include_directories(${PROJECT_NAME} PRIVATE external/easyvm/include)
endif()
if(TARGET sm)
    target_link_libraries(${PROJECT_NAME} PRIVATE sm)
else()
    target_include_directories(${PROJECT_NAME} PRIVATE external/sm)
endif()
if(TARGET polymesh3)
    target_link_libraries(${PROJECT_NAME} PRIVATE polymesh3)
else()
    target_include_directories(${PROJECT_NAME} PRIVATE external/polymesh3/include)
endif()
if(TARGET halfedge)
    target_link_libraries(${PROJECT_NAME} PRIVATE halfedge)
else()
    target_include_directories(${PROJECT_NAME} PRIVATE external/halfedge/include)
endif()
