bin_PROGRAMS = chat 
chat_CXXFLAGS = $(AM_CXXFLAGS)
chat_SOURCES = chat_main.cc chatclient.cc console.cc textprocesstask.cc
noinst_HEADERS = chatclient.h  console.h textprocesstask.h
chat_LDADD = $(top_srcdir)/talk/examples/login/libcricketexampleslogin.la \
	$(top_srcdir)/talk/session/tunnel/libcricketsessiontunnel.la  \
	$(top_srcdir)/talk/p2p/base/libcricketp2pbase.la \
	$(top_srcdir)/talk/p2p/client/libcricketp2pclient.la \
    $(top_srcdir)/talk/xmpp/libcricketxmpp.la \
    $(top_srcdir)/talk/xmllite/libcricketxmllite.la \
    $(top_srcdir)/talk/base/libcricketbase.la \
    $(EXPAT_LIBS) -lpthread -lssl -lcrypto

AM_CPPFLAGS = -DPOSIX
