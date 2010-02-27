
#include <string>
#include <vector>

#include "talk/xmpp/constants.h"
#include "talk/base/helpers.h"
#include "talk/base/thread.h"
#include "talk/base/network.h"
#include "talk/base/socketaddress.h"
#include "talk/p2p/base/sessionmanager.h"
#include "talk/p2p/client/httpportallocator.h"
#include "talk/p2p/client/sessionmanagertask.h"
#include "talk/session/phone/phonesessionclient.h"
#include "talk/examples/chat/chatclient.h"
#include "talk/examples/chat/console.h"
#include "talk/examples/login/presencepushtask.h"
#include "talk/examples/login/presenceouttask.h"
#include "talk/examples/login/jingleinfotask.h"
#include "talk/xmpp/constants.h"

namespace {

const char* DescribeStatus(buzz::Status::Show show, const std::string& desc) {
  switch (show) {
  case buzz::Status::SHOW_XA:      return desc.c_str();
  case buzz::Status::SHOW_ONLINE:  return "online";
  case buzz::Status::SHOW_AWAY:    return "away";
  case buzz::Status::SHOW_DND:     return "do not disturb";
  case buzz::Status::SHOW_CHAT:    return "ready to chat";
  default:                         return "offline";
  }
}

} // namespace


void CallClient::ParseLine(const std::string& line) {
  std::vector<std::string> words;
  int start = -1;
  int state = 0;
  for (int index = 0; index <= static_cast<int>(line.size()); ++index) {
    if (state == 0) {
      if (!isspace(line[index])) {
        start = index;
        state = 1;
      }
    } else {
      assert(state == 1);
      assert(start >= 0);
      if (isspace(line[index])) {
        std::string word(line, start, index - start);
		words.push_back(word);
        start = -1;
        state = 0;
      }
    }
  }

  // Global commands
  if ((words.size() == 1) && (words[0] == "quit")) {
      exit(0);
  }

   if ((words.size() == 1) && (words[0] == "roster")) {
      PrintRoster();
    } else if (words.size() >= 2){
      // console_->Print(CONSOLE_COMMANDS);
	  tpt->sendMessages("wtf", lastChatId);
   } else {

	}
}

CallClient::CallClient(buzz::XmppClient* xmpp_client)
    : xmpp_client_(xmpp_client), roster_(new RosterMap) {
  xmpp_client_->SignalStateChange.connect(this, &CallClient::OnStateChange);
}

CallClient::~CallClient() {
  delete roster_;
}

const std::string CallClient::strerror(buzz::XmppEngine::Error err) {
  switch (err) {
   case  buzz::XmppEngine::ERROR_NONE: 
     return "";
   case  buzz::XmppEngine::ERROR_XML:  
     return "Malformed XML or encoding error";
   case  buzz::XmppEngine::ERROR_STREAM: 
     return "XMPP stream error";
   case  buzz::XmppEngine::ERROR_VERSION:
     return "XMPP version error";
   case  buzz::XmppEngine::ERROR_UNAUTHORIZED:
     return "User is not authorized (Check your username and password)";
   case  buzz::XmppEngine::ERROR_TLS:
     return "TLS could not be negotiated";
   case	 buzz::XmppEngine::ERROR_AUTH:
     return "Authentication could not be negotiated";
   case  buzz::XmppEngine::ERROR_BIND:
     return "Resource or session binding could not be negotiated";
   case  buzz::XmppEngine::ERROR_CONNECTION_CLOSED:
     return "Connection closed by output handler.";
   case  buzz::XmppEngine::ERROR_DOCUMENT_CLOSED:
     return "Closed by </stream:stream>";
   case  buzz::XmppEngine::ERROR_SOCKET:
     return "Socket error";
   default:
	 return "Unknown error";
  }
}



void CallClient::OnStateChange(buzz::XmppEngine::State state) {
  switch (state) {
  case buzz::XmppEngine::STATE_START:
    console_->Print("connecting...");
    break;

  case buzz::XmppEngine::STATE_OPENING:
    console_->Print("logging in...");
    break;

  case buzz::XmppEngine::STATE_OPEN:
    console_->Print("logged in...");
    InitPresence();
    InitTextProcess();
    break;

  case buzz::XmppEngine::STATE_CLOSED:
    buzz::XmppEngine::Error error = xmpp_client_->GetError(NULL);
    console_->Print("logged out..." + strerror(error));
    exit(0);
  }
}

void CallClient::InitTextProcess() {
  tpt = new buzz::textprocesstask(xmpp_client_);
  tpt->SignalTextInfo.connect(this, &CallClient::OnTextReceive);
  tpt->Start();
}

void CallClient::OnTextReceive(const std::string& msg, const std::string& id) {
	console_->Printf("from %s:\n%s", id.c_str(), msg.c_str());
	lastChatId = id;
}

void CallClient::InitPresence() {
  presence_push_ = new buzz::PresencePushTask(xmpp_client_);
  presence_push_->SignalStatusUpdate.connect(
    this, &CallClient::OnStatusUpdate);
  presence_push_->Start();

  buzz::Status my_status;
  my_status.set_jid(xmpp_client_->jid());
  my_status.set_available(true);
  my_status.set_show(buzz::Status::SHOW_ONLINE);
  my_status.set_priority(0);
  my_status.set_know_capabilities(true);
  my_status.set_phone_capability(true);
  my_status.set_is_google_client(true);
  my_status.set_version("1.0.0.66");

  buzz::PresenceOutTask* presence_out_ =
      new buzz::PresenceOutTask(xmpp_client_);
  presence_out_->Send(my_status);
  presence_out_->Start();
}

void CallClient::OnStatusUpdate(const buzz::Status& status) {
  RosterItem item;
  item.jid = status.jid();
  item.show = status.show();
  item.status = status.status();

  std::string key = item.jid.Str();

  if (status.available()) {
     console_->Printf("Adding to roster: %s", key.c_str());
    (*roster_)[key] = item;
  } else {
    console_->Printf("Removing from roster: %s", key.c_str());
    RosterMap::iterator iter = roster_->find(key);
    if (iter != roster_->end())
      roster_->erase(iter);
  }
}

void CallClient::PrintRoster() {	
 console_->SetPrompting(false);
 console_->Printf("Roster contains %d callable", roster_->size());
 RosterMap::iterator iter = roster_->begin();
 while (iter != roster_->end()) {
   console_->Printf("%s - %s",
                    iter->second.jid.BareJid().Str().c_str(),
                    DescribeStatus(iter->second.show, iter->second.status));
    iter++;
  }
 console_->SetPrompting(true);
}

