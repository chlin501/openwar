cmake_minimum_required(VERSION 2.8)
project(openwar CXX)

#set(CMAKE_VERBOSE_MAKEFILE 1)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)


find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(OpenGLES2 REQUIRED)


set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})


include_directories(
        ../External/glm
        ../Sources-Cpp
        ${OPENGLES2_INCLUDE_DIR}
        ${SDL2_INCLUDE_DIR}
        ${SDL2_IMAGE_INCLUDE_DIR}
        ${SDL2_TTF_INCLUDE_DIR}
)


add_definitions(-DGLM_SWIZZLE)
add_definitions(-DGLM_FORCE_RADIANS)
add_definitions(-DOPENWAR_PLATFORM_LINUX)


set(SOURCE_FILES
        main.cpp
        ../Sources-Cpp/OpenWarSurface.cpp
        ../Sources-Cpp/Algebra/geometry.cpp
        ../Sources-Cpp/Algorithms/bspline.cpp
        ../Sources-Cpp/Algorithms/bspline_patch.cpp
        ../Sources-Cpp/Algorithms/GaussBlur.cpp
        ../Sources-Cpp/Algorithms/quadtree.cpp
        ../Sources-Cpp/Algorithms/vec2_sampler.cpp
        ../Sources-Cpp/Audio/MusicDirector.cpp
        ../Sources-Cpp/Audio/SoundLoader.cpp
        ../Sources-Cpp/Audio/SoundPlayer.cpp
        ../Sources-Cpp/BattleMap/BattleMap.cpp
        ../Sources-Cpp/BattleMap/GroundMap.cpp
        ../Sources-Cpp/BattleMap/HeightMap.cpp
        ../Sources-Cpp/BattleMap/MapEditor.cpp
        ../Sources-Cpp/BattleMap/SmoothGroundMap.cpp
        ../Sources-Cpp/BattleMap/TiledGroundMap.cpp
        ../Sources-Cpp/BattleModel/BattleCommander.cpp
        ../Sources-Cpp/BattleModel/BattleObjects.cpp
        ../Sources-Cpp/BattleModel/BattleObjects_v1.cpp
        ../Sources-Cpp/BattleModel/BattleObserver.cpp
        ../Sources-Cpp/BattleModel/BattleScenario.cpp
        ../Sources-Cpp/BattleModel/BattleSimulator.cpp
        ../Sources-Cpp/BattleModel/BattleSimulator_v1_0_0.cpp
        ../Sources-Cpp/BattleScript/BattleScript.cpp
        ../Sources-Cpp/BattleScript/MonkeyScript.cpp
        ../Sources-Cpp/BattleScript/PracticeScript.cpp
        ../Sources-Cpp/BattleView/BattleGesture.cpp
        ../Sources-Cpp/BattleView/BattleHotspot.cpp
        ../Sources-Cpp/BattleView/BattleLayer.cpp
        ../Sources-Cpp/BattleView/BattleView.cpp
        ../Sources-Cpp/BattleView/CasualtyMarker.cpp
        ../Sources-Cpp/BattleView/RangeMarker.cpp
        ../Sources-Cpp/BattleView/ShootingCounter.cpp
        ../Sources-Cpp/BattleView/SmokeCounter.cpp
        ../Sources-Cpp/BattleView/UnitCounter.cpp
        ../Sources-Cpp/BattleView/UnitMarker.cpp
        ../Sources-Cpp/BattleView/UnitMovementMarker.cpp
        ../Sources-Cpp/BattleView/UnitTrackingMarker.cpp
        ../Sources-Cpp/Graphics/CommonShaders.cpp
        ../Sources-Cpp/Graphics/FontAdapter.cpp
        ../Sources-Cpp/Graphics/FontDescriptor.cpp
        ../Sources-Cpp/Graphics/FrameBuffer.cpp
        ../Sources-Cpp/Graphics/GraphicsContext.cpp
        ../Sources-Cpp/Graphics/Image.cpp
        ../Sources-Cpp/Graphics/RenderBuffer.cpp
        ../Sources-Cpp/Graphics/RenderCall.cpp
        ../Sources-Cpp/Graphics/Sampler.cpp
        ../Sources-Cpp/Graphics/ShaderProgram.cpp
        ../Sources-Cpp/Graphics/Texture.cpp
        ../Sources-Cpp/Graphics/TextureAtlas.cpp
        ../Sources-Cpp/Graphics/TextureFont.cpp
        ../Sources-Cpp/Graphics/TextureResource.cpp
        ../Sources-Cpp/Graphics/Vertex.cpp
        ../Sources-Cpp/Graphics/VertexBuffer.cpp
        ../Sources-Cpp/Graphics/Viewport.cpp
        ../Sources-Cpp/Shapes/BillboardColorShader.cpp
        ../Sources-Cpp/Shapes/BillboardTextureShader.cpp
        ../Sources-Cpp/Shapes/BillboardTextureShape.cpp
        ../Sources-Cpp/Shapes/PathRenderer.cpp
        ../Sources-Cpp/Shapes/VertexShape.cpp
        ../Sources-Cpp/SmoothTerrain/SmoothTerrainRenderer.cpp
        ../Sources-Cpp/SmoothTerrain/SmoothTerrainShaders.cpp
        ../Sources-Cpp/SmoothTerrain/SmoothTerrainSky.cpp
        ../Sources-Cpp/SmoothTerrain/SmoothTerrainWater.cpp
        ../Sources-Cpp/Storage/Preferences.cpp
        ../Sources-Cpp/Storage/Resource.cpp
        ../Sources-Cpp/Surface/Animation.cpp
        ../Sources-Cpp/Surface/ClickGesture.cpp
        ../Sources-Cpp/Surface/ClickHotspot.cpp
        ../Sources-Cpp/Surface/Gesture.cpp
        ../Sources-Cpp/Surface/Hotspot.cpp
        ../Sources-Cpp/Surface/ScrollbarGesture.cpp
        ../Sources-Cpp/Surface/ScrollbarHotspot.cpp
        ../Sources-Cpp/Surface/ScrollbarGesture.cpp
        ../Sources-Cpp/Surface/ScrollerGesture.cpp
        ../Sources-Cpp/Surface/ScrollerHotspot.cpp
        ../Sources-Cpp/Surface/ScrollerView.cpp
        ../Sources-Cpp/Surface/Surface.cpp
        ../Sources-Cpp/Surface/SurfaceAdapter_SDL.cpp
        ../Sources-Cpp/Surface/Touch.cpp
        ../Sources-Cpp/Surface/View.cpp
        ../Sources-Cpp/TerrainForest/BillboardTerrainForest.cpp
        ../Sources-Cpp/TerrainForest/TerrainForest.cpp
        ../Sources-Cpp/TerrainSky/TerrainSky.cpp
        ../Sources-Cpp/TerrainView/EditorGesture.cpp
        ../Sources-Cpp/TerrainView/EditorHotspot.cpp
        ../Sources-Cpp/TerrainView/EditorModel.cpp
        ../Sources-Cpp/TerrainView/TerrainGesture.cpp
        ../Sources-Cpp/TerrainView/TerrainHotspot.cpp
        ../Sources-Cpp/TerrainView/TerrainView.cpp
        ../Sources-Cpp/TerrainView/TerrainViewport.cpp
        ../Sources-Cpp/TiledTerrain/TiledTerrainRenderer.cpp
        ../Sources-Cpp/Widgets/ButtonGrid.cpp
        ../Sources-Cpp/Widgets/ButtonGridTextureSheet.cpp
        ../Sources-Cpp/Widgets/ImageWidget.cpp
        ../Sources-Cpp/Widgets/StringWidget.cpp
        ../Sources-Cpp/Widgets/Widget.cpp
        ../Sources-Cpp/Widgets/WidgetShader.cpp
        ../Sources-Cpp/Widgets/WidgetView.cpp
        )


add_executable(openwar ${SOURCE_FILES})
target_link_libraries(openwar ${OPENGLES2_LIBRARIES} ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARY} ${SDL2_TTF_LIBRARY})

set_property(TARGET openwar PROPERTY CXX_STANDARD 11)
set_property(TARGET openwar PROPERTY CXX_STANDARD_REQUIRED ON)
