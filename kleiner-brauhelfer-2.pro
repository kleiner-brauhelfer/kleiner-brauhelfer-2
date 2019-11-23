TEMPLATE = subdirs
SUBDIRS += kleiner-brauhelfer-core \
           kleiner-brauhelfer
CONFIG += ordered

kleiner-brauhelfer-core.subdir = kleiner-brauhelfer-core
kleiner-brauhelfer.subdir = kleiner-brauhelfer
kleiner-brauhelfer.depends = kleiner-brauhelfer-core
