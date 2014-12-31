/**
 * Author: Aravinth Panchadcharam
 * Email: me@aravinth.info
 * Date: 29/12/14.
 * Project: Gesture Recogntion for Human-Robot Interaction
 */

#include <iostream>
#include <sstream>
#include <boost/log/trivial.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "NiTE.h"
#include "skeleton_tracker.h"


skeleton_tracker::skeleton_tracker(udp_server *server){
    server_ = server;
}

void skeleton_tracker::init_nite(){
    niteRc = nite::NiTE::initialize();
    if (niteRc != nite::STATUS_OK)
    {
        BOOST_LOG_TRIVIAL(error) << "NiTE initialization failed" ;
    }
    else
    {
        BOOST_LOG_TRIVIAL(info) << "NiTE initialized" ;
    }
    
    niteRc = userTracker.create();
    if (niteRc != nite::STATUS_OK)
    {
        BOOST_LOG_TRIVIAL(error) << "Couldn't create user tracker" ;
    }
    else
    {
        BOOST_LOG_TRIVIAL(info) << "User tracker created" ;
    }
}



void print_status(std::string message, int userId){
    BOOST_LOG_TRIVIAL(info) << "User " << userId << " : " << message ;
}

#define MAX_USERS 10
void skeleton_tracker::update_user_state(const nite::UserData& user)
{
    bool g_visibleUsers[MAX_USERS] = {false};
    nite::SkeletonState g_skeletonStates[MAX_USERS] = {nite::SKELETON_NONE};
    
    if (user.isNew()){
        print_status("New", user.getId());
    }else if (user.isVisible() && !g_visibleUsers[user.getId()]){
        print_status("Visible", user.getId());
    }else if (!user.isVisible() && g_visibleUsers[user.getId()]){
        print_status("Out of Scene", user.getId());
    }else if (user.isLost()){
        print_status("Lost", user.getId());
        g_visibleUsers[user.getId()] = user.isVisible();
    }
    
    
    //    if(g_skeletonStates[user.getId()] != user.getSkeleton().getState())
    //    {
    //        switch(g_skeletonStates[user.getId()] = user.getSkeleton().getState())
    //        {
    //            case nite::SKELETON_NONE:
    //                print_status("Stopped tracking.", user.getId());
    //                break;
    //            case nite::SKELETON_CALIBRATING:
    //                print_status("Calibrating", user.getId());
    //                break;
    //            case nite::SKELETON_TRACKED:
    //                print_status("Tracking", user.getId());
    //                break;
    //            case nite::SKELETON_CALIBRATION_ERROR_TORSO:
    //                print_status("Calibration Failed", user.getId());
    //                break;
    //        }
    //    }
}



using boost::property_tree::ptree;
using boost::property_tree::write_json;

ptree joint_serializer(const nite::SkeletonJoint& joint){
    ptree joint_array, xAxis, yAxis, zAxis;
    
    xAxis.put("", joint.getPosition().x);
    yAxis.put("", joint.getPosition().y);
    zAxis.put("", joint.getPosition().z);
    
    joint_array.push_back(std::make_pair("", xAxis));
    joint_array.push_back(std::make_pair("", yAxis));
    joint_array.push_back(std::make_pair("", zAxis));
    
    return joint_array;
}

