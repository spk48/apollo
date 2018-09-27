// Copyright 2018 Baidu Inc. All Rights Reserved.
// @author: hui yujiang (huiyujiang@baidu.com)
// @file: radar_tracker_conti_ars_tracker_test.cc
// @brief: conti ars tracker test

#include <gtest/gtest.h>
#include "cybertron/common/log.h"
#include "modules/perception/radar/lib/tracker/conti_ars_tracker/conti_ars_tracker.h"

namespace apollo {
namespace perception {
namespace lib {
DECLARE_string(work_root);
}  // namespace lib
namespace radar {
TEST(ContiArsTrackerTest, conti_ars_tracker_init_test) {
  BaseTracker* tracker = new ContiArsTracker();
  lib::FLAGS_work_root = "./radar_test_data/conti_ars_tracker";
  EXPECT_EQ(tracker->Init(), true);
  EXPECT_EQ(tracker->Name(), "ContiArsTracker");
  delete tracker;
}

TEST(ContiArsTrackerTest, conti_ars_tracker_track_test) {
  BaseTracker* tracker = new ContiArsTracker();
  lib::FLAGS_work_root = "./radar_test_data/conti_ars_tracker";
  tracker->Init();
  base::Frame radar_frame;
  radar_frame.timestamp = 123456789.1;
  radar_frame.objects.resize(2);
  radar_frame.objects[0].reset(new base::Object);
  radar_frame.objects[0]->track_id = 100;
  radar_frame.objects[0]->center << 12.0, 15.0, 0.0;
  radar_frame.objects[0]->velocity << 3.0, 4.0, 0.0;
  radar_frame.objects[1].reset(new base::Object);
  radar_frame.objects[1]->track_id = 200;
  radar_frame.objects[1]->center << 50.3, 100.4, 0.0;
  radar_frame.objects[1]->velocity << 3.0, 4.0, 0.0;
  TrackerOptions options;
  base::FramePtr tracked_frame(new base::Frame);
  bool state = tracker->Track(radar_frame, options, tracked_frame);
  EXPECT_EQ(state, true);
  delete tracker;
}

TEST(ContiArsTrackerTest, conti_ars_tracker_collect_test) {
  base::ObjectPtr object(new base::Object);
  object->track_id = 100;
  object->center << 10.0, 20.0, 0.0;
  object->velocity << 3.0, 4.0, 0.0;
  double timestamp = 123456789.0;
  RadarTrackPtr radar_track(new RadarTrack(object, timestamp));

  ContiArsTracker* tracker = new ContiArsTracker();
  lib::FLAGS_work_root = "./radar_test_data/conti_ars_tracker";
  tracker->Init();
  tracker->track_manager_->ClearTracks();
  tracker->track_manager_->AddTrack(radar_track);
  base::FramePtr tracked_frame(new base::Frame);
  tracker->CollectTrackedFrame(tracked_frame);
  EXPECT_EQ(tracked_frame->objects.size(), 0);
  RadarTrack::SetTrackedTimesThreshold(0);
  tracker->CollectTrackedFrame(tracked_frame);
  EXPECT_EQ(tracked_frame->objects.size(), 1);
  delete tracker;
}

TEST(ContiArsTrackerTest, conti_ars_tracker_unassigned_test) {
  base::ObjectPtr object(new base::Object);
  object->track_id = 100;
  object->center << 10.0, 20.0, 0.0;
  object->velocity << 3.0, 4.0, 0.0;
  double timestamp = 123456789.0;
  RadarTrackPtr radar_track(new RadarTrack(object, timestamp));

  std::vector<size_t> unassigned_tracks;
  unassigned_tracks.push_back(0);

  base::Frame radar_frame;
  radar_frame.timestamp =
      timestamp + ContiArsTracker::s_tracking_time_win_ + 1e-5;

  ContiArsTracker* tracker = new ContiArsTracker();
  lib::FLAGS_work_root = "./radar_test_data/conti_ars_tracker";
  tracker->Init();
  tracker->track_manager_->ClearTracks();
  tracker->track_manager_->AddTrack(radar_track);
  tracker->UpdateUnassignedTracks(radar_frame, unassigned_tracks);
  tracker->track_manager_->RemoveLostTracks();
  EXPECT_EQ(tracker->track_manager_->GetTracks().size(), 0);

  tracker->track_manager_->ClearTracks();
  RadarTrackPtr radar_track2(new RadarTrack(object, timestamp));
  tracker->track_manager_->AddTrack(radar_track2);
  radar_frame.timestamp =
      timestamp + ContiArsTracker::s_tracking_time_win_ - 1e-5;
  tracker->UpdateUnassignedTracks(radar_frame, unassigned_tracks);
  tracker->track_manager_->RemoveLostTracks();
  EXPECT_EQ(tracker->track_manager_->GetTracks().size(), 1);

  tracker->track_manager_->ClearTracks();
  RadarTrackPtr radar_track3(new RadarTrack(object, timestamp));
  radar_track3->SetObsRadarNullptr();
  tracker->track_manager_->AddTrack(radar_track3);
  radar_frame.timestamp =
      timestamp + ContiArsTracker::s_tracking_time_win_ - 1e-5;
  tracker->UpdateUnassignedTracks(radar_frame, unassigned_tracks);
  tracker->track_manager_->RemoveLostTracks();
  EXPECT_EQ(tracker->track_manager_->GetTracks().size(), 0);
  delete tracker;
}

}  // namespace radar
}  // namespace perception
}  // namespace apollo
