#ifndef CRICKET_EXAMPLES_CALL_CALLCLIENT_H__
#define CRICKET_EXAMPLES_CALL_CALLCLIENT_H__

#include <map>
#include <string>
#include "talk/base/autodetectproxy.h"
#include "talk/p2p/base/session.h"
#include "talk/p2p/client/httpportallocator.h"
#include "talk/xmpp/xmppclient.h"
#include "talk/examples/login/status.h"
#include "talk/examples/chat/console.h"
#include "talk/examples/chat/textprocesstask.h"

namespace buzz {
class PresencePushTask;
class Status;
}

namespace talk_base {
class Thread;
class NetworkManager;
}


struct RosterItem {
  buzz::Jid jid;
  buzz::Status::Show show;
  std::string status;
};

class CallClient: public sigslot::has_slots<> {
public:
  CallClient(buzz::XmppClient* xmpp_client);
  ~CallClient();

  void PrintRoster();
  void SetConsole(Console *console) {console_ = console;}
  void ParseLine(const std::string &str);

private:
  typedef std::map<std::string,RosterItem> RosterMap;
  
  Console *console_;
  buzz::XmppClient* xmpp_client_;

  buzz::PresencePushTask* presence_push_;
  RosterMap* roster_;
  
  std::string lastChatId;
  buzz::textprocesstask *tpt;
  void OnStateChange(buzz::XmppEngine::State state);
  void OnTextReceive(const std::string& msg, const std::string& id);

  const std::string strerror(buzz::XmppEngine::Error err);
 
  void InitPresence();
  void OnStatusUpdate(const buzz::Status& status);
  void InitTextProcess();
};

#endif 
