TARGET = Test1
TEMPLATE = app
QT += core
QT += network
QT += gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
QMAKE_CXXFLAGS += -std=c++11
INCLUDEPATH += ../../bxcore
INCLUDEPATH += ../../bxlib
DEFINES += __BX_DEBUG
win32::DEFINES += __BX_MEMORY
osx::DEFINES += __BX_USED_LONG_SIZET
ios::DEFINES += __BX_USED_LONG_SIZET

SOURCES += ../source/TestA.cpp
SOURCES += ../source/TestB.cpp
SOURCES += ../source/TestList.cpp
HEADERS += ../source/TestA.hpp
HEADERS += ../source/TestB.hpp
HEADERS += ../source/TestList.hpp

SOURCES += ../../bxcore/BxCoreImpl.cpp
SOURCES += ../../bxcore/BxCoreForQT5.cpp
SOURCES += ../../bxlib/BxMain.cpp
SOURCES += ../../bxaddon/BxAddOn_GIF.cpp
SOURCES += ../../bxaddon/BxAddOn_HQX.cpp
SOURCES += ../../bxaddon/BxAddOn_JPG.cpp
SOURCES += ../../bxaddon/BxAddOn_MD5.cpp
SOURCES += ../../bxaddon/BxAddOn_ZIP.cpp
HEADERS += ../../bxcore/BxCore.hpp
HEADERS += ../../bxcore/BxCoreImpl.hpp
HEADERS += ../../bxcore/BxCoreForQT5.hpp
HEADERS += ../../bxlib/BxAnimate.hpp
HEADERS += ../../bxlib/BxAutoBuffer.hpp
HEADERS += ../../bxlib/BxDraw.hpp
HEADERS += ../../bxlib/BxDynamic.hpp
HEADERS += ../../bxlib/BxExpress.hpp
HEADERS += ../../bxlib/BxGUI.hpp
HEADERS += ../../bxlib/BxImage.hpp
HEADERS += ../../bxlib/BxKeyword.hpp
HEADERS += ../../bxlib/BxMemory.hpp
HEADERS += ../../bxlib/BxPathFind.hpp
HEADERS += ../../bxlib/BxPool.hpp
HEADERS += ../../bxlib/BxScene.hpp
HEADERS += ../../bxlib/BxSingleton.hpp
HEADERS += ../../bxlib/BxString.hpp
HEADERS += ../../bxlib/BxTelex.hpp
HEADERS += ../../bxlib/BxThrow.hpp
HEADERS += ../../bxlib/BxTween.hpp
HEADERS += ../../bxlib/BxType.hpp
HEADERS += ../../bxlib/BxUtil.hpp
HEADERS += ../../bxlib/BxVar.hpp
HEADERS += ../../bxlib/BxVarMap.hpp
HEADERS += ../../bxlib/BxVarVector.hpp
HEADERS += ../../bxlib/BxWebContent.hpp
HEADERS += ../../bxlib/BxXml.hpp
