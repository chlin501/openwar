cmake_minimum_required(VERSION 2.8)
project(openwar CXX)

#set(CMAKE_VERBOSE_MAKEFILE 1)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)


find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(OpenGLES2 REQUIRED)


set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/output)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/output)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/output)


include_directories(
        ./
        External/glm
        Library
        Platform
        ${OPENGLES2_INCLUDE_DIR}
        ${SDL2_INCLUDE_DIR}
        ${SDL2_IMAGE_INCLUDE_DIR}
        ${SDL2_TTF_INCLUDE_DIR}
)


add_definitions(-DGLM_SWIZZLE)
add_definitions(-DGLM_FORCE_RADIANS)

add_definitions(-DPHALANX_TARGET_OS_LINUX)
add_definitions(-DPHALANX_TARGET_UI_SDL)
add_definitions(-DOPENWAR_USE_SDL)
add_definitions(-DPHALANX_ENABLE_UBIDI=0)


set(SOURCE_FILES
        main.cpp
        Library/Algebra/geometry.cpp
        Library/Algorithms/bspline.cpp
        Library/Algorithms/bspline_patch.cpp
        Library/Algorithms/GaussBlur.cpp
        Library/Algorithms/quadtree.cpp
        Library/Algorithms/vec2_sampler.cpp
        Library/Graphics/CommonShaders.cpp
        Library/Graphics/FontAdapter.cpp
        Library/Graphics/FontDescriptor.cpp
        Library/Graphics/FrameBuffer.cpp
        Library/Graphics/GraphicsContext.cpp
        Library/Graphics/Image.cpp
        Library/Graphics/RenderBuffer.cpp
        Library/Graphics/RenderCall.cpp
        Library/Graphics/Sampler.cpp
        Library/Graphics/ShaderProgram.cpp
        Library/Graphics/Texture.cpp
        Library/Graphics/TextureAtlas.cpp
        Library/Graphics/TextureFont.cpp
        Library/Graphics/TextureResource.cpp
        Library/Graphics/Vertex.cpp
        Library/Graphics/VertexBuffer.cpp
        Library/Graphics/Viewport.cpp
        Library/Shapes/BillboardColorShader.cpp
        Library/Shapes/BillboardTextureShader.cpp
        Library/Shapes/BillboardTextureShape.cpp
        Library/Shapes/PathRenderer.cpp
        Library/Shapes/VertexShape.cpp
        Library/Storage/Preferences.cpp
        Library/Storage/Resource.cpp
        Library/Surface/Animation.cpp
        Library/Surface/ClickGesture.cpp
        Library/Surface/ClickHotspot.cpp
        Library/Surface/Gesture.cpp
        Library/Surface/Hotspot.cpp
        Library/Surface/Surface.cpp
        Library/Surface/SurfaceAdapter_SDL.cpp
        Library/Surface/Touch.cpp
        Library/Surface/View.cpp
        Library/Widgets/ButtonGrid.cpp
        Library/Widgets/ButtonGridTextureSheet.cpp
        Library/Widgets/ImageWidget.cpp
        Library/Widgets/StringWidget.cpp
        Library/Widgets/Widget.cpp
        Library/Widgets/WidgetShader.cpp
        Library/Widgets/WidgetView.cpp
        Platform/OpenWarSurface.cpp
        Platform/Audio/MusicDirector.cpp
        Platform/Audio/SoundLoader.cpp
        Platform/Audio/SoundPlayer.cpp
        Platform/BattleMap/BattleMap.cpp
        Platform/BattleMap/GroundMap.cpp
        Platform/BattleMap/HeightMap.cpp
        Platform/BattleMap/MapEditor.cpp
        Platform/BattleMap/SmoothGroundMap.cpp
        Platform/BattleMap/TiledGroundMap.cpp
        Platform/BattleModel/BattleCommander.cpp
        Platform/BattleModel/BattleObjects.cpp
        Platform/BattleModel/BattleObjects_v1.cpp
        Platform/BattleModel/BattleObserver.cpp
        Platform/BattleModel/BattleScenario.cpp
        Platform/BattleModel/BattleSimulator.cpp
        Platform/BattleModel/BattleSimulator_v1_0_0.cpp
        Platform/BattleScript/BattleScript.cpp
        Platform/BattleScript/MonkeyScript.cpp
        Platform/BattleScript/PracticeScript.cpp
        Platform/BattleView/BattleGesture.cpp
        Platform/BattleView/BattleHotspot.cpp
        Platform/BattleView/BattleLayer.cpp
        Platform/BattleView/BattleView.cpp
        Platform/BattleView/CasualtyMarker.cpp
        Platform/BattleView/RangeMarker.cpp
        Platform/BattleView/ShootingCounter.cpp
        Platform/BattleView/SmokeCounter.cpp
        Platform/BattleView/UnitCounter.cpp
        Platform/BattleView/UnitMarker.cpp
        Platform/BattleView/UnitMovementMarker.cpp
        Platform/BattleView/UnitTrackingMarker.cpp
        Platform/SmoothTerrain/SmoothTerrainRenderer.cpp
        Platform/SmoothTerrain/SmoothTerrainShaders.cpp
        Platform/SmoothTerrain/SmoothTerrainSky.cpp
        Platform/SmoothTerrain/SmoothTerrainWater.cpp
        Platform/TerrainForest/BillboardTerrainForest.cpp
        Platform/TerrainForest/TerrainForest.cpp
        Platform/TerrainSky/TerrainSky.cpp
        Platform/TerrainView/EditorGesture.cpp
        Platform/TerrainView/EditorHotspot.cpp
        Platform/TerrainView/EditorModel.cpp
        Platform/TerrainView/TerrainGesture.cpp
        Platform/TerrainView/TerrainHotspot.cpp
        Platform/TerrainView/TerrainView.cpp
        Platform/TerrainView/TerrainViewport.cpp
        Platform/TiledTerrain/TiledTerrainRenderer.cpp
        )


add_executable(openwar ${SOURCE_FILES})
target_link_libraries(openwar  ${OPENGLES2_LIBRARIES} ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARY} ${SDL2_TTF_LIBRARY})

set_property(TARGET openwar PROPERTY CXX_STANDARD 11)
set_property(TARGET openwar PROPERTY CXX_STANDARD_REQUIRED ON)