void skeleton_tracker::send_skeleton(const nite::UserData& user){
    ptree skeleton;
    const nite::SkeletonJoint& JOINT_HEAD = user.getSkeleton().getJoint(nite::JOINT_HEAD);
    const nite::SkeletonJoint& JOINT_NECK = user.getSkeleton().getJoint(nite::JOINT_NECK);
    const nite::SkeletonJoint& JOINT_LEFT_SHOULDER = user.getSkeleton().getJoint(nite::JOINT_LEFT_SHOULDER);
    const nite::SkeletonJoint& JOINT_RIGHT_SHOULDER = user.getSkeleton().getJoint(nite::JOINT_RIGHT_SHOULDER);
    const nite::SkeletonJoint& JOINT_LEFT_ELBOW = user.getSkeleton().getJoint(nite::JOINT_LEFT_ELBOW);
    const nite::SkeletonJoint& JOINT_RIGHT_ELBOW = user.getSkeleton().getJoint(nite::JOINT_RIGHT_ELBOW);
    const nite::SkeletonJoint& JOINT_LEFT_HAND = user.getSkeleton().getJoint(nite::JOINT_LEFT_HAND);
    const nite::SkeletonJoint& JOINT_RIGHT_HAND = user.getSkeleton().getJoint(nite::JOINT_RIGHT_HAND);
    const nite::SkeletonJoint& JOINT_TORSO = user.getSkeleton().getJoint(nite::JOINT_TORSO);
    const nite::SkeletonJoint& JOINT_LEFT_HIP = user.getSkeleton().getJoint(nite::JOINT_LEFT_HIP);
    const nite::SkeletonJoint& JOINT_RIGHT_HIP = user.getSkeleton().getJoint(nite::JOINT_RIGHT_HIP);
    const nite::SkeletonJoint& JOINT_LEFT_KNEE = user.getSkeleton().getJoint(nite::JOINT_LEFT_KNEE);
    const nite::SkeletonJoint& JOINT_RIGHT_KNEE = user.getSkeleton().getJoint(nite::JOINT_RIGHT_KNEE);
    const nite::SkeletonJoint& JOINT_LEFT_FOOT = user.getSkeleton().getJoint(nite::JOINT_LEFT_FOOT);
    const nite::SkeletonJoint& JOINT_RIGHT_FOOT = user.getSkeleton().getJoint(nite::JOINT_RIGHT_FOOT);
    
    skeleton.add_child("HEAD", joint_serializer(JOINT_HEAD));
    skeleton.add_child("NECK", joint_serializer(JOINT_NECK));
    skeleton.add_child("LEFT_SHOULDER", joint_serializer(JOINT_LEFT_SHOULDER));
    skeleton.add_child("RIGHT_SHOULDER", joint_serializer(JOINT_RIGHT_SHOULDER));
    skeleton.add_child("LEFT_ELBOW", joint_serializer(JOINT_LEFT_ELBOW));
    skeleton.add_child("RIGHT_ELBOW", joint_serializer(JOINT_RIGHT_ELBOW));
    skeleton.add_child("LEFT_HAND", joint_serializer(JOINT_LEFT_HAND));
    skeleton.add_child("RIGHT_HAND", joint_serializer(JOINT_RIGHT_HAND));
    skeleton.add_child("TORSO", joint_serializer(JOINT_TORSO));
    skeleton.add_child("LEFT_HIP", joint_serializer(JOINT_LEFT_HIP));
    skeleton.add_child("RIGHT_HIP", joint_serializer(JOINT_RIGHT_HIP));
    skeleton.add_child("LEFT_KNEE", joint_serializer(JOINT_LEFT_KNEE));
    skeleton.add_child("RIGHT_KNEE", joint_serializer(JOINT_RIGHT_KNEE));
    skeleton.add_child("LEFT_FOOT", joint_serializer(JOINT_LEFT_FOOT));
    skeleton.add_child("RIGHT_FOOT", joint_serializer(JOINT_RIGHT_FOOT));
    
    std::ostringstream skeleton_buffer;
    write_json (skeleton_buffer, skeleton, false);
    std::string json = skeleton_buffer.str();
    std::cout << json;
}


void skeleton_tracker::track_skeleton(){
    nite::UserTrackerFrameRef userTrackerFrame;
    for(;;)
    {
        niteRc = userTracker.readFrame(&userTrackerFrame);
        if (niteRc != nite::STATUS_OK)
        {
            printf("Get next frame failed\n");
            continue;
        }
        
        const nite::Array<nite::UserData>& users = userTrackerFrame.getUsers();
        for (int i = 0; i < users.getSize(); ++i)
        {
            const nite::UserData& user = users[i];
            
            if (user.isNew())
            {
                userTracker.startSkeletonTracking(user.getId());
                print_status("New User", user.getId());
            }
            else if (user.getSkeleton().getState() == nite::SKELETON_TRACKED)
            {
                const nite::SkeletonJoint& head = user.getSkeleton().getJoint(nite::JOINT_HEAD);
                if (head.getPositionConfidence() > .5){
                    //                    printf("%d. (%5.2f, %5.2f, %5.2f)\n", user.getId(), head.getPosition().x, head.getPosition().y, head.getPosition().z);
                    send_skeleton(user);
                }
            }
        }
    }
}

void skeleton_tracker::run(){
    init_nite();
    track_skeleton();
}

