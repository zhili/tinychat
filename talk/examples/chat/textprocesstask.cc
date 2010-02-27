#include "textprocesstask.h"
#include "talk/xmpp/constants.h"
#include <iostream>
#include <talk/xmpp/xmppclient.h>

namespace buzz {


bool
textprocesstask::HandleStanza(const XmlElement* stanza) {
  if (stanza->Name() != QN_MESSAGE)
	return false;
  if (stanza->HasAttr(QN_TYPE) && stanza->Attr(QN_TYPE) != STR_CHAT)
    return false;
  QueueStanza(stanza);
  return true;
}

int textprocesstask::ProcessStart() {
  const XmlElement * stanza = NextStanza();
  if (stanza == NULL)
    return STATE_BLOCKED;

   const XmlChild *child = stanza->FirstChild();
   if (!child)
        return STATE_START;
    const XmlElement *body = child->AsElement();

	if (body != NULL) {

		SignalTextInfo(body->BodyText(), stanza->Attr(QN_FROM));
		//sendMessages("auto", stanza->Attr(QN_FROM));
    }
    return STATE_START;
}

int textprocesstask::sendMessages(const std::string& textContent, const std::string& jid_) {

	scoped_ptr<XmlElement> result(new XmlElement(QN_MESSAGE));
    result->AddAttr(QN_FROM, GetClient()->jid().Str());
    result->AddAttr(QN_TO, jid_);
    result->AddAttr(QN_TYPE, STR_CHAT);
    result->AddElement(new XmlElement(QN_BODY));
	result->AddText(textContent, 1);
    if (SendStanza(result.get()) != XMPP_RETURN_OK)
		return STATE_ERROR;

	return STATE_DONE;
}
}