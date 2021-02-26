// The Notices and Disclaimers for Ocean Worlds Autonomy Testbed for Exploration
// Research and Simulation can be found in README.md in the root directory of
// this repository.

#ifndef Ow_Interface_H
#define Ow_Interface_H

// Interface to lander simulator.  Singleton, because only once instance will
// ever be needed in the current autonomy scheme, which has one autonomy
// executive per lander.

#include <ros/ros.h>

// ROS Actions
#include <actionlib/client/simple_action_client.h>
#include <ow_lander/UnstowAction.h>
#include <ow_lander/StowAction.h>
#include <ow_lander/GrindAction.h>
#include <ow_lander/GuardedmoveAction.h>

#include <control_msgs/JointControllerState.h>
#include <sensor_msgs/JointState.h>
#include <sensor_msgs/Image.h>
#include <geometry_msgs/Point.h>
#include <string>

using UnstowActionClient =
  actionlib::SimpleActionClient<ow_lander::UnstowAction>;
using StowActionClient =
  actionlib::SimpleActionClient<ow_lander::StowAction>;
using GrindActionClient =
  actionlib::SimpleActionClient<ow_lander::GrindAction>;
using GuardedMoveActionClient =
  actionlib::SimpleActionClient<ow_lander::GuardedmoveAction>;

class OwInterface
{
 public:
  static OwInterface* instance();
  OwInterface ();
  ~OwInterface ();
  OwInterface (const OwInterface&) = delete;
  OwInterface& operator= (const OwInterface&) = delete;
  void initialize ();

  // Operational interface

  void guardedMove (double x, double y, double z,
                    double direction_x, double direction_y, double direction_z,
                    double search_distance, int id);
  void tiltAntenna (double degrees, int id);
  void panAntenna (double degrees, int id);
  void takePicture (int id);
  void digLinear (double x, double y, double depth, double length,
                  double ground_pos, int id);
  void digCircular (double x, double y, double depth,
                    double ground_pos, bool parallel, int id);
  void grind (double x, double y, double depth, double length,
              bool parallel, double ground_pos, int id);
  void stow (int id);
  void unstow (int id);
  void deliverSample (double x, double y, double z, int id);
  void takePanorama (double elev_lo, double elev_hi,
                     double lat_overlap, double vert_overlap);

  // State interface
  double getTilt () const;
  double getPanDegrees () const;
  double getPanVelocity () const;
  double getTiltVelocity () const;
  double getVoltage () const;
  double getRemainingUsefulLife () const;
  bool   groundFound () const;
  double groundPosition () const;

  // Is the given operation (as named in .cpp file) running?
  bool running (const std::string& name) const;

  bool hardTorqueLimitReached (const std::string& joint_name) const;
  bool softTorqueLimitReached (const std::string& joint_name) const;

  // Command feedback
  void setCommandStatusCallback (void (*callback) (int, bool));


 private:
  void unstow1 (int id);
  void stow1 (int id);
  void grind1 (double x, double y, double depth, double length,
               bool parallel, double ground_pos, int id);
  void guardedMove1 (double x, double y, double z,
                     double direction_x, double direction_y, double direction_z,
                     double search_distance, int id);

  bool operationRunning (const std::string& name) const;

  void jointStatesCallback (const sensor_msgs::JointState::ConstPtr&);
  void tiltCallback (const control_msgs::JointControllerState::ConstPtr&);
  void panCallback (const control_msgs::JointControllerState::ConstPtr&);
  void cameraCallback (const sensor_msgs::Image::ConstPtr&);
  void managePanTilt (const std::string& opname,
                      double position, double velocity,
                      double current, double goal,
                      const ros::Time& start);

  static OwInterface* m_instance;
  ros::NodeHandle* m_genericNodeHandle;

  // Publishers and subscribers

  ros::Publisher*  m_antennaTiltPublisher;
  ros::Publisher*  m_antennaPanPublisher;
  ros::Publisher*  m_leftImageTriggerPublisher;

  ros::Subscriber* m_antennaPanSubscriber;
  ros::Subscriber* m_antennaTiltSubscriber;
  ros::Subscriber* m_jointStatesSubscriber;
  ros::Subscriber* m_cameraSubscriber;
  ros::Subscriber* m_socSubscriber;
  ros::Subscriber* m_rulSubscriber;
  ros::Subscriber* m_guardedMoveSubscriber;

  // Action clients
  std::unique_ptr<GuardedMoveActionClient> m_guardedMoveClient;
  std::unique_ptr<UnstowActionClient> m_unstowClient;
  std::unique_ptr<StowActionClient> m_stowClient;
  std::unique_ptr<GrindActionClient> m_grindClient;

  // Antenna state - note that pan and tilt can be concurrent.
  double m_currentPan, m_currentTilt;
  double m_goalPan, m_goalTilt;      // commanded pan/tilt values
  ros::Time m_panStart, m_tiltStart; // pan/tilt start times
};

#endif
