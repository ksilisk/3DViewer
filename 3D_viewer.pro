QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += openglwidgets

CONFIG += c++17
LIBS += -lglu32 -lopengl32 # -lstdc++fs

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ui/gl/viewerwindow.cpp \
    ui/gl/openglwindow.cpp \
    ui/main/mainwindow.cpp \
    ui/main.cpp \
    parser/s21_parser.c

HEADERS += \
    ui/gl/viewerwindow.h \
    ui/gl/viewerwindow.h \
    ui/gl/openglwindow.h \
    ui/main/mainwindow.h \
    parser/s21_parser.h

FORMS += \
    ui/mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
