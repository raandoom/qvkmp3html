QT +=       core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET =    qvkmp3html
TEMPLATE =  app


SOURCES +=  main.cpp\
            mainwindow.cpp \
            vkhtmlparse.cpp \
            filedownloader.cpp \
            useful_funcs.cpp \
    htmlcxx/ParserDom.cc \
    htmlcxx/Node.cc \
    htmlcxx/ParserSax.cc

HEADERS +=  mainwindow.h \
            vkhtmlparse.h \
            filedownloader.h \
            useful_funcs.h \
    htmlcxx/ParserDom.h \
    htmlcxx/Node.h \
    htmlcxx/ParserSax.h

FORMS +=    mainwindow.ui
