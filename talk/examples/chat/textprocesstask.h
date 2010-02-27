#pragma once
#include "talk/xmpp/xmpptask.h"
#include "talk/xmpp/xmppengine.h"
#include "talk/base/sigslot.h"

namespace buzz {

const std::string STR_CHAT("chat");

class textprocesstask : public XmppTask {
public:
	textprocesstask(Task* parent) : 
	  XmppTask(parent, XmppEngine::HL_TYPE) {}
    
    virtual int ProcessStart();
	
	sigslot::signal2<const std::string &, const std::string &> SignalTextInfo;

	int sendMessages(const std::string& textContent, const std::string& jid_);
protected:
	virtual bool HandleStanza(const XmlElement * stanza);
};
}