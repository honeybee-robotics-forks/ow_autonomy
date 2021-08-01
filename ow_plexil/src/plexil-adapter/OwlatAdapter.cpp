// The Notices and Disclaimers for Ocean Worlds Autonomy Testbed for Exploration
// Research and Simulation can be found in README.md in the root directory of
// this repository.

// Implementation of PLEXIL interface adapter for OWLAT simulator.

// OW
#include "OwlatAdapter.h"
#include "OWLATSimInterface.h"
#include "subscriber.h"

// ROS
#include <ros/ros.h>
#include <geometry_msgs/Point.h>

// PLEXIL API
#include <AdapterConfiguration.hh>
#include <AdapterFactory.hh>
#include <AdapterExecInterface.hh>
#include <ArrayImpl.hh>
#include <Debug.hh>
#include <Expression.hh>
#include <StateCacheEntry.hh>

// C++
#include <map>
using std::string;
using std::vector;


static void owlat_unstow (Command* cmd, AdapterExecInterface* intf)
{
  std::unique_ptr<CommandRecord>& cr = new_command_record(cmd, intf);
  OWLATSimInterface::instance()->owlatUnstow (CommandId);
  send_ack_once(*cr);
}

static void owlat_stow (Command* cmd, AdapterExecInterface* intf)
{
  std::unique_ptr<CommandRecord>& cr = new_command_record(cmd, intf);
  OWLATSimInterface::instance()->owlatStow (CommandId);
  send_ack_once(*cr);
}

OwlatAdapter::OwlatAdapter (AdapterExecInterface& execInterface,
                            const pugi::xml_node& configXml)
  : CommonAdapter (execInterface, configXml)
{
  debugMsg("OwlatAdapter", " created.");
}

OwlatAdapter::~OwlatAdapter ()
{
}

bool OwlatAdapter::initialize()
{
  CommonAdapter::initialize();
  g_configuration->registerCommandHandler("owlat_unstow", owlat_unstow);
  g_configuration->registerCommandHandler("owlat_stow", owlat_stow);
  OWLATSimInterface::instance()->
    setCommandStatusCallback (command_status_callback);
  debugMsg("OwlatAdapter", " initialized.");
  return true;
}

void OwlatAdapter::lookupNow (const State& state, StateCacheEntry& entry)
{
  debugMsg("OwlatAdapter:lookupNow", " called on " << state.name() << " with "
           << state.parameters().size() << " arguments");

  Value retval = Unknown;  // the value of the queried state

  // At the moment there are no lookups in OWLAT, so return Unknown.
  //  if (! lookup(state.name(), state.parameters(), retval)) {
  //    ROS_ERROR("PLEXIL Adapter: Invalid lookup name: %s", state.name().c_str());
  //  }

  entry.update(retval);
}


extern "C" {
  void initowlat_adapter() {
    REGISTER_ADAPTER(OwlatAdapter, "owlat_adapter");
  }
}
