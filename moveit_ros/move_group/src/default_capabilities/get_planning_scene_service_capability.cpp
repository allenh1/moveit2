/*********************************************************************
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2012, Willow Garage, Inc.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of Willow Garage nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *********************************************************************/

/* Author: Ioan Sucan */

#include "get_planning_scene_service_capability.h"
#include <moveit/move_group/capability_names.h>

namespace move_group
{
MoveGroupGetPlanningSceneService::MoveGroupGetPlanningSceneService() : MoveGroupCapability("GetPlanningSceneService")
{
}

void MoveGroupGetPlanningSceneService::initialize()
{
  using std::placeholders::_1;
  using std::placeholders::_2;
  using std::placeholders::_3;

  get_scene_service_ = node_->create_service<moveit_msgs::srv::GetPlanningScene>(
    GET_PLANNING_SCENE_SERVICE_NAME, 
    std::bind(&MoveGroupGetPlanningSceneService::getPlanningSceneService, this, _1, _2, _3)
  );
}

bool MoveGroupGetPlanningSceneService::getPlanningSceneService(
  const std::shared_ptr<rmw_request_id_t> request_header,
  const std::shared_ptr<moveit_msgs::srv::GetPlanningScene::Request> req,
  std::shared_ptr<moveit_msgs::srv::GetPlanningScene::Response> res)
{
  if (req->components.components & moveit_msgs::msg::PlanningSceneComponents::TRANSFORMS)
    context_->planning_scene_monitor_->updateFrameTransforms();
  planning_scene_monitor::LockedPlanningSceneRO ps(context_->planning_scene_monitor_);

  moveit_msgs::msg::PlanningSceneComponents all_components;
  all_components.components = UINT_MAX;  // Return all scene components if nothing is specified.
  ps->getPlanningSceneMsg(res->scene, req->components.components ? req->components : all_components);

  return true;
}
}  // namespace move_group

#include <pluginlib/class_list_macros.hpp>

PLUGINLIB_EXPORT_CLASS(
  move_group::MoveGroupGetPlanningSceneService, move_group::MoveGroupCapability)
